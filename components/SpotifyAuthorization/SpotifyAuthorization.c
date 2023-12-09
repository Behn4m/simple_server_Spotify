#include <stdio.h>
#include "SpotifyAuthorization.h"
#include "GlobalInit.h"
#include "JsonExtraction.h"
bool SpotifyAuthorizationSuccessful;
SemaphoreHandle_t FindCodeSemaphore = NULL;
SemaphoreHandle_t FailToFindCodeSemaphore = NULL;
extern QueueHandle_t BufQueue1;
extern SemaphoreHandle_t GetResponseSemaphore;
static const char *TAG = "SpotifyTask";
static const char *TAG_APP = "SPOTIFY";
extern struct Token_ TokenParam;
extern struct UserInfo_ UserInfo;
void SpotifyAuth();
bool SpotifyTokenRenew();
static void SpotifyTask(void *pvparameters);

/**
 * @brief This function handles the first HTTPS request to Spotify and redirects the user to the authorization page.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t FirstRequest(httpd_req_t *req)
{
    char loc_url[SMALLBUF + 150];
    ESP_LOGI(TAG, "here send to client - Request_\n");
    sprintf(loc_url, "http://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state", ClientId, ReDirectUri);
    httpd_resp_set_hdr(req, "Location", loc_url);
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_status(req, "302");
    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * @brief This function handles the callback HTTPS request from Spotify and processes the response data.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t HttpsUserCallBackFunc(httpd_req_t *req)
{
    char Buf[MEDIUMBUF];
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        ESP_LOGI(TAG, "\n\n\n%s\n\n", Buf);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        uint8_t a = FindCode(Buf, sizeof(Buf));
        ESP_LOGI(TAG, "\na=%d", a);
        if (a == 1)
        {
            vTaskDelay(Sec / portTICK_PERIOD_MS);
            xSemaphoreGive(FindCodeSemaphore);
            if (xQueueSend(BufQueue1, Buf, 0) == pdTRUE)
            {
                ESP_LOGI(TAG, "Sent CODE with queue \n");
            }
        }
        ESP_LOGI(TAG, "\nafter find code ");
    }
    else
    {
        xSemaphoreGive(FailToFindCodeSemaphore);
        sprintf(Buf, "BAD ARGS");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_500);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        ESP_LOGI(TAG_APP, "bad arguments - the response does not include correct structure");
    }
    return ESP_OK;
}

/**
 * this strcut is http URL handler if receive "/" FirstRequest getting run
 */
static const httpd_uri_t Request_ = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = FirstRequest};

/**
 * this strcut is http URL handler if receive "/callback" HttpsUserCallBackFunc getting run
 */
static const httpd_uri_t Responce_ = {
    .uri = "/callback/",
    .method = HTTP_GET,
    .handler = HttpsUserCallBackFunc};

/**
 * @brief This function starts the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
static httpd_handle_t StartWebServer(void)
{
    httpd_handle_t Server_ = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&Server_, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(Server_, &Request_);
        httpd_register_uri_handler(Server_, &Responce_);
        return Server_;
    }
    return Server_;
}

/**
 * @brief This function stops the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
static esp_err_t StopWebServer(httpd_handle_t server)
{
    return httpd_stop(server);
}

/**
 * @brief This function is the handler for the disconnect event.
 * @param[in] arg Pointer to the HTTP server handle.
 * @param[in] event_base The event base.
 * @param[in] event_id The event ID.
 * @param[in] event_data The event data.
 */
static void HttpLocalServerDisconnectHandler(void *arg, esp_event_base_t event_base,
                                             int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        if (StopWebServer(*server) == ESP_OK)
        {
            *server = NULL;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}

/**
 * @brief This function is the handler for the connect event.
 * @param[in] arg Pointer to the HTTP server handle.
 * @param[in] event_base The event base.
 * @param[in] event_id The event ID.
 * @param[in] event_data The event data.
 */
static void HttpLocalServerConnectHandler(void *arg, esp_event_base_t event_base,
                                          int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        *server = StartWebServer();
    }
}

/**
 * @brief This function starts the mDNS service.
 */
void StartMDNSService()
{
    esp_err_t err = mdns_init();
    if (err)
    {
        ESP_LOGI(TAG_APP, "MDNS Init failed: %d\n", err);
        return;
    }
    mdns_hostname_set("deskhub");
    mdns_instance_name_set("Spotify");
}

/**
 * @brief This function handles the Spotify authorization process.
 */
void SpotifyModule()
{
    FindCodeSemaphore = xSemaphoreCreateBinary();
    FailToFindCodeSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(&SpotifyTask, "SpotifyTask", SpotifyTaskStackSize, NULL, 1, NULL);
}

/**
 * @brief This function is the entry point for handling HTTPS requests for Spotify authorization.
 * @param[in] pvparameters  need it because its Task !
 */
static void SpotifyTask(void *pvparameters)
{
    static httpd_handle_t _Server = NULL;
    
    // start the Local Server for Spotify
    StartMDNSService();
    _Server = StartWebServer();
    
    ESP_LOGI(TAG, "\nSpotify local server created!\n");
    
    bool TokenSaved = 0;
    bool RefreshTokenSaved = 0;
    // check if Spotify already authorized or not
    if (xSemaphoreTake(IsSpotifyAthurizedSemaphore, 1) == pdTRUE)
    {
        SpotifyAuthorizationSuccessful = SpotifyTokenRenew();
    }
    else
    {
        SpotifyAuthorizationSuccessful = 0;
    }

    while (1)
    {
        if (SpotifyAuthorizationSuccessful == 1)
        {
            if (RefreshTokenSaved == 1)
            {
                SpiffsRemoveFile(SpotifyConfigAddressInSpiffs);
                SaveFileInSpiffsWithTxtFormat(SpotifyConfigAddressInSpiffs, "refresh_token", TokenParam.refresh_token, NULL, NULL);
            }
            if (TokenSaved == 0)
            {
                // SaveFileInSpiffsWithTxtFormat(SpotifyConfigAddressInSpiffs, "access_token", TokenParam.access_token,
                //                               "token_type", TokenParam.token_type, "expires_in_ms", TokenParam.expires_in_ms, "refresh_token", TokenParam.refresh_token, "scope", TokenParam.scope, NULL, NULL);
                SaveFileInSpiffsWithTxtFormat(SpotifyConfigAddressInSpiffs, "refresh_token", TokenParam.refresh_token, NULL, NULL);
                RefreshTokenSaved = 1;
                TokenSaved = 1;
            }
            char receivedData[LONGBUF];
            ESP_LOGI(TAG, "\nSpotifyAuth has done !\n");
            vTaskDelay((8 * Sec) / portTICK_PERIOD_MS);
            
            // GetUserStatus();
            GetCurrentPlaying();
            vTaskDelay(((Hour * Sec) -100) / portTICK_PERIOD_MS);
            
            // DO WE NEED THIS?
            SendRequest_ExchangeTokenWithRefreshToken(receivedData, sizeof(receivedData), TokenParam.refresh_token);
        }
        else
        {
            SpotifyAuth();
        }
    }
}

/**
 *  The authorization code grant type is used to obtain both access
 * tokens and refresh tokens and is optimized for confidential clients.
 * Since this is a redirection-based flow, the client must be capable of
   interacting with the resource owner's user-agent (typically a web
   browser) and capable of receiving incoming requests (via redirection)
   from the authorization server.
 *  *     +----------+
     | Resource |
     |   Owner  |
     |          |
     +----------+
          ^
          |
         (B)
     +----|-----+          Client Identifier      +---------------+
     |         -+----(A)-- & Redirection URI ---->|               |
     |  User-   |                                 | Authorization |
     |  Agent  -+----(B)-- User authenticates --->|     Server    |
     |          |                                 |               |
     |         -+----(C)-- Authorization Code ---<|               |
     +-|----|---+                                 +---------------+
       |    |                                         ^      v
      (A)  (C)                                        |      |
       |    |                                         |      |
       ^    v                                         |      |
     +---------+                                      |      |
     |         |>---(D)-- Authorization Code ---------'      |
     |  Client |          & Redirection URI                  |
     |         |                                             |
     |         |<---(E)----- Access Token -------------------'
     +---------+       (w/ Optional Refresh Token)
 * @brief This function do all thing for authorisation in Spotify web api
 ** we have 4 stage for pass authorisation in Spotify server
 * 1-give CODE
 * 2-send request for give access token
 * 3-  extract json from RAW response
 * 4-extract JSON and get param from needed parameter
 */
void SpotifyAuth()
{
    char receivedData[LONGBUF];
    if (xSemaphoreTake(FindCodeSemaphore, portMAX_DELAY) == pdTRUE)
    {
        // 1-give CODE
        if (xQueueReceive(BufQueue1, receivedData, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received CODE by Queue: %s\n", receivedData);
        }
        // 2-send request for give access token
        SendRequestAndGiveToken(receivedData, sizeof(receivedData), receivedData, sizeof(receivedData));
        if (xSemaphoreTake(GetResponseSemaphore, portMAX_DELAY) == pdTRUE)
        {
            memset(receivedData, 0x0, sizeof(receivedData));
            // wait until get the response from Spotify
            ESP_LOGI(TAG, "waiting for spotify to response\n");

            if (xQueueReceive(BufQueue1, receivedData, portMAX_DELAY) == pdTRUE)
            {
                ESP_LOGI(TAG, "Received TOKEN by Queue: %s\n", receivedData);
            }
            else
            {
                ESP_LOGI(TAG, "Spotify not responds %s\n", receivedData);
            }

            // 3-  extract json from RAW response
            if (FindToken(receivedData, sizeof(receivedData)) != 1)
            {
                vTaskDelay((60 * Sec) / portTICK_PERIOD_MS);
                SendRequestAndGiveToken(receivedData, sizeof(receivedData), receivedData, sizeof(receivedData));
                if (FindToken(receivedData, sizeof(receivedData)) != 1)
                {
                    ESP_LOGI(TAG, "fail to get Token !!!");
                    esp_restart();
                }
            }
            //  4-extract JSON and get param from needed parameter
            ExtractionJsonParamForFindAccessToken(receivedData, sizeof(receivedData));
            SpotifyAuthorizationSuccessful = 1;
        }
    }
    else
    {
        ESP_LOGI(TAG, "fail to get code !!!");
        esp_restart();
    }
}

/**
 * @brief This function reads refresh token from spiffs and send request for new token
 * @param null
 * @return True if token received and saved, false for otherwise
 */
bool SpotifyTokenRenew(void)
{
    char receivedData[LONGBUF];

    ReadTxtFileFromSpiffs(SpotifyConfigAddressInSpiffs, "refresh_token", TokenParam.refresh_token, NULL, NULL);
    SendRequest_ExchangeTokenWithRefreshToken(receivedData, sizeof(receivedData), TokenParam.refresh_token);
    if (xQueueReceive(BufQueue1, receivedData, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGI(TAG, "Token received by Queue: %s\n", receivedData);
        if (FindToken(receivedData, sizeof(receivedData)) == 1)
        {
            ESP_LOGI(TAG, "Token found!");
            return true;
        }
        else
        {
            ESP_LOGW(TAG, "Token not found!");
            return false;
        }
    }
    else
    {
        // SpotifyAuthorizationSuccessful = 0;
        ESP_LOGW(TAG, "timeout - Spotify not respond!");
        return false;
    }

    // TokenSaved = 1;
        
}

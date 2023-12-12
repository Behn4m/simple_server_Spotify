#include <stdio.h>
#include "SpotifyAuthorization.h"
#include "MakeSpotifyRequest.h"
#include "GlobalInit.h"
#include "JsonExtraction.h"

// ****************************** Extern Variables 
extern struct Token_t TokenParam;
extern struct UserInfo_t UserInfo;

// ****************************** Local Variables 
bool SpotifyAuthorizationSuccessful;
SemaphoreHandle_t Spotify_FindCodeSemaphore = NULL;
SemaphoreHandle_t FailToFindCodeSemaphore = NULL;
extern SemaphoreHandle_t GetResponseSemaphore;
static const char *TAG = "SpotifyTask";
static const char *TAG_APP = "SPOTIFY";
static SpotifyInterfaceHandler_t  InterfaceHandler;

// QueueHandle_t Spotify_BufQueue;

// ****************************** Local Functions 
void Spotify_Authorization(char *code);
static void Spotify_MainTask(void *pvparameters);

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
{
    Spotify_FindCodeSemaphore = xSemaphoreCreateBinary();
    FailToFindCodeSemaphore = xSemaphoreCreateBinary();
    // Spotify_BufQueue = xQueueCreate(1, sizeof(char) * sizeof(char[2500]));
    InterfaceHandler = *SpotifyInterfaceHandler;
    xTaskCreate(&Spotify_MainTask, "Spotify_MainTask", SpotifyTaskStackSize, NULL, 1, NULL);
    return true;

}

void Spotify_UpdateStatus()
{
    GetCurrentPlaying();
    ESP_LOGI(TAG, "Trying to get NOW PLAYING");
}

/**
 * @brief This function handles the first HTTPS request to Spotify and redirects the user to the authorization page.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t Spotify_RequestDataAccess(httpd_req_t *req)
{
    char loc_url[SMALLBUF + 150];
    ESP_LOGI(TAG, "here send to client - Spotify_Request_Access_URI\n");
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
static esp_err_t Spotify_HttpsCallbackHandler(httpd_req_t *req)
{
    char Buf[MEDIUMBUF];
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        ESP_LOGI(TAG, "\n\n\n%s\n\n", Buf);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        if (FindCode(Buf, sizeof(Buf)) == true)
        {
            vTaskDelay(Sec / portTICK_PERIOD_MS);
            xSemaphoreGive(Spotify_FindCodeSemaphore);
            strncpy(InterfaceHandler.code, Buf, sizeof(Buf));
            ESP_LOGI(TAG, "CODE copied in the Handler \n");
        }
        ESP_LOGI(TAG, "after find code ");
    }
    else
    {
        xSemaphoreGive(FailToFindCodeSemaphore);
        sprintf(Buf, "BAD ARGS");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_500);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        ESP_LOGW(TAG_APP, "bad arguments - the response does not include correct structure");
    }
    return ESP_OK;
}

/**
 * this strcut is http URL handler if receive "/" Spotify_Update_Info getting run
 */
static const httpd_uri_t Spotify_Update_Info = {
    .uri = "/spotify/update",
    .method = HTTP_GET,
    .handler = Spotify_UpdateStatus};

/**
 * this strcut is http URL handler if receive "/" Spotify_RequestDataAccess getting run
 */
static const httpd_uri_t Spotify_Request_Access_URI = {
    .uri = "/spotify/init",
    .method = HTTP_GET,
    .handler = Spotify_RequestDataAccess};

/**
 * this strcut is http URL handler if receive "/callback" Spotify_HttpsCallbackHandler getting run
 */
static const httpd_uri_t Spotify_Get_Access_Response_URI = {
    .uri = "/callback/",
    .method = HTTP_GET,
    .handler = Spotify_HttpsCallbackHandler};

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
        httpd_register_uri_handler(Server_, &Spotify_Request_Access_URI);
        httpd_register_uri_handler(Server_, &Spotify_Get_Access_Response_URI);
        httpd_register_uri_handler(Server_, &Spotify_Update_Info);
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
 * @brief This function reads refresh token from spiffs and send request for new token
 * @param null
 * @return True if token received and saved, false for otherwise
 */
bool Spotify_TokenRenew(QueueHandle_t BufQueue)
{
    char receivedData[LONGBUF];
    ReadTxtFileFromSpiffs(SpotifyConfigAddressInSpiffs, "refresh_token", InterfaceHandler.token.refresh_token, NULL, NULL);
    SendRequest_ExchangeTokenWithRefreshToken(receivedData, sizeof(receivedData), InterfaceHandler.token.refresh_token);
    if (xQueueReceive(BufQueue, receivedData, SPOTIFY_RESPONSE_TIMEOUT) == pdTRUE)
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
}

/**
 * @brief This function find token in HTTP body request and check it for 
 * accuracy , if we doesn't find token for two time ESP getting restart !
 * @return true if token found, flase otherwisw
 */
void Spotify_FindTokenInHttpsBody(char *receivedData, size_t SizeOfReceivedData)
{
    if (FindToken(receivedData, SizeOfReceivedData) != 1)
    {
        ESP_LOGI(TAG, "looing for TOKEN !!!");
        vTaskDelay((60 * Sec) / portTICK_PERIOD_MS);
        SendRequestAndGiveToken(receivedData, SizeOfReceivedData, receivedData, SizeOfReceivedData);
        if (FindToken(receivedData, SizeOfReceivedData) != 1)
        {
            ESP_LOGI(TAG, "fail to get Token !!!");
            esp_restart();
        }
        else
        {
            ESP_LOGI(TAG, "TOKEN found in second try");
        }
    }
    else
    {
        ESP_LOGI(TAG, "TOKEN found in first try");
    }
}

/**
 * @brief This function is the entry point for handling HTTPS requests for Spotify authorization.
 * @param[in] pvparameters because it is a Task!
 * @return none
 */
static void Spotify_MainTask(void *pvparameters)
{
    static httpd_handle_t _Server = NULL;
    // start the Local Server for Spotify
    StartMDNSService();
    _Server = StartWebServer();

    ESP_LOGI(TAG, "** Spotify local server created!\n");

    bool TokenSaved = 0;
    bool RefreshTokenSaved = 0;
    
    // check if Spotify already authorized or not
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, 1) == pdTRUE)
    {
        SpotifyAuthorizationSuccessful = Spotify_TokenRenew(InterfaceHandler.code);
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
                SaveFileInSpiffsWithTxtFormat(SpotifyConfigAddressInSpiffs, "refresh_token", InterfaceHandler.token.refresh_token, NULL, NULL);
            }
            if (TokenSaved == 0)
            {
                // SaveFileInSpiffsWithTxtFormat(SpotifyConfigAddressInSpiffs, "access_token", TokenParam.access_token,
                //                               "token_type", TokenParam.token_type, "expires_in_ms", TokenParam.expires_in_ms, "refresh_token", TokenParam.refresh_token, "scope", TokenParam.scope, NULL, NULL);
                SaveFileInSpiffsWithTxtFormat(SpotifyConfigAddressInSpiffs, "refresh_token", InterfaceHandler.token.refresh_token, NULL, NULL);
                RefreshTokenSaved = 1;
                TokenSaved = 1;
            }
            char receivedData[LONGBUF];
            ESP_LOGI(TAG, "\nSpotifyAuth has done !\n");
            vTaskDelay((8 * Sec) / portTICK_PERIOD_MS);
            //GetUserStatus(InterfaceHandler.token.access_token);
            GetCurrentPlaying();
            vTaskDelay(((Hour * Sec) - 100) / portTICK_PERIOD_MS);
            // DO WE NEED THIS?
            SendRequest_ExchangeTokenWithRefreshToken(receivedData, sizeof(receivedData), InterfaceHandler.token.refresh_token);
        }
        else
        {
            Spotify_Authorization(InterfaceHandler.code);
        }
    }
}

/**
 * The authorization code grant type is used to obtain both access
 * tokens and refresh tokens and is optimized for confidential clients.
 * Since this is a redirection-based flow, the client must be capable of
   interacting with the resource owner's user-agent (typically a web
   browser) and capable of receiving incoming requests (via redirection)
   from the authorization server.
 **  +----------+
     |          |
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
void Spotify_Authorization(char *code)
{
    char receivedData[LONGBUF];
    strcpy(receivedData, code);
    if (xSemaphoreTake(Spotify_FindCodeSemaphore, portMAX_DELAY) == pdTRUE)
    {
        // 1- Read the code came from Spotify and received by https module
        ESP_LOGI(TAG, "CODE received by Queue: %s\n", receivedData);
        SendRequestAndGiveToken(code, sizeof(code), code, sizeof(code));

        // 2-send request for exchange code with access token 
        memset(receivedData, 0x0, sizeof(receivedData));
        // wait until get the response from Spotify
        ESP_LOGI(TAG, "waiting for spotify to response\n");

        if (xQueueReceive(*(InterfaceHandler.BufQueue), receivedData, SPOTIFY_RESPONSE_TIMEOUT) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received TOKEN by Queue: %s", receivedData);
            // 3-  extract json from RAW response
            Spotify_FindTokenInHttpsBody(receivedData, sizeof(receivedData));
            //  4-extract JSON and get param from needed parameter
            ExtractionJsonParamForFindAccessToken(receivedData, sizeof(receivedData), InterfaceHandler.token.access_token, InterfaceHandler.token.token_type, InterfaceHandler.token.refresh_token, InterfaceHandler.token.granted_scope, InterfaceHandler.token.expires_in_ms);
            SpotifyAuthorizationSuccessful = 1;
        }
        else
        {
            ESP_LOGE(TAG, "TIMEOUT - Spotify does not respond to the token request");
            esp_restart();
        }

    }
}


#include "SpotifyHttpLocalServer.h"
#include "SpotifyInterface.h"
#include "SpotifyTypedef.h"

static const char *TAG = "SpotifyTask";
// ******************************

QueueHandle_t SendCodeFromHttpToSpotifyTask = NULL;

/**
 * @brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was Found.
 * @param[in] Response The character array to search within, and Response is response from first stage from spotify athurisiation
 * @param[in] SizeRes The size of the character array.
 * @return Returns true if either the 'code' or 'state' pattern was Found, and false otherwise.
 */
bool Spotify_FindCode(char *Response, uint16_t SizeRes)
{
    char *codeString = {"code"};
    uint16_t codeLength = strlen(codeString);

    if (Response == NULL || SizeRes < codeLength)
    {
        // Invalid input, either null pointer or insufficient buffer size
        return false;
    }
    for (uint16_t i = 0; i <= SizeRes - codeLength; ++i)
    {
        bool Found = true;
        for (uint16_t j = 0; j < codeLength; ++j)
        {
            if (Response[i + j] != codeString[j])
            {
                Found = false;
                break;
            }
        }
        if (Found)
        {
            return true;    // Found the access token substring
        }
    }
    return false;           // Access token substring not Found
}

/**
 * @brief This function handles the first HTTPS request to Spotify and redirects the user to the authorization page.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t Spotify_RequestDataAccess(httpd_req_t *req)
{
    char *LocalURL;
    LocalURL = (char *)malloc((2 * SMALL_BUF) * sizeof(char));
    if (LocalURL == NULL)
    {
        ESP_LOGI(TAG, "Failed to allocate memory for the array.\n\n");
    }
    memset(LocalURL, 0x0, SMALL_BUF * 2);
    ESP_LOGI(TAG, "Starting authorization, sending request for TOKEN");
    sprintf(LocalURL, "http://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state", ClientId, ReDirectUri);
    httpd_resp_set_hdr(req, "Location", LocalURL);
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_status(req, "302");
    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    free(LocalURL);
    return ESP_OK;
}

/**
 * @brief This function handles the callback HTTPS request from Spotify and processes the response data.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t Spotify_HttpsCallbackHandler(httpd_req_t *req)
{
    char Buf[SMALL_BUF*2];
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        if (Spotify_FindCode(Buf, sizeof(Buf)) == true)
        {
            if (xQueueSend(SendCodeFromHttpToSpotifyTask, Buf,  pdMS_TO_TICKS(SEC*15)) != pdTRUE)
            {
                ESP_LOGE(TAG, "Sent data with queue failed!");
            }
            ESP_LOGI(TAG, "the CODE found in response");
        }
        else
        {
            ESP_LOGE(TAG, "response does not include code at the beginning ");
            return ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGW(TAG, "bad arguments - the response does not include correct structure");
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
 * this strcut is http URL handler if receive "/" Spotify_RequestDataAccess getting run
 */
static const httpd_uri_t Spotify_Request_Access_URI = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = Spotify_RequestDataAccess};

/**
 * this strcut is http URL handler if receive "/callback" HttpsUserCallBackFunc getting run
 */
static const httpd_uri_t Spotify_Response_Access_URI = {
    .uri = "/callback/",
    .method = HTTP_GET,
    .handler = Spotify_HttpsCallbackHandler};

/**
 * @brief This function starts the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
httpd_handle_t Spotify_StartWebServer()
{
    httpd_handle_t LocalServer = NULL;
    httpd_config_t Config = HTTPD_DEFAULT_CONFIG();
    Config.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting server on port: '%d'", Config.server_port);
    if (httpd_start(&LocalServer, &Config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(LocalServer, &Spotify_Request_Access_URI);
        httpd_register_uri_handler(LocalServer, &Spotify_Response_Access_URI);
        return LocalServer;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief This function stops the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
 esp_err_t Spotify_StopSpotifyWebServer(httpd_handle_t server)
{
    return httpd_stop(server);
}

/**
 * @brief This function starts the mDNS service.
 */
bool Spotify_StartMDNSService()
{
    esp_err_t err = mdns_init();
    if (err)
    {
        ESP_LOGE(TAG, "MDNS Init failed: %d", err);
        return false;
    }
    else
    {
        mdns_hostname_set("deskhub");
        mdns_instance_name_set("Spotify");
        return true;
    }
}

/**
 * @brief This function stops the mDNS service.
 */
void Spotify_StopMDNSService()
{
    mdns_free();
}

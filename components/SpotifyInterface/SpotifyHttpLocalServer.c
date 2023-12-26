#include "SpotifyHttpLocalServer.h"
static const char *TAG = "SpotifyTask";
HttpLocalServerParam_t HttpLocalServerLocalParam;

/**
 * @brief This checks if the application is initiated and connected to Spotify web service
 * @param SpotifyInterfaceHandler as the handler
 * @return true if everything is OK, false for ERROR
 */
// bool Spotify_IsConnected(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
// {
//     // TO DO: implement the function
//     return true;
// }

/**
 * @brief This function get and apply the command to Spotify service.
 * @param command could be play, pause, stop, next, previous, user_info, song_img, artist_img, etc.
 * @return true if function successfully sent the command to Spotify
 */
bool Spotify_PlaybackCommand(char *command)
{
    // TO DO: implement the function
    return true;
}

/**
 * @brief This function handles the first HTTPS request to Spotify and redirects the user to the authorization page.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t Spotify_RequestDataAccess(httpd_req_t *req)
{
    char *loc_url;
    loc_url = (char *)malloc((100+SMALLBUF) * sizeof(char));
    if (loc_url == NULL)
    {
        printf("Failed to allocate memory for the array.\n\n");
    }
    memset(loc_url, 0x0, SMALLBUF);
    if ((HttpLocalServerLocalParam.status == IDLE))
    {

        ESP_LOGI(TAG, "Starting authorization, sending request for TOKEN");
        sprintf(loc_url, "http://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state", ClientId, ReDirectUri);
        httpd_resp_set_hdr(req, "Location", loc_url);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, "302");
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    }
    else
    {
        ESP_LOGW(TAG, "Spotify is already initiated");
    }
    return ESP_OK;
}

/**
 * @brief This function handles the callback HTTPS request from Spotify and processes the response data.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t Spotify_HttpsCallbackHandler(httpd_req_t *req)
{
    char *Buf;
    Buf = (char *)malloc(MEDIUMBUF * sizeof(char));
    if (Buf == NULL)
    {
        printf("Failed to allocate memory for the array.\n\n");
    }
    memset(Buf, 0x0, MEDIUMBUF);
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        if (Spotify_FindCode(Buf, sizeof(Buf)) == true)
        {
            vTaskDelay(Sec / portTICK_PERIOD_MS);
            if (xQueueSend(*(HttpLocalServerLocalParam.HttpsBufQueue), Buf, 0) == pdTRUE)
            {
                printf("Sent data with queue \n");
            }
            ESP_LOGI(TAG, "we find CODE");
        }
        else
        {
            ESP_LOGE(TAG, "response does not include code at the beginning ");
            return ESP_FAIL;
        }
    }
    else
    {
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_500);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        ESP_LOGW(TAG, "bad arguments - the response does not include correct structure");
    }
    free(Buf);
    return ESP_OK;
}

/**
 * this strcut is http URL handler if receive "/" Spotify_Update_Info getting run
 */
// static const httpd_uri_t Spotify_Update_Info = {
//     .uri = "/spotify/IsConnected",
//     .method = HTTP_GET,
//     .handler = Spotify_IsConnected};

/**
 * this strcut is http URL handler if receive "/" Spotify_RequestDataAccess getting run
 */
static const httpd_uri_t Spotify_Request_Access_URI = {
    .uri = "/",
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
 * @brief Setup parameter for starting Http Local server
 * @param[in] HttpLocalServerParam_t HttpLocalServerParam_)
 */
void SetupHttpLocalServer(HttpLocalServerParam_t HttpLocalServerParam_)
{
    HttpLocalServerLocalParam.HttpsBufQueue = HttpLocalServerParam_.HttpsBufQueue;
    HttpLocalServerLocalParam.status = HttpLocalServerParam_.status;
}

/**
 * @brief This function starts the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
httpd_handle_t StartWebServer()
{
    httpd_handle_t localServer = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&localServer, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(localServer, &Spotify_Request_Access_URI);
        httpd_register_uri_handler(localServer, &Spotify_Get_Access_Response_URI);
        // httpd_register_uri_handler(localServer, &Spotify_Update_Info);
        return localServer;
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
bool StartMDNSService()
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

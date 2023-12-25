#include <stdio.h>
#include "SpotifyInterface.h"

// ****************************** Extern Variables
//  struct Token_t TokenParam;
//  struct UserInfo_t UserInfo;
extern QueueHandle_t BufQueue1;

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";
static const char *TAG_APP = "SPOTIFY";
SpotifyPrivateHandler_t PrivateHandler;
static SpotifyInterfaceHandler_t InterfaceHandler;
EventHandlerDataStruct_t EventHandlerData;
// QueueHandle_t Spotify_BufQueue;

// ****************************** Local Functions
static void Spotify_GetToken(char *code);
static bool Spotify_IsTokenValid(void);
static bool Spotify_SendCommand(Command_t *command);
static void Spotify_RenewAccessToken(Token_t *token);
static void Spotify_MainTask(void *pvparameters);

ESP_EVENT_DECLARE_BASE(BASE_SPOTIFY_EVENTS);
/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler, uint16_t SpotifyTaskStackSize)
{
    InterfaceHandler = *SpotifyInterfaceHandler;
    PrivateHandler.status = IDLE;
    PrivateHandler.command = NO_COMMAND;

    if (InterfaceHandler.CheckAddressInSpiffs != NULL &&
        InterfaceHandler.ReadTxtFileFromSpiffs != NULL &&
        InterfaceHandler.WriteTxtFileToSpiffs != NULL &&
        InterfaceHandler.ConfigAddressInSpiffs != NULL &&
        InterfaceHandler.HttpsResponseReadySemaphore != NULL &&
        InterfaceHandler.EventHandlerCallBackFunction != NULL &&
        InterfaceHandler.HttpsBufQueue != NULL)
    {
        xTaskCreate(&Spotify_MainTask, "Spotify_MainTask", SpotifyTaskStackSize, NULL, 9, NULL);
        ESP_LOGI(TAG, "Spotify app initiated successfully");
    }
    else
    {
        ESP_LOGE(TAG, "SpotifyIntefaceHandler is is missing some pointers, can not run the app");
        return false;
    }
    return true;
}

/**
 * @brief This checks if the application is initiated and connected to Spotify web service
 * @param SpotifyInterfaceHandler as the handler
 * @return true if everything is OK, false for ERROR
 */
bool Spotify_IsConnected(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
{
    // TO DO: implement the function
    return true;
}

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
    char loc_url[SMALLBUF + 150];
    if ((PrivateHandler.status == IDLE))
    {
        if (InterfaceHandler.CheckAddressInSpiffs(InterfaceHandler.ConfigAddressInSpiffs) == true)
        {
            char ReadBuf[MEDIUMBUF];
            InterfaceHandler.ReadTxtFileFromSpiffs(InterfaceHandler.ConfigAddressInSpiffs, ReadBuf, sizeof(ReadBuf));
            ESP_LOGI(TAG, "refresh token found on device");
            PrivateHandler.status = EXPIRED_USER;
        }
        else
        {
            ESP_LOGI(TAG, "Starting authorization, sending request for TOKEN");
            sprintf(loc_url, "http://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state", ClientId, ReDirectUri);
            httpd_resp_set_hdr(req, "Location", loc_url);
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_set_status(req, "302");
            httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
        }
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
    char Buf[MEDIUMBUF];
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_send(req, Buf, HTTPD_RESP_USE_STRLEN);
        if (Spotify_FindCode(Buf, sizeof(Buf)) == true)
        {
            vTaskDelay(Sec / portTICK_PERIOD_MS);
            PrivateHandler.code = malloc(MEDIUMBUF);
            if (PrivateHandler.code != NULL)
            {
                strncpy(PrivateHandler.code, Buf, MEDIUMBUF);
                PrivateHandler.status = AUTHORIZED;
                ESP_LOGI(TAG, "CODE copied in the Handler: %s\n", PrivateHandler.code);
            }
            else
            {
                ESP_LOGE(TAG, "Error in memory allocation for CODE");
                return ESP_FAIL;
            }
        }
        else
        {
            ESP_LOGE(TAG, "response does not include code at the begining ");
            return ESP_FAIL;
        }
    }
    else
    {
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
    .uri = "/spotify/IsConnected",
    .method = HTTP_GET,
    .handler = Spotify_IsConnected};

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
 * @brief This function starts the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
static httpd_handle_t StartWebServer(void)
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
        httpd_register_uri_handler(localServer, &Spotify_Update_Info);
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
        ESP_LOGE(TAG_APP, "MDNS Init failed: %d", err);
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
 * @brief This function is the entry point for handling HTTPS requests for Spotify authorization.
 * @param[in] pvparameters because it is a Task!
 * @return none
 */
static void Spotify_MainTask()
{
    static httpd_handle_t SpotifyLocalServer = NULL;
    // start the Local Server for Spotify
    StartMDNSService();
    SpotifyLocalServer = StartWebServer();
    if (SpotifyLocalServer != NULL)
    {
        ESP_LOGI(TAG, "** Spotify local server created! **");
    }
    else
    {
        ESP_LOGW(TAG, "Creating Spotify local server failed!");
    }

    while (1)
    {
        switch (PrivateHandler.status)
        {
        case IDLE:
        {
            // wait for user to ask for authentication
            break;
        }
        case AUTHORIZED:
        {
            Spotify_GetToken(PrivateHandler.code);
            break;
        }
        case ACTIVE_USER:
        {
            if (Spotify_IsTokenValid() != true)
            {
                PrivateHandler.status = EXPIRED_USER;
            }
            else
            {
                PrivateHandler.command = PLAY_NEXT;
                if (PrivateHandler.command != NO_COMMAND)
                {
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    if (Spotify_SendCommand(&PrivateHandler.command) == true)
                    {
                        ESP_LOGI(TAG, "Command succssefully applied");
                        // if (xQueueReceive(BufQueue1, buf, portMAX_DELAY) == pdTRUE)
                        // {
                        //     ESP_LOGI(TAG, "we receive https responce callback");
                        // }
                        vTaskDelay(pdMS_TO_TICKS(300000));
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Command not applied");
                    }
                    PrivateHandler.command = NO_COMMAND;
                }
            }
            break;
        }
        case EXPIRED_USER:
        {
            void Spotify_RenewAccessToken(Token_t * token);
        }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Retrieves the Spotify token based on the provided code.
 * This function sends a request to obtain the Spotify token and processes the response data.
 * @param code The code used to request the Spotify token.
 * @return None
 * */
void Spotify_GetToken(char *code)
{
    ESP_LOGI(TAG, "Spotify_GetToken RUN");
    char receivedData[LONGBUF];
    memset(receivedData, 0x0, sizeof(receivedData));
    Spotify_SendTokenRequest(receivedData, sizeof(receivedData), PrivateHandler.code, sizeof(PrivateHandler.code));
    if (xSemaphoreTake(InterfaceHandler.HttpsResponseReadySemaphore, portMAX_DELAY) == pdTRUE)
    {
        if (xQueueReceive(*(InterfaceHandler.HttpsBufQueue), receivedData, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received TOKEN by Queue: %s\n", receivedData);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Timeout, Spotify dont respond");
        return;
        // TO DO: the handler should reset here
    }
    // extract keys from JSON
    // if (ExtractJsonFromHttpResponse(receivedData, receivedData) == true)
    // {
    if (ExtractionJsonParamForFindAccessToken(receivedData, LONGBUF,
                                              PrivateHandler.token.access_token,
                                              PrivateHandler.token.token_type,
                                              PrivateHandler.token.refresh_token,
                                              PrivateHandler.token.granted_scope,
                                              PrivateHandler.token.expires_in_ms) == true)
    {
        PrivateHandler.status = ACTIVE_USER;
        ESP_LOGI(TAG, "ACTIVE_USER");
        EventHandlerData.EventHandlerCallBackFunction = InterfaceHandler.EventHandlerCallBackFunction;
        EventHandlerData.token = PrivateHandler.token;
        EventHandlerData.HttpsBufQueue = InterfaceHandler.HttpsBufQueue;
        Spotify_RegisterEventHandler();
    }
    else
    {
        PrivateHandler.status = IDLE;
        ESP_LOGE(TAG, "TOKEN extraction failed, back to IDLE state");
        // TO DO: the handler should reset here
    }
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "Something went wrong, response doesn't include JSON file");
    //     // TO DO: the handler should reset here
    // }
}

/**
 * @brief Renews the Spotify access token using the refresh token.
 * This function renews the Spotify access token by utilizing the provided refresh token.
 * The refreshed access token will be updated in the provided token structure.
 * @param token A pointer to the Token_t structure containing the refresh token.
 */
void Spotify_RenewAccessToken(Token_t *token)
{
    // TO DO: send request and get new access token, and save it on memory
}

/**
 * @brief
 * @param code T
 * @return true if valid, false if not valid
 * */
bool Spotify_IsTokenValid(void)
{
    // TO DO: check the time of last access_token update
    return true;
}
void func()
{
    ESP_LOGW("CallBackFunc", "bibibibib");
}

/**
 * @brief Sends a command to control Spotify.
 * This function sends various commands to control the Spotify application based on the given command value.
 * @param command A pointer to the command to be sent to Spotify.
 * @return true if the command is successfully sent, false otherwise.
 *
 * @note Possible command values include:
 * - NO_COMMAND: Waits for a command.
 * - PLAY: Sends the PLAY command to Spotify.
 * - PAUSE: Sends the PAUSE command to Spotify.
 * - PLAY_NEXT: Sends the PLAY_NEXT command to Spotify.
 * - PLAY_PREV: Sends the PLAY_PREV command to Spotify.
 * - GET_NOW_PLAYING: Sends the GET_NOW_PLAYING command to Spotify.
 * - GET_USER_INFO: Sends the GET_USER_INFO command to Spotify.
 * - GET_SONG_IMAGE_URL: Sends the GET_SONG_IMAGE_URL command to Spotify.
 * - GET_ARTIST_IMAGE_URL: Sends the GET_ARTIST_IMAGE_URL command to Spotify.
 */
bool Spotify_SendCommand(Command_t *command)
{
    bool retValue = true;
    int userCommand = *command;
    ESP_LOGI(TAG, "user Command is %d", userCommand);
    switch (userCommand)
    {
    case PLAY:
    {
        /* Send PLAY command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPlay_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case PAUSE:
    {
        /* Send PAUSE command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPause_, NULL, 0, portMAX_DELAY));
        break;
    }
    case PLAY_NEXT:
    {
        /* Send PLAY_NEXT command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case PLAY_PREV:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPrevious_, NULL, 0, portMAX_DELAY));
        /* Send PLAY_PREV command to Spotify */
        break;
    }
    case GET_NOW_PLAYING:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventGetCurrentPlaying_, NULL, 0, portMAX_DELAY));
        /* Send GET_NOW_PLAYING command to Spotify */
        break;
    }
    case GET_USER_INFO:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventGetUserStatus_, &func, sizeof(&func), portMAX_DELAY));
        /* Send GET_USER_INFO command to Spotify */
        break;
    }
    case GET_SONG_IMAGE_URL:
    {
        // ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext_, NULL, 0, portMAX_DELAY));
        /* Send GET_SONG_IMAGE_URL command to Spotify */
        break;
    }
    case GET_ARTIST_IMAGE_URL:
    {
        // ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext_, NULL, 0, portMAX_DELAY));
        /* Send GET_ARTIST_IMAGE_URL command to Spotify */
        break;
    }
    }

    return retValue;
}

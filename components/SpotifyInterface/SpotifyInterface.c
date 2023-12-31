#include <stdio.h>
#include "SpotifyInterface.h"

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

// ****************************** Local Functions
static void Spotify_GetToken(char *code, size_t SizeOfCode);
static bool Spotify_IsTokenValid(void);
static void Spotify_RenewAccessToken(Token_t *token);
static void Spotify_MainTask(void *pvparameters);

// ******************************
SpotifyPrivateHandler_t PrivateHandler;
SpotifyInterfaceHandler_t InterfaceHandler;
EventHandlerDataStruct_t EventHandlerData;
HttpLocalServerParam_t HttpLocalServerParam;
static httpd_handle_t SpotifyLocalServer = NULL;

// ******************************

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
    if (InterfaceHandler.CheckAddressInSpiffs != NULL &&
        InterfaceHandler.ReadTxtFileFromSpiffs != NULL &&
        InterfaceHandler.WriteTxtFileToSpiffs != NULL &&
        InterfaceHandler.ConfigAddressInSpiffs != NULL &&
        InterfaceHandler.HttpsResponseReadySemaphore != NULL &&
        InterfaceHandler.IsSpotifyAuthorizedSemaphore != NULL &&
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
 * @brief Run Http local service
 */
void HttpServerService()
{
    HttpLocalServerParam.HttpsBufQueue = InterfaceHandler.HttpsBufQueue;
    HttpLocalServerParam.status = &(PrivateHandler.status);
    SetupHttpLocalServer(HttpLocalServerParam);
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
}
/**
 * @brief This function is the entry point for handling HTTPS requests for Spotify authorization.
 * @param[in] parameters because it is a Task!
 */
static void Spotify_MainTask(void *pvparameters)
{
    HttpServerService();
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
            {
                char receiveData[LONG_BUF];
                if (xQueueReceive(*(InterfaceHandler.HttpsBufQueue), receiveData, portMAX_DELAY) == pdTRUE)
                {
                    ESP_LOGI(TAG, "Received TOKEN by Queue: %s\n", receiveData);
                }
                Spotify_GetToken(receiveData, sizeof(receiveData));
            }
            break;
        }
        case ACTIVE_USER:
        {
            xSemaphoreGive((*InterfaceHandler.IsSpotifyAuthorizedSemaphore));
            StopSpotifyWebServer(SpotifyLocalServer);
            SpotifyLocalServer = NULL;
            vTaskDelete(NULL);
            break;
        }
        case EXPIRED_USER:
        {
            void Spotify_RenewAccessToken(Token_t * token);
            break;
        }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Retrieves the Spotify token based on the provided code.
 * This function sends a request to obtain the Spotify token and processes the response data.
 * @param code The code used to request the Spotify token.
 * @param[in] size_t SizeOfCode , size of code buffer
 * @return None
 * */
static void Spotify_GetToken(char *code, size_t SizeOfCode)
{
    ESP_LOGI(TAG, "Spotify_GetToken RUN");
    char receivedData[LONG_BUF];
    memset(receivedData, 0x0, sizeof(receivedData));
    Spotify_SendTokenRequest(receivedData, sizeof(receivedData), code, SizeOfCode);
    if (xSemaphoreTake(*(InterfaceHandler.HttpsResponseReadySemaphore), portMAX_DELAY) == pdTRUE)
    {
        vTaskDelay(pdMS_TO_TICKS(SEC));
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
    if (ExtractJsonFromHttpResponse(receivedData, sizeof(receivedData)) == true)
    {
        if (ExtractionJsonParamForFindAccessToken(receivedData, LONG_BUF,
                                                  PrivateHandler.token.AccessToken,
                                                  PrivateHandler.token.TokenType,
                                                  PrivateHandler.token.RefreshToken,
                                                  PrivateHandler.token.GrantedScope,
                                                  PrivateHandler.token.ExpiresInMS) == true)
        {
            PrivateHandler.status = ACTIVE_USER;
            ESP_LOGI(TAG, "ACTIVE_USER");
            EventHandlerData.EventHandlerCallBackFunction = InterfaceHandler.EventHandlerCallBackFunction;
            EventHandlerData.token = &(PrivateHandler.token);
            EventHandlerData.HttpsBufQueue = InterfaceHandler.HttpsBufQueue;
            Spotify_RegisterEventHandler();
        }
        else
        {
            PrivateHandler.status = IDLE;
            ESP_LOGE(TAG, "TOKEN extraction failed, back to IDLE state");
            // TO DO: the handler should reset here
        }
    }
    else
    {
        ESP_LOGE(TAG, "Something went wrong, response doesn't include JSON file");
        // TO DO: the handler should reset here
    }
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
    // TO DO: check the time of last AccessToken update
    return true;
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
bool Spotify_SendCommand(int command)
{
    bool retValue = true;
    ESP_LOGI(TAG, "user Command is %d", command);
    if (PrivateHandler.status != ACTIVE_USER)
    {
        ESP_LOGE(TAG, "You are not authorized !");
        return false;
    }
    switch (command)
    {
    case Play:
    {
        /* Send PLAY command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPlay_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case Pause:
    {
        /* Send PAUSE command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPause_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case PlayNext:
    {
        /* Send PLAY_NEXT command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case PlayPrev:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPrevious_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send PLAY_PREV command to Spotify */
        break;
    }
    case GetNowPlaying:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventGetCurrentPlaying_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send GET_NOW_PLAYING command to Spotify */
        break;
    }
    case GetUserInfo:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventGetUserStatus_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send GET_USER_INFO command to Spotify */
        break;
    }
    case GetSongImageUrl:
    {
        // ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send GET_SONG_IMAGE_URL command to Spotify */
        break;
    }
    case GetArtisImageUrl:
    {
        // ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext_, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send GET_ARTIST_IMAGE_URL command to Spotify */
        break;
    }
    }

    return true;
}

#include <stdio.h>
#include "SpotifyInterface.h"
#include "SpiffsManger.h"
// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

// ****************************** Local Functions
static void Spotify_GetToken(char *code, size_t SizeOfCode);
static bool Spotify_IsTokenValid(void);
static void Spotify_RenewTokenWithRefreshToken(Token_t *token);
static void Spotify_MainTask(void *pvparameters);
static bool Spotify_TokenRenew(void);
static bool Spotify_ExpireTimeChecker(bool ExpireFLG);
bool Spotify_ValueOfVariables(char *ReceivedData, size_t SizeOfReceivedData);
// ******************************
SpotifyPrivateHandler_t PrivateHandler;
SpotifyInterfaceHandler_t *InterfaceHandler;
EventHandlerDataStruct_t EventHandlerData;
HttpLocalServerParam_t HttpLocalServerParam;
static httpd_handle_t SpotifyLocalServer = NULL;
QueueHandle_t SendCodeFromHttpToSpotifyTask = NULL;
bool SaveExistence = 0;
// ******************************

ESP_EVENT_DECLARE_BASE(BASE_SPOTIFY_EVENTS);

void Spotify_CheckRefreshTokenExistence()
{
    if (xSemaphoreTake(*(InterfaceHandler->WorkWithStorageInSpotifyComponentSemaphore), 1) == pdTRUE)
    {
        PrivateHandler.status = EXPIRED;
        SaveExistence = 1;
    }
}

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
{
    InterfaceHandler = SpotifyInterfaceHandler;
    PrivateHandler.status = IDLE;
    Spotify_CheckRefreshTokenExistence();
    if (InterfaceHandler->ConfigAddressInSpiffs != NULL &&
        InterfaceHandler->HttpsResponseReadySemaphore != NULL &&
        InterfaceHandler->IsSpotifyAuthorizedSemaphore != NULL &&
        InterfaceHandler->EventHandlerCallBackFunction != NULL &&
        InterfaceHandler->HttpsBufQueue != NULL)
    {
        StaticTask_t *xTaskBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
        StackType_t *xStack = (StackType_t *)malloc(SPOTIFY_TASK_STACK_SIZE * sizeof(StackType_t)); // Assuming a stack size of 400 words (adjust as needed)
        if (xTaskBuffer == NULL || xStack == NULL)
        {
            ESP_LOGE(TAG, "Memory allocation failed!\n");
            free(xTaskBuffer);
            free(xStack);
            return false; // Exit with an error code
        }
        xTaskCreateStatic(
            Spotify_MainTask,                    // Task function
            "Spotify_MainTask",                  // Task name (for debugging)
            SPOTIFY_TASK_STACK_SIZE,             // Stack size (in words)
            NULL,                                // Task parameters (passed to the task function)
            tskIDLE_PRIORITY + SPOTIFY_PRIORITY, // Task priority (adjust as needed)
            xStack,                              // Stack buffer
            xTaskBuffer                          // Task control block
        );
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
void Spotify_HttpServerServiceInit()
{
    SendCodeFromHttpToSpotifyTask = xQueueCreate(1, sizeof(char) * sizeof(char[MEDIUM_BUF]));
    HttpLocalServerParam.SendCodeFromHttpToSpotifyTask = &SendCodeFromHttpToSpotifyTask;
    HttpLocalServerParam.status = &(PrivateHandler.status);
    Spotify_SetupHttpLocalServer(HttpLocalServerParam);
    Spotify_StartMDNSService();
    SpotifyLocalServer = Spotify_StartWebServer();
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
    Spotify_HttpServerServiceInit();
    bool ExpireFLG = 1;
    while (1)
    {
        switch (PrivateHandler.status)
        {
        case IDLE:
        {
            // wait for user to ask for authentication
            break;
        }
        case AUTHENTICATED:
        {
            {
                ESP_LOGI(TAG, "AUTHENTICATED");
                char ReceiveData[LONG_BUF];
                if (xQueueReceive(SendCodeFromHttpToSpotifyTask, ReceiveData, pdMS_TO_TICKS(SEC * 15)) == pdTRUE)
                {
                    ESP_LOGI(TAG, "Received CODE by queue: %s\n", ReceiveData);
                }
                Spotify_GetToken(ReceiveData, sizeof(ReceiveData));
            }
            break;
        }
        case AUTHORIZED:
        {
            ESP_LOGI(TAG, "AUTHORIZED");
            xSemaphoreGive((*InterfaceHandler->IsSpotifyAuthorizedSemaphore));
            if (SaveExistence != 1)
            {
                Spotify_StopSpotifyWebServer(SpotifyLocalServer);
                SpotifyLocalServer = NULL;
                PrivateHandler.status = SAVE_NEW_TOKEN;
            }
            else
            {
                PrivateHandler.status = CHECK_TIME;
            }
            break;
        }
        case CHECK_TIME:
        {
            Spotify_ExpireTimeChecker(ExpireFLG);
            break;
        }
        case SAVE_NEW_TOKEN:
        {
            ESP_LOGI(TAG, "SAVE_NEW_TOKEN");
            SpiffsRemoveFile(InterfaceHandler->ConfigAddressInSpiffs);
            SaveFileInSpiffsWithTxtFormat(InterfaceHandler->ConfigAddressInSpiffs, "refresh_token", PrivateHandler.token.RefreshToken, NULL, NULL);
            PrivateHandler.status = CHECK_TIME;
            break;
        }
        case EXPIRED:
        {
            ESP_LOGW(TAG, "Expire");
            if (Spotify_TokenRenew() == true)
            {
                PrivateHandler.status = AUTHORIZED;
            }
            else
            {
                PrivateHandler.status = IDLE;
            }
            break;
        }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief This function check Time for
 * @param[in] bool ExpireFLG , this flag showing token is expire or not
 * @return ExpireFLG
 * */
static bool Spotify_ExpireTimeChecker(bool ExpireFLG)
{
    TickType_t StartTick = 0;
    TickType_t CurrentTick = 0;
    if (ExpireFLG)
    {
        StartTick = xTaskGetTickCount();
        ExpireFLG = 0;
        PrivateHandler.status = CHECK_TIME;
        return ExpireFLG;
    }
    else
    {
        CurrentTick = xTaskGetTickCount();
        uint32_t ElapsedTime = (CurrentTick - StartTick) * portTICK_PERIOD_MS;
        ElapsedTime = ElapsedTime / 1000;
        if (ElapsedTime > (HOUR - 300))
        {
            PrivateHandler.status = EXPIRED;
        }
        ESP_LOGE(TAG, "token getting expired !");

        return ExpireFLG;
    }
}

/**
 * @brief This function reads refresh token from spiffs and send request for new token
 * @return True if token received and saved, false for otherwise
 */
static bool Spotify_TokenRenew(void)
{
    char ReceivedData[LONG_BUF];
    ReadTxtFileFromSpiffs(InterfaceHandler->ConfigAddressInSpiffs, "refresh_token", ReceivedData, NULL, NULL);
    ESP_LOGI(TAG, "RefreshToken=%s", ReceivedData);
    SendRequest_ExchangeTokenWithRefreshToken(ReceivedData, sizeof(ReceivedData), ReceivedData);
    memset(ReceivedData, 0x0, LONG_BUF);
    if (xQueueReceive((*InterfaceHandler->HttpsBufQueue), ReceivedData, pdMS_TO_TICKS(SEC * 15)) == pdTRUE)
    {
        ESP_LOGI(TAG, "Token received by Queue: %s\n", ReceivedData);
        if (Spotify_ValueOfVariables(ReceivedData, sizeof(ReceivedData)) == 1)
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
        ESP_LOGW(TAG, "timeout - Spotify not respond!");
        return false;
    }
}

/**
 * @brief This function  Extracts JSON content from an HTTP response string
 *        and give value to privet handler variable , if all things was true , spotify event handler
 *        register
 * @return True if token received and saved, false for otherwise
 */
bool Spotify_ValueOfVariables(char *ReceivedData, size_t SizeOfReceivedData)
{
    // extract keys from JSON
    if (ExtractJsonFromHttpResponse(ReceivedData, SizeOfReceivedData) == true)
    {
        if (ExtractionJsonParamForFindAccessToken(ReceivedData, LONG_BUF,
                                                  PrivateHandler.token.AccessToken,
                                                  PrivateHandler.token.TokenType,
                                                  PrivateHandler.token.RefreshToken,
                                                  PrivateHandler.token.GrantedScope,
                                                  PrivateHandler.token.ExpiresInMS) == true)
        {
            EventHandlerData.EventHandlerCallBackFunction = InterfaceHandler->EventHandlerCallBackFunction;
            EventHandlerData.token = &(PrivateHandler.token);
            EventHandlerData.HttpsBufQueue = InterfaceHandler->HttpsBufQueue;
            Spotify_RegisterEventHandler();
            return true;
        }
        else
        {
            ESP_LOGE(TAG, "TOKEN extraction failed, back to IDLE state");
            return false;
            // TO DO: the handler should reset here
        }
    }
    else
    {
        ESP_LOGE(TAG, "Something went wrong, response doesn't include JSON file");
        return false;
        // TO DO: the handler should reset here
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
    char ReceivedData[LONG_BUF];
    memset(ReceivedData, 0x0, sizeof(ReceivedData));
    Spotify_SendTokenRequest(code, SizeOfCode);

    if (Spotify_ValueOfVariables(ReceivedData, sizeof(ReceivedData)) == 1)
    {
        ESP_LOGI(TAG, "Token found!");
        PrivateHandler.status = AUTHORIZED;
    }
    else
    {
        ESP_LOGW(TAG, "Token not found!");
        PrivateHandler.status = IDLE;
    }
}

/**
 * @brief
 * @param code
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
    if (PrivateHandler.status == IDLE || PrivateHandler.status == AUTHENTICATED)
    {
        ESP_LOGE(TAG, "You are not authorized !");
        return false;
    }
    switch (command)
    {
    case Play:
    {
        /* Send PLAY command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPlay, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case Pause:
    {
        /* Send PAUSE command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPause, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case PlayNext:
    {
        /* Send PLAY_NEXT command to Spotify */
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForNext, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        break;
    }
    case PlayPrev:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventSendRequestForPrevious, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send PLAY_PREV command to Spotify */
        break;
    }
    case GetNowPlaying:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventGetCurrentPlaying, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send GET_NOW_PLAYING command to Spotify */
        break;
    }
    case GetUserInfo:
    {
        ESP_ERROR_CHECK(esp_event_post_to(Spotify_EventLoopHandle, BASE_SPOTIFY_EVENTS, SpotifyEventGetUserStatus, &EventHandlerData, sizeof(EventHandlerDataStruct_t), portMAX_DELAY));
        /* Send GET_USER_INFO command to Spotify */
        break;
    }
    case GetSongImageUrl:
    {
        /* Send GET_SONG_IMAGE_URL command to Spotify */
        break;
    }
    case GetArtisImageUrl:
    {
        /* Send GET_ARTIST_IMAGE_URL command to Spotify */
        break;
    }
    }
    return true;
}

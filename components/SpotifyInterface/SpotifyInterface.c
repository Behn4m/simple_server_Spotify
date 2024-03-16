#include <stdio.h>
#include "SpotifyTypedef.h"
#include "SpotifyInterface.h"
#include "SpotifyAPICall.h"
#include "SpotifyHttpLocalServer.h"
#include "SpiffsManger.h"
#include "JsonExtraction.h"
#include "SpotifyTypedef.h"
#include "rtc.h"

// ****************************** Global Variables
SpotifyInterfaceHandler_t *InterfaceHandler;
SpotifyPrivateHandler_t PrivateHandler;

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

// ****************************** Local Functions
static void Spotify_MainTask(void *pvparameters);
static bool Spotify_TokenRenew(void);
static bool Spotify_IsTokenExpired();

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
{
    InterfaceHandler = SpotifyInterfaceHandler;
    InterfaceHandler->PlaybackInfo = (PlaybackInfo_t *)malloc(sizeof(PlaybackInfo_t));
    InterfaceHandler->UserInfo = (UserInfo_t *)malloc(sizeof(UserInfo_t));
    PrivateHandler.Status = INIT;
    if (InterfaceHandler->ConfigAddressInSpiffs != NULL &&
        InterfaceHandler->IsSpotifyAuthorizedSemaphore != NULL)
    {
        StaticTask_t *xTaskBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
        StackType_t *xStack = (StackType_t *)malloc(SPOTIFY_TASK_STACK_SIZE * sizeof(StackType_t));                 // Assuming a stack size of 400 words (adjust as needed)
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

        // Allocate buffer and Initialize the Spotify API call
        PrivateHandler.SpotifyBuffer.MessageBuffer = (char *)malloc(SUPER_BUF * sizeof(char));    
        PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag = xSemaphoreCreateBinary();    
        SpotifyAPICallInit(&PrivateHandler.SpotifyBuffer);

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
bool Spotify_HttpServerServiceInit()
{
    SendCodeFromHttpToSpotifyTask = xQueueCreate(1, sizeof(char) * sizeof(char[MEDIUM_BUF]));
    
    httpd_handle_t spotifyLocalServer = Spotify_StartWebServer();
    if (spotifyLocalServer == NULL)
    {
        ESP_LOGE(TAG, "Creating Spotify local server failed!");
        return false;
    }
    
    bool IsMdnsStarted = Spotify_StartMDNSService();
    if (!IsMdnsStarted)
    {
        ESP_LOGE(TAG, "Running mDNS failed!");
        return false;
    };

    ESP_LOGI(TAG, "** Spotify local server created, mDNS is running! **");
    return true;
}

/**
 * @brief Deinitiate the Spotify app
 */
void Spotify_TaskDeinit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
{
    if (SpotifyInterfaceHandler->PlaybackInfo != NULL)
    {
        free(SpotifyInterfaceHandler->PlaybackInfo);
    }
    if (SpotifyInterfaceHandler->UserInfo != NULL)
    {
        free(SpotifyInterfaceHandler->UserInfo);
    }
    if (PrivateHandler.SpotifyBuffer.MessageBuffer != NULL)
    {
        free(PrivateHandler.SpotifyBuffer.MessageBuffer);
    }
    if (PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag != NULL)
    {
        vSemaphoreDelete(PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag);
    }
    if (SendCodeFromHttpToSpotifyTask != NULL)
    {
        vQueueDelete(SendCodeFromHttpToSpotifyTask);
    }
    ESP_LOGI(TAG, "Spotify app deinitiated successfully");
}

/**
 * @brief This function is the entry point for handling HTTPS requests for Spotify authorization.
 * @param[in] parameters because it is a Task!
 */
static void Spotify_MainTask(void *pvparameters)
{
    while (1)
    {
        char receivedData[MEDIUM_BUF];
        switch (PrivateHandler.Status)
        {
            case INIT:
            {
                bool IsServerInit = Spotify_HttpServerServiceInit();                                                     // Init the webserver in initialization
                if (!IsServerInit)
                {
                    ESP_LOGE(TAG, "Authorization initialization failed!");
                    break;
                }

                bool IsSpiffExists = SpiffsExistenceCheck(InterfaceHandler->ConfigAddressInSpiffs);                      // Check if the refresh token exists in the spiffs
                if (!IsSpiffExists)
                {
                    ESP_LOGE(TAG, "Spotify autorization is needed");
                    PrivateHandler.Status = LOGIN;                                                              // If the refresh token does not exist, set the status to LOGIN 
                    break;
                }

                ESP_LOGI(TAG, "RefreshToken found");
                PrivateHandler.Status = EXPIRED;                                                            // If the refresh token exists, set the status to EXPIRED
                break;
            }
            case LOGIN:
            {
                bool IsCodeReceived = xQueueReceive(SendCodeFromHttpToSpotifyTask, receivedData, pdMS_TO_TICKS(SEC));       // Waiting for Code to be recieved by queue
                if (!IsCodeReceived)
                {
                    // stay in this state until receive the Code
                    break;
                }
                Spotify_SendTokenRequest(receivedData);                                                         // send request for Token
                PrivateHandler.Status = AUTHENTICATED;                                                          // Code received and checked, so update Status to AUTHENTICATED         
                break;
            }
            case AUTHENTICATED:
            {
                ESP_LOGI(TAG, "AUTHENTICATED");
                bool IsSpotifyResponded = xSemaphoreTake(PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag, pdMS_TO_TICKS(SEC));              // Waiting for Token to be recieved by queue
                if (!IsSpotifyResponded)
                {
                    PrivateHandler.Status = LOGIN;                                                                  // if the response did not come within the expected time, set Status back to LOGIN
                    ESP_LOGW(TAG, "Timeout - Spotify did not respond within the expected time.!");
                    break;
                }

                bool IsTokenExtracted = ExtractAccessTokenParamsTokenFromJson(PrivateHandler.SpotifyBuffer.MessageBuffer, 
                                                                              &PrivateHandler.token
                                                                              );                       // extract all keys from spotify server response
                if (!IsTokenExtracted)
                {
                    ESP_LOGW(TAG, "Token not found!");
                    PrivateHandler.Status = LOGIN;                                                               // the reponse did not include all needed keys, so set Status back to LOGIN
                    break;
                }

                ESP_LOGI(TAG, "Token found!");
                PrivateHandler.TokenLastUpdate = xTaskGetTickCount();                                       // Save the time when the token was received
                PrivateHandler.Status = AUTHORIZED;                                                         // Token recieved and checked, so update Status to AUTHORIZED
                break;
            }
            case AUTHORIZED:
            {
                ESP_LOGI(TAG, "AUTHORIZED");
                SpiffsRemoveFile(InterfaceHandler->ConfigAddressInSpiffs);                                          // Delete old value at the directory
                                                                                                                    // so applicaiton can know the Spotify Module is authorized 
                                                                                                                    // and ready for sending commands 
                SaveFileInSpiffsWithTxtFormat(InterfaceHandler->ConfigAddressInSpiffs,                              // Save new file in the directory
                                              "refresh_token", PrivateHandler.token.RefreshToken, 
                                              NULL, NULL);
                xSemaphoreGive((*InterfaceHandler->IsSpotifyAuthorizedSemaphore));                                  // give IsSpotifyAuthorizedSemaphore semaphore in the IntefaceHandler                
                PrivateHandler.Status = CHECK_TIME;                                                                 // set Status to CHECK_TIME     
                break;
            }
            case CHECK_TIME:
            {
                bool IsTokenExpired = Spotify_IsTokenExpired();                                                               // Check if the expiration time has elapsed since the last received token
                if (!IsTokenExpired)
                {
                    // keep state
                    break;
                }  
                PrivateHandler.Status = EXPIRED;                                                                // set Status to EXPIRED if token expired
          
                break;
            }
            case EXPIRED:
            {
                ESP_LOGW(TAG, "token is expired");
                bool IsTokenRenewed = Spotify_TokenRenew();
                if (!IsTokenRenewed)                                                                   // Run function to renew the token
                {
                    PrivateHandler.Status = LOGIN;
                    break;                                                                   // set Status to ILDLE if token renewed unsuccessfully
                }

                xSemaphoreGive((*InterfaceHandler->IsSpotifyAuthorizedSemaphore));                              // give IsSpotifyAuthorizedSemaphore semaphore in the IntefaceHandler
                PrivateHandler.TokenLastUpdate = xTaskGetTickCount();                                           // Save the time when the token was received
                PrivateHandler.Status = CHECK_TIME;                                                             // set Status to CHECK_TIME if token renewed successfully
                break;
            }
        }   
        vTaskDelay(pdMS_TO_TICKS(100));                                                                             // Task delay at the end of while(1) loop
    }
}

/**
 * @brief This function check Time for
 * @return true if token expired, false otherwise
 * */
static bool Spotify_IsTokenExpired()
{
    bool tokenExpired = false;
    uint32_t elapsedTime = (xTaskGetTickCount() - PrivateHandler.TokenLastUpdate) * portTICK_PERIOD_MS;             // Calculate the elapsed time since the last token was received
    elapsedTime = elapsedTime / 1000;                                                                               // Convert the elapsed time to seconds                                             
    if (elapsedTime > (HOUR - 300))
    {
        tokenExpired = true;  
        ESP_LOGW(TAG , "Spotify Token is Expired");                                                                 // If the elapsed time is greater than the expiration time, set tokenExpired to true
    }
    return tokenExpired;
}

/**
 * @brief This function reads refresh token from spiffs and send request for new token
 * @return True if token received and saved, false for otherwise
 */
static bool Spotify_TokenRenew(void)
{
    char receivedData[LONG_BUF];
    ReadTxtFileFromSpiffs(InterfaceHandler->ConfigAddressInSpiffs, "refresh_token", receivedData, NULL, NULL);
    SendRequest_ExchangeTokenWithRefreshToken(receivedData);
    memset(receivedData, 0x0, LONG_BUF);
    
    bool IsResponseReady = xSemaphoreTake(PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag, pdMS_TO_TICKS(SEC));
    if (!IsResponseReady)
    {
        ESP_LOGW(TAG, "timeout - Spotify not responded!");
        return false;
    }

    bool IsTokenExtracted = ExtractAccessTokenParamsTokenFromJson(PrivateHandler.SpotifyBuffer.MessageBuffer, &PrivateHandler.token);
    if (!IsTokenExtracted)
    {
        ESP_LOGW(TAG, "new Token not found!");
        return false;
    }

    ESP_LOGI(TAG, "new Token found!");
    PrivateHandler.TokenLastUpdate = xTaskGetTickCount();
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
bool Spotify_SendCommand(SpotifyInterfaceHandler_t SpotifyInterfaceHandler, int Command)
{
    bool retValue = false;

    ESP_LOGI(TAG, "user Command is %d", Command);
    if (PrivateHandler.Status == LOGIN || PrivateHandler.Status == AUTHENTICATED)
    {
        ESP_LOGE(TAG, "You are not authorized !");
        return false;
    }

    bool IsSuccessfull = false;
    switch (Command)
    {
        case Play:
        case Pause:
        case PlayNext:
        case PlayPrev:
            Spotify_ControlPlayback(Command, PrivateHandler.token.AccessToken);
            IsSuccessfull = PrivateHandler.SpotifyBuffer.status == 204;
            if(!IsSuccessfull)
            {
                ESP_LOGW(TAG, "Command is not sent successfully");
                retValue = false;
                break;
            }
            ESP_LOGI(TAG, "Command is sent successfully");
            retValue = true;
            break;
        case GetNowPlaying:
            Spotify_GetInfo(Command, PrivateHandler.token.AccessToken);
            IsSuccessfull = PrivateHandler.SpotifyBuffer.status == 200;
            if (!IsSuccessfull)
            {
                ESP_LOGW(TAG, "No song is found");
                retValue = false;
                break;
            }
            ExtractPlaybackInfoParamsfromJson(PrivateHandler.SpotifyBuffer.MessageBuffer, InterfaceHandler->PlaybackInfo);
            retValue = true;
            break;

        case GetUserInfo:
            Spotify_GetInfo(Command, PrivateHandler.token.AccessToken);
            IsSuccessfull = PrivateHandler.SpotifyBuffer.status == 200;
            if (!IsSuccessfull)
            {
                ESP_LOGW(TAG, "No user is found");
                retValue = false;
                break;
            }
            ExtractUserInfoParamsfromJson(PrivateHandler.SpotifyBuffer.MessageBuffer, InterfaceHandler->UserInfo);
            retValue = true;
            break;
        default:
            break;
    }
    return retValue;
}

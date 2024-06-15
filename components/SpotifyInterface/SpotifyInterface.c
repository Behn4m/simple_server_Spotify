#include <stdio.h>
#include "SpotifyInterface.h"
#include "authorization.h"
#include "OauthAPICall.h"
#include "OauthHttpLocalServer.h"
#include "JsonExtraction.h"
#include "rtc.h"

// ****************************** Global Variables
SpotifyInterfaceHandler_t *InterfaceHandler;
PrivateHandler_t PrivateHandler;

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

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

// /**
//  * @brief Deinitiate the Spotify app
//  */
// void Spotify_TaskDeinit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler)
// {
//     if (SpotifyInterfaceHandler->PlaybackInfo != NULL)
//     {
//         free(SpotifyInterfaceHandler->PlaybackInfo);
//     }
//     if (SpotifyInterfaceHandler->UserInfo != NULL)
//     {
//         free(SpotifyInterfaceHandler->UserInfo);
//     }
//     if (PrivateHandler.SpotifyBuffer.MessageBuffer != NULL)
//     {
//         free(PrivateHandler.SpotifyBuffer.MessageBuffer);
//     }
//     if (PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag != NULL)
//     {
//         vSemaphoreDelete(PrivateHandler.SpotifyBuffer.SpotifyResponseReadyFlag);
//     }
//     if (SendCodeFromHttpToSpotifyTask != NULL)
//     {
//         vQueueDelete(SendCodeFromHttpToSpotifyTask);
//     }
//     ESP_LOGI(TAG, "Spotify app deinitiated successfully");
// }

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

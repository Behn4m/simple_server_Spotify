#include <stdio.h>
#include "SpotifyInterface.h"
#include "JsonExtraction.h"
#include "rtc.h"

// ****************************** Global Variables
SpotifyPrivateHandler_t SpotifyPrivateHandler;

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

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

    SpotifyInterfaceHandler->PlaybackInfo = (PlaybackInfo_t *)malloc(sizeof(PlaybackInfo_t));
    SpotifyInterfaceHandler->UserInfo = (UserInfo_t *)malloc(sizeof(UserInfo_t));

    ESP_LOGI(TAG, "user Command is %d", Command);
    if (SpotifyPrivateHandler.Status == LOGIN || SpotifyPrivateHandler.Status == AUTHENTICATED)
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
            Spotify_ControlPlayback(Command, SpotifyPrivateHandler.token.AccessToken);
            IsSuccessfull = SpotifyPrivateHandler.SpotifyBuffer.status == 204;
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
            Spotify_GetInfo(Command, SpotifyPrivateHandler.token.AccessToken);
            IsSuccessfull = SpotifyPrivateHandler.SpotifyBuffer.status == 200;
            if (!IsSuccessfull)
            {
                ESP_LOGW(TAG, "No song is found");
                retValue = false;
                break;
            }
            ExtractPlaybackInfoParamsfromJson(SpotifyPrivateHandler.SpotifyBuffer.MessageBuffer, SpotifyInterfaceHandler->PlaybackInfo);
            retValue = true;
            break;

        case GetUserInfo:
            Spotify_GetInfo(Command, SpotifyPrivateHandler.token.AccessToken);
            IsSuccessfull = SpotifyPrivateHandler.SpotifyBuffer.status == 200;
            if (!IsSuccessfull)
            {
                ESP_LOGW(TAG, "No user is found");
                retValue = false;
                break;
            }
            ExtractUserInfoParamsfromJson(SpotifyPrivateHandler.SpotifyBuffer.MessageBuffer, SpotifyInterfaceHandler->UserInfo);
            retValue = true;
            break;
        default:
            break;
    }
    return retValue;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SpotifyTypedef.h"
#include "SpotifyInterface.h"
#include "JsonExtraction.h"
#include "SpotifyTypedef.h"
#include "rtc.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

// ****************************** Global Variables

APIBuffer_t *ServiceBuffer;
SpotifyInterfaceHandler_t *InterfaceHandler;
SpotifyPrivateHandler_t PrivateHandler;

// ****************************** Local Variables
static const char *TAG = "SpotifyTask";

/**
 * @brief Deinitiate the Spotify app
 */
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
* @brief This function sends a request to the Service API to give Curent playing information 
* @param[in]  Token is the access token received from Service during authorization progress 
* @param[in]  Command is the specific player command to be executed (e.g., "next", "previous", "play", "pause").
*/
static void Spotify_GetInfo(int Command, char *AccessToken)
{
    // fill clientPath and clientMethod variables based on the Service API document
    char clientPath[50] = {};
    if (Command == GetNowPlaying)
    {
        sprintf(clientPath, "/v1/me/player/currently-playing");
    }
    else if(Command == GetUserInfo)
    {
        sprintf(clientPath, "/v1/me");
    }
    else if(Command == GetUserTopItems)
    {
        sprintf(clientPath, "v1/me/top/artists");
    }
    else
    {
        ESP_LOGE(TAG, "Incorrect function call - incompatible command code for 'Spotify_GetInfo'");
    }

   // Configure client object  
    esp_http_client_config_t clientConfig = {
        .host = "api.spotify.com",                                                          // host for Service api call
        .path = clientPath,                                                                 // clientPath already filled based in API
        .method = HTTP_METHOD_GET,                                                          // Get for all of this category requests  
        .event_handler = HttpEventHandler,                                                  // Event handler function
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&clientConfig);                  // apply configuration to the client object
    if (httpClient == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create the HTTP client");
        return;
    }

    // Set headers for authentication
    char authorizationHeader[SMALL_BUF];
    snprintf(authorizationHeader, sizeof(authorizationHeader), "Bearer %s", AccessToken);
    esp_http_client_set_header(httpClient, "Authorization", authorizationHeader);               // authorization set based on Service API
    esp_http_client_set_header(httpClient, "Content-Length", "0");                              // thess requests are not going to send any data to the host

    // Perform the GET request
    esp_err_t err = esp_http_client_perform(httpClient) == ESP_OK;                                        // perform http request
    if (!err) 
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return;
    }
    SpotifyBuffer->status = esp_http_client_get_status_code(httpClient);
    SpotifyBuffer->ContentLength = esp_http_client_get_content_length(httpClient);
    ESP_LOGI(TAG, "HTTP GET Status = %lld, content_length = %lld",
            SpotifyBuffer->status,
            SpotifyBuffer->ContentLength);
    // Cleanup
    esp_http_client_cleanup(httpClient);                                                        // close all connection releated to this client object 
}

/**
* @brief This function sends a request to the Service API to perform a play/pause/next/previous commands.
* @param[in] Command_ The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[in] AccessToken given by authorization step.
* @return This function does not return a value.
*/
static void Spotify_ControlPlayback(int Command, char *AccessToken)
{
    // fill clientPath variables based on the Service API document
    char clientPath[30] = {};
    esp_http_client_method_t clientMethod = HTTP_METHOD_POST;
    if (Command == Play)
    {
        sprintf(clientPath, "/v1/me/player/play");
        clientMethod = HTTP_METHOD_PUT;
    }
    else if(Command == Pause)
    {
        sprintf(clientPath, "/v1/me/player/pause");
        clientMethod = HTTP_METHOD_PUT;
    }
    else if(Command == PlayNext)
    {
        sprintf(clientPath, "/v1/me/player/next");
        clientMethod = HTTP_METHOD_POST;
    }
    else if(Command == PlayPrev)
    {
        sprintf(clientPath, "/v1/me/player/previous");
        clientMethod = HTTP_METHOD_POST;
    }
    else
    {
        ESP_LOGE(TAG, "Incorrect function call - incompatible command code for 'Spotify_ControlPlayback'");
    }
    
    // Configure client object  
    esp_http_client_config_t clientConfig = {
        .host = "api.spotify.com",                                                              // host for Service api call
        .path = clientPath,                                                                     // clientPath already filled based in API
        .method = clientMethod,                                                                 // method variable already filled based on API 
        .event_handler = HttpEventHandler,                                                      // Event handler function
        .disable_auto_redirect = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&clientConfig);                  // apply configuration to the client object

    if (httpClient == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create the HTTP client");
        return;
    }

    // Set headers for authentication
    char authorizationHeader[SMALL_BUF];
    snprintf(authorizationHeader, sizeof(authorizationHeader), "Bearer %s", AccessToken);
    esp_http_client_set_header(httpClient, "Authorization", authorizationHeader);               // authorization set based on Service API
    esp_http_client_set_header(httpClient, "Content-Length", "0");                              // thess requests are not going to send any data to the host

    // Perform the GET request
    esp_err_t err = esp_http_client_perform(httpClient) == ESP_OK;                                        // perform http request
    if (!err) 
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return;
    }

    SpotifyBuffer->status = esp_http_client_get_status_code(httpClient);
    SpotifyBuffer->ContentLength = esp_http_client_get_content_length(httpClient);
    ESP_LOGI(TAG, "HTTP GET Status = %lld, content_length = %lld",
            SpotifyBuffer->status, SpotifyBuffer->ContentLength);
    // Cleanup
    esp_http_client_cleanup(httpClient);                                                        // close all connection releated to this client object 
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
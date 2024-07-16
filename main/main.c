#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "authorization.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"

#define TAG "MAIN"
#define TIMER_TIME pdMS_TO_TICKS(500) // in millis

// ****************************** GLobal Variables ****************************** //
//static const char *TAG = "Main";
OAuthInterfaceHandler_t OAuthInterfaceHandler;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;
SpotifyHttpInfo_t Spotify_ClientInfo;
// ****************************** GLobal Functions ****************************** //
/**
 * @brief Function to change colors based on a timer callback
 */
// void SpotifyPeriodicTimer(TimerHandle_t xTimer)
// {
//     bool CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying, 
//                          OAuthInterfaceHandler.Status, OAuthInterfaceHandler.token.AccessToken,
//                          OAuthInterfaceHandler.OAuthBuffer.status, 
//                          OAuthInterfaceHandler.OAuthBuffer.MessageBuffer);
//     if (CommandResult == false)
//     {
//         ESP_LOGE(TAG, "Playback info update failed");
//         return;
//     }
//     GUI_UpdateSpotifyScreen(SpotifyInterfaceHandler.PlaybackInfo->ArtistName,
//                             SpotifyInterfaceHandler.PlaybackInfo->SongName,
//                             SpotifyInterfaceHandler.PlaybackInfo->AlbumName,
//                             SpotifyInterfaceHandler.PlaybackInfo->Duration,
//                             SpotifyInterfaceHandler.PlaybackInfo->Progress);
//     ESP_LOGI(TAG, "Playback info updated");
// }

void app_main(void)//                          OAuthInterfaceHandler.OAuthBuffer.status, 
//                          OAuthInterfaceHandler.OAuthBuffer.MessageBuffer);
//     if (CommandResult == false)
//     {
//         ESP_LOGE(TAG, "Playback info update failed");
//         return;
//     }
//     GUI_UpdateSpotifyScreen(SpotifyInterfaceHandler.PlaybackInfo->ArtistName,
//                             SpotifyInterfaceHandler.PlaybackInfo->SongName,
//                             SpotifyInterfaceHandler.PlaybackInfo->AlbumName,
//                             SpotifyInterfaceHandler.PlaybackInfo->Duration,
//                             SpotifyInterfaceHandler.PlaybackInfo->Progress);
//     ESP_LOGI(TAG, "Playback info updated");
// }
{
    //GUI_TaskInit();
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    // WifiStationMode("Done_IOT_1", "87654321@");
    WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif

#ifdef SpotifyEnable

//TODO merge all in a static function
    OAuthInterfaceHandler.IsServiceAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    OAuthInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    Spotify_ClientInfo = Spotify_ClientConfigInit();
    OAuthInterfaceHandler.ClientConfig.url = Spotify_ClientInfo.url;
    OAuthInterfaceHandler.ClientConfig.host = Spotify_ClientInfo.host;
    OAuthInterfaceHandler.ClientConfig.path = Spotify_ClientInfo.path;
    OAuthInterfaceHandler.ClientConfig.requestURI = Spotify_ClientInfo.requestURI;
    OAuthInterfaceHandler.ClientConfig.responseURI = Spotify_ClientInfo.responseURI;
    OAuthInterfaceHandler.ClientConfig.hostname = Spotify_ClientInfo.hostname;
    OAuthInterfaceHandler.ClientConfig.requestURL = Spotify_ClientInfo.requestURL; 
    OAuthInterfaceHandler.ClientConfig.clientID = Spotify_ClientInfo.clientID;
    OAuthInterfaceHandler.ClientConfig.base64Credintials = Spotify_ClientInfo.base64Credintials;
    OAuthInterfaceHandler.ClientConfig.redirectURL = Spotify_ClientInfo.redirectURL;

    Oauth_TaskInit(&OAuthInterfaceHandler);

    //after this semaphore you can use playback command function in every where !
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGW(TAG, "Access Token: %s", Oauth_GetAccessToken());
    //     bool CommandResult = false;
    //     CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetUserInfo, 
    //                      OAuthInterfaceHandler.Status, OAuthInterfaceHandler.token.AccessToken,
    //                      OAuthInterfaceHandler.OAuthBuffer.status, 
    //                      OAuthInterfaceHandler.OAuthBuffer.MessageBuffer);
    //     if (CommandResult == false)
    //     {
    //         ESP_LOGE(TAG, "User info update failed");
    //         return;
    //     }
    //     ESP_LOGI(TAG, "User info updated");
    //     TimerHandle_t xTimer = xTimerCreate("update", TIMER_TIME, pdTRUE, NULL, SpotifyPeriodicTimer);
    //     xTimerStart(xTimer, 0);
    //     if (xTimer != NULL)
    //     {
    //         if (xTimerStart(xTimer, 0) == pdPASS)
    //         {
    //             ESP_LOGI(TAG, "Timer getting start");
    //         }
    //     }
    }
#endif
}

#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ****************************** GLobal Variables ****************************** //
static const char *TAG = "Main";

SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

// ****************************** GLobal Functions ****************************** //
/**
 * @brief Function to change colors based on a timer callback
 */
void SpotifyPeriodicTimer()
{
    bool CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
    if (CommandResult == false)
    {
        ESP_LOGE(TAG, "Playback info update failed");
        return;
    }
    LVGL_UpdateSpotifyScreen(SpotifyInterfaceHandler.PlaybackInfo->ArtistName,
                             SpotifyInterfaceHandler.PlaybackInfo->SongName,
                             SpotifyInterfaceHandler.PlaybackInfo->AlbumName);
    ESP_LOGI(TAG, "Playback info updated");
}

void app_main(void)
{
    LVGL_TaskInit();
    vTaskDelay(500);
    LVGL_UpdateSpotifyScreen("Mohammad", "javad", "Abbasi");
    GlobalInit();
    nvsFlashInit();
    // SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    // WifiStationMode("Hardware20", "87654321");
    // WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif

#ifdef SpotifyEnable

    // SpotifyInterfaceHandler.IsSpotifyAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    // SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    // Spotify_TaskInit(&SpotifyInterfaceHandler);

    // // after this semaphore you can use playback command function in every where !
    // if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    // {
    //     bool CommandResult = false;
    //     CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetUserInfo);
    //     if (CommandResult == false)
    //     {
    //         ESP_LOGE(TAG, "User info update failed");
    //         return;
    //     }

    //     ESP_LOGI(TAG, "User info updated");

    //     // Create and start a periodic timer to update Spotify screen info
    //     esp_timer_create_args_t periodic_timer_args = {.callback = SpotifyPeriodicTimer,
    //                                                    .name = "periodic_spotify"};
    //     esp_timer_handle_t periodic_timer;
    //     esp_timer_create(&periodic_timer_args, &periodic_timer);
    //     esp_timer_start_periodic(periodic_timer, 1000000);
    // }
#endif
}

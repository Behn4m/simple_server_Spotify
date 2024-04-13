#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define TIMER_TIME pdMS_TO_TICKS(500) // in millis

// ****************************** GLobal Variables ****************************** //
static const char *TAG = "Main";
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;
// ****************************** GLobal Functions ****************************** //
/**
 * @brief Function to change colors based on a timer callback
 */
void SpotifyPeriodicTimer(TimerHandle_t xTimer)
{
    static char imgLink[100];
    bool CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
    if (CommandResult == false)
    {
        ESP_LOGE(TAG, "Playback info update failed");
        return;
    }

    CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
    vTaskDelay(pdMS_TO_TICKS(1000));
    bool isNewSong = strcmp(SpotifyInterfaceHandler.PlaybackInfo->SongImageURL, &imgLink);
    if (isNewSong)
    {
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetCoverPhoto);
        vTaskDelay(pdMS_TO_TICKS(100));
        // if (CommandResult == false)
        // {
        //     ESP_LOGE(TAG, "Cover photo update failed");
        //     return;
        // }
        strcpy(imgLink, SpotifyInterfaceHandler.PlaybackInfo->SongImageURL);
    }

    GUI_UpdateSpotifyScreen(isNewSong,
                    SpotifyInterfaceHandler.PlaybackInfo->ArtistName,
                    SpotifyInterfaceHandler.PlaybackInfo->SongName,
                    SpotifyInterfaceHandler.PlaybackInfo->AlbumName,
                    SpotifyInterfaceHandler.PlaybackInfo->Duration,
                    SpotifyInterfaceHandler.PlaybackInfo->Progress,
                    SpotifyInterfaceHandler.CoverPhoto);
    ESP_LOGI(TAG, "Playback info updated");
}
void app_main(void)
{
    GUI_TaskInit();
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    // WifiStationMode("Hardware10", "87654321");
    WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif

#ifdef SpotifyEnable

    SpotifyInterfaceHandler.IsSpotifyAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    Spotify_TaskInit(&SpotifyInterfaceHandler);

    // after this semaphore you can use playback command function in every where !
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        bool CommandResult = false;
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetUserInfo);
        if (CommandResult == false)
        {
            ESP_LOGE(TAG, "User info update failed");
            return;
        }
        ESP_LOGI(TAG, "User info updated");

        TimerHandle_t xTimer = xTimerCreate("update", TIMER_TIME, pdTRUE, NULL, SpotifyPeriodicTimer);
        xTimerStart(xTimer, 0);
        if (xTimer != NULL)
        {
            if (xTimerStart(xTimer, 0) == pdPASS)
            {
                ESP_LOGI(TAG, "Timer getting start");
            }
        }
    }

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
#endif
}

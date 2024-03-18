#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Setup_GPIO.h"
#define TIMER_TIME pdMS_TO_TICKS(5000) // in millis

// ****************************** GLobal Variables ****************************** //
static const char *TAG = "Main";

SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

// ****************************** GLobal Functions ****************************** //
/**
 * @brief Function to change colors based on a timer callback
 */
void SpotifyPeriodicTimer(TimerHandle_t xTimer)
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
void IRAM_ATTR BackBottomCallBack(void *arg, void *data)
{
    ESP_LOGE(TAG, "Bottom callback");
}
void IRAM_ATTR AcceptBottomCallBack(void *arg, void *data)
{
    ESP_LOGE(TAG, "Bottom callback");
}
void app_main(void)
{
    LVGL_TaskInit();
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    WifiStationMode("Hardware", "87654321");
    // WifiStationMode("BELL789", "167271A164A9");
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
        BottomCallBackFunctions_t BottomCallBackFunctions;
        BottomCallBackFunctions.BackBottomCallBack;
        BottomCallBackFunctions.AcceptBottomCallBack;
    }
#endif
}

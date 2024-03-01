#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// ****************************** GLobal Variables ****************************** //
static const char *TAG = "Main";
QueueHandle_t BufQueue1 = NULL;

// ****************************** GLobal Functions ****************************** //
void CallbackTest(char *buffer)
{
    ESP_LOGW("Spotify_callback_test ", "%s", buffer);
}
void app_main(void)
{
    // LVGL_TaskInit();
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    // WifiStationMode("Hardware", "87654321");
    WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif
    
#ifdef SpotifyEnable
    SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

    SpotifyInterfaceHandler.HttpsBufQueue = &BufQueue1;
    SpotifyInterfaceHandler.IsSpotifyAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    Spotify_TaskInit(&SpotifyInterfaceHandler);
    // after this semaphore you can use playback command function in every where !
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        bool CommandResult = false;
        // int64_t BufferSize = 0;
        char *Buffer = NULL; // Initialize Buffer variable
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, Pause);

        vTaskDelay((pdMS_TO_TICKS(SEC * 3)));
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, Play);

        vTaskDelay((pdMS_TO_TICKS(SEC * 3)));
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, PlayPrev);

        vTaskDelay((pdMS_TO_TICKS(SEC * 3)));
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, PlayNext);
        
        vTaskDelay((pdMS_TO_TICKS(SEC * 3)));        
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetUserInfo);
        if (CommandResult == true)
        {
        }
        vTaskDelay((pdMS_TO_TICKS(SEC * 3)));
        
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
        if (CommandResult == true)
        {
            ESP_LOGI(TAG, "NowPlaying updated");
        }
    }
#endif
}
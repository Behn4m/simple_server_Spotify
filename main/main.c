#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// ****************************** GLobal Variables ****************************** //
QueueHandle_t BufQueue1 = NULL;
SemaphoreHandle_t HttpsResponseReadySemaphore = NULL;

// ****************************** GLobal Functions ****************************** //
void CallbackTest(char *buffer)
{
    ESP_LOGW("Spotify_callback_test ", "%s", buffer);
}
void app_main(void)
{
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    // WifiStationMode("Hardware10", "87654321");
    WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif
    // lvglGui();
#ifdef SpotifyEnable
    SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

    SpotifyInterfaceHandler.HttpsBufQueue = &BufQueue1;
    SpotifyInterfaceHandler.HttpsResponseReadySemaphore = &HttpsResponseReadySemaphore;
    SpotifyInterfaceHandler.IsSpotifyAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    SpotifyInterfaceHandler.WorkWithStorageInSpotifyComponentSemaphore = &WorkWithStorageInSpotifyComponentSemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    SpotifyInterfaceHandler.EventHandlerCallBackFunction = CallbackTest;
    Spotify_TaskInit(&SpotifyInterfaceHandler);
    // after this semaphore you can use playback command function in every where !
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        Spotify_SendCommand(Pause);
        vTaskDelay((pdMS_TO_TICKS(SEC * 5)));
        Spotify_SendCommand(Play);
        vTaskDelay((pdMS_TO_TICKS(SEC * 5)));
        Spotify_SendCommand(PlayPrev);
        vTaskDelay((pdMS_TO_TICKS(SEC * 5)));
        Spotify_SendCommand(PlayNext);
    }
#endif
}
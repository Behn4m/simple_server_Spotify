#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "HttpsRequests.h"
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
    HttpsRequestsHandler_t HttpsRequestsHandler; 

    HttpsRequestsHandler.ResponseBufQueue = &BufQueue1;
    HttpsRequestsHandler.ResponseReadySemaphore = &HttpsResponseReadySemaphore;
    Https_ComponentInit(&HttpsRequestsHandler);

    SpotifyInterfaceHandler.HttpsBufQueue = &BufQueue1;
    SpotifyInterfaceHandler.HttpsResponseReadySemaphore = &HttpsResponseReadySemaphore;
    SpotifyInterfaceHandler.IsSpotifyAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    SpotifyInterfaceHandler.WorkWithStorageInSpotifyComponentSemaphore = &WorkWithStorageInSpotifyComponentSemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    SpotifyInterfaceHandler.EventHandlerCallBackFunction = CallbackTest;
    Spotify_TaskInit(&SpotifyInterfaceHandler, SPOTIFY_TASK_STACK_SIZE);
    // after this semaphore you can use playback command function in every where !
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        Spotify_SendCommand(GetNowPlaying);
        vTaskDelay((pdMS_TO_TICKS(SEC * 10)));
        Spotify_SendCommand(Play);
        vTaskDelay((pdMS_TO_TICKS(SEC * 15)));
        Spotify_SendCommand(GetNowPlaying);
        vTaskDelay((pdMS_TO_TICKS(SEC * 15)));
        Spotify_SendCommand(GetUserInfo);
        vTaskDelay((pdMS_TO_TICKS(SEC * 15)));
        Spotify_SendCommand(Pause);
    }
#endif
}
#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// ****************************** GLobal Variables ****************************** //
QueueHandle_t BufQueue1;
SemaphoreHandle_t HttpsResponseReadySemaphore = NULL;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

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
    WifiStationMode("Hardware10", "87654321");
#else
    wifiConnectionModule();
#endif
    // lvglGui();

#ifdef SpotifyEnable
    SpotifyInterfaceHandler.HttpsBufQueue = &BufQueue1;
    SpotifyInterfaceHandler.HttpsResponseReadySemaphore = &HttpsResponseReadySemaphore;
    SpotifyInterfaceHandler.CheckSaveSemaphore = &IsSpotifyAuthorizedSemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    SpotifyInterfaceHandler.ReadTxtFileFromSpiffs = ReadTxtFileFromSpiffs;
    SpotifyInterfaceHandler.WriteTxtFileToSpiffs = SaveFileInSpiffsWithTxtFormat;
    SpotifyInterfaceHandler.CheckAddressInSpiffs = SpiffsExistenceCheck;
    SpotifyInterfaceHandler.EventHandlerCallBackFunction = CallbackTest;
    Spotify_TaskInit(&SpotifyInterfaceHandler, SPOTIFY_TASK_STACK_SIZE);
#endif
}

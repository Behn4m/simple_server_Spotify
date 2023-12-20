#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define configTICK_RATE_HZ 10*1000
// ****************************** GLobal Variables ****************************** //
QueueHandle_t BufQueue1;
SemaphoreHandle_t HttpsResponseReadySemaphore = NULL;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

// ****************************** GLobal Functions ****************************** //
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
    SpotifyInterfaceHandler.HttpsResponseReadySemaphore = HttpsResponseReadySemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    SpotifyInterfaceHandler.ReadTxtFileFromSpiffs = SpiffsWrite;
    SpotifyInterfaceHandler.WriteTxtFileToSpiffs = SaveFileInSpiffsWithTxtFormat;
    SpotifyInterfaceHandler.CheckAddressInSpiffs = SpiffsExistenceCheck;

    Spotify_TaskInit(&SpotifyInterfaceHandler, SPOTIFY_TASK_STACK_SIZE);
    vTaskDelay(10);
    unsigned int numberOfTasks = uxTaskGetNumberOfTasks();
    printf("Number of tasks: %u\n", numberOfTasks);
    printf("\n CONFIG_FREERTOS_HZ =%d",CONFIG_FREERTOS_HZ);
#endif
}
#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"

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
    WifiStationMode("BELL789","167271A164A9");
#else
    wifiConnectionModule();
#endif
    // lvglGui();

#ifdef SpotifyEnable
    SpotifyInterfaceHandler.HttpsBufQueue = &BufQueue1;
    SpotifyInterfaceHandler.HttpsResponseReadySemaphore = HttpsResponseReadySemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    SpotifyInterfaceHandler.ReadTxtFileFromSpiffs = &SpiffsWrite;
    SpotifyInterfaceHandler.WriteTxtFileToSpiffs = &SaveFileInSpiffsWithTxtFormat;
    SpotifyInterfaceHandler.CheckAddressInSpiffs = &SpiffsExistenceCheck;

    Spotify_TaskInit(&SpotifyInterfaceHandler);
#endif
}
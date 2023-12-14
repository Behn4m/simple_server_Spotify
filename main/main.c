#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"

// ****************************** GLobal Variables ****************************** //
QueueHandle_t BufQueue1;
SemaphoreHandle_t GetResponseSemaphore = NULL;
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
    SpotifyInterfaceHandler.BufQueue = &BufQueue1;
    Spotify_TaskInit(&SpotifyInterfaceHandler);
#endif
}
#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyAuthorization.h"
struct Token_ TokenParam;
struct UserInfo_ UserInfo;
QueueHandle_t BufQueue1;
SemaphoreHandle_t GetResponseSemaphore = NULL;


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
    Spotify_TaskInit();
#endif
}
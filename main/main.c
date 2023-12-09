#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyAuthorization.h"
struct Token_ TokenParam;
struct UserInfo_ UserInfo;
QueueHandle_t BufQueue1;
SemaphoreHandle_t GetResponseSemaphore = NULL;
SemaphoreHandle_t FinishWifiConfig = NULL;

void app_main(void)
{
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
    wifiConnectionModule();
    // lvglGui();
    
#ifdef SpotifyEnable
    SpotifyModule();
#endif
}
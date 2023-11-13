#include "lvglGui.h"
#include "main.h"
#include "nvsFlash.h"

struct Token_ TokenParam;
struct UserInfo_ UserInfo;

QueueHandle_t BufQueue1;
SemaphoreHandle_t GetResponseSemaphore = NULL;

char UserWifiPassWord[64];
char UserWifiSSID[32];
void app_main(void)
{
    GlobalInit();    
    nvsFlashInit();
    // lvglGui();
    // SpotifyComponent();

}
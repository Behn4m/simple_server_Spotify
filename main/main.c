

#include "lvglGui.h"

#include "main.h"
#include "nvsFlash.h"
struct Token_ TokenParam;
struct UserInfo_ UserInfo;

QueueHandle_t BufQueue1;
SemaphoreHandle_t GetResponseSemaphore = NULL;


// wifi ssid is "Hardware10"  and wifi pass is "87654321"


void app_main(void)
{
    GlobalInit();
    SpotifyComponent();
    // nvsFlashInit();
    // lvglGui();

}
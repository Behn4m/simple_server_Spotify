#include "lvglGui.h"
#include "main.h"
#include "nvsFlash.h"

struct Token_ TokenParam;
struct UserInfo_ UserInfo;

QueueHandle_t BufQueue1;
SemaphoreHandle_t GetResponseSemaphore = NULL;


void app_main(void)
{
    GlobalInit();    
    nvsFlashInit();
    // lvglGui();
    SpotifyComponent();

}
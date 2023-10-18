
#include "nvsFlash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvglGui.h"

void app_main(void)
{   
    nvsFlashInit();    

    lvglGui();
}
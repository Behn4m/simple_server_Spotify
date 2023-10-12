
#include "nvsFlash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

void app_main(void)
{   
    nvsFlashInit();    

    lv_init();

    lvgl_driver_init();
}
#include "lvglTimer.h"
static const char *TAG = "LVGL_timer";
/**
 * @brief timer handler for scheduling gui (for refreshing display,  we need it !)
 */
static void lv_tick_task(TimerHandle_t xTimer)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

/**
 * @brief Function to initialize and start LVGL timer ,
 * LVGL for working need a timer ,
 * timer used for making animation and refresh screen and ...
 */
void LVGL_Timer()
{
    TimerHandle_t xLVGLTimer = xTimerCreate("LVGLTimer", pdMS_TO_TICKS(20), pdTRUE, NULL, lv_tick_task);
    xTimerStart(xLVGLTimer, 0);
    if (xLVGLTimer != NULL)
    {
        if (xTimerStart(xLVGLTimer, 0) == pdPASS)
        {
            ESP_LOGI(TAG, "LVGLTimer getting start");
        }
    }
}
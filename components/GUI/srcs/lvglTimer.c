#include "lvglTimer.h"
static const char *TAG = "LVGL_timer";
#define TIMER_TIME pdMS_TO_TICKS(1000) // in millis
/**
 * @brief timer handler for scheduling gui (for refreshing display,  we need it !)
 */
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

/**
 * @brief Function to initialize and start LVGL timer
 */
void LVGL_Timer()
{
    // Create and start a periodic timer interrupt to call lv_tick_inc
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 20));
}


/**
 * @brief Function to change colors based on a timer callback
 */
void SongTimerUpdateCallback(TimerHandle_t xTimer)
{
    ESP_LOGI(TAG, "Timer getting interrupt");
}
void SongTimerUpdate()
{
    TimerHandle_t xTimer = xTimerCreate("SongTimerUpdate", TIMER_TIME, pdTRUE, NULL, SongTimerUpdateCallback);
    xTimerStart(xTimer, 0);
    if (xTimer != NULL)
    {
        if (xTimerStart(xTimer, 0) == pdPASS)
        {
            ESP_LOGI(TAG, "Timer getting start");
        }
    }
}
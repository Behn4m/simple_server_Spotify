#include"lvglTimer.h"
#include"esp_timer.h"
static const char *TAG = "LVGL_timer";

#define TIMER_TIME  pdMS_TO_TICKS(30) // in millis 

/**
 * @brief timer handler for scheduling gui (for refreshing display,  we need it !)
 */
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

/**
 * @brief Function to change colors based on a timer callback
 */
void GUITimerInterrupt(TimerHandle_t xTimer)
{
    lv_refr_now(NULL);
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

void UpdateSongTimer()
{
    
}
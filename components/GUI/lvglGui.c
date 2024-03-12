#include "lvglGui.h"
#include "image_test.h"
#include <inttypes.h>
#include "driver/gpio.h"
#include "esp_pm.h"
#include "iot_button.h"
#include "lvgl__lvgl/src/core/lv_obj.h"
#define MULTIPLIER 20

#define BUTTON_BACK GPIO_NUM_21
#define BUTTON_ACCEPT GPIO_NUM_13
#define BUTTON_UP GPIO_NUM_12
#define BUTTON_PAUSE GPIO_NUM_11

#define BOOT_BUTTON_NUM 21
#define BUTTON_ACTIVE_LEVEL 1

#define LV_TICK_PERIOD_MS 1
#define LVGL_STACK 2500
#define TIMER_CALLBACK_TIME 10 * 1000 /* in milliseconds */
lv_obj_t *Cont;
lv_obj_t *lvglButtomObject;
lv_obj_t *lvglLableUI3;
lv_obj_t *Label_UI5;

static const char *TAG = "LVGL_GUI";

static lv_disp_draw_buf_t disp_draw_buf;

lv_color_t *LVGL_BigBuf1;
lv_color_t *LVGL_BigBuf2;
int i = 0;
int k = 0;
/**
 * @brief timer handler for scheduling gui (for refreshing display we need it !)
 */
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void button_event_cb(void *arg, void *data)
{
    if (k == 1)
    {
        ESP_LOGE(TAG, "Bottom callback");
        i = i + 10;
        ESP_LOGW(TAG, "i=%d", i);
        if (i >= 100)
            i = 0;
        // lv_event_send(BarObject, LV_EVENT_ALL, (void *)&i);
        // lv_event_send(matterObject, LV_EVENT_ALL, (void *)&i);

        // lv_disp_load_scr(SpotifyPage);
        lv_obj_clean(lv_scr_act());
        // SpotifyPageFunc();
        // lv_obj_del(MenuPage);
        // vTaskDelay(1);

        // lv_disp_load_scr(SpotifyPage);
    }
}

void button_init(uint32_t button_num)
{
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = button_num,
            .active_level = BUTTON_ACTIVE_LEVEL,
        },
    };
    button_handle_t btn = iot_button_create(&btn_cfg);
    assert(btn);
    esp_err_t err = iot_button_register_cb(btn, BUTTON_PRESS_DOWN, button_event_cb, NULL);
    ESP_ERROR_CHECK(err);
}

void gpio_test()
{
    button_init(BUTTON_BACK);
    button_init(BUTTON_ACCEPT);
    button_init(BUTTON_UP);
    button_init(BUTTON_PAUSE);
}
/**
 * @brief Function to change colors based on a timer callback
 */
void GUITimerInterrupt(TimerHandle_t xTimer)
{
     ESP_LOGI(TAG, "Timer getting interrupt");

    // lv_obj_clean(lv_scr_act());
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
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    // Create and start a timer for color change
    TimerHandle_t xTimer = xTimerCreate("ColorTimer", pdMS_TO_TICKS(100000), pdTRUE, NULL, GUITimerInterrupt);
    xTimerStart(xTimer, 0);
    if (xTimer != NULL)
    {
        if (xTimerStart(xTimer, 0) == pdPASS)
        {
            ESP_LOGI(TAG, "Timer getting start");
        }
    }
}

/**
 * @brief main LVGL gui TASK
 */
static void LVGL_mainTask(void *pvParameter)
{
    // Allocate memory for LVGL display buffers
    lv_color_t *LVGL_BigBuf1 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * MULTIPLIER * sizeof(lv_color_t));
    lv_color_t *LVGL_BigBuf2 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * MULTIPLIER * sizeof(lv_color_t));

    // Check memory allocation
    if (LVGL_BigBuf2 == NULL || LVGL_BigBuf2 == NULL)
    {
        ESP_LOGE("TAG", "Memory allocation failed!");
        free(LVGL_BigBuf2);
        free(LVGL_BigBuf2);
        vTaskDelete(NULL);
    }
    // Initialize LVGL and display driver
    lv_init();
    lvgl_driver_init();

    // Initialize display buffer
    lv_disp_draw_buf_init(&disp_draw_buf, LVGL_BigBuf1, LVGL_BigBuf2, LV_HOR_RES_MAX * 100);

    // Initialize display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_draw_buf;
    lv_disp_drv_register(&disp_drv);
    gpio_test();
    // Start LVGL timer and create UI
    LVGL_Timer();
    // MainMenu();
    k = 1;
    myMenu();
    // LV_UI2();
    // LV_UI3();
    // RailBar();
    // SpotifyPage();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        lv_task_handler();
    }
}

/**
 * @brief Function to initialize LVGL task
 */
void LVGL_TaskInit(void)
{
    StaticTask_t *xTaskLVGLBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
    StackType_t *xLVGLStack = (StackType_t *)malloc(LVGL_STACK * 8 * MULTIPLIER * sizeof(StackType_t));
    if (xTaskLVGLBuffer == NULL || xLVGLStack == NULL)
    {
        ESP_LOGE("TAG", "Memory allocation failed!");
        free(xTaskLVGLBuffer);
        free(xLVGLStack);
        return; // Exit with an error code
    }
    xTaskCreateStatic(
        LVGL_mainTask,               // Task function
        "LVGL_mainTask",             // Task name (for debugging)
        LVGL_STACK * 8 * MULTIPLIER, // Stack size (in words)
        NULL,                        // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + 1,        // Task priority (adjust as needed)
        xLVGLStack,                  // Stack buffer
        xTaskLVGLBuffer              // Task control block
    );
    // this delay so important
    vTaskDelay(500);
}

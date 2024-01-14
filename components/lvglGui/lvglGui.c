#include <stdio.h>
#include "lvglGui.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include "lvgl.h"
#include "lvgl_helpers.h"
#include "rm67162Lilygo.h"
#include "rm67162.h"

#include "lv_demo.h"

#define LV_TICK_PERIOD_MS 1

// #include "lv_examples.h"

/**
 * @brief       timer handler for scheduling gui
 * @param[in]   nothing
 * @return nothing
 */
static void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void create_demo_application(void)
{
    // lv_obj_t *label1 = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    // lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    // lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
    //                   "and wrap long text automatically.");
    // lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    // lv_obj_t *label2 = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    // lv_obj_set_width(label2, 150);
    // lv_label_set_text(label2, "It is a circularly scrolling text. ");
    // lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);

    lv_demo_music();
}

/**
 * @brief      global display buffer and its semaphore
 */
static lv_disp_draw_buf_t disp_draw_buf;
lv_color_t *buf1;
lv_color_t *buf2;

SemaphoreHandle_t xGuiSemaphore;

/**
 * @brief       main lvgl gui implementation
 * @param[in]   pvParameter not importatn
 * @return nothing
 */
static void guiTask(void *pvParameter)
{

    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_color_t *buf1 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));
    lv_color_t *buf2 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));
    lv_init();

    lvgl_driver_init();

    lv_disp_draw_buf_init(&disp_draw_buf, buf1, buf2, LV_HOR_RES_MAX * 100); // Lilygo

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_draw_buf;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    lv_theme_t *theme = lv_theme_default_init(disp,
                                              lv_palette_main(LV_PALETTE_BLUE),
                                              lv_palette_main(LV_PALETTE_RED),
                                              LV_THEME_DEFAULT_DARK,
                                              LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, theme);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    create_demo_application();

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    /* A task should NEVER return */
    vTaskDelete(NULL);
}

void lvglGui(void)
{
    unsigned int freeHeapSize;
    freeHeapSize = xPortGetFreeHeapSize();
    printf("before Free Heap Size: %u bytes\n", freeHeapSize);
    StaticTask_t *xTaskBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
    StackType_t *xStack = (StackType_t *)malloc(2000 * 8 * sizeof(StackType_t)); // Assuming a stack size of 400 words (adjust as needed)
    if (xTaskBuffer == NULL || xStack == NULL)
    {
        // ESP_LOGE("TAG", "Memory allocation failed!\n");
        free(xTaskBuffer);
        free(xStack);
        return; // Exit with an error code
    }
    xTaskCreateStatic(
        guiTask,              // Task function
        "guiTask",            // Task name (for debugging)
        2000 * 8,             // Stack size (in words)
        NULL,                 // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + 4, // Task priority (adjust as needed)
        xStack,               // Stack buffer
        xTaskBuffer           // Task control block
    );
    freeHeapSize = xPortGetFreeHeapSize();
    printf("before Free Heap Size: %u bytes\n", freeHeapSize);
    // ESP_LOGI("TAG", "LVGL app initiated successfully");

    // TaskHandle_t xHandle = NULL;

    // xTaskCreate(guiTask, "guiTask", 2000*8, NULL, tskIDLE_PRIORITY, &xHandle);
    // configASSERT(xHandle);

    // if(xHandle == NULL)
    //     vTaskDelete(xHandle);
}
#include <stdio.h>
#include "lvglGui.h"
#include "styles/lv_example_style.h"
#include "get_started/lv_example_get_started.h"
#include "widgets/lv_example_widgets.h"
#include "layouts/lv_example_layout.h"
#include "scroll/lv_example_scroll.h"
#include "anim/lv_example_anim.h"
#include "event/lv_example_event.h"
#include "styles/lv_example_style.h"
#include "others/lv_example_others.h"
#include "libs/lv_example_libs.h"
#include "lvgl.h"
#include "lvgl/src/font/lv_font.h"
#include "image_test.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define LV_TICK_PERIOD_MS 1
#define LVGL_STACK 2500
#define TIMER_CALLBACK_TIME 10 * 1000 /* in milliseconds */

static const char *TAG = "LVGL_GUI";

static lv_disp_draw_buf_t disp_draw_buf;
static lv_style_t TitleBox;
static lv_style_t MusicBox;
lv_color_t *LVGL_BigBuf1;
lv_color_t *LVGL_BigBuf2;

/**
 * @brief       timer handler for scheduling gui (for refreshing display we need it !)
 */
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

// void changeColors()
void changeColors(TimerHandle_t xTimer)
{
    lv_color_t musicBoxColor;
    musicBoxColor.full = 0x39e9;
    lv_color_t titleBoxColor = lv_color_make(0x0, 0x0, 0x00);
    lv_style_set_bg_color(&MusicBox, musicBoxColor);
    lv_style_set_bg_color(&TitleBox, titleBoxColor);
}

/*
 * * * * * * *  * * * * *
 *                      *
 * * * * * * *  * * * * *
 *                      *
 * * * * * * *  * * * * *
 *                      *
 * * * * * * *  * * * * *
 */
static void LVGL_MyUI(void)
{
    /*A base style*/
    lv_style_init(&MusicBox);
    lv_style_set_bg_color(&MusicBox, lv_color_make(17, 39, 28));
    lv_style_set_text_color(&MusicBox, lv_color_make(0, 0, 0));
    lv_style_set_width(&MusicBox, 100);
    lv_style_set_height(&MusicBox, LV_SIZE_CONTENT);

    /*Set only the properties that should be different*/
    lv_style_init(&TitleBox);
    lv_style_set_bg_color(&TitleBox, lv_color_make(0xff, 0x0, 0x00));
    lv_style_set_text_color(&TitleBox, lv_color_make(0xff, 0x0, 0x00));
    lv_style_set_text_font(&TitleBox, &lv_font_montserrat_18); // Set the font

    /*Create an object with the base style only*/
    lv_obj_t *obj_base = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_base, &MusicBox, 0);
    lv_obj_align(obj_base, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(obj_base, LV_HOR_RES, LV_VER_RES / 2); // Set size to cover entire horizontal, half vertical

    /*graphical object used for displaying text*/
    lv_obj_t *label = lv_label_create(obj_base);
    lv_obj_add_style(label, label, &TitleBox);
    lv_style_set_text_font(&MusicBox, &lv_font_montserrat_18); // Set the font
    lv_label_set_text(label, "");

    /*Create another object with the base style and earnings style too*/
    lv_obj_t *obj_txt = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_txt, &MusicBox, 0);
    lv_obj_add_style(obj_txt, &TitleBox, 0);
    lv_obj_align(obj_txt, LV_ALIGN_BOTTOM_MID, 0, -100);  //  shift it in Y axis
    lv_obj_set_size(obj_txt, LV_HOR_RES, LV_VER_RES / 4); // Set size to cover entire horizontal, 0.25 vertical

    static lv_style_t circular;
    lv_style_init(&circular);
    lv_style_set_bg_color(&circular, lv_color_make(0xff, 0x0, 0x00));
    lv_style_set_text_color(&circular, lv_color_white());
    lv_style_set_text_font(&circular, &lv_font_montserrat_18); // Set the font

    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_obj_add_style(label2, &circular, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);

    /* show a picture of music bottom*/
    lv_obj_t *img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &music);
    lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, -10);
    // lv_demo_music();
}
void LVGL_HardwareTimer()
{
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    TimerHandle_t xTimer = xTimerCreate("ColorTimer", pdMS_TO_TICKS(10000), pdTRUE, NULL, changeColors);
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
static void GUI_mainTask(void *pvParameter)
{

    lv_color_t *LVGL_BigBuf1 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));
    lv_color_t *LVGL_BigBuf2 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));
    if (LVGL_BigBuf2 == NULL || LVGL_BigBuf2 == NULL)
    {
        ESP_LOGE("TAG", "Memory allocation failed!");
        free(LVGL_BigBuf2);
        free(LVGL_BigBuf2);
        vTaskDelete(NULL);
    }
    lv_init();
    lvgl_driver_init();
    lv_disp_draw_buf_init(&disp_draw_buf, LVGL_BigBuf1, LVGL_BigBuf2, LV_HOR_RES_MAX * 100); // Lilygo
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_draw_buf;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    LVGL_HardwareTimer();
    LVGL_MyUI();
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 1ms )*/
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
}

void LVGL_TaskInit(void)
{
    StaticTask_t *xTaskLVGLBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
    StackType_t *xLVGLStack = (StackType_t *)malloc(LVGL_STACK * 8 * sizeof(StackType_t));
    if (xTaskLVGLBuffer == NULL || xLVGLStack == NULL)
    {
        ESP_LOGE("TAG", "Memory allocation failed!");
        free(xTaskLVGLBuffer);
        free(xLVGLStack);
        return; // Exit with an error code
    }
    xTaskCreateStatic(
        GUI_mainTask,         // Task function
        "GUI_mainTask",       // Task name (for debugging)
        LVGL_STACK * 8,       // Stack size (in words)
        NULL,                 // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + 1, // Task priority (adjust as needed)
        xLVGLStack,           // Stack buffer
        xTaskLVGLBuffer       // Task control block
    );
}

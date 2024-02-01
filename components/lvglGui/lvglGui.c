#include <stdio.h>
#include "lvglGui.h"

#define LV_TICK_PERIOD_MS 1

static const char *TAG = "LVGL_GUI";
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

// #define LV_FONT_MONTSERRAT_14
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
<<<<<<< HEAD

=======
uint16_t ColorConvertor(uint16_t InputColor)
{

}
<<<<<<< HEAD
>>>>>>> parent of d5b92c0 (make map for color)
=======
>>>>>>> parent of d5b92c0 (make map for color)
void lv_example_style_11111(void)
{
    /*A base style*/
    static lv_style_t style_base;
    lv_style_init(&style_base);
    lv_style_set_bg_color(&style_base, lv_color_make(0x00, 0x0, 0xff));
    lv_style_set_text_color(&style_base, lv_color_white());
    lv_style_set_width(&style_base, 100);
    lv_style_set_height(&style_base, LV_SIZE_CONTENT);

    /*Set only the properties that should be different*/
    static lv_style_t style_warning;
    lv_style_init(&style_warning);
    lv_style_set_bg_color(&style_warning, lv_color_make(0xff, 0x0, 0x00));
    // lv_style_set_text_color(&style_warning, lv_color_white());

    /*Create an object with the base style only*/
    lv_obj_t *obj_base = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_base, &style_base, 0);
    lv_obj_align(obj_base, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(obj_base, LV_HOR_RES, LV_VER_RES /2); // Set size to cover entire horizontal, half vertic
    lv_obj_t *label = lv_label_create(obj_base);

    // lv_style_set_text_font(&style_warning, &lv_font_montserrat_18); // Set the font
    lv_style_set_text_font(&style_base, &lv_font_montserrat_18);    // Set the font

    // lv_obj_add_style(label, label, &style_warning);

    // lv_label_set_text(label, "Shadmehr");
    // lv_obj_center(label);

    /*Create another object with the base style and earnings style too*/
    lv_obj_t *obj_warning = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj_warning, &style_base, 0);
    lv_obj_add_style(obj_warning, &style_warning, 0);
    lv_obj_align(obj_warning, LV_ALIGN_BOTTOM_MID, 0, -100);
    lv_obj_set_size(obj_warning, LV_HOR_RES, LV_VER_RES / 4); // Set size to cover entire horizontal, half vertical

    label = lv_label_create(obj_warning);
    lv_label_set_text(label, "Tardid");
    lv_obj_center(label);


    lv_obj_t *img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &music);
    lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, 0);

}
static void create_demo_application(void)
{
    // lv_obj_t *label1 = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    // lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    // lv_label_set_text(label1, "#0000ff Shadmehr Aghili #"
    //                   "#ff00ff Tardid#");
    // lv_obj_set_width(label1, 1000);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    // lv_obj_t *label2 = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    // lv_obj_set_width(label2, 500);
    // lv_label_set_text(label2, "It is a circularly scrolling text. ");
    // lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
    // lv_example_style_11111();
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

    // lv_theme_t *theme = lv_theme_default_init(disp,
    //                                           lv_palette_main(LV_PALETTE_BLUE),
    //                                           lv_palette_main(LV_PALETTE_RED),
    //                                           LV_THEME_DEFAULT_DARK,
    //                                           LV_FONT_DEFAULT);
    // lv_disp_set_theme(disp, theme);

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
    size_t free_heap = esp_get_free_heap_size();
    ESP_LOGW(TAG, "Free external Heap Size: %d bytes\n", free_heap);
    ESP_LOGW(TAG, " Free Local Heap Size: %u bytes\n", freeHeapSize);
    StaticTask_t *xTaskLVgLBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
    StackType_t *xLVGLStack = (StackType_t *)malloc(2000 * 8 * sizeof(StackType_t)); // Assuming a stack size of 400 words (adjust as needed)
    if (xTaskLVgLBuffer == NULL || xLVGLStack == NULL)
    {
        // ESP_LOGE("TAG", "Memory allocation failed!\n");
        free(xTaskLVgLBuffer);
        free(xLVGLStack);
        return; // Exit with an error code
    }
    xTaskCreateStatic(
        guiTask,              // Task function
        "guiTask",            // Task name (for debugging)
        2000 * 8,             // Stack size (in words)
        NULL,                 // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + 4, // Task priority (adjust as needed)
        xLVGLStack,           // Stack buffer
        xTaskLVgLBuffer       // Task control block
    );
    free_heap = esp_get_free_heap_size();
    ESP_LOGW(TAG, "Free external Heap Size: %d bytes\n", free_heap);
}
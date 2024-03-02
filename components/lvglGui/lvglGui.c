#include "lvglGui.h"
#include "image_test.h"
#include <inttypes.h>
#include "driver/gpio.h"
#include "esp_pm.h"
#include "iot_button.h"
#include "lvgl__lvgl/src/core/lv_obj.h"

#define BUTTON_UP_GPIO GPIO_NUM_21
#define BOOT_BUTTON_NUM 0
#define BUTTON_ACTIVE_LEVEL 0

#define LV_TICK_PERIOD_MS 1
#define LVGL_STACK 2500
#define TIMER_CALLBACK_TIME 10 * 1000 /* in milliseconds */
lv_obj_t *Cont;
lv_obj_t *lvglButtomObject;
lv_obj_t *lvglLableUI3;
lv_obj_t *Label_UI5;

static const char *TAG = "LVGL_GUI";

static lv_disp_draw_buf_t disp_draw_buf;
static lv_style_t TitleBox;
static lv_style_t MusicBox;
static lv_style_t PanelStyle;
lv_color_t *LVGL_BigBuf1;
lv_color_t *LVGL_BigBuf2;
static void scroll_event_cb(lv_event_t *e);
int i =0;
lv_obj_t *SwitchObject;
lv_obj_t *OneclickLable;
lv_obj_t *BigpalnelScrollObject;
static void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}
lv_obj_t *BarObject;
static void IRAM_ATTR button_event_cb(void *arg, void *data)
{
    ESP_LOGE(TAG, "Buttom callback");

    // if (lv_obj_has_state(SwitchObject, LV_STATE_CHECKED))
    // {
    //     lv_obj_clear_state(SwitchObject, LV_STATE_CHECKED);
    // }
    // else
    // {
    //     lv_obj_add_state(SwitchObject, LV_STATE_CHECKED);
    // }
    // lv_event_send(SwitchObject, LV_EVENT_VALUE_CHANGED, NULL);

    // lv_obj_add_state(BigpalnelScrollObject, LV_STATE_SCROLLED);
    // lv_event_send(BigpalnelScrollObject, LV_EVENT_VALUE_CHANGED, NULL);
    i = i +10;
    ESP_LOGW(TAG, "i=%d", i);
    if (i >= 100)
        i = 1;
    lv_event_send(BarObject, LV_EVENT_ALL, (void *)i);
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
    button_init(BOOT_BUTTON_NUM);
}

#if LV_USE_FLEX

static void scroll_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for (i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if (diff_y >= r)
        {
            x = r;
        }
        else
        {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000); /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

/**
 * Translate the object as they scroll
 */

void LV_UI3(void)
{
    Cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(Cont, 200, 200);
    lv_obj_center(Cont);
    lv_obj_set_flex_flow(Cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(Cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_scroll_dir(Cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(Cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(Cont, LV_SCROLLBAR_MODE_OFF);

    uint32_t i;
    for (i = 0; i < 20; i++)
    {
        lv_obj_t *btn = lv_btn_create(Cont);
        lv_obj_set_width(btn, lv_pct(100));

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Button %" LV_PRIu32, i);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(Cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(Cont, 1), LV_ANIM_ON);
}

#endif

static void sw_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *sw = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t *list = lv_event_get_user_data(e);

        if (lv_obj_has_state(sw, LV_STATE_CHECKED))
        {
            lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
        }
        else
            lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
    }
}
static void panel_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *sw = lv_event_get_target(e);

    if (code == LV_EVENT_SCROLL)
    {
        lv_obj_t *list = lv_event_get_user_data(e);
        if (lv_obj_has_state(sw, LV_STATE_CHECKED))
        {
            lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
        }
        else
            lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
    }
}
/**
 * Show an example to scroll snap
 */
void LV_UI2(void)
{
    // Base style for MusicBox
    lv_style_init(&PanelStyle);
    lv_style_set_bg_color(&PanelStyle, lv_color_black());
    lv_obj_t *panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(panel, &PanelStyle, 0);
    lv_obj_set_size(panel, LV_HOR_RES, LV_VER_RES); // Set size to cover entire horizontal, half vertical
    lv_obj_set_scroll_snap_x(panel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(BigpalnelScrollObject, panel_event_cb, LV_EVENT_ALL, panel);
    uint32_t i;
    for (i = 0; i < 10; i++)
    {
        BigpalnelScrollObject = lv_btn_create(panel);
        lv_obj_set_size(BigpalnelScrollObject, 150, LV_VER_RES * 2 / 3);
        lv_obj_center(BigpalnelScrollObject);

        lv_obj_t *label = lv_label_create(BigpalnelScrollObject);
        if (i == 3)
        {
            lv_label_set_text_fmt(label, "Panel %" LV_PRIu32 "\nno snap", i);
            lv_obj_clear_flag(BigpalnelScrollObject, LV_OBJ_FLAG_SNAPPABLE);
        }
        else
        {
            lv_label_set_text_fmt(label, "Panel %" LV_PRIu32, i);
        }
        lv_obj_center(label);
    }
    lv_obj_update_snap(panel, LV_ANIM_ON);
    SwitchObject = lv_switch_create(lv_scr_act());
    lv_obj_align(SwitchObject, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_add_event_cb(SwitchObject, sw_event_cb, LV_EVENT_ALL, panel);
    OneclickLable = lv_label_create(lv_scr_act());
    lv_label_set_text(OneclickLable, "One scroll");
    lv_obj_align_to(OneclickLable, SwitchObject, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}

static void event_cb(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
    if (dsc->part != LV_PART_INDICATOR)
        return;

    lv_obj_t *obj = lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                    label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if (lv_area_get_width(dsc->draw_area) > txt_size.x + 20)
    {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else
    {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
}

/**
 * Custom drawer on the bar to display the current value
 */
void lv_example_bar_6(void)
{
    lv_obj_t *bar = lv_bar_create(lv_scr_act());
    lv_obj_add_event_cb(bar, event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_center(bar);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void bar_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    // int ii=0;
    // ii = (int32_t)(e->param);
    // ESP_LOGW(TAG, "param ii=%d", ii);
    lv_bar_set_value(cont, i, LV_ANIM_OFF);
    // lv_bar_set_value(cont, 45, LV_ANIM_ON);
}
void RailBar(void)
{
    static lv_style_t style_bg;
    static lv_style_t style_indic;

    lv_style_init(&style_bg);
    lv_style_set_border_color(&style_bg, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_border_width(&style_bg, 2);
    lv_style_set_pad_all(&style_bg, 6); /*To make the indicator smaller*/
    lv_style_set_radius(&style_bg, 6);
    lv_style_set_anim_time(&style_bg, 1000);

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_radius(&style_indic, 3);

    BarObject = lv_bar_create(lv_scr_act());
    lv_obj_remove_style_all(BarObject); /*To have a clean start*/
    lv_obj_add_style(BarObject, &style_bg, 0);
    lv_obj_add_style(BarObject, &style_indic, LV_PART_INDICATOR);

    lv_obj_set_size(BarObject, 200, 20);
    lv_obj_center(BarObject);
    lv_obj_add_event_cb(BarObject, bar_event_cb, LV_EVENT_ALL, NULL);

    // lv_bar_set_value(BarObject, 46, LV_ANIM_ON);
}

/**
 * @brief timer handler for scheduling gui (for refreshing display we need it !)
 */
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

/**
 * @brief Function to change colors based on a timer callback
 */
void LVGL_ChangeColors(TimerHandle_t xTimer)
{
    // Define colors for MusicBox and TitleBox
    lv_color_t musicBoxColor;
    musicBoxColor.full = 0x39e9;
    // Apply colors to MusicBox and TitleBox styles
    lv_color_t titleBoxColor = lv_color_make(0x0, 0x0, 0x00);
    lv_style_set_bg_color(&MusicBox, musicBoxColor);
    lv_style_set_bg_color(&TitleBox, titleBoxColor);
}

/**
 * @brief Function to create the main LVGL user interface
 */
static void LVGL_MyUI(void)
{
    // Base style for MusicBox
    lv_style_init(&MusicBox);
    lv_style_set_bg_color(&MusicBox, lv_color_make(17, 39, 28));
    // Create an object for the music display with the MusicBox style
    lv_obj_t *musicObject = lv_obj_create(lv_scr_act());
    lv_obj_add_style(musicObject, &MusicBox, 0);
    lv_obj_align(musicObject, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(musicObject, LV_HOR_RES, LV_VER_RES / 2); // Set size to cover entire horizontal, half vertical

    // Base style for TitleBox
    lv_style_init(&TitleBox);
    lv_style_set_bg_color(&TitleBox, lv_color_make(0xff, 0x0, 0x00));

    /*Create another object with the base style and earnings style too*/
    lv_obj_t *textObject = lv_obj_create(lv_scr_act());
    lv_obj_add_style(textObject, &TitleBox, 0);
    lv_obj_align(textObject, LV_ALIGN_BOTTOM_MID, 0, -100);  //  shift it in Y axis
    lv_obj_set_size(textObject, LV_HOR_RES, LV_VER_RES / 4); // Set size to cover entire horizontal, 0.25 vertical

    /*Circular scroll style*/
    static lv_style_t circular;
    lv_style_init(&circular);
    lv_style_set_bg_color(&circular, lv_color_make(0xff, 0x0, 0x00));
    lv_style_set_text_color(&circular, lv_color_white());
    lv_style_set_text_font(&circular, &lv_font_montserrat_18); // Set the font

    // Create a circular scrolling text object
    lv_obj_t *circularObject = lv_label_create(lv_scr_act());
    lv_obj_add_style(circularObject, &circular, 0);
    lv_label_set_long_mode(circularObject, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(circularObject, 150);
    lv_label_set_text(circularObject, "It is a circularly scrolling text. ");
    lv_obj_align(circularObject, LV_ALIGN_CENTER, 0, 0);

    /* Create an image object for a music bottom picture*/
    lv_obj_t *imageObject = lv_img_create(lv_scr_act());
    lv_img_set_src(imageObject, &music);
    lv_obj_align(imageObject, LV_ALIGN_BOTTOM_MID, 0, -10);
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
    TimerHandle_t xTimer = xTimerCreate("ColorTimer", pdMS_TO_TICKS(10000), pdTRUE, NULL, LVGL_ChangeColors);
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
    lv_color_t *LVGL_BigBuf1 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));
    lv_color_t *LVGL_BigBuf2 = (lv_color_t *)malloc(LV_HOR_RES_MAX * 100 * sizeof(lv_color_t));

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
    // LVGL_MyUI();
    // LV_UI2();
    // LV_UI3();
    RailBar();
    // lv_example_bar_6();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
}

/**
 * @brief Function to initialize LVGL task
 */
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
        LVGL_mainTask,        // Task function
        "LVGL_mainTask",      // Task name (for debugging)
        LVGL_STACK * 8,       // Stack size (in words)
        NULL,                 // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + 1, // Task priority (adjust as needed)
        xLVGLStack,           // Stack buffer
        xTaskLVGLBuffer       // Task control block
    );
    // this delay so important
    vTaskDelay(500);
}

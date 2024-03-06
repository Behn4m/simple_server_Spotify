#include "lvglGui.h"
#include "image_test.h"
#include <inttypes.h>
#include "driver/gpio.h"
#include "esp_pm.h"
#include "iot_button.h"
#include "lvgl__lvgl/src/core/lv_obj.h"

static void SpotifyPageFunc(void);

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
int i = 0;
lv_obj_t *BarObject;
static void IRAM_ATTR button_event_cb(void *arg, void *data)
{
    ESP_LOGE(TAG, "Buttom callback");
    i = i + 10;
    ESP_LOGW(TAG, "i=%d", i);
    if (i >= 100)
        i = 0;
    lv_event_send(BarObject, LV_EVENT_ALL, (void *)&i);
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
static void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}
static void bar_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    int32_t percentOfFillingBar = 0;
    percentOfFillingBar = *((int32_t *)e->param);
    if (percentOfFillingBar <= 100)
    {
        lv_bar_set_value(cont, i, LV_ANIM_OFF);
    }
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
    // // Define colors for MusicBox and TitleBox
    lv_color_t musicBoxColor;
    musicBoxColor.full = 0x39e9;
    // Apply colors to MusicBox and TitleBox styles
    lv_color_t titleBoxColor = lv_color_make(0x0, 0x0, 0x00);
    lv_style_set_bg_color(&MusicBox, musicBoxColor);
    lv_style_set_bg_color(&TitleBox, titleBoxColor);
}
void MainMenu(void)
{
    /*A base style*/
    static lv_style_t parentStyle;
    lv_style_init(&parentStyle);
    lv_style_set_border_width(&parentStyle, 2);
    lv_style_set_radius(&parentStyle, 10);
    lv_style_set_shadow_width(&parentStyle, 10);
    lv_style_set_shadow_ofs_y(&parentStyle, 5);
    lv_style_set_shadow_opa(&parentStyle, LV_OPA_50);
    lv_style_set_text_color(&parentStyle, lv_color_white());
    lv_style_set_text_font(&parentStyle, &lv_font_montserrat_18); // Set the font
    lv_style_set_width(&parentStyle, LV_HOR_RES * 0.4);
    lv_style_set_height(&parentStyle, LV_VER_RES / 3);

    /*Set only the properties that should be different*/
    static lv_style_t matterStyle;
    lv_style_init(&matterStyle);
    lv_style_set_bg_color(&matterStyle, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_color(&matterStyle, lv_palette_darken(LV_PALETTE_RED, 3));

    /*Create an object with the base style only*/
    lv_obj_t *matterObject = lv_obj_create(lv_scr_act());
    lv_obj_add_style(matterObject, &parentStyle, 0);
    lv_obj_add_style(matterObject, &matterStyle, 0);
    lv_obj_align(matterObject, LV_ALIGN_LEFT_MID, 20, -35);

    lv_obj_t *matterLabel = lv_label_create(matterObject);
    lv_label_set_text(matterLabel, "1.Matter");
    lv_obj_center(matterLabel);

    /*Set only the properties that should be different*/
    static lv_style_t spotifyStyle;
    lv_style_init(&spotifyStyle);
    lv_style_set_bg_color(&spotifyStyle, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_border_color(&spotifyStyle, lv_palette_darken(LV_PALETTE_GREEN, 3));

    /*Create another object with the base style and earnings style too*/
    lv_obj_t *spotifyObject = lv_obj_create(lv_scr_act());
    lv_obj_add_style(spotifyObject, &parentStyle, 0);
    lv_obj_add_style(spotifyObject, &spotifyStyle, 0);
    lv_obj_align(spotifyObject, LV_ALIGN_RIGHT_MID, -20, 35);

    lv_obj_t *spotifyLabel = lv_label_create(spotifyObject);
    lv_label_set_text(spotifyLabel, "2.Spotify");
    lv_obj_center(spotifyLabel);
}

/**
 * @brief Function to create the main LVGL user interface
 */
static void SpotifyPageFunc(void)
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

lv_obj_t *UiScreen;
lv_obj_t *Menu;
lv_obj_t *MatterPage;
lv_obj_t *SpotifyPage;
lv_obj_t *BackBottom;
void UiScreenInit(void)
{
    /*Create a UiScreen object*/
    UiScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(UiScreen, LV_OBJ_FLAG_CLICKABLE); /// Flags
    lv_obj_remove_style_all(UiScreen);
    /*Create a Menu object*/
    Menu = lv_obj_create(NULL);
    lv_obj_clear_flag(Menu, LV_OBJ_FLAG_CLICKABLE); /// Flags
    lv_obj_remove_style_all(Menu);
    /*Create a MatterPage object*/
    MatterPage = lv_obj_create(NULL);
    lv_obj_clear_flag(MatterPage, LV_OBJ_FLAG_CLICKABLE); /// Flags
    lv_obj_remove_style_all(MatterPage);
    /*Create a SpotifyPage object*/
    SpotifyPage = lv_obj_create(NULL);
    lv_obj_clear_flag(SpotifyPage, LV_OBJ_FLAG_CLICKABLE); /// Flags
    lv_obj_remove_style_all(SpotifyPage);
    /*Create a BackBottom object*/
    BackBottom = lv_obj_create(NULL);
    lv_obj_clear_flag(BackBottom, LV_OBJ_FLAG_CLICKABLE); /// Flags
    lv_obj_remove_style_all(BackBottom);
}
void CreatMenu()
{
    /*Create a Menu object*/
    Menu = lv_menu_create(lv_scr_act());
    lv_obj_set_size(Menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(Menu);
}
static void backBottom(lv_event_t *e)
{
}
void CreatBottomKeyForMenu()
{
    /*Modify the header*/
    BackBottom = lv_menu_get_main_header_back_btn(Menu);
    lv_obj_t *back_btn_label = lv_label_create(BackBottom);
    lv_obj_add_event_cb(BackBottom, backBottom, LV_EVENT_CLICKED, BackBottom);
    lv_label_set_text(back_btn_label, "Back");
}

void myMenu(void)
{
    lv_obj_t *cont;
    lv_obj_t *label;
    UiScreenInit();
    CreatMenu();
    CreatBottomKeyForMenu();
    /*Create a main page*/
    lv_obj_t *main_page = lv_menu_page_create(Menu, NULL);
    lv_menu_set_page(Menu, main_page);

    /*Create sub pages*/
    lv_obj_t *sub_1_page = lv_menu_page_create(Menu, "Page 1");
    cont = lv_menu_cont_create(sub_1_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 1 (Click me!)");
    lv_menu_set_load_page_event(Menu, cont, sub_1_page);
    
    /*Create sub pages*/
    lv_obj_t *sub_2_page = lv_menu_page_create(Menu, "Page 2");
    cont = lv_menu_cont_create(sub_2_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 2 (Click me!)");
    lv_menu_set_load_page_event(Menu, cont, sub_2_page);

    // lv_obj_t *sub_3_page = lv_menu_page_create(Menu, "Page 3");
    // cont = lv_menu_cont_create(sub_3_page);
    // label = lv_label_create(cont);
    // lv_label_set_text(label, "Hello, I am hiding here");
    // cont = lv_menu_cont_create(main_page);
    // label = lv_label_create(cont);
    // lv_label_set_text(label, "Item 3 (Click me!)");
    // lv_menu_set_load_page_event(Menu, cont, sub_3_page);
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
    // MainMenu();
    myMenu();
    // LV_UI2();
    // LV_UI3();
    // RailBar();
    // SpotifyPage();
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

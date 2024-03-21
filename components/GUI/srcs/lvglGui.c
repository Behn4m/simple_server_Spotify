#include "lvglGui.h"
#include "custom.h"
#include "gui_guider.h"

static const char *TAG = "LVGL_GUI";
static lv_disp_draw_buf_t disp_draw_buf;
lv_color_t *LVGL_BigBuf1;
lv_color_t *LVGL_BigBuf2;

/**
 * @brief main LVGL gui TASK
 */
static void GUI_mainTask(void *pvParameter)
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
    lv_disp_draw_buf_init(&disp_draw_buf, LVGL_BigBuf1, LVGL_BigBuf2, LV_HOR_RES_MAX * 100);
    // Initialize display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_draw_buf;
    lv_disp_drv_register(&disp_drv);
    setup_ui(&guider_ui);
        // LVGLBottomInit();
    LVGL_Timer();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        lv_task_handler();
    }
}

/**
 * @brief Function to initialize LVGL task
 */
void GUI_TaskInit(void)
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
        GUI_mainTask,               // Task function
        "GUI_mainTask",             // Task name (for debugging)
        LVGL_STACK * 8 * MULTIPLIER, // Stack size (in words)
        NULL,                        // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + 1,        // Task priority (adjust as needed)
        xLVGLStack,                  // Stack buffer
        xTaskLVGLBuffer              // Task control block
    );
    // this delay so important
    vTaskDelay(500);
}


/**
 * @brief Function to update the LVGL screen
 * @param Artist: Artist name
 * @param Title: Title of the song
 * @param Album: Album name
 * @return void
 */
void GUI_UpdateSpotifyScreen(char *Artist, char *Song, char *Album, int DurationMS, int ProgressMS)
{
    lv_event_send(guider_ui.Spotify_Page_Artist_name, LV_EVENT_VALUE_CHANGED, Artist);
    lv_event_send(guider_ui.Spotify_Page_Song_name, LV_EVENT_VALUE_CHANGED, Song);
    lv_event_send(guider_ui.Spotify_Page_Album_name, LV_EVENT_VALUE_CHANGED, Album);

    int minutues = ProgressMS / 60000;
    int second = (ProgressMS % 60000) / 1000;
    char time [20];
    sprintf(time, "%d:%d", minutues, second);
    ESP_LOGW(TAG, "Time: %s", time);
    lv_event_send(guider_ui.Spotify_Page_label_time, LV_EVENT_VALUE_CHANGED, time);

    int progress = (ProgressMS * 100) / DurationMS;
    lv_event_send(guider_ui.Spotify_Page_bar_progress, LV_EVENT_VALUE_CHANGED, progress);
}


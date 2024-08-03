#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "Authorization.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"

#define TAG "MAIN"
#define TIMER_TIME pdMS_TO_TICKS(500) // in millis

// ****************************** GLobal Variables ****************************** //
//static const char *TAG = "Main";
OAuthInterfaceHandler_t OAuthInterfaceHandler;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;
SpotifyHttpInfo_t Spotify_ClientInfo;

void app_main(void)
{
    GUI_TaskInit();
    GlobalInit();
    nvsFlashInit();
    // lv_obj_t *obj = lv_obj_get_child(lv_scr_act(), 0); // Assuming the button is the first child
    // test_lvgl_send_event(obj);
    send_event_test();
    // while (1) {
    //     lv_task_handler();
    //     vTaskDelay(5000); // Sleep for 5 ms
    // }
}

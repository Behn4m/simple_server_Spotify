#include <stdio.h>
#include "Setup_GPIO.h"
#define BUTTON_BACK GPIO_NUM_21
#define BUTTON_ACCEPT GPIO_NUM_13
#define BUTTON_UP GPIO_NUM_12
#define BUTTON_PAUSE GPIO_NUM_11

#define BOOT_BUTTON_NUM 21
#define BUTTON_ACTIVE_LEVEL 1


int i=1;
static void IRAM_ATTR button_event_cb(void *arg, void *data)
{
    ESP_LOGE(TAG, "Bottom callback");
    i = i + 10;
    ESP_LOGW(TAG, "i=%d", i);
    if (i >= 100)
        i = 0;

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



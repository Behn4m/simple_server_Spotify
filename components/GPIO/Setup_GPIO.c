#include <stdio.h>
#include "Setup_GPIO.h"

static const char *TAG = "GPIO_Bottom";

void IRAM_ATTR BackBottomCallBAck(void *arg, void *data)
{
    ESP_LOGE(TAG, "Bottom callback");
}
void IRAM_ATTR AcceptBottomCallBack(void *arg, void *data)
{
    ESP_LOGE(TAG, "Bottom callback");
}
void BackBottom()
{
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = BUTTON_BACK,
            .active_level = BUTTON_ACTIVE_LEVEL,
        },
    };
    button_handle_t btn = iot_button_create(&btn_cfg);
    assert(btn);
    esp_err_t err = iot_button_register_cb(btn, BUTTON_PRESS_DOWN, BackBottomCallBAck, NULL);
    ESP_ERROR_CHECK(err);
}

void AcceptBottom()
{
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = BUTTON_ACCEPT,
            .active_level = BUTTON_ACTIVE_LEVEL,
        },
    };
    button_handle_t btn = iot_button_create(&btn_cfg);
    assert(btn);
    esp_err_t err = iot_button_register_cb(btn, BUTTON_PRESS_DOWN, AcceptBottomCallBack, NULL);
    ESP_ERROR_CHECK(err);
}

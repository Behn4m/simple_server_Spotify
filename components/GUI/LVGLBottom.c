#include"LVGLBottom.h"
#include"GUIEvent.h"


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

void LVGLBottomInit(void)
{
    button_init(BUTTON_BACK);
    button_init(BUTTON_ACCEPT);
    button_init(BUTTON_UP);
    button_init(BUTTON_PAUSE);
}


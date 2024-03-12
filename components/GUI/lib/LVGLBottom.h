#ifndef LVGL_LVGLBOTTOM_H_
#define LVGL_LVGLBOTTOM_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <inttypes.h>
#include "driver/gpio.h"
#include "esp_pm.h"
#include "iot_button.h"

#define BUTTON_BACK GPIO_NUM_21
#define BUTTON_ACCEPT GPIO_NUM_13
#define BUTTON_UP GPIO_NUM_12
#define BUTTON_PAUSE GPIO_NUM_11

#define BOOT_BUTTON_NUM 21
#define BUTTON_ACTIVE_LEVEL 1


void LVGLBottomInit(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

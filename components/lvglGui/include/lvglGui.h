/*
 * a component for create indipendent test for gui
 */
#ifndef LVGL_GUI_H_
#define LVGL_GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"

#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "lvgl.h"
#include "lvgl_helpers.h"
#include "rm67162Lilygo.h"
#include "rm67162.h"

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

/**
 * @brief      create guiTask and its global method
 */

void LVGL_TaskInit(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

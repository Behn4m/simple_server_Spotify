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
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "rm67162Lilygo.h"
#include "rm67162.h"
#include "lv_demo.h"
#include "esp_log.h"

/**
 * @brief      create guiTask and its global method
 */

void LVGL_TaskInit(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

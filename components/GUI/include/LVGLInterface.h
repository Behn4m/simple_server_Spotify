#ifndef LVGL_INTERFACE_H_
#define LVGL_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "freertos/queue.h"



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

#define MULTIPLIER 20
#define LV_TICK_PERIOD_MS 1
#define LVGL_STACK 2500
#define TIMER_CALLBACK_TIME 10 * 1000 /* in milliseconds */
    // typedef enum
    // {
    //     Menu = 0,
    //     Next = 1,
    //     Back = 2,
    //     Apply = 3
    // } Command_t;

    // typedef struct
    // {
    //     typedef void (*LVGL_GPIOCallBackPtrApply)(void *arg, void *data);
    //     typedef void (*LVGL_GPIOCallBackPtrNext)(void *arg, void *data);
    //     typedef void (*LVGL_GPIOCallBackPtrBack)(void *arg, void *data);
    // } LVGL_GPIO_t;

    // typedef struct
    // {
    //     LVGL_GPIO_t LVGL_GPIOCallBackFunction;
    //     QueueHandle_t *LVGLBufQueue;
    //     SemaphoreHandle_t *SignalingSemaphore;
    //     TaskHandle_t LVGLTaskHandler;

    // } LVGLInterfaceHandler_t;

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

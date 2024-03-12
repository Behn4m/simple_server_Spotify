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

    typedef enum
    {
        Menu = 0,
        Next = 1,
        Back = 2,
        Apply = 3
    } Command_t;

    typedef struct
    {
        typedef void (*LVGL_GPIOCallBackPtrApply)(void *arg, void *data);
        typedef void (*LVGL_GPIOCallBackPtrNext)(void *arg, void *data);
        typedef void (*LVGL_GPIOCallBackPtrBack)(void *arg, void *data);
    } LVGL_GPIO_t;

    typedef struct
    {
        LVGL_GPIO_t LVGL_GPIOCallBackFunction;
        QueueHandle_t *LVGLBufQueue;
        SemaphoreHandle_t *SignalingSemaphore;
        TaskHandle_t LVGLTaskHandler;

    } LVGLInterfaceHandler_t;

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

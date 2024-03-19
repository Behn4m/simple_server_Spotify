#ifdef __cplusplus
extern "C"
{
#endif
#ifndef SETUP_GPIO_H_
#define SETUP_GPIO_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "button_gpio.h"
#include "iot_button.h"
#define BUTTON_BACK GPIO_NUM_0
#define BUTTON_ACCEPT GPIO_NUM_21
#define BUTTON_ACTIVE_LEVEL 1

typedef void (*BackBottomCallBack_t)(void *arg, void *data);
typedef void (*AcceptBottomCallBack_t)(void *arg, void *data);
typedef struct
{
    BackBottomCallBack_t BackBottomCallBack;
    AcceptBottomCallBack_t AcceptBottomCallBack;
} BottomCallBackFunctions_t;

/*
* @brief: Function to initialize the GPIOs
* @param: BottomCallBackFunctions_t BottomCallBackFunctions
* @return: void
*/
void GPIO_init(BottomCallBackFunctions_t BottomCallBackFunctions);
#endif
#ifdef __cplusplus
}
#endif
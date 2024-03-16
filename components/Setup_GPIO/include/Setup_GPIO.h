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

#include "esp_system.h"
#include "driver/gpio.h"

#define BUTTON_BACK GPIO_NUM_21
#define BUTTON_ACCEPT GPIO_NUM_13
#define BUTTON_UP GPIO_NUM_12
#define BUTTON_PAUSE GPIO_NUM_11

#define BOOT_BUTTON_NUM 21
#define BUTTON_ACTIVE_LEVEL 1

void GPIO_INIT();




#endif
#ifdef __cplusplus
}
#endif
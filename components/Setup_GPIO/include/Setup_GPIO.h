#ifdef __cplusplus
extern "C"
{
#endif
#ifndef SETUP_GPIO_H_
#define SETUP_GPIO_H_




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
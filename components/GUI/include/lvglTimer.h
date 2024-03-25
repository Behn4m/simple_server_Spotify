#ifndef _LVGL_TINER_H
#define _LVGL_TINER_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
#include "LVGLInterface.h"

/**
 * @brief Function to initialize and start LVGL timer ,
 * LVGL for working need a timer ,
 * timer used for making animation and refresh screen and ...
 */
void LVGL_Timer();

#ifdef __cplusplus
}
#endif
#endif
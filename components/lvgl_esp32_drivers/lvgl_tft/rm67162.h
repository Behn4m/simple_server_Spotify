/**
 * @file rm67162.h
 * @author SEZ@Done(ehsan.ziyaee@gmail.com) 
 * @brief a custom lvgl driver for rm67162 
 *        init 2023.10.09
 */

#ifndef RM67162_H
#define RM67162_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "../lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/
#define RM67162_MADCTL      0x36
#define RM67162_MAD_MY      0x80
#define RM67162_MAD_MX      0x40
#define RM67162_MAD_MV      0x20
#define RM67162_MAD_ML      0x10
#define RM67162_MAD_BGR     0x08
#define RM67162_MAD_MH      0x04
#define RM67162_MAD_RGB     0x00

#define RM67162_INVOFF      0x20
#define RM67162_INVON       0x21

// SPI pinout
// #define RM67162_DC      GPIO_NUM_7 
// #define RM67162_RST     GPIO_NUM_17
// #define RM67162_CS      GPIO_NUM_6
// #define RM67162_MOSI    GPIO_NUM_18
// #define RM67162_SCK     GPIO_NUM_47

// QSPI pinout
// #define RM67162_QSPI_CS           GPIO_NUM_6
// #define RM67162_QSPI_SCK          GPIO_NUM_47
// #define RM67162_QSPI_D0           GPIO_NUM_18
// #define RM67162_QSPI_D1           GPIO_NUM_7
// #define RM67162_QSPI_D2           GPIO_NUM_48
// #define RM67162_QSPI_D3           GPIO_NUM_5
// #define RM67162_QSPI_RST          GPIO_NUM_17

#define RM67162_RST         CONFIG_LV_DISP_PIN_RST
#define RM67162_USE_RST     CONFIG_LV_DISP_USE_RST

#define RM67162_CS          CONFIG_LV_DISP_SPI_CS
#define RM67162_USE_CS      CONFIG_LV_DISPLAY_USE_SPI_CS

#define LV_HOR_RES_MAX            536
#define LV_VER_RES_MAX            240
#define LV_TICK_PERIOD_MS         1
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief      init rm67162 and use in disp_drivers.c
 * @param[in]  void   nothing
 * @param[out] void   nothing
 * @return nothing
 */
void rm67162_init(void);

/**
 * @brief      flush all pixel of lcd and use in disp_drivers.c
 *             we dont fill any input output parameter. we just pass it to 
 *             lv_disp_drv_t staruct of lvglGui.c
 * @param[in]  void   nothing
 * @param[out] void   nothing
 * @return nothing
 */
void rm67162_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RM67162_H*/

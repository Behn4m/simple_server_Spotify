
#ifndef RM67162_LILYGO_H
#define RM67162_LILYGO_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "stdint.h"
#include <stdbool.h>

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "../lvgl_helpers.h"

/***********************config*************************/
#define LCD_USB_QSPI_DREVER   1

#define SPI_FREQUENCY         80000000
#define TFT_SPI_MODE          0
#define TFT_SPI_HOST          SPI2_HOST

#define EXAMPLE_LCD_H_RES     536
#define EXAMPLE_LCD_V_RES     240
#define LVGL_LCD_BUF_SIZE     (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)

#define TFT_WIDTH             240
#define TFT_HEIGHT            536
#define SEND_BUF_SIZE         (0x4000) //(LCD_WIDTH * LCD_HEIGHT + 8) / 10

#define TFT_TE                9
#define TFT_SDO               8

#define TFT_DC                7
#define TFT_RES               17
#define TFT_CS                6
#define TFT_MOSI              18
#define TFT_SCK               47

#define TFT_QSPI_CS           6
#define TFT_QSPI_SCK          47
#define TFT_QSPI_D0           18
#define TFT_QSPI_D1           7
#define TFT_QSPI_D2           48
#define TFT_QSPI_D3           5
#define TFT_QSPI_RST          17

#define PIN_LED               38
#define PIN_BAT_VOLT          4

#define PIN_BUTTON_1          0
#define PIN_BUTTON_2          21

#define TFT_MADCTL 0x36
#define TFT_MAD_MY 0x80
#define TFT_MAD_MX 0x40
#define TFT_MAD_MV 0x20
#define TFT_MAD_ML 0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH 0x04
#define TFT_MAD_RGB 0x00

#define TFT_INVOFF 0x20
#define TFT_INVON 0x21

/**
 * @brief      init rm67162 and use in disp_drivers.c
 * @param[in]  void   nothing
 * @param[out] void   nothing
 * @return nothing
 */
void lcd_init(void);

/**
 * @brief      flush all pixel of lcd and use in disp_drivers.c
 *             we dont fill any input output parameter. we just pass it to 
 *             lv_disp_drv_t staruct of lvglGui.c
 * @param[in]  void   nothing
 * @param[out] void   nothing
 * @return nothing
 */
void lcd_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RM67162_LILYGO_H*/
#ifdef __cplusplus
extern "C" {
#endif
#ifndef JPEG_DECODE_H_
#define JPEG_DECODE_H_

#include <stdio.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "jpeg_decoder.h"

/*
 * @brief  Convert a JPEG image to a raw image buffer
 * @param  jpeg_img_buf: pointer to the JPEG image buffer
 * @param  jpeg_img_buf_size: size of the JPEG image buffer
 * @param  out_img_buf: pointer to the output image buffer
 * @param  out_img_buf_size: size of the output image buffer
 * @return void
*/
void convertJpeg(char *jpeg_img_buf, size_t jpeg_img_buf_size, uint8_t *out_img_buf, uint16_t out_img_buf_size);


#endif
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include "JpegDecode.h"

static const char *TAG = "JpegDecode";

void convertJpeg(char *jpeg_img_buf, size_t jpeg_img_buf_size, uint8_t *out_img_buf, uint16_t out_img_buf_size)
{
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)jpeg_img_buf,
        .indata_size = jpeg_img_buf_size,
        .outbuf = (uint8_t *)out_img_buf,
        .outbuf_size = out_img_buf_size,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 1,
        }
    };
    esp_jpeg_image_output_t outimg;
    esp_jpeg_decode(&jpeg_cfg, &outimg);
    ESP_LOGI(TAG, "Image size: %d x %d\n", outimg.width, outimg.height);
}


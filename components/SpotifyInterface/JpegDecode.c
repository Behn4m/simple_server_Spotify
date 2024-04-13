
#include <stdio.h>
#include <string.h>
#include "JpegDecode.h"


static const char *TAG = "JpegDecode";

bool convertJpeg(char *jpeg_img_buf, size_t jpeg_img_buf_size, uint8_t *out_img_buf, uint16_t out_img_buf_size)
{
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)jpeg_img_buf,
        .indata_size = jpeg_img_buf_size,
        .outbuf = (uint8_t *)out_img_buf,
        .outbuf_size = out_img_buf_size,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_1_2,
        .flags = {
            .swap_color_bytes = 1,
        }
    };
    esp_jpeg_image_output_t outimg;
    bool err = esp_jpeg_decode(&jpeg_cfg, &outimg);
    if (err)
    {
        ESP_LOGE(TAG, "JPEG decode failed");
        return false;
    }
    printf("%d\n", sizeof(out_img_buf));
    ESP_LOGI(TAG, "Image size: %d x %d\n", outimg.width, outimg.height);
    return true;
}

void addAlphaPixeltoImage(uint8_t *image, uint16_t width, uint16_t height, uint8_t alpha)
{
    uint8_t *buffer = (uint8_t *)malloc(width * height * 3);

    for (int i = 0; i < width * height; i++)
    {
        buffer[i * 3 + 0] = image[i * 2 + 0];
        buffer[i * 3 + 1] = image[i * 2 + 1];
        buffer[i * 3 + 2] = alpha;
    }

    memcpy(image, buffer, width * height * 3);
    free(buffer);
}


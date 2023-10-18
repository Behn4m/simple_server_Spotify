#include "rm67162Lilygo.h"
//#include "SPI.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct
{
    uint8_t cmd;
    uint8_t data[4];
    uint8_t len;
} lcd_cmd_t;

const static lcd_cmd_t rm67162_qspi_init[] = {
    {0x11, {0x00}, 0x80}, // Sleep Out
    // {0x44, {0x01, 0x66},        0x02}, //Set_Tear_Scanline
    // {0x35, {0x00},        0x00}, //TE ON
    // {0x34, {0x00},        0x00}, //TE OFF
    // {0x36, {0x00},        0x01}, //Scan Direction Control
    {0x3A, {0x55}, 0x01}, // Interface Pixel Format 16bit/pixel
    // {0x3A, {0x66},        0x01}, //Interface Pixel Format    18bit/pixel
    // {0x3A, {0x77},        0x01}, //Interface Pixel Format    24bit/pixel
    {0x51, {0x00}, 0x01}, // Write Display Brightness MAX_VAL=0XFF
    {0x29, {0x00}, 0x80}, // Display on
    {0x51, {0xD0}, 0x01}, // Write Display Brightness   MAX_VAL=0XFF
};

static spi_device_handle_t spi;

static void lcd_send_cmd(uint32_t cmd, uint8_t *dat, uint32_t len)
{
    gpio_set_level(TFT_QSPI_CS, 0);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR);
    t.cmd = 0x02;
    t.addr = cmd << 8;
    
    if (len != 0) {
        t.tx_buffer = dat;
        t.length = 8 * len;
    } else {
        t.tx_buffer = NULL;
        t.length = 0;
    }

    spi_device_polling_transmit(spi, &t);
    gpio_set_level(TFT_QSPI_CS, 1);    
}

static void lcd_setRotation(uint8_t r)
{
    uint8_t gbr = TFT_MAD_RGB;

    switch (r) {
    case 0: // Portrait
        break;
    case 1: // Landscape (Portrait + 90)
        gbr = TFT_MAD_MX | TFT_MAD_MV | gbr;
        break;
    case 2: // Inverter portrait
        gbr = TFT_MAD_MX | TFT_MAD_MY | gbr;
        break;
    case 3: // Inverted landscape
        gbr = TFT_MAD_MV | TFT_MAD_MY | gbr;
        break;
    }

    lcd_send_cmd(TFT_MADCTL, &gbr, 1);
}

static void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_cmd_t t[3] = {
        {0x2a, {(uint8_t)(x1 >> 8), (uint8_t)x1, (uint8_t)(x2 >> 8), (uint8_t) x2}, 0x04},
        {0x2b, {(uint8_t)(y1 >> 8), (uint8_t)y1, (uint8_t)(y2 >> 8), (uint8_t) y2}, 0x04},
        {0x2c, {0x00}, 0x00},
    };

    for (uint32_t i = 0; i < 3; i++) {
        lcd_send_cmd(t[i].cmd, t[i].data, t[i].len);
    }
}

static void lcd_PushColor (uint16_t x,
                    uint16_t y,
                    uint16_t width,
                    uint16_t high,
                    uint16_t *data)
{
    bool first_send = 1;
    size_t len = width * high;
    uint16_t *p = (uint16_t *)data;

    lcd_address_set(x, y, x + width - 1, y + high - 1);
    gpio_set_level(TFT_QSPI_CS, 0);
    do {
        size_t chunk_size = len;
        spi_transaction_ext_t t = {0};
        memset(&t, 0, sizeof(t));
        if (first_send) {
            t.base.flags =
                SPI_TRANS_MODE_QIO /* | SPI_TRANS_MODE_DIOQIO_ADDR */;
            t.base.cmd = 0x32 /* 0x12 */;
            t.base.addr = 0x002C00;
            first_send = 0;
        } else {
            t.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                           SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
            t.command_bits = 0;
            t.address_bits = 0;
            t.dummy_bits = 0;
        }
        if (chunk_size > SEND_BUF_SIZE) {
            chunk_size = SEND_BUF_SIZE;
        }
        t.base.tx_buffer = p;
        t.base.length = chunk_size * 16;

        // spi_device_queue_trans(spi, (spi_transaction_t *)&t, portMAX_DELAY);
        spi_device_polling_transmit(spi, (spi_transaction_t *)&t);
        len -= chunk_size;
        p += chunk_size;
    } while (len > 0);
    gpio_set_level(TFT_QSPI_CS, 1);
}

void lcd_init(void)
{
    esp_rom_gpio_pad_select_gpio(TFT_QSPI_RST);
	gpio_set_direction(TFT_QSPI_RST, GPIO_MODE_OUTPUT);

    esp_rom_gpio_pad_select_gpio(TFT_QSPI_CS);
	gpio_set_direction(TFT_QSPI_CS, GPIO_MODE_OUTPUT);

    //Reset the display
	gpio_set_level(TFT_QSPI_RST, 0);
	vTaskDelay(300 / portTICK_RATE_MS);//default 100ms
	gpio_set_level(TFT_QSPI_RST, 1);
	vTaskDelay(300 / portTICK_RATE_MS);//default 100ms

    esp_err_t ret;

    // spi_bus_config_t buscfg = {
    //     .data0_io_num = TFT_QSPI_D0,
    //     .data1_io_num = TFT_QSPI_D1,
    //     .sclk_io_num = TFT_QSPI_SCK,
    //     .data2_io_num = TFT_QSPI_D2,
    //     .data3_io_num = TFT_QSPI_D3,
    //     .max_transfer_sz = (SEND_BUF_SIZE * 16) + 8,
    //     .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS  |
    //              SPICOMMON_BUSFLAG_QUAD 
    //     ,
    // };
    
    spi_device_interface_config_t devcfg = {
        .command_bits = 8,
        .address_bits = 24,
        .mode = TFT_SPI_MODE,
        .clock_speed_hz = SPI_FREQUENCY,
        .spics_io_num = -1,
        //.spics_io_num = TFT_QSPI_CS,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 17,
    };
    // ret = spi_bus_initialize(TFT_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    // ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(TFT_SPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    // Initialize the screen multiple times to prevent initialization failure
    //int i = 3;
    //while (i--) {
        const lcd_cmd_t *lcd_init = rm67162_qspi_init;
        for (int i = 0; i < sizeof(rm67162_qspi_init) / sizeof(lcd_cmd_t); i++)
        {
            lcd_send_cmd(lcd_init[i].cmd,
                         (uint8_t *)lcd_init[i].data,
                         lcd_init[i].len & 0x7f);

            if (lcd_init[i].len & 0x80)
                vTaskDelay(120 / portTICK_RATE_MS);
        }
    //}

    lcd_setRotation(1); //Landscape
}

void lcd_flush( lv_disp_drv_t *disp,
                const lv_area_t *area,
                lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lcd_PushColor(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}
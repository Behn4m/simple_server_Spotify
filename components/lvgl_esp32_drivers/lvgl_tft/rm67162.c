/**
 * @file rm67162.c
 * @author SEZ@Done(ehsan.ziyaee@gmail.com) 
 * @brief a custom lvgl driver for rm67162 
 *        init 2023.10.09
 */
/*********************
 *      INCLUDES
 *********************/
#include "rm67162.h"
#include "disp_spi.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*********************
 *      DEFINES
 *********************/
 #define TAG "RM67162"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint8_t cmd;
    uint8_t data[4];
    uint8_t databytes; //No. of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_cmd_t;

#if RM67162_QSPI == 1
	const static lcd_cmd_t rm67162_init_cmd[]={
        {0x11, {0x00}, 0x80}, // Sleep Out
        // {0x44, {0x01, 0x66},  0x02}, //Set_Tear_Scanline
        // {0x35, {0x00},        0x00}, //TE ON
        // {0x34, {0x00},        0x00}, //TE OFF
        // {0x36, {0x00},        0x01}, //Scan Direction Control
        {0x3A, {0x55}, 0x01}, // Interface Pixel Format 16bit/pixel
        // {0x3A, {0x66},        0x01}, //Interface Pixel Format    18bit/pixel
        // {0x3A, {0x77},        0x01}, //Interface Pixel Format    24bit/pixel
        {0x51, {0x00}, 0x01}, // Write Display Brightness MAX_VAL=0XFF
        {0x29, {0x00}, 0x80}, // Display on
        {0x51, {0xD0}, 0x01}, // Write Display Brightness   MAX_VAL=0XFF
        {0x00, {0}, 0xff},
    };
#else
    const static lcd_cmd_t rm67162_init_cmd[] = {
        {0xFE, {0x00}, 0x01}, // PAGE
        // {0x35, {0x00},        0x00}, //TE ON
        // {0x34, {0x00},        0x00}, //TE OFF
        {0x36, {0x00}, 0x01}, // Scan Direction Control
        {0x3A, {0x75}, 0x01}, // Interface Pixel Format 16bit/pixel
        // {0x3A, {0x76},        0x01}, //Interface Pixel Format    18bit/pixel
        // {0x3A, {0x77},        0x01}, //Interface Pixel Format    24bit/pixel
        {0x51, {0x00}, 0x01},        // Write Display Brightness MAX_VAL=0XFF
        {0x11, {0x00}, 0x01 | 0x80}, // Sleep Out
        {0x29, {0x00}, 0x01 | 0x80}, // Display on
        {0x51, {0xD0}, 0x01},        // Write Display Brightness    MAX_VAL=0XFF
        {0x00, {0}, 0xff},
    };    
#endif
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void rm67162_send_cmd(uint8_t cmd);
static void rm67162_send_data(void * data, uint16_t length);
static void rm67162_send_color(void * data, uint16_t length);
static void rm67162_set_orientation(uint8_t orientation);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void rm67162_init(void)
{
    //Initialize non-SPI GPIOs
    gpio_pad_select_gpio(RM67162_DC);
	gpio_set_direction(RM67162_DC, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(RM67162_CS);
	gpio_set_direction(RM67162_CS, GPIO_MODE_OUTPUT);

#if RM67162_USE_RST
    gpio_pad_select_gpio(RM67162_RST);
	gpio_set_direction(RM67162_RST, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_level(RM67162_RST, 0);
	vTaskDelay(300 / portTICK_RATE_MS);//default 100ms
	gpio_set_level(RM67162_RST, 1);
	vTaskDelay(200 / portTICK_RATE_MS);//default 100ms
#endif

    ESP_LOGI(TAG, "RM67162 Initialization.");

    //Send all the commands
	uint16_t cmd = 0;
	while (rm67162_init_cmd[cmd].databytes!=0xff) {
		rm67162_send_cmd(rm67162_init_cmd[cmd].cmd);
		rm67162_send_data(rm67162_init_cmd[cmd].data, rm67162_init_cmd[cmd].databytes&0x1F);
		if (rm67162_init_cmd[cmd].databytes & 0x80) {
			vTaskDelay(120 / portTICK_RATE_MS);//deafult 100ms
		}
		cmd++;
	}

    rm67162_set_orientation(CONFIG_LV_DISPLAY_ORIENTATION);
}

void rm67162_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    uint32_t size = 0;

    lcd_cmd_t flush[3] = {
        /*Column addresses*/
        {0x2A,{uint8_t(area->x1 >> 8), (uint8_t)area->x1}, uint8_t(area->x2 >> 8), uint8_t(area->x2), 0x04}, 
        /*Page addresses*/
        {0x2B,{uint8_t(area->y1 >> 8), (uint8_t)area->y1}, uint8_t(area->y2 >> 8), uint8_t(area->y2), 0x04}, 
        /*Memory write*/
        {0x2C, {0x00}, 0x00},
    };

	for (uint8_t i = 0; i < 3; i++) {
        rm67162_send_cmd(flush[i].cmd);
        rm67162_send_data(flush[i].data, flush[i].databytes);        
    }
	
	size = lv_area_get_width(area) * lv_area_get_height(area);

    rm67162_send_color((void*) color_map, size * 2);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void rm67162_send_cmd(uint8_t cmd)
{
#if RM67162_QSPI == 1    
	uint8_t to16bit[] = {
	    0x02, cmd
        };

    gpio_set_level(RM67162_CS, 0);
	disp_wait_for_pending_transactions();
	gpio_set_level(RM67162_DC, 0);	 /*Command mode*/
	disp_spi_send_data(to16bit, sizeof(to16bit));
    gpio_set_level(RM67162_CS, 1);
#else
    uint8_t to16bit[] = {
	    0x00, cmd
        };
    
	disp_wait_for_pending_transactions();
	gpio_set_level(RM67162_DC, 0);	 /*Command mode*/
	disp_spi_send_data(to16bit, sizeof(to16bit));
#endif
}

static void rm67162_send_data(void * data, uint16_t length)
{
#if RM67162_QSPI == 1        
	uint32_t i;
	uint8_t to16bit[32];
	uint8_t * dummy = data;

	for(i=0; i < (length); i++)
	{
	  to16bit[2*i+1] = dummy[i];
	  to16bit[2*i] = 0x00;
	}

	disp_wait_for_pending_transactions();
	gpio_set_level(RM67162_DC, 1);	 /*Data mode*/
	disp_spi_send_data(to16bit, (length*2));
#else
    disp_wait_for_pending_transactions();
    gpio_set_level(RM67162_DC, 1);	 /*Data mode*/
    disp_spi_send_data(data, length);    
#endif    
}

static void rm67162_send_color(void * data, uint16_t length)
{
    disp_wait_for_pending_transactions();
    gpio_set_level(RM67162_DC, 1);   /*Data mode*/
    disp_spi_send_colors(data, length);
}

static void rm67162_set_orientation(uint8_t orientation)
{
    const char *orientation_str[] = {
        "PORTRAIT", 
        "PORTRAIT_INVERTED", 
        "LANDSCAPE", 
        "LANDSCAPE_INVERTED"
    };

    uint8_t data[] = {
        RM67162_MAD_MX | RM67162_MAD_MV | RM67162_MAD_RGB, 
        RM67162_MAD_MX | RM67162_MAD_MY | RM67162_MAD_RGB, 
        RM67162_MAD_MX | RM67162_MAD_MV | RM67162_MAD_RGB, 
        RM67162_MAD_MV | RM67162_MAD_MY | RM67162_MAD_RGB};

    ESP_LOGI(TAG, "Display orientation: %s", orientation_str[orientation]);

    ESP_LOGI(TAG, "0x36 command value: 0x%02X", data[orientation]);

    rm67162_send_cmd(0x36);
    rm67162_send_data((void *) &data[orientation], 1);
}
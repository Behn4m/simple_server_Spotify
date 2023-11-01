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
/**
 * @brief      lcd cmd-data packet
 */
typedef struct {
    uint8_t cmd;
    uint8_t data[4];
    uint8_t databytes; //No. of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_cmd_t;

/**
 * @brief      some cmd for init lcd
 */
const static lcd_cmd_t rm67162_init_table[]={
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
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void rm67162_send_cmd(uint8_t cmd);
static void rm67162_send_data(uint8_t* data, uint16_t length);
static void rm67162_send_color(void * data, uint16_t length);
static void rm67162_set_orientation(uint8_t orientation);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void rm67162_init(void)
{    
#if RM67162_USE_CS
    esp_rom_gpio_pad_select_gpio(TFT_QSPI_CS);
	gpio_set_direction(TFT_QSPI_CS, GPIO_MODE_OUTPUT);
#endif

#if RM67162_USE_RST
    esp_rom_gpio_pad_select_gpio(RM67162_RST);
	gpio_set_direction(RM67162_RST, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_level(RM67162_RST, 0);
	vTaskDelay(300 / portTICK_PERIOD_MS);//default 100ms
	gpio_set_level(RM67162_RST, 1);
	vTaskDelay(300 / portTICK_PERIOD_MS);//default 100ms
#endif

    ESP_LOGI(TAG, "RM67162 Initialization.");

    //Send all the commands
	uint16_t cnt = 0;
	while (rm67162_init_table[cnt].databytes!=0xff) {
		rm67162_send_cmd(rm67162_init_table[cnt].cmd);
		rm67162_send_data((uint8_t*)rm67162_init_table[cnt].data, rm67162_init_table[cnt].databytes & 0x7F);
		if (rm67162_init_table[cnt].databytes & 0x80) {
			vTaskDelay(120 / portTICK_PERIOD_MS);//deafult 100ms
		}
		cnt++;
	}

    rm67162_set_orientation(CONFIG_LV_DISPLAY_ORIENTATION);
}

void rm67162_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    //ESP_LOGI(TAG, "rm67162_flush");

    uint32_t size = 0;

    lcd_cmd_t flush[3] = {
        /*Column addresses*/
        {0x2A,{(uint8_t)(area->x1 >> 8), (uint8_t)area->x1, (uint8_t)(area->x2 >> 8), (uint8_t)(area->x2)}, 0x04}, 
        /*Page addresses*/
        {0x2B,{(uint8_t)(area->y1 >> 8), (uint8_t)area->y1, (uint8_t)(area->y2 >> 8), (uint8_t)(area->y2)}, 0x04}, 
        /*Memory write*/
        {0x2C, {0x00}, 0x00},
    };

	for (uint8_t i = 0; i < 3; i++) {
        rm67162_send_cmd(flush[i].cmd);
        rm67162_send_data((uint8_t*)flush[i].data, flush[i].databytes);        
    }
	
	// size = lv_area_get_width(area) * lv_area_get_height(area);
    // ESP_LOGI(TAG, "lcd spi buffer size: %lu", size);
    // ESP_LOGI(TAG, "lcd DISP_BUF_SIZE  : %d", DISP_BUF_SIZE);    
    
    rm67162_send_color((void*) color_map, SPI_BUS_MAX_TRANSFER_SZ);//size * 2);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief       send cmd to lcd-chip
 * @param[in]   cmd   8-bit cmd
 * @return nothing
 */
static void rm67162_send_cmd(uint8_t cmd)
{
    uint8_t cmdPacket[] = {
	    0x02, //for D/C
        0x00, 
        cmd , 
        0x00}; 
    
	disp_wait_for_pending_transactions();

	disp_spi_send_data(cmdPacket, 4);
}

/**
 * @brief       send data to lcd-chip
 * @param[in]   data   data for lcd registers
 * @param[in]   length   lengh of data
 * @return nothing
 */
static void rm67162_send_data(uint8_t* data, uint16_t length)
{
    disp_wait_for_pending_transactions();
    
#if defined (CONFIG_LV_DISPLAY_USE_SPI_CS)
    gpio_set_level(CONFIG_LV_DISP_SPI_CS, 0);    
    disp_spi_send_data(data, length);    
    gpio_set_level(CONFIG_LV_DISP_SPI_CS, 1);
#else
    disp_spi_send_data(data, length);    
#endif
}

/**
 * @brief       send data to lcd-chip with queue
 * @param[in]   data   data for lcd registers
 * @param[in]   length   lengh of data
 * @return nothing
 */
static void rm67162_send_color(void * data, uint16_t length)
{
    disp_wait_for_pending_transactions();

#if defined (CONFIG_LV_DISPLAY_USE_SPI_CS)
    gpio_set_level(CONFIG_LV_DISP_SPI_CS, 0);        
    disp_spi_send_colors(data, length);
    gpio_set_level(CONFIG_LV_DISP_SPI_CS, 1);
#else    
    disp_spi_send_colors(data, length);
#endif   
}

/**
 * @brief                     rotation of lcd
 * @param[in]   orientation   0:portrait 1:landscape 
 *                            2:inverted portrait 3:inverted landscape
 * @return nothing
 */
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

    //ESP_LOGI(TAG, "RM67162_MADCTL command value: 0x%02X", data[orientation]);

    rm67162_send_cmd(RM67162_MADCTL);
    rm67162_send_data(&data[0], 1);
}
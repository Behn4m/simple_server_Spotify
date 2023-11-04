
#include"main.h"

extern QueueHandle_t BufQueue1;
extern SemaphoreHandle_t GetResponseSemaphore;





void GlobalInit()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // wifi_connection();
    GetResponseSemaphore = xSemaphoreCreateBinary();
    BufQueue1 = xQueueCreate(1, sizeof(char) * sizeof(char[2500]));
    vTaskDelay((3 * 500) / portTICK_PERIOD_MS);
}
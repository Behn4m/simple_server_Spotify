
#include "main.h"
static const char *TAG = "Global init";
extern QueueHandle_t BufQueue1;
extern SemaphoreHandle_t GetResponseSemaphore;
extern SemaphoreHandle_t FinishWifiConfig;
/**
 * in this function we init hardware or variable that need them
 *  globally
 */

void GlobalInit()
{

    GetResponseSemaphore = xSemaphoreCreateBinary();
    BufQueue1 = xQueueCreate(1, sizeof(char) * sizeof(char[2500]));
    FinishWifiConfig = xSemaphoreCreateBinary();
    wifiConnectionModule();
    if (xSemaphoreTake(FinishWifiConfig,portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGI(TAG, "wifi configuration get finish !");
    }
}
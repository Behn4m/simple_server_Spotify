#include "GlobalInit.h"
extern QueueHandle_t BufQueue1;
extern SemaphoreHandle_t GetResponseSemaphore;
/**
 * in this function we init hardware or variable that need them
 *  globally
 */
void GlobalInit()
{
    GetResponseSemaphore = xSemaphoreCreateBinary();
    BufQueue1 = xQueueCreate(1, sizeof(char) * sizeof(char[2500]));
    vTaskDelay((3 * 500) / portTICK_PERIOD_MS);
    // wifiConnectionTaskCreation();
}
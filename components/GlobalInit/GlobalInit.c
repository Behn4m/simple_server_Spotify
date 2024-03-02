#include "GlobalInit.h"

static const char *TAG = "Global init";
SemaphoreHandle_t FinishWifiConfig = NULL;

#ifdef SpotifyEnable
SemaphoreHandle_t WifiParamExistenceCheckerSemaphore = NULL;
SemaphoreHandle_t IsSpotifyAuthorizedSemaphore = NULL;

#endif
/**
 * in this function we init hardware or variable that need them
 *  globally
 */
void GlobalInit()
{
    FinishWifiConfig = xSemaphoreCreateBinary();
    WifiParamExistenceCheckerSemaphore = xSemaphoreCreateBinary();
#ifdef SpotifyEnable
    IsSpotifyAuthorizedSemaphore = xSemaphoreCreateBinary();
#endif

    ESP_LOGI(TAG, "Eventloop create");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}
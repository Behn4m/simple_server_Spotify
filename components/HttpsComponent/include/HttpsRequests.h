#ifdef __cplusplus
extern "C" {
#endif
#ifndef HTTPS_COMPONENT_H_
#define HTTPS_COMPONENT_H_

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_sntp.h"
#include "esp_netif.h"
#include "freertos/queue.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_tls.h"
#include "sdkconfig.h"
#include "time_sync.h"
#include "GlobalInit.h"

typedef struct
{
    QueueHandle_t *BufQueue1;
    SemaphoreHandle_t *HttpsResponseReadySemaphore;
} HttpsRequestsHandler_t;

/**
 * @brief Initializes the HTTPS requests handler.
 * @param HttpsRequestsHandler Pointer to the HTTPS requests handler structure.
 * @return Returns true if initialization succeeds, false otherwise.
 */
bool Https_ComponentInit(HttpsRequestsHandler_t *pHandler);

/**
* @brief This function performs an HTTPS GET request to a specified server URL with the provided header request.
* @param[in] HeaderOfRequest The header of the HTTPS request.
* @param[in] SizeHeaderOfRequest The size of the header of the HTTPS request.
* @param[in] Url The URL to which the request should be sent.
* @param[in] SizeUrl The size of the URL.
* @param[in] Server The server address.
* @param[in] SizeServer The size of the server address.
*/
void HttpsHandler(char *HeaderOfRequest, size_t SizeHeaderOfRequest, char* Url,size_t SizeUrl,char* Server,size_t SizeServer);

#endif
#ifdef __cplusplus
}
#endif

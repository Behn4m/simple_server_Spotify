#ifdef __cplusplus
extern "C" {
#endif
#ifndef WIFI_CONFIG_H_
#define WIFI_CONFIG_H_
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_mac.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi_types.h"
#include "mdns.h"
#include <string.h>
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include"GlobalInit.h"
extern SemaphoreHandle_t WaitSemaphore ;
extern SemaphoreHandle_t ExitFromApModeSemaphore ;
extern SemaphoreHandle_t StayInApModeSemaphore ;
extern esp_netif_t * NetifAccessPointStruct ;

/**
 * @brief Entry point for the Wi-Fi connection task.
 * This function is the entry point for the Wi-Fi connection task. It initializes necessary components, sets up the SPIFFS, starts the mDNS service, starts the web server, and waits for Wi-Fi connection events.
 */
void wifiConnectionModule();

#ifdef WIFI_INIT_STA_MODE
/**
 * @brief Sets up WiFi station mode.
 * @param[in] ssid      SSID of the access point.
 * @param[in] password  Password of the access point.
 * @return esp_err_t
 */
esp_err_t WifiStationMode(char *UserWifiSSID_, char *UserWifiPassWord_);
#endif

#endif
#ifdef __cplusplus
}
#endif
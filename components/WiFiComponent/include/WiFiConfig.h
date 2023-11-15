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
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi_types.h"
#include "mdns.h"
#include <string.h>
#include "esp_vfs.h"
#include "esp_spiffs.h"


extern bool ForFirstTimeFlag ;
extern SemaphoreHandle_t WaitSemaphore ;
extern SemaphoreHandle_t ExitFromApModeSemaphore ;
extern SemaphoreHandle_t StayInApModeSemaphore ;
extern esp_netif_t * NetifAccessPointStruct ;
void wifiConnectionModule();
esp_err_t WifiStationMode(char *UserWifiSSID_, char *UserWifiPassWord_);
esp_err_t WifiSoftAccessPointMode(char *WifiAccessPointSSID, char *WifiAccessPointPassWord);




#endif



#ifdef __cplusplus
}
#endif
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
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_mac.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi_types.h"
#include "mdns.h"
#include <string.h>
#include <nvs_flash.h>
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "esp_mac.h"

void wifiConnectionTaskCreation();
void WifiSTAMode();

#define EXAMPLE_ESP_WIFI_SSID "ESP32-S3"
#define EXAMPLE_ESP_WIFI_PASS "87654321"
#define EXAMPLE_MAX_STA_CONN 5




#endif



#ifdef __cplusplus
}
#endif
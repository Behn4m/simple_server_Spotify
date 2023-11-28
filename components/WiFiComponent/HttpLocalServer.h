#ifdef __cplusplus
extern "C" {
#endif
#ifndef HTTP_LOCAL_SERVER_
#define HTTP_LOCAL_SERVER_
#include "WiFiConfig.h"
#define ESP_WIFI_SSID "ESP32-S3"
#define ESP_WIFI_PASS "87654321"
#define MAX_STA_CONN 5
struct UserWifiStruct 
{
    char PassWord[64];
    char SSID[32];
};
extern struct UserWifiStruct UserWifi;
// /**
//  * @brief Initializes the SPIFFS (SPI Flash File System).
//  * This function initializes the SPIFFS file system with the provided configuration. It registers the SPIFFS file system and checks the partition information.
//  */
// void SpiffsInit();

/**
 * @brief Starts a web server locally.
 * This function starts a web server locally using the default configuration. It registers various URI handlers for different endpoints and returns the server handle.
 * @returns The handle to the started web server.
 */
 httpd_handle_t StartWebServerLocally(void);

/**
 * @brief Stops a running web server.
 * This function stops a running web server identified by the provided server handle.
 * @param[in] server The handle to the web server.
 * @returns An esp_err_t indicating the success or failure of stopping the web server.
 */

 esp_err_t StopWebServer(httpd_handle_t server);
/**
 * @brief Initializes and starts the mDNS service.
 * This function initializes and starts the mDNS (Multicast DNS) service. It sets the hostname and instance name for mDNS.
 */
void StartMDNSServiceForWifi();

#endif
#ifdef __cplusplus
}
#endif
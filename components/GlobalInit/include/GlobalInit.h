#ifdef __cplusplus
extern "C" {
#endif
#ifndef GLOBAL_INIT_H_
#define GLOBAL_INIT_H_
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "SpiffsManger.h"

// **************************************************************** applicaiton configurations
#define SpotifyEnable
#define WIFI_INIT_STA_MODE

// spiffs directories
#define WifiConfigDirectoryAddressInSpiffs  "/spiffs/WifiConfig.txt"
#define SpotifyConfigAddressInSpiffs "/spiffs/SpotifyConfig.txt"

// **************************************************************** constant macros
#define Sec 1000
#define Hour 3600
#define LONGBUF 2500
#define MEDIUMBUF 1000
#define SMALLBUF   250
#define SpotifyTaskStackSize 10*1000
#define HttpsTaskStackSize   9*1000
#define WifiModuleTaskStackSize   10*1000

// **************************************************************** URI links
#define ReDirectUri "http%3A%2F%2Fdeskhub.local%2Fcallback%2f"
#define ClientId  "55bb974a0667481ab0b2a49fd0abea6d"

extern SemaphoreHandle_t WifiParamExistenceCheckerSemaphore;
extern SemaphoreHandle_t FinishWifiConfig;

#ifdef SpotifyEnable
extern SemaphoreHandle_t IsSpotifyAuthorizedSemaphore;
/**
 * timeout definition part 
*/
#define SPOTIFY_RESPONSE_TIMEOUT (30*1000)/portTICK_PERIOD_MS
#endif

// **************************************************************** initilization functions
/**
 * in this function we init hardware or variable that need them
 *  globally
 */
void GlobalInit();
#endif
#ifdef __cplusplus
}
#endif
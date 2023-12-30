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
#define SEC 1000
#define HOUR 3600
#define LONGBUF 2500
#define MEDIUMBUF 1000
#define SMALLBUF   250
#define SPOTIFY_TASK_STACK_SIZE (uint16_t)(10*1000U)
#define HttpsTaskStackSize   (uint16_t)(9*1000U)
#define WifiModuleTaskStackSize   (uint16_t)(10*1000U)

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
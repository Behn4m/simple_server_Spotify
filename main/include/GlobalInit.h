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
#include "SpotifyAuthorization.h"
#include "HttpsRequests.h"
#include "WiFiConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "nvsFlash.h"
#define LONGBUF 2500
#define MEDIUMBUF 1000
#define SMALLBUF   250
#define SpotifyTaskStackSize 10*1000
#define HttpsTaskStackSize   9*1000
/**
 * in this function we init hardware or variable that need them
 *  globally
 */
void GlobalInit();
#define ReDirectUri "http%3A%2F%2Fdeskhub.local%2Fcallback%2f"
#define ClientId  "55bb974a0667481ab0b2a49fd0abea6d"
struct Token_
{
    char access_token[500];
    char token_type[20];
    int expires_in;
    char refresh_token[500];
    char scope[200];
};
struct UserInfo_
{
    char DisplayName[128];
    char SpotifyProfileURL[256];
    char UserID[128];
    char Image1[256];
    char Image2[256];
    int Follower;
    char Country[30];
    char Product[30];
};
#endif
#ifdef __cplusplus
}
#endif


#ifndef MAIN_H_
#define MAIN_H_

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "protocol_examples_common.h"
#include "SpotifyAuthorization.h"
#include "HttpsRequest_.h"
#include "WiFiConfig_.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "nvsFlash.h"

#define MYSSID "Hardware12"
#define MYPASS "87654321"

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


// CONFIG_LV_FONT_MONTSERRAT_12=y
// CONFIG_LV_FONT_MONTSERRAT_14=y
// CONFIG_LV_FONT_MONTSERRAT_16=y
// CONFIG_LV_FONT_MONTSERRAT_18=y
// CONFIG_LV_TFT_DISPLAY_CONTROLLER_RM67162=y
// CONFIG_LV_TFT_DISPLAY_PROTOCOL_SPI=y
// CONFIG_LV_DISPLAY_ORIENTATION_PORTRAIT=y
#endif
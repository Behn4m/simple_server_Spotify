#ifdef __cplusplus
extern "C"
{
#endif
#ifndef TYPE_DEF_H_
#define TYPE_DEF_H_
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include <esp_https_server.h>
#include "esp_tls.h"
#include "sdkconfig.h"
#include "mdns.h"
#include "freertos/queue.h"
#include"esp_psram.h"
// **************************************************************** URI links
#define ReDirectUri "http%3A%2F%2Fdeskhub.local%2Fcallback%2f"
#define ClientId "55bb974a0667481ab0b2a49fd0abea6d"

// ****************************************************************
#define SpotifyEventStack (uint32_t)20 * 1000

#define SEC 1000
#define HOUR 3600
#define LONG_BUF 2500
#define MEDIUM_BUF 1000
#define SMALL_BUF 250

#define ACCESS_TOKEN_STR_SIZE 512
#define TOKEN_TYPE_STR_SIZE 20
#define REFRESH_TOKEN_STP_SIZE 512
#define GRANTED_SCOP_STR_SIZE 512

#define DISPLAY_NAME_STR_SIZE 128
#define PROFILE_STR_SIZE 256
#define USER_ID_SIZE 128
#define IMAGE1_STR_SIZE 256
#define IMAGE2_STR_SIZE 25
#define COUNTERY_STR_SIZE 30
#define PRODUCT_STR_SIZE 30

#define IDLE 0
#define AUTHENTICATED 1
#define AUTHORIZED 2
#define SAVE_NEW_TOKEN 3
#define EXPIRED 4
#define CHECK_TIME 5

#define EVENT_LOOP_QUEUE 5

    typedef struct Token_t
    {
        char AccessToken[ACCESS_TOKEN_STR_SIZE];
        char TokenType[TOKEN_TYPE_STR_SIZE];
        int ExpiresInMS;
        char RefreshToken[REFRESH_TOKEN_STP_SIZE];
        char GrantedScope[GRANTED_SCOP_STR_SIZE];
    } Token_t;

    typedef struct UserInfo_t
    {
        char DisplayName[DISPLAY_NAME_STR_SIZE];
        char ProfileURL[PROFILE_STR_SIZE];
        char UserID[USER_ID_SIZE];
        char Image1[IMAGE1_STR_SIZE];
        char Image2[IMAGE2_STR_SIZE];
        int Follower;
        char Country[COUNTERY_STR_SIZE];
        char Product[PRODUCT_STR_SIZE];
    } UserInfo_t;

    typedef enum
    {
        idle = 0,
        authorized = 1,
        active_user = 2,
        save_new_token=3,
        expired_user = 4,
        check_time=5

    } Status_t;

    typedef void (*EventHandlerCallBackPtr)(char *Buffer);

    typedef struct
    {
        char *code;
        Token_t token;       // Nested struct for token information
        UserInfo_t userInfo; // Nested struct for user information
        Status_t status;
    } SpotifyPrivateHandler_t;
    typedef struct
    {
        EventHandlerCallBackPtr EventHandlerCallBackFunction;
        Token_t *token;
        QueueHandle_t *HttpsBufQueue;
        UserInfo_t UserInfo;
    } EventHandlerDataStruct_t;
    typedef struct
    {
        Status_t *status;
        QueueHandle_t *SendCodeFromHttpToSpotifyTask;
    } HttpLocalServerParam_t;
#endif
#ifdef __cplusplus
}
#endif
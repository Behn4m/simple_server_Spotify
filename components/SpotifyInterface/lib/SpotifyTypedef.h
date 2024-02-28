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
#include "esp_psram.h"
// **************************************************************** URI links
#define ReDirectUri "http%3A%2F%2Fdeskhub.local%2Fcallback%2f"
#define ClientId "55bb974a0667481ab0b2a49fd0abea6d"

// ****************************************************************
#define SPOTIFY_TASK_STACK_SIZE (uint32_t)(10*1000U)
#define SPOTIFY_PRIORITY 4

#define SEC                             1000
#define HOUR                            3600
#define SUPER_BUF                       10000
#define LONG_BUF                        2500
#define MEDIUM_BUF                      1000
#define SMALL_BUF                       250

#define ACCESS_TOKEN_STR_SIZE           512
#define TOKEN_TYPE_STR_SIZE             20
#define REFRESH_TOKEN_STP_SIZE          512
#define GRANTED_SCOP_STR_SIZE           512

#define DISPLAY_NAME_STR_SIZE           128
#define PROFILE_STR_SIZE                256
#define USER_ID_SIZE                    128
#define IMAGE1_STR_SIZE                 256
#define IMAGE2_STR_SIZE                 25
#define COUNTERY_STR_SIZE               30
#define PRODUCT_STR_SIZE                30

#define INIT                            0
#define LOGIN                           1 
#define AUTHENTICATED                   2
#define AUTHORIZED                      3
#define EXPIRED                         4
#define CHECK_TIME                      5

typedef struct Token_t
{
    char AccessToken[ACCESS_TOKEN_STR_SIZE];
    char TokenType[TOKEN_TYPE_STR_SIZE];
    char RefreshToken[REFRESH_TOKEN_STP_SIZE];
    char GrantedScope[GRANTED_SCOP_STR_SIZE];
    int ExpiresInMS;
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
    INIT_SERVICE = 0,
    LOGIN_USER = 1,
    AUTHENTICATED_USER = 2,
    AUTHORIZED_USER = 3,
    EXPIRED_USER = 4,
    CHECK_TIME_USER = 5
} Status_t;

typedef void (*EventHandlerCallBackPtr)(char *Buffer);

typedef struct
{
    char *Code;                     // code received from Apotify api
    Token_t token;                  // Nested struct for token information
    TickType_t TokenLastUpdate;     // System Tick of last token update
    UserInfo_t UserInfo;            // Nested struct for user information
    Status_t Status;                // state machine
    char *SpotifyBuffer;             // Buffer for https request
    bool IsResponseReady;           // Flag for response ready 
} SpotifyPrivateHandler_t;


#endif
#ifdef __cplusplus
}
#endif
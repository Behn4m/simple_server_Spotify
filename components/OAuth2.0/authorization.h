#ifdef __cplusplus
extern "C"
{
#endif //cpp
#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

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
#include "SpotifyWebAppInfo.h"

#define SPOTIFY_TASK_STACK_SIZE (uint32_t)(30*1000U)
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

#define INIT                            0
#define LOGIN                           1
#define AUTHENTICATED                   2
#define AUTHORIZED                      3
#define EXPIRED                         4
#define CHECK_TIME                      5

typedef enum
{
    INIT_SERVICE = 0,
    LOGIN_USER = 1,
    AUTHENTICATED_USER = 2,
    AUTHORIZED_USER = 3,
    EXPIRED_USER = 4,
    CHECK_TIME_USER = 5
} Status_t;

typedef struct Token_t
{
    char AccessToken[ACCESS_TOKEN_STR_SIZE];
    char TokenType[TOKEN_TYPE_STR_SIZE];
    char RefreshToken[REFRESH_TOKEN_STP_SIZE];
    char GrantedScope[GRANTED_SCOP_STR_SIZE];
    int ExpiresInMS;
} Token_t;

typedef struct APIBuffer_t
{
    char *MessageBuffer;
    int64_t status;
    int64_t ContentLength;
    SemaphoreHandle_t ResponseReadyFlag;
} APIBuffer_t;

typedef struct
{
    char *Code;                                    // code received from Apotify api
    Token_t token;                                   // Nested struct for token information
    TickType_t TokenLastUpdate;                    // System Tick of last token update
    Status_t Status;                              // state machine
    APIBuffer_t ServiceBuffer;            // Buffer for https request
} PrivateHandler_t;

/**
 * @brief This function extracts specific parameters from a JSON string and assigns them to corresponding fields in a TokenParam structure.
 * @param[in] Json The input JSON string.
 * @param[in] token address to save tokin extracted information.
 * @return fasle if fail, true if finish successfull.
 */
bool ExtractTokenJson(char *Json, Token_t *token) ;

#endif //file

#ifdef __cplusplus
}
#endif // cpp
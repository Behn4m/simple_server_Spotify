#ifdef __cplusplus
extern "C"
{
#endif //cpp
#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include "esp_http_client.h"
#include "esp_tls.h"
#include "mdns.h"
#include "freertos/queue.h"
#include "esp_psram.h"

#define SERVICE_TASK_STACK_SIZE (uint32_t)(30*1000U)
#define SERVICE_PRIORITY 4

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

typedef struct HttpClientInfo_t
{
    char *url;
    char *host;
    char *path;
    char *requestURI;
    char *responseURI;
    char *hostname;
    char *requestURL;
    char *clientID;
    char *base64Credintials;
    char *redirectURL;
} HttpClientInfo_t;

typedef struct OAuthInterfaceHandler_t
{
    char *ConfigAddressInSpiffs;
    SemaphoreHandle_t *IsServiceAuthorizedSemaphore;
    HttpClientInfo_t ClientConfig;
} OAuthInterfaceHandler_t;

typedef struct OAuthPrivateHandler_t
{
    Token_t token;                                   // Nested struct for token information
    TickType_t TokenLastUpdate;                    // System Tick of last token update
    Status_t Status;                              // state machine
    APIBuffer_t OAuthBuffer;            // Buffer for https request
} OAuthPrivateHandler_t;

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Oauth_TaskInit(OAuthInterfaceHandler_t *OAuthInterfaceHandler);

/**
 * @brief This function returns the access token.
 * @param OAuthInterfaceHandler as the handler
 * @return the access token
 */
char* Oauth_GetAccessToken(void);

#endif //file

#ifdef __cplusplus
}
#endif // cpp
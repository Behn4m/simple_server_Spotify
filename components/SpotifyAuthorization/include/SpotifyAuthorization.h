#ifdef __cplusplus
extern "C" {
#endif
#ifndef HTTPS_SPOTIFY_H
#define HTTPS_SPOTIFY_H
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
#include "GlobalInit.h"
#include "MakeSpotifyRequest.h"
#include "freertos/queue.h"

#define access_token_str_size   512
#define token_type_str_size     20
#define refresh_token_str_size  512
#define granted_scope_str_size  512

#define DisplayName_str_size    128
#define ProfileURL_str_size     256
#define UserID_str_size         128
#define Image1_str_size         256
#define Image2_str_size         256
#define Country_str_size        30
#define Product_str_size        30

typedef struct Token_t {
    char access_token[access_token_str_size];
    char token_type[token_type_str_size];
    int  expires_in_ms;
    char refresh_token[refresh_token_str_size];
    char granted_scope[granted_scope_str_size];
} Token_t;

typedef struct UserInfo_t {
    char DisplayName[DisplayName_str_size];
    char ProfileURL[ProfileURL_str_size];
    char UserID[UserID_str_size];
    char Image1[Image1_str_size];
    char Image2[Image2_str_size];
    int Follower;
    char Country[Country_str_size];
    char Product[Product_str_size];
} UserInfo_t;

typedef enum {
    IDLE = 0,
    AUTHORIZED = 1,
    ACTIVE_USER = 2,
    EXPIRED_USER = 3
} TokenStatus;

typedef struct {
    QueueHandle_t *BufQueue;
    char    code[MEDIUMBUF];
    Token_t token;        // Nested struct for token information
    UserInfo_t userInfo;  // Nested struct for user information
    TokenStatus    status;
} SpotifyInterfaceHandler_t;

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Spotify_TaskInit(SpotifyInterfaceHandler_t *SpotifyInterfaceHandler);
#endif

#ifdef __cplusplus
}
#endif

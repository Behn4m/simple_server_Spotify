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

typedef struct Token_t {
    char access_token[500];
    char token_type[20];
    int expires_in_ms;
    char refresh_token[500];
    char granted_scope[200];
} Token_t;

typedef struct UserInfo_t {
    char DisplayName[128];
    char SpotifyProfileURL[256];
    char UserID[128];
    char Image1[256];
    char Image2[256];
    int Follower;
    char Country[30];
    char Product[30];
} UserInfo_t;

typedef struct {
    QueueHandle_t *BufQueue;
    char    code[MEDIUMBUF];
    Token_t token;        // Nested struct for token information
    UserInfo_t userInfo;  // Nested struct for user information
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

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

/**
 * @brief This function handles the Spotify authorization process.
 */
void Spotify_TaskInit();
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef TYPE_DEF_H_
#define TYPE_DEF_H_
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include "mdns.h"
#include "freertos/queue.h"
#include "esp_psram.h"

// ****************************************************************
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

#endif
#ifdef __cplusplus
}
#endif
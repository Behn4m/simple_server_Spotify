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
// **************************************************************** URI links
#define ReDirectUri "http%3A%2F%2Fdeskhub.local%2Fcallback%2f"
#define ClientId "55bb974a0667481ab0b2a49fd0abea6d"

// ****************************************************************
#define SpotifyEventStack (uint32_t)20*1000

#define Sec 1000
#define Hour 3600
#define LONGBUF 2500
#define MEDIUMBUF 1000
#define SMALLBUF 250

#define access_token_str_size 512
#define token_type_str_size 20
#define refresh_token_str_size 512
#define granted_scope_str_size 512

#define DisplayName_str_size 128
#define ProfileURL_str_size 256
#define UserID_str_size 128
#define Image1_str_size 256
#define Image2_str_size 256
#define Country_str_size 30
#define Product_str_size 30

#define IDLE 0
#define AUTHORIZED 1
#define ACTIVE_USER 2
#define EXPIRED_USER 3

#define NO_COMMAND 0
#define PLAY_PAUSE 1
#define PLAY_NEXT 2
#define PLAY_PREV 3
#define STOP 4
#define PLAY 5
#define PAUSE 6
#define GET_NOW_PLAYING 7
#define GET_USER_INFO 8
#define GET_SONG_IMAGE_URL 9
#define GET_ARTIST_IMAGE_URL 10

    typedef struct Token_t
    {
        char access_token[access_token_str_size];
        char token_type[token_type_str_size];
        int expires_in_ms;
        char refresh_token[refresh_token_str_size];
        char granted_scope[granted_scope_str_size];
    } Token_t;

    typedef struct UserInfo_t
    {
        char DisplayName[DisplayName_str_size];
        char ProfileURL[ProfileURL_str_size];
        char UserID[UserID_str_size];
        char Image1[Image1_str_size];
        char Image2[Image2_str_size];
        int Follower;
        char Country[Country_str_size];
        char Product[Product_str_size];
    } UserInfo_t;

    typedef enum
    {
        idle = 0,
        authorized = 1,
        active_user = 2,
        expired_user = 3
    } Status_t;

    typedef enum
    {
        NoCommand = 0,
        PlayPause = 1,
        PlayNext = 2,
        PlayPrev = 3,
        Stop = 4,
        Play = 5,
        Pause = 6,
        GetNowPlaying = 7,
        GetUserInfo = 8,
        GetSongImageUrl = 9,
        GetArtisImageUrl = 10,
    } Command_t;

    typedef void (*ReadTxtFileFromSpiffsPtr)(char *addressInSpiffs, char *key, char *value, ...);
    typedef void (*WriteTxtFileToSpiffsPtr)(char *addressInSpiffs, char *key, char *value, ...);
    typedef bool (*CheckAddressInSpiffsPtr)(char *addressInSpiffs);
    typedef void (*EventHandlerCallBackPtr)(char *Buffer);

    typedef struct
    {
        char *code;
        Token_t token;       // Nested struct for token information
        UserInfo_t userInfo; // Nested struct for user information
        Status_t status;
        Command_t command;
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
        Status_t status;
        QueueHandle_t *HttpsBufQueue;
    }HttpLocalServerParam_t;


#endif
#ifdef __cplusplus
}
#endif
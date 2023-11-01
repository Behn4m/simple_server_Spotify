

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
#include "main.h"



void GetCurrentPlaying();
void SendRequest_AndGiveTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_);
void ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson);
void SpotifyComponent();
bool FindToken(char *Res, uint16_t SizeRes);
bool FindCode(char *Res, uint16_t SizeRes);
void SendRequest_AndGiveToken(char *Buf, size_t SizeBuf, char *code, size_t SizeCode);
void SendToPlayerCommand(const char *Method_, const char *Command_, char *Buf, size_t SizeBuf);
void SendRequestForPause();
void SendRequestForPlay();
void SendRequestForPrevious();
void SendRequestForNext();
void UserStatus();
void GetUserTopItems();
void GetUserProfile(char *UserId_);
void ExtractionJsonParamForFindAccessToken(char *Json, size_t SizeJson);

#endif
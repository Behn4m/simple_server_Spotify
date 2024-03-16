#ifdef __cplusplus
extern "C" {
#endif
#ifndef   MAKE_SPOTIFY_REQUEST_H_
#define   MAKE_SPOTIFY_REQUEST_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "esp_http_client.h"
#include "SpotifyInterface.h"
#include"SpotifyTypedef.h"
#include "esp_crt_bundle.h"

/**
 * @brief This function sends a request to the Spotify login API to authorize the user.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @return This function does not return a value.
 */
void SpotifyAPICallInit(SpotifyAPIBuffer_t *SpotifyAPIBuffer);

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *RefreshToken_);

/**
* @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
* @param[in] code is parameter recived from Spotify api.
* @return This function does not return a value.
*/
void Spotify_SendTokenRequest(char *Code);

/**
* @brief This function sends a request to the Spotify API to perform a player command.
* @param[in] Command The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[in] AcessToken the spotify authorized AccessToken.
* @return This function does not return a value.
*/
void Spotify_ControlPlayback(int Command, char *AccessToken);

/**
* @brief This function sends a request to the Spotify API to give Curent playing information 
* @param[in]  Token is the access token received from Spotify during authorization progress 
* @param[in]  Command is the specific player command to be executed (e.g., "next", "previous", "play", "pause").
*/
void Spotify_GetInfo(int Command, char *AccessToken);

#endif
#ifdef __cplusplus
}
#endif


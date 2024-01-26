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

/**
* @brief This function sends a request to the Spotify API to give Curent playing information 
* @param[in]  Token_t *Token 
*/
void Spotify_GetCurrentPlaying(Token_t *Token);

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_);

/**
* @brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was found.
* @param[in] Res The character array to search within, and Res is response from first stage from spotify athurisiation
* @param[in] SizeRes The size of the character array.
* @return Returns true if either the 'code' or 'state' pattern was found, and false otherwise.
*/
bool Spotify_FindCode(char *Res, uint16_t SizeRes);

/**
* @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
* @param[in,out] Buf The character buffer to store the request and receive the response.
* @param[in] SizeBuf The size of the character buffer.
* @param[in] code is parameter that we give it before .
* @param[in] SizeCode The size of the authorization code.
* @return This function does not return a value.
*/
void Spotify_SendTokenRequest(char *code, size_t SizeCode);

/**
* @brief This function sends a request to the Spotify API to perform a player command.
* @param[in] Command The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[in] AcessToken the spotify authorized AccessToken.
* @return This function does not return a value.
*/
void Spotify_ControlPlayback(int Command, char *AccessToken);

/**
* @brief This function sends a request to the Spotify API to retrieve the user's current status.
* @param[in]  char *AccessToken 
* @return This function does not return a value.
*/
void Spotify_GetUserStatus(char *AccessToken);

/**
* @brief This function sends a request to the Spotify API to retrieve the user's top item.
* @param[in]  char *AccessToken 
* @return This function does not return a value.
*/
void Spotify_GetUserTopItems(char *AccessToken);

/**
* @brief This function sends a request to the Spotify API to retrieve the profile information of a specific user.
* @param[in] char *AccessToken
* @param[in] char *UserId_
* @return This function does not return a value.
*/
void Spotify_GetUserProfile(char *UserId_, char *Token);
#endif
#ifdef __cplusplus
}
#endif


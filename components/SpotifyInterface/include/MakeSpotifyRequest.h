#ifdef __cplusplus
extern "C" {
#endif
#ifndef   MAKE_SPOTIFY_REQUEST_H_
#define   MAKE_SPOTIFY_REQUEST_H_
#include "SpotifyInterface.h"

/**
* @brief This function sends a request to the Spotify API to give Curent playing information 
*/
void GetCurrentPlaying();

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_);

/**
* @brief This function searches for a token within a character array and extracts the corresponding JSON object.
* @param[in] Res The character array to search within. and Res is response from first stage from spotify athurisiation
* @param[in] SizeRes The size of the character array.
* @return Returns true if the token is found and the corresponding JSON object is successfully extracted, otherwise returns false.
*/
bool Spotify_FindToken(char *Res, uint16_t SizeRes);

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
void Spotify_SendTokenRequest(char *Buf, size_t SizeBuf, char *code, size_t SizeCode);

/**
* @brief This function sends a request to the Spotify API to perform a player command.
* @param[in] Method_ The HTTP method to be used in the request (e.g., "POST", "PUT").
* @param[in] Command_ The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[out] Buf The character buffer to store the request and receive the response.
* @param[in] SizeBuf The size of the character buffer.
* @return This function does not return a value.
*/
void MakePlayerCommandAndSendIt(const char *Method_, const char *Command_, char *Buf, size_t SizeBuf, char *access_token);

/**
* @brief This function sends a request to the Spotify API to pause the current track in the player.
* @return This function does not return a value.
*/
void SendRequestForPause();

/**
* @brief This function sends a request to the Spotify API to play the current track in the player.
* @return This function does not return a value.
*/
void SendRequestForPlay();

/**
* @brief This function sends a request to the Spotify API to skip to the previous track in the player.
* @return This function does not return a value.
*/
void SendRequestForPrevious();

/**
* @brief This function sends a request to the Spotify API to skip to the next track in the player.
* @return This function does not return a value.
*/
void SendRequestForNext();

/**
* @brief This function sends a request to the Spotify API to retrieve the user's current status.
* @return This function does not return a value.
*/
void GetUserStatus(char *access_token);

/**
* @brief This function sends a request to the Spotify API to retrieve the user's top item.
* @return This function does not return a value.
*/
void GetUserTopItems(char *access_token);

/**
* @brief This function sends a request to the Spotify API to retrieve the profile information of a specific user.
* @param[in] UserId_ The ID of the user whose profile information is to be retrieved.
* @return This function does not return a value.
*/
void GetUserProfile(char *UserId_, char *Token);


#endif
#ifdef __cplusplus
}
#endif


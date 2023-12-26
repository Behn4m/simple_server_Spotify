
// #include "GlobalInit.h"
#include "SpotifyMakeRequest.h"
// extern struct Token_t TokenParam;
// extern struct UserInfo_t UserInfo;
static const char *TAG = "SpotifyTask";

/**
* @brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was found.
* @param[in] Res The character array to search within, and Res is response from first stage from spotify athurisiation
* @param[in] SizeRes The size of the character array.
* @return Returns true if either the 'code' or 'state' pattern was found, and false otherwise.
*/
bool Spotify_FindCode(char *Res, uint16_t SizeRes)
{
    char *codeString = {"code"};
    uint16_t codeLength = strlen(codeString);

    if (Res == NULL || SizeRes < codeLength) 
    {
        // Invalid input, either null pointer or insufficient buffer size
        return false;
    }

    for (uint16_t i = 0; i <= SizeRes - codeLength; ++i) 
    {
        bool found = true;
        for (uint16_t j = 0; j < codeLength; ++j)
        {
            if (Res[i + j] != codeString[j])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return true; // Found the access token substring
        }
    }
    return false; // Access token substring not found
}


/**
* @brief This function searches for a token within a character array and extracts the corresponding JSON object.
* @param[in] Res The character array to search within. and Res is response from first stage from spotify athurisiation
* @param[in] SizeRes The size of the character array.
* @return Returns true if the token is found and the corresponding JSON object is successfully extracted, otherwise returns false.
*/
bool Spotify_FindToken(char *Res, uint16_t SizeRes) {
    const char *tokenString = "{\"access_token\"";
    uint16_t tokenLength = strlen(tokenString);

    if (Res == NULL || SizeRes < tokenLength)
    {
        // Invalid input, either null pointer or insufficient buffer size
        return false;
    }

    for (uint16_t i = 0; i <= SizeRes - tokenLength; ++i) 
    {
        bool found = true;
        for (uint16_t j = 0; j < tokenLength; ++j)
        {
            if (Res[i + j] != tokenString[j])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return true; // Found the access token substring
        }
    }

    return false; // Access token substring not found
}

/**
* @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
* @param[in,out] Buf The character buffer to store the request and receive the response.
* @param[in] SizeBuf The size of the character buffer.
* @param[in] code is parameter that we give it before .
* @param[in] SizeCode The size of the authorization code.
* @return This function does not return a value.
*/
void Spotify_SendTokenRequest(char *Buf, size_t SizeBuf, char *code, size_t SizeCode)
{
    char grand[MEDIUMBUF] = {0};
    ESP_LOGI(TAG,"\n%s\n", code);
    sprintf(grand, "grant_type=authorization_code&redirect_uri=%s&%s", ReDirectUri, code);
    memset(Buf, 0x0, SizeBuf);
    sprintf(Buf, "POST /api/token HTTP/1.1 \r\n"
                 "Host: accounts.spotify.com\r\n"
                 "Authorization: Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=\r\n"
                 "Content-Length: %d\r\n"
                 "Cookie: __Host-device_id=AQAwmp7jxagopcWw89BjSDAA530mHwIieOZdJ9Im8nI0-70oEsSInx3jkeSO09YQ7sPgPaIUyMEvZ-tct7I6OlshJrzVYOqcgo0; sp_tr=false\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "\r\n"
                 "%s\r",
            strlen(grand), grand);
    char url[SMALLBUF] = "https://accounts.spotify.com/api/token";
    char server[SMALLBUF] = "accounts.spotify.com";
    HttpsHandler(Buf, SizeBuf, url, sizeof(url), server, sizeof(server));
}

/**
* @brief This function sends a request to the Spotify API to perform a player command.
* @param[in] Method_ The HTTP method to be used in the request (e.g., "POST", "PUT").
* @param[in] Command_ The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[out] Buf The character buffer to store the request and receive the response.
* @param[in] SizeBuf The size of the character buffer.
* @return This function does not return a value.
*/
void Spotify_MakePlayerCommandAndSendIt(const char *Method_, const char *Command_, char *Buf, size_t SizeBuf, char *access_token)
{
    //   copy tow of them because ,we dont give size of Method and command
    char Method[50] = {0};
    char Command[50] = {0};
    strcpy(Method, Method_);
    strcpy(Command, Command_);
    memset(Buf, 0x00, SizeBuf);
    sprintf(Buf, "%s /v1/me/player/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            Method, Command, access_token);
    ESP_LOGI(TAG,"\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[SMALLBUF] = "https://api.spotify.com";
    char server[SMALLBUF] = "api.spotify.com";
    HttpsHandler(Buf, SizeBuf, url, sizeof(url), server, sizeof(server));
}

/**
* @brief This function sends a request to the Spotify API to skip to the next track in the player.
* @return This function does not return a value.
*/
void Spotify_SendRequestForNext(Token_t *Token)
{
    char Buf[LONGBUF];
    Spotify_MakePlayerCommandAndSendIt("PUT", "pause", Buf, LONGBUF, Token->access_token);
}

/**
* @brief This function sends a request to the Spotify API to skip to the previous track in the player.
* @return This function does not return a value.
*/
void Spotify_SendRequestForPrevious(Token_t *Token)
{
    char Buf[LONGBUF];
    Spotify_MakePlayerCommandAndSendIt("PUT", "pause", Buf, LONGBUF, Token->access_token);
}

/**
* @brief This function sends a request to the Spotify API to play the current track in the player.
* @return This function does not return a value.
*/
void Spotify_SendRequestForPlay(Token_t *Token)
{
    char Buf[LONGBUF];
    Spotify_MakePlayerCommandAndSendIt("PUT", "pause", Buf, LONGBUF, Token->access_token);
}

/**
* @brief This function sends a request to the Spotify API to pause the current track in the player.
* @return This function does not return a value.
*/
void Spotify_SendRequestForPause(Token_t *Token)
{
    char Buf[LONGBUF];
    Spotify_MakePlayerCommandAndSendIt("PUT", "pause", Buf, LONGBUF, Token->access_token);
}

/**
* @brief This function sends a request to the Spotify API to retrieve the user's profile information.
* @return This function does not return a value.
*/
void Spotify_GetUserStatus(char *access_token)
{
    char Buf[LONGBUF];
    sprintf(Buf, "GET /v1/me/ HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            access_token);
    ESP_LOGI(TAG,"\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[SMALLBUF] = "https://api.spotify.com";
    char server[SMALLBUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}

/**
* @brief This function sends a request to the Spotify API to retrieve the user's top item.
* @return This function does not return a value.
*/
void Spotify_GetUserTopItems(char *access_token)
{
    char Buf[LONGBUF];
    sprintf(Buf, "GET /v1/me/top/artists HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            access_token);
    ESP_LOGI(TAG,"\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[SMALLBUF] = "https://api.spotify.com";
    char server[SMALLBUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}

/**
* @brief This function sends a request to the Spotify API to retrieve the profile information of a specific user.
* @param[in] UserId_ The ID of the user whose profile information is to be retrieved.
* @return This function does not return a value.
*/
void Spotify_GetUserProfile(char *UserId_, char *Token)
{
    char Buf[LONGBUF];
    char UserId[50];
    strcpy(UserId, UserId_);
    sprintf(Buf, "GET /v1/users/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            UserId, Token);

    ESP_LOGI(TAG,"\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[SMALLBUF] = "https://api.spotify.com";
    char server[SMALLBUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}

/**
* @brief This function sends a request to the Spotify API to give Curent playing information 
*/
void Spotify_GetCurrentPlaying(Token_t *Token)
{
    // return json is so long and we need longer buffer 
    char Buf[LONGBUF+1500]; 
    sprintf(Buf, "GET /v1/me/player/currently-playing HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Connection: close\r\n\r\n",
            Token->access_token);
    ESP_LOGI(TAG,"\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[SMALLBUF] = "https://api.spotify.com";
    char server[SMALLBUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_)
{
    char RefreshToken[SMALLBUF+150];
    strcpy(RefreshToken, RefreshToken_);
    char grand[MEDIUMBUF] = {0};
    sprintf(grand, "grant_type=refresh_token&refresh_token=%s&redirect_uri=%s", RefreshToken, ReDirectUri);
    memset(Buf, 0x0, SizeBuf);
    sprintf(Buf, "POST /api/token HTTP/1.1 \r\n"
                 "Host: accounts.spotify.com\r\n"
                 "Authorization: Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=\r\n"
                 "Content-Length: %d\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "\r\n"
                 "%s\r",
            strlen(grand), grand);
    ESP_LOGI(TAG,"\n%s\n", Buf);
    char url[SMALLBUF] = "https://accounts.spotify.com/api/token";
    char server[SMALLBUF] = "accounts.spotify.com";
    HttpsHandler(Buf, SizeBuf, url, sizeof(url), server, sizeof(server));
}
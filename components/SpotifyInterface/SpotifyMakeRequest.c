#include "SpotifyMakeRequest.h"

static const char *TAG = "SpotifyTask";

/**
 * @brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was Found.
 * @param[in] Response The character array to search within, and Response is response from first stage from spotify athurisiation
 * @param[in] SizeRes The size of the character array.
 * @return Returns true if either the 'code' or 'state' pattern was Found, and false otherwise.
 */
bool Spotify_FindCode(char *Response, uint16_t SizeRes)
{
    char *CodeString = {"code"};
    uint16_t CodeLength = strlen(CodeString);

    if (Response == NULL || SizeRes < CodeLength)
    {
        // Invalid input, either null pointer or insufficient buffer size
        return false;
    }

    for (uint16_t i = 0; i <= SizeRes - CodeLength; ++i)
    {
        bool Found = true;
        for (uint16_t j = 0; j < CodeLength; ++j)
        {
            if (Response[i + j] != CodeString[j])
            {
                Found = false;
                break;
            }
        }
        if (Found)
        {
            return true; // Found the access token substring
        }
    }
    return false; // Access token substring not Found
}

/**
 * @brief This function searches for a token within a character array and extracts the corresponding JSON object.
 * @param[in] Response The character array to search within. and Response is response from first stage from spotify athurisiation
 * @param[in] SizeRes The size of the character array.
 * @return Returns true if the token is Found and the corresponding JSON object is successfully extracted, otherwise returns false.
 */
bool Spotify_FindToken(char *Response, uint16_t SizeRes)
{
    uint8_t FlgFindToken = 0;
    uint32_t SizeOfJson = 0;
    char json[MEDIUM_BUF] = {0};
    for (uint16_t i = 0; i < SizeRes; i++)
    {
        if (Response[i] == '{')
        {
            if (Response[i + 1] == '"' && Response[i + 2] == 'a' && Response[i + 3] == 'c' && Response[i + 4] == 'c' && Response[i + 5] == 'e' && Response[i + 6] == 's')
            {
                FlgFindToken = 1;
                SizeOfJson = i;
            }
        }
        if (Response[i] == '}')
        {
            for (uint16_t j = SizeOfJson; j <= i; j++)
            {
                json[j - SizeOfJson] = Response[j];
            }
            memset(Response, 0x000, SizeRes);
            for (uint16_t j = 0; j < sizeof(json); j++)
            {
                Response[j] = json[j];
            }
        }
    }
    return FlgFindToken;
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
    char Grand[MEDIUM_BUF] = {0};
    ESP_LOGI(TAG, "\n%s\n", code);
    sprintf(Grand, "grant_type=authorization_code&redirect_uri=%s&%s", ReDirectUri, code);
    memset(Buf, 0x0, SizeBuf);
    sprintf(Buf, "POST /api/token HTTP/1.1 \r\n"
                 "Host: accounts.spotify.com\r\n"
                 "Authorization: Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=\r\n"
                 "Content-Length: %d\r\n"
                 "Cookie: __Host-device_id=AQAwmp7jxagopcWw89BjSDAA530mHwIieOZdJ9Im8nI0-70oEsSInx3jkeSO09YQ7sPgPaIUyMEvZ-tct7I6OlshJrzVYOqcgo0; sp_tr=false\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "\r\n"
                 "%s\r",
            strlen(Grand), Grand);
    char URL[SMALL_BUF] = "https://accounts.spotify.com/api/token";
    char Server[SMALL_BUF] = "accounts.spotify.com";
    HttpsHandler(Buf, SizeBuf, URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to perform a player command.
 * @param[in] Method_t The HTTP method to be used in the request (e.g., "POST", "PUT").
 * @param[in] Command_t The specific player command to be executed (e.g., "next", "previous", "play", "pause").
 * @param[out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @return This function does not return a value.
 */
void Spotify_MakePlayerCommandAndSendIt(const char *Method_t, const char *Command_t, char *Buf, size_t SizeBuf, char *AccessToken)
{
    //   copy tow of them because ,we dont give size of Method and command
    char Method[50] = {0};
    char Command[50] = {0};
    strcpy(Method, Method_t);
    strcpy(Command, Command_t);
    memset(Buf, 0x00, SizeBuf);
    sprintf(Buf, "%s /v1/me/player/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            Method, Command, AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for %s \n%s\nand size it =%d\n\n",Command, Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    HttpsHandler(Buf, SizeBuf, URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to skip to the next track in the player.
 * @return This function does not return a value.
 */
void Spotify_SendRequestForNext(Token_t *Token)
{
    char Buf[LONG_BUF];
    ESP_LOGE(TAG, "we are in Send Spotify_SendRequestForNext ");
    Spotify_MakePlayerCommandAndSendIt("POST", "next", Buf, LONG_BUF, Token->AccessToken);
}

/**
 * @brief This function sends a request to the Spotify API to skip to the previous track in the player.
 * @return This function does not return a value.
 */
void Spotify_SendRequestForPrevious(Token_t *Token)
{
    char Buf[LONG_BUF];
    Spotify_MakePlayerCommandAndSendIt("POST", "previous", Buf, LONG_BUF, Token->AccessToken);
}

/**
 * @brief This function sends a request to the Spotify API to play the current track in the player.
 * @return This function does not return a value.
 */
void Spotify_SendRequestForPlay(Token_t *Token)
{
    char Buf[LONG_BUF];
    Spotify_MakePlayerCommandAndSendIt("PUT", "play", Buf, LONG_BUF, Token->AccessToken);
}

/**
 * @brief This function sends a request to the Spotify API to pause the current track in the player.
 * @return This function does not return a value.
 */
void Spotify_SendRequestForPause(Token_t *Token)
{
    char Buf[LONG_BUF];
    Spotify_MakePlayerCommandAndSendIt("PUT", "pause", Buf, LONG_BUF, Token->AccessToken);
}

/**
 * @brief This function sends a request to the Spotify API to retrieve the user's profile information.
 * @return This function does not return a value.
 */
void Spotify_GetUserStatus(char *AccessToken)
{
    char Buf[LONG_BUF];
    sprintf(Buf, "GET /v1/me/ HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetUserStatus\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to retrieve the user's top item.
 * @return This function does not return a value.
 */
void Spotify_GetUserTopItems(char *AccessToken)
{
    char Buf[LONG_BUF];
    sprintf(Buf, "GET /v1/me/top/artists HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetUserTopItems\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to retrieve the profile information of a specific user.
 * @param[in] UserId_t The ID of the user whose profile information is to be retrieved.
 * @return This function does not return a value.
 */
void Spotify_GetUserProfile(char *UserId_t, char *Token)
{
    char Buf[LONG_BUF];
    char UserId[50];
    strcpy(UserId, UserId_t);
    sprintf(Buf, "GET /v1/users/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            UserId, Token);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetUserProfile\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to give Curent playing information
 */
void Spotify_GetCurrentPlaying(Token_t *Token)
{
    // return json is so long and we need longer buffer
    char Buf[LONG_BUF + MEDIUM_BUF+SMALL_BUF];
    sprintf(Buf, "GET /v1/me/player/currently-playing HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Connection: close\r\n\r\n",
            Token->AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetCurrentPlaying\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    HttpsHandler(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_t)
{
    char RefreshToken[SMALL_BUF + 150];
    strcpy(RefreshToken, RefreshToken_t);
    char Grand[MEDIUM_BUF] = {0};
    sprintf(Grand, "grant_type=refresh_token&refresh_token=%s&redirect_uri=%s", RefreshToken, ReDirectUri);
    memset(Buf, 0x0, SizeBuf);
    sprintf(Buf, "POST /api/token HTTP/1.1 \r\n"
                 "Host: accounts.spotify.com\r\n"
                 "Authorization: Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=\r\n"
                 "Content-Length: %d\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "\r\n"
                 "%s\r",
            strlen(Grand), Grand);
    char URL[SMALL_BUF] = "https://accounts.spotify.com/api/token";
    char Server[SMALL_BUF] = "accounts.spotify.com";
    HttpsHandler(Buf, SizeBuf, URL, sizeof(URL), Server, sizeof(Server));
}
#ifdef __cplusplus
extern "C"
{
#endif //cpp
#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include "OauthTypedef.h"

typedef enum
{
    INIT_SERVICE = 0,
    LOGIN_USER = 1,
    AUTHENTICATED_USER = 2,
    AUTHORIZED_USER = 3,
    EXPIRED_USER = 4,
    CHECK_TIME_USER = 5
} Status_t;

typedef struct Token_t
{
    char AccessToken[ACCESS_TOKEN_STR_SIZE];
    char TokenType[TOKEN_TYPE_STR_SIZE];
    char RefreshToken[REFRESH_TOKEN_STP_SIZE];
    char GrantedScope[GRANTED_SCOP_STR_SIZE];
    int ExpiresInMS;
} Token_t;

typedef struct OAuthInterfaceHandler_t
{
    char *ConfigAddressInSpiffs;
    SemaphoreHandle_t *IsServiceAuthorizedSemaphore;
    HttpClientInfo_t ClientConfig;
} OAuthInterfaceHandler_t;

typedef struct OAuthPrivateHandler_t
{
    Token_t token;                                   // Nested struct for token information
    TickType_t TokenLastUpdate;                    // System Tick of last token update
    Status_t Status;                              // state machine
    APIBuffer_t OAuthBuffer;            // Buffer for https request
} OAuthPrivateHandler_t;

/**
 * @brief This function initiates the Spotify authorization process.
 * @param SpotifyInterfaceHandler as the handler
 * @return true if task run to the end
 */
bool Oauth_TaskInit(OAuthInterfaceHandler_t *OAuthInterfaceHandler);

/**
 * @brief This function returns the access token.
 * @param OAuthInterfaceHandler as the handler
 * @return the access token
 */
char* Oauth_GetAccessToken(void);

#endif //file

#ifdef __cplusplus
}
#endif // cpp
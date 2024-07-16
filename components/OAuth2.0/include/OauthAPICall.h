#ifdef __cplusplus
extern "C" {
#endif
#ifndef   MAKE_REQUEST_H_
#define   MAKE_REQUEST_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "esp_http_client.h"
#include "authorization.h"
#include "esp_crt_bundle.h"

void APICallInit(APIBuffer_t *APIBuffer, char *base64Cred);

/**
 * @brief  This function sends a request to the Service login API 
 *         to exchange an authorization code for an access token.
 * @param  RefreshToken The refresh token.
 * @param  ClientConfig The client configuration.
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *RefreshToken, HttpClientInfo_t *ClientConfig);

/**
 * @brief  This function sends a request to the Service login API 
 *         to exchange an authorization code for an access token.
 * @param  Code The authorization code.
 * @param  ClientConfig The client configuration.
 * @return This function does not return a value.
 */
void SendTokenRequest(char *Code, HttpClientInfo_t *ClientConfig);

#endif
#ifdef __cplusplus
}
#endif


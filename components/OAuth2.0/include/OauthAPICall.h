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

void APICallInit(APIBuffer_t *APIBuffer);

/**
 * @brief This function sends a request to the Service login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *RefreshToken, esp_http_client_config_t ClientConfig);

/**
* @brief This function sends a request to the Service login API to exchange an authorization code for an access token.
* @param[in] code is parameter recived from Service api.
* @return This function does not return a value.
*/
void SendTokenRequest(char *Code, esp_http_client_config_t ClientConfig);

#endif
#ifdef __cplusplus
}
#endif


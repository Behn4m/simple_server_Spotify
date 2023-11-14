#ifdef __cplusplus
extern "C" {
#endif
#ifndef HTTPS_COMPONENT_H_
#define HTTPS_COMPONENT_H_

/**
* @brief This function performs an HTTPS GET request to a specified server URL with the provided header request.
* @param[in] HeaderOfRequest The header of the HTTPS request.
* @param[in] SizeHeaderOfRequest The size of the header of the HTTPS request.
* @param[in] Url The URL to which the request should be sent.
* @param[in] SizeUrl The size of the URL.
* @param[in] Server The server address.
* @param[in] SizeServer The size of the server address.
*/
void HttpsHandler(char *HeaderOfRequest, size_t SizeHeaderOfRequest, char* Url,size_t SizeUrl,char* Server,size_t SizeServer);
#endif
#ifdef __cplusplus
}
#endif

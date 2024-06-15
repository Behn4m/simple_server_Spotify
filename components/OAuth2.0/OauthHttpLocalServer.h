#ifdef __cplusplus
extern "C"
{
#endif
#ifndef OAUTH_HTTP_SERVER
#define OAUTH_HTTP_SERVER
#include "SpotifyTypedef.h"

extern QueueHandle_t SendCodeFromHttpToTask;

/**
 * @brief This function starts the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
httpd_handle_t StartWebServer();

/**
 * @brief This function stops the web server for handling HTTPS requests.
 * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
 */
esp_err_t StopWebServer(httpd_handle_t server);

/**
 * @brief This function starts the mDNS service.
 */
bool StartMDNSService();

#endif
#ifdef __cplusplus
}
#endif

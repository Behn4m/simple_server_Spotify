#ifdef __cplusplus
extern "C"
{
#endif
#ifndef OAUTH_HTTP_SERVER
#define OAUTH_HTTP_SERVER

#include "authorization.h"

extern QueueHandle_t SendCodeFromHttpToTask;

/**
 * @brief This function stops the web HttpdServerHandler for handling HTTPS requests.
 * @param HttpdServerHandler The URI handler for the request access.
 * @return Returns the HTTP HttpdServerHandler handle if it is started successfully, or NULL otherwise.
 */
esp_err_t StopWebServer(httpd_handle_t server);

/**
 * @brief Run Http local service
 * @param hostname The hostname of the device.
 * @param Request_Access_URI The URI handler for the request access.
 * @param Response_Access_URI The URI handler for the response access.
 * @return Returns true if the Http local service is started successfully, or false otherwise.
 */
bool HttpServerServiceInit(char *hostname, httpd_uri_t RequestURIInit, httpd_uri_t ResponseURIInit);

#endif
#ifdef __cplusplus
}
#endif

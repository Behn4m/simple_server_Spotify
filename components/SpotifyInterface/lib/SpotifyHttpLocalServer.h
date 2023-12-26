#ifdef __cplusplus
extern "C"
{
#endif
#ifndef SPOTIFY_HTTP_SERVER
#define SPOTIFY_HTTP_SERVER
#include "SpotifyTypedef.h"
#include "SpotifyMakeRequest.h"
    /**
     * @brief Setup parameter for starting Http Local server
     * @param[in] HttpLocalServerParam_t HttpLocalServerParam_)
     */
    void SetupHttpLocalServer(HttpLocalServerParam_t HttpLocalServerParam_);

    /**
     * @brief This function starts the web server for handling HTTPS requests.
     * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
     */
    httpd_handle_t StartWebServer();

    /**
     * @brief This function starts the mDNS service.
     */
    bool StartMDNSService();
#endif
#ifdef __cplusplus
}
#endif

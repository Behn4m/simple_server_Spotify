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
    void Spotify_SetupHttpLocalServer(HttpLocalServerParam_t HttpLocalServerParam_);

    /**
     * @brief This function starts the web server for handling HTTPS requests.
     * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
     */
    httpd_handle_t Spotify_StartWebServer();

    /**
     * @brief This function starts the mDNS service.
     */
    bool Spotify_StartMDNSService();

    /**
     * @brief This function stops the web server for handling HTTPS requests.
     * @return Returns the HTTP server handle if it is started successfully, or NULL otherwise.
     */
    esp_err_t Spotify_StopSpotifyWebServer(httpd_handle_t server);
#endif
#ifdef __cplusplus
}
#endif

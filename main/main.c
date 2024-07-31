#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "Authorization.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"

#define TAG "MAIN"
#define TIMER_TIME pdMS_TO_TICKS(500) // in millis

// ****************************** GLobal Variables ****************************** //
//static const char *TAG = "Main";
OAuthInterfaceHandler_t OAuthInterfaceHandler;
SpotifyInterfaceHandler_t SpotifyInterfaceHandler;
SpotifyHttpInfo_t Spotify_ClientInfo;

void app_main(void)
{
    GUI_TaskInit();
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
#ifdef WIFI_INIT_STA_MODE
    WifiStationMode("Done_IOT_1", "87654321@");
    //WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif
GUI_Screen();

#ifdef SpotifyEnable

//TODO merge all in a static function
    OAuthInterfaceHandler.IsServiceAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    OAuthInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    Spotify_ClientInfo = Spotify_ClientConfigInit();
    OAuthInterfaceHandler.ClientConfig.url = Spotify_ClientInfo.url;
    OAuthInterfaceHandler.ClientConfig.host = Spotify_ClientInfo.host;
    OAuthInterfaceHandler.ClientConfig.path = Spotify_ClientInfo.path;
    OAuthInterfaceHandler.ClientConfig.requestURI = Spotify_ClientInfo.requestURI;
    OAuthInterfaceHandler.ClientConfig.responseURI = Spotify_ClientInfo.responseURI;
    OAuthInterfaceHandler.ClientConfig.hostnameMDNS = Spotify_ClientInfo.hostnameMDNS;
    OAuthInterfaceHandler.ClientConfig.requestURL = Spotify_ClientInfo.requestURL; 
    OAuthInterfaceHandler.ClientConfig.clientID = Spotify_ClientInfo.clientID;
    OAuthInterfaceHandler.ClientConfig.base64Credintials = Spotify_ClientInfo.base64Credintials;
    OAuthInterfaceHandler.ClientConfig.redirectURL = Spotify_ClientInfo.redirectURL;

    if(!Oauth_TaskInit(&OAuthInterfaceHandler))
        ESP_LOGE(TAG, "OAuth task initialization failed.");

    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGW(TAG, "Access Token: %s", Oauth_GetAccessToken());
    }
#endif
}

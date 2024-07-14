#include "OauthHttpLocalServer.h"
#include "authorization.h"

static const char *TAG = "OauthTask";
// ******************************

QueueHandle_t SendCodeFromHttpToTask = NULL;

/**
 * @brief This function starts the web HttpdServerHandler for handling HTTPS requests.
 * @return Returns the HTTP HttpdServerHandler handle if it is started successfully, or NULL otherwise.
 */
static httpd_handle_t Oauth_StartWebServer(httpd_uri_t Request_Access_URI, httpd_uri_t Response_Access_URI)
{
    httpd_handle_t httpdHandler = NULL;
    httpd_config_t httpdConfig = HTTPD_DEFAULT_CONFIG();
    httpdConfig.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting HttpdServerHandler on port: '%d'", httpdConfig.server_port);

    bool IsHttpdStarted = httpd_start(&httpdHandler, &httpdConfig) == ESP_OK;
    if (!IsHttpdStarted)
    {
        ESP_LOGE(TAG, "Failed to start HttpdServerHandler");
        return NULL;
    }
    
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(httpdHandler, &Request_Access_URI);
    httpd_register_uri_handler(httpdHandler, &Response_Access_URI);
    return httpdHandler;
}


/**
 * @brief This function stops the web HttpdServerHandler for handling HTTPS requests.
 * @return Returns the HTTP HttpdServerHandler handle if it is started successfully, or NULL otherwise.
 */
// esp_err_t StopWebServer(httpd_handle_t HttpdServerHandler)
// {
//     return httpd_stop(HttpdServerHandler);
// }

/**
 * @brief This function starts the mDNS service.
 */
static bool Oauth_StartMDNSService(char *hostname)
{
    esp_err_t err;
    err = mdns_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "MDNS Init failed: %d", err);
        return false;
    }

    err = mdns_hostname_set("deskhub");
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "mdns_hostname_set  failed: %d", err);
        return false;
    }

    err = mdns_instance_name_set(hostname);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "mdns_instance_name_set  failed: %d", err);
        return false;
    }

    err = mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "mdns_service_add  failed: %d", err);
        return false;
    }
    
    ESP_LOGI(TAG, "MDNS Init done");
    return true;
}

/**
 * @brief Run Http local service
 */
bool HttpServerServiceInit(char *hostname, httpd_uri_t Request_Access_URI, httpd_uri_t Response_Access_URI)
{
    SendCodeFromHttpToTask = 
            xQueueCreate(1, sizeof(char) * sizeof(char[MEDIUM_BUF]));
    
    httpd_handle_t serviceLocalServer = Oauth_StartWebServer(Request_Access_URI, Response_Access_URI);
    if (serviceLocalServer == NULL)
    {
        ESP_LOGE(TAG, "Creating Service local server failed!");
        return false;
    }
    
    bool IsMdnsStarted = Oauth_StartMDNSService(hostname);
    if (!IsMdnsStarted)
    {
        ESP_LOGE(TAG, "Running mDNS failed!");
        return false;
    };

    ESP_LOGI(TAG, "** local server created, %s is running! **", hostname);
    return true;
}
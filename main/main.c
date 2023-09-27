/* Simple HTTP + SSL Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"

#include <esp_https_server.h>
#include "esp_tls.h"
#include "sdkconfig.h"
#include "mdns.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/


static const char *TAG = "example";
static const char *TAG_APP = "SPOTIFY";


static const char *clientId = "2c2dadbd46244f2cb9f71251bc004caa";
static const char *redirectUri = "http%3A%2F%2Fdeskhub.local%2Fcallback%2f";

/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    char loc_url[400];
    ESP_LOGI("APPLOG", "here we are - root");
    sprintf(loc_url,"https://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state", clientId,redirectUri);
    httpd_resp_set_hdr(req,"Location",loc_url);
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_status(req,"302");
    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t user_callback_func(httpd_req_t *req)
{
    char message[1000];
    char code[800];

    if (httpd_req_get_url_query_str(req,code,1000) == ESP_OK)
    {
        sprintf(message,"q=%s",code);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req,"200");
        httpd_resp_send(req, message, HTTPD_RESP_USE_STRLEN);
        ESP_LOGI(TAG_APP, "code recevied");
    }
    else
    {
        sprintf(message,"BAD ARGS");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req,"500");
        httpd_resp_send(req, message, HTTPD_RESP_USE_STRLEN);
        ESP_LOGI(TAG_APP, "bad arguments - the response does not include correct structure");
    }
    return ESP_OK;
}

// Assign root handler to root uri
static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler
};

// Assign callback handler to callback uri
static const httpd_uri_t user_callback = {
    .uri       = "/callback/",
    .method    = HTTP_GET,
    .handler   = user_callback_func
};

// Config and Start webserver 
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");
    conf.servercert = servercert_start;
    conf.servercert_len = servercert_end - servercert_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &user_callback);
    return server;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_ssl_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = start_webserver();
    }
}

// Start mDSN to resolves hostnames to IP addresses within local network 
void start_mdns_service()
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGI(TAG_APP, "MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    mdns_hostname_set("deskhub");
    //set default instance
    mdns_instance_name_set("Behnam's ESP32 Thing");
}

void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    start_mdns_service();

    /* Register event handlers to start server when Wi-Fi or Ethernet is connected,
     * and stop server when disconnection happens.
     */

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
}
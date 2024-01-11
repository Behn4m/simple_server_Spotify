/*
 * HTTPS GET Example using plain Mbed TLS sockets
 *
 * Contacts the howsmyssl.com API via TLS v1.2 and reads a JSON
 * response.
 *
 * Adapted from the ssl_client1 example in Mbed TLS.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2015-2022 Espressif Systems (Shanghai) CO LTD
 */
#include "HttpsRequests.h"

#define WEB_PORT "443"
#define SERVER_URL_MAX_SZ 1024
#define TIME_PERIOD (86400000000ULL)

TaskHandle_t xTaskHandlerHTTPS;
char *WebServerAddress;
char *Web_URL;
char *HttpsBuf;

static const char *TAG = "Https";
static void https_request_task(void *pvparameters);
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[] asm("_binary_server_root_cert_pem_end");
extern const uint8_t local_server_cert_pem_start[] asm("_binary_local_server_cert_pem_start");
extern const uint8_t local_server_cert_pem_end[] asm("_binary_local_server_cert_pem_end");

HttpsRequestsHandler_t *HttpsRequestsHandler;

bool Https_ComponentInit(HttpsRequestsHandler_t *pHandler)
{
    HttpsRequestsHandler = pHandler;
    return true;
}

/**
 * @brief This function performs an HTTPS GET request to a specified server URL with the provided configuration.
 * @param[in] cfg The TLS configuration for the request.
 * @param[in] WEB_SERVER_URL The URL to which the request should be sent.
 * @param[in] REQUEST The HTTP request to be sent.
 */
static void https_get_request(esp_tls_cfg_t cfg, const char *WEB_SERVER_URL, const char *REQUEST)
{
    char buf[LONG_BUF];
    int ret, len;

    esp_tls_t *tls = esp_tls_init();
    if (!tls)
    {
        ESP_LOGE(TAG, "Failed to allocate esp_tls handle!");
        goto exit;
    }

    if (esp_tls_conn_http_new_sync(WEB_SERVER_URL, &cfg, tls) == true)
    {
        ESP_LOGI(TAG, "Connection established...");
    }
    else
    {
        ESP_LOGE(TAG, "Connection failed...");
        goto cleanup;
    }

    size_t written_bytes = 0;
    do
    {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 strlen(REQUEST) - written_bytes);
        if (ret >= 0)
        {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        }
        else if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE)
        {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            goto cleanup;
        }
    } while (written_bytes < strlen(REQUEST));
    ESP_LOGI(TAG, "Reading HTTP response...");
    
    do {
        len = sizeof(buf) - 1;
        memset(buf, 0x00, sizeof(buf));

        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            continue;
        } else if (ret < 0) {
            ESP_LOGE(TAG, "esp_tls_conn_read returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        } else if (ret == 0) {
            ESP_LOGI(TAG, "connection closed");
            vTaskDelete(xTaskHandlerHTTPS);
            break;
        }

        len = ret;
        buf[len] = '\0'; // Null-terminate the received data

        
        if (len > 0) {                  // Check if the read operation completed
            if (xQueueSend(*HttpsRequestsHandler->ResponseBufQueue, buf, portMAX_DELAY) == pdTRUE)    // Data reading completed; process the data here if needed
            {
                ESP_LOGI(TAG, "Reading response data finished, data sent by queue!");
                xSemaphoreGive(*HttpsRequestsHandler->ResponseReadySemaphore);
            }
            else
            {
                ESP_LOGE(TAG, "Reading response data finished, but sending data by queue failed!");
                vTaskDelete(xTaskHandlerHTTPS);
            }

            ESP_LOGI(TAG, "%d bytes read", len);
        }

    } while (len > 0);

cleanup:
    esp_tls_conn_destroy(tls);
exit:
    for (int countdown = 10; countdown >= 0; countdown--)
    {
        ESP_LOGI(TAG, "%d...", countdown);
        vTaskDelay(SEC / portTICK_PERIOD_MS);
    }
}

static void https_get_request_using_cacert_buf(void)
{
    ESP_LOGI(TAG, "https_request using cacert_buf");
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *)server_root_cert_pem_start,
        .cacert_bytes = server_root_cert_pem_end - server_root_cert_pem_start,
    };
    https_get_request(cfg, Web_URL, HttpsBuf);
}

static void https_get_request_using_global_ca_store(void)
{
    esp_err_t esp_ret = ESP_FAIL;
    ESP_LOGI(TAG, "https_request using global ca_store");
    esp_ret = esp_tls_set_global_ca_store(server_root_cert_pem_start, server_root_cert_pem_end - server_root_cert_pem_start);
    if (esp_ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error in setting the global ca store: [%02X] (%s),could not complete the https_request using global_ca_store", esp_ret, esp_err_to_name(esp_ret));
        return;
    }
    esp_tls_cfg_t cfg = {
        .use_global_ca_store = true,
    };
    https_get_request(cfg, Web_URL, HttpsBuf);
    esp_tls_free_global_ca_store();
}


static void https_request_task(void *pvparameters)
{
    ESP_LOGI(TAG, "Starting to send HTTP request");
    ESP_LOGI(TAG, "Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());
    https_get_request_using_global_ca_store();
    ESP_LOGI(TAG, "Sending HTTP request finished");
    free(HttpsBuf);
    free(Web_URL);
    free(WebServerAddress);
    vTaskDelete(NULL);
}

/**
 * @brief This function performs an HTTPS GET request to a specified server URL with the provided header request.
 * @param[in] HeaderOfRequest The header of the HTTPS request.
 * @param[in] SizeHeaderOfRequest The size of the header of the HTTPS request.
 * @param[in] Url The URL to which the request should be sent.
 * @param[in] SizeUrl The size of the URL.
 * @param[in] Server The server address.
 * @param[in] SizeServer The size of the server address.
 */
void Https_GetRequest(char *HeaderOfRequest, size_t SizeHeaderOfRequest, char *Url, size_t SizeUrl, char *Server, size_t SizeServer)
{
    HttpsBuf = (char *)malloc(SizeHeaderOfRequest * sizeof(char));
    Web_URL = (char *)malloc(SizeUrl * sizeof(char));
    WebServerAddress = (char *)malloc(SizeServer * sizeof(char));
    if (HttpsBuf == NULL || Web_URL == NULL || WebServerAddress == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for the array.\n\n");
    }
    memset(HttpsBuf, 0x0, SizeHeaderOfRequest - 1);
    memset(Web_URL, 0x0, SizeUrl - 1);
    memset(WebServerAddress, 0x0, SizeServer - 1);
    const esp_timer_create_args_t nvs_update_timer_args = {
        .callback = (void *)&fetch_and_store_time_in_nvs,
    };

    strncpy(Web_URL, Url, SizeUrl);
    strncpy(WebServerAddress, Server, SizeServer);
    strncpy(HttpsBuf, HeaderOfRequest, SizeHeaderOfRequest);

    esp_timer_handle_t nvs_update_timer;
    ESP_ERROR_CHECK(esp_timer_create(&nvs_update_timer_args, &nvs_update_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(nvs_update_timer, TIME_PERIOD));

    xTaskCreate(&https_request_task, "https_get_task", HttpsTaskStackSize, NULL, 1, &xTaskHandlerHTTPS);
}

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

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "protocol_examples_common.h"
#include "esp_sntp.h"
#include "esp_netif.h"
#include "freertos/queue.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_tls.h"
#include "sdkconfig.h"
#include"esp_psram.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif
#include "time_sync.h"
#include "GlobalInit.h"
extern SemaphoreHandle_t HttpsResponseReadySemaphore;
extern QueueHandle_t BufQueue1;
TaskHandle_t xTaskHandlerHTTPS;
char *WebServerAddress;
char *Web_URL;
char *HttpsBuf;
#define WEB_PORT "443"
#define SERVER_URL_MAX_SZ 1024
#define TIME_PERIOD (86400000000ULL)
static const char *TAG = "Https";
static void https_request_task(void *pvparameters);
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[] asm("_binary_server_root_cert_pem_end");
extern const uint8_t local_server_cert_pem_start[] asm("_binary_local_server_cert_pem_start");
extern const uint8_t local_server_cert_pem_end[] asm("_binary_local_server_cert_pem_end");
#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static esp_tls_client_session_t *tls_client_session = NULL;
static bool save_client_session = false;
#endif

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

    if (esp_tls_conn_http_new_sync(WEB_SERVER_URL, &cfg, tls) == 1)
    {
        ESP_LOGI(TAG, "Connection established...");
    }
    else
    {
        ESP_LOGE(TAG, "Connection failed...");
        goto cleanup;
    }

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
    /* The TLS session is successfully established, now saving the session ctx for reuse */
    if (save_client_session)
    {
        esp_tls_free_client_session(tls_client_session);
        tls_client_session = esp_tls_get_client_session(tls);
    }
#endif
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
    do
    {
        len = sizeof(buf) - 1;
        memset(buf, 0x00, sizeof(buf));
        // vTaskDelay((SEC*5) / portTICK_PERIOD_MS);
        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE || ret == ESP_TLS_ERR_SSL_WANT_READ)
        {
            continue;
        }
        else if (ret < 0)
        {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        }
        else if (ret == 0)
        {
            ESP_LOGI(TAG, "connection closed");

            vTaskDelete(xTaskHandlerHTTPS);
            break;
        }
        len = ret;
        buf[len + 1] = '\n';
        xSemaphoreGive(HttpsResponseReadySemaphore);
        if (xQueueSend(BufQueue1, buf, portMAX_DELAY) != pdTRUE)
        {
            ESP_LOGE(TAG, "sending data from Https by queue failed !");
            vTaskDelete(xTaskHandlerHTTPS);
        }

        ESP_LOGD(TAG, "%d bytes read", len);

    } while (1);
cleanup:
    esp_tls_conn_destroy(tls);
exit:
    for (int countdown = 10; countdown >= 0; countdown--)
    {
        ESP_LOGI(TAG, "%d...", countdown);
        vTaskDelay(SEC / portTICK_PERIOD_MS);
    }
}

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
static void https_get_request_using_crt_bundle(void)
{
    ESP_LOGI(TAG, "https_request using crt bundle");
    esp_tls_cfg_t cfg = {
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    https_get_request(cfg, Web_URL, HttpsBuf);
}
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE

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

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static void https_get_request_to_local_server(const char *url)
{
    ESP_LOGI(TAG, "https_request to local server");
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *)local_server_cert_pem_start,
        .cacert_bytes = local_server_cert_pem_end - local_server_cert_pem_start,
        .skip_common_name = true,
    };
    save_client_session = true;
    https_get_request(cfg, url, LOCAL_SRV_REQUEST);
}

static void https_get_request_using_already_saved_session(const char *url)
{
    ESP_LOGI(TAG, "https_request using saved client session");
    esp_tls_cfg_t cfg = {
        .client_session = tls_client_session,
    };
    https_get_request(cfg, url, LOCAL_SRV_REQUEST);
    esp_tls_free_client_session(tls_client_session);
    save_client_session = false;
    tls_client_session = NULL;
}
#endif

static void https_request_task(void *pvparameters)
{
    ESP_LOGI(TAG, "Start https_request example");

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
    char *server_url = NULL;
#ifdef CONFIG_EXAMPLE_LOCAL_SERVER_URL_FROM_STDIN
    char url_buf[SERVER_URL_MAX_SZ];
    if (strcmp(CONFIG_EXAMPLE_LOCAL_SERVER_URL, "FROM_STDIN") == 0)
    {
        example_configure_stdin_stdout();
        fgets(url_buf, SERVER_URL_MAX_SZ, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        server_url = url_buf;
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: invalid url for local server");
        abort();
    }
    printf("\nServer URL obtained is %s\n", url_buf);
#else
    server_url = CONFIG_EXAMPLE_LOCAL_SERVER_URL;
#endif /* CONFIG_EXAMPLE_LOCAL_SERVER_URL_FROM_STDIN */
    https_get_request_to_local_server(server_url);
    https_get_request_using_already_saved_session(server_url);
#endif

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
    https_get_request_using_crt_bundle();
#endif
    ESP_LOGI(TAG, "Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());
    https_get_request_using_cacert_buf();
    https_get_request_using_global_ca_store();
    ESP_LOGI(TAG, "Finish https_request example");
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
void HttpsHandler(char *HeaderOfRequest, size_t SizeHeaderOfRequest, char *Url, size_t SizeUrl, char *Server, size_t SizeServer)
{
    HttpsBuf = (char *)malloc(SizeHeaderOfRequest * sizeof(char));
    Web_URL = (char *)malloc(SizeUrl * sizeof(char));
    WebServerAddress = (char *)malloc(SizeServer * sizeof(char));
    if (HttpsBuf == NULL || Web_URL == NULL || WebServerAddress == NULL)
    {
        printf("Failed to allocate memory for the array.\n\n");
    }
    memset(HttpsBuf, 0x0, SizeHeaderOfRequest);
    memset(Web_URL, 0x0, SizeUrl);
    memset(WebServerAddress, 0x0, SizeServer);
    const esp_timer_create_args_t nvs_update_timer_args = {
        .callback = (void *)&fetch_and_store_time_in_nvs,
    };
    for (int i = 0; i < SizeUrl; i++)
    {
        Web_URL[i] = Url[i];
    }
    for (int i = 0; i < SizeServer; i++)
    {
        WebServerAddress[i] = Server[i];
    }
    for (int i = 0; i < SizeHeaderOfRequest; i++)
    {
        HttpsBuf[i] = HeaderOfRequest[i];
    }
    esp_timer_handle_t nvs_update_timer;
    ESP_ERROR_CHECK(esp_timer_create(&nvs_update_timer_args, &nvs_update_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(nvs_update_timer, TIME_PERIOD));
    xTaskCreate(&https_request_task, "https_get_task", HttpsTaskStackSize, NULL, 1, &xTaskHandlerHTTPS);
}

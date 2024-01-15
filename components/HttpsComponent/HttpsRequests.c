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

TaskHandle_t xTaskHttpsBuffer;  // Updated definition for TaskHandle_t
StackType_t *xStackHttpsStack;

char *WebServerAddress;
char *Web_URL;
char *HttpsBuf;

static const char *TAG = "Https";
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[] asm("_binary_server_root_cert_pem_end");
extern const uint8_t local_server_cert_pem_start[] asm("_binary_local_server_cert_pem_start");
extern const uint8_t local_server_cert_pem_end[] asm("_binary_local_server_cert_pem_end");

static void https_request_task(void *pvparameters);

HttpsRequestsHandler_t *HttpsRequestsHandler;

bool Https_ComponentInit(HttpsRequestsHandler_t *pHandler)
 {
    HttpsRequestsHandler = pHandler;
    return true;
}

static esp_err_t http_event_handler(esp_http_client_event_t *evt) 
{
    switch (evt->event_id) 
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            // Handle HTTP header received, if needed
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (xQueueSend(*HttpsRequestsHandler->ResponseBufQueue, evt->data, portMAX_DELAY) == pdTRUE) {
                ESP_LOGI(TAG, "Reading response data finished, data sent by queue!");
                xSemaphoreGive(*HttpsRequestsHandler->ResponseReadySemaphore);
            } else {
                ESP_LOGE(TAG, "Reading response data finished, but sending data by queue failed!");
            }            
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "redirected");
            break;
    }
    return ESP_OK;
}


static void https_get_request(const char *WEB_SERVER_URL) {
    esp_http_client_config_t config = {
        .url = WEB_SERVER_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .cert_pem = (char *)server_root_cert_pem_start, // Add your certificate data
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "HTTP client perfored");
    }

    esp_http_client_cleanup(client);
}

static void https_request_task(void *pvparameters) {
    ESP_LOGI(TAG, "Starting to send HTTP request");

    https_get_request(Web_URL);

    ESP_LOGI(TAG, "Sending HTTP request finished");

    // Your existing cleanup and free statements
    free(HttpsBuf);
    free(Web_URL);
    free(WebServerAddress);
    // free(xTaskHttpsBuffer);
    // free(xStackHttpsStack);

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
void Https_GetRequest(char *HeaderOfRequest, size_t SizeHeaderOfRequest, char *Url, size_t SizeUrl, char *Server, size_t SizeServer) {
    HttpsBuf = (char *)malloc((SizeHeaderOfRequest + 1) * sizeof(char));
    Web_URL = (char *)malloc((SizeUrl + 1) * sizeof(char));
    WebServerAddress = (char *)malloc((SizeServer + 1) * sizeof(char));

    if (HttpsBuf == NULL || Web_URL == NULL || WebServerAddress == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the array.\n\n");
        free(HttpsBuf);
        free(Web_URL);
        free(WebServerAddress);
        return;
    }

    memset(HttpsBuf, 0x0, SizeHeaderOfRequest + 1);
    memset(Web_URL, 0x0, SizeUrl + 1);
    memset(WebServerAddress, 0x0, SizeServer + 1);

    strncpy(Web_URL, Url, SizeUrl);
    Web_URL[SizeUrl] = '\0';

    strncpy(WebServerAddress, Server, SizeServer);
    WebServerAddress[SizeServer] = '\0';

    strncpy(HttpsBuf, HeaderOfRequest, SizeHeaderOfRequest);
    HttpsBuf[SizeHeaderOfRequest] = '\0';

    esp_timer_handle_t nvs_update_timer;
    const esp_timer_create_args_t nvs_update_timer_args = {
        .callback = (void *)&fetch_and_store_time_in_nvs,
    };

    ESP_ERROR_CHECK(esp_timer_create(&nvs_update_timer_args, &nvs_update_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(nvs_update_timer, TIME_PERIOD));

    xTaskHttpsBuffer = (StaticTask_t *)malloc(sizeof(StaticTask_t));
    xStackHttpsStack = (StackType_t *)malloc(HTTPS_TASK_STACK_SIZE * sizeof(StackType_t)); // Assuming a stack size of 400 words (adjust as needed)
    if (xTaskHttpsBuffer == NULL || xStackHttpsStack == NULL) {
        ESP_LOGI(TAG, "Memory allocation failed!\n");
        free(HttpsBuf);
        free(Web_URL);
        free(WebServerAddress);
        free(xTaskHttpsBuffer);
        free(xStackHttpsStack);
        return; // Exit with an error code
    }

    xTaskCreateStatic(
        https_request_task,     // Task function
        "https_request_task",   // Task name (for debugging)
        HTTPS_TASK_STACK_SIZE,   // Stack size (in words)
        NULL,                   // Task parameters (passed to the task function)
        tskIDLE_PRIORITY + HTTPS_PRIORITY, // Task priority (adjust as needed)
        xStackHttpsStack,       // Stack buffer
        xTaskHttpsBuffer        // Task control block
    );

    size_t free_heap = esp_get_free_heap_size();
    printf("Free Heap Size: %d bytes\n", free_heap);
}
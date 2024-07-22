#include "OauthAPICall.h"

static const char *TAG = "OAuthAPICall";
APIBuffer_t *OAuthBuffer;
char Base64Credintials[SMALL_BUF] = {0};

void APICallInit(APIBuffer_t *APIBuffer, char *Base64Cred)
{
    OAuthBuffer = APIBuffer;
    sprintf(Base64Credintials, "Basic %s", Base64Cred);
}

/**
 * @brief  This function is the event handler for the HTTP client.
 * @param  evt The event handler for the HTTP client.
 * @return ESP_OK if the event is handled successfully, or ESP_FAIL otherwise.
 */
static esp_err_t HttpEventHandler(esp_http_client_event_t *evt) 
{
    static int totalLen = 0;
    // Create the queue and semaphore

    switch (evt->event_id) 
    {
        case HTTP_EVENT_ERROR:
            break;
        case HTTP_EVENT_ON_CONNECTED:
            break;
        case HTTP_EVENT_HEADER_SENT:
            break;
        case HTTP_EVENT_ON_HEADER:
            // Handle HTTP header received, if needed
            break;
        case HTTP_EVENT_ON_DATA:
            if (totalLen + evt->data_len < SUPER_BUF)
            {   
                memcpy(OAuthBuffer->MessageBuffer + totalLen, evt->data, evt->data_len);
                totalLen += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            OAuthBuffer->MessageBuffer[totalLen] = '\0';
                                            // write 0 to the end of string
            break;
        case HTTP_EVENT_DISCONNECTED:
            xSemaphoreGive(OAuthBuffer->ResponseReadyFlag);
            totalLen = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "redirected");
            break;
    }
    return ESP_OK;
}

/**
 * @brief  This function sends a request to the Service login API 
 *         to exchange an authorization code for an access token.
 * @param  Code The authorization code.
 * @param  ClientConfig The client configuration.
 * @return This function does not return a value.
 */
void SendTokenRequest(char *Code, HttpClientInfo_t *ClientConfig)
{
    esp_http_client_config_t clientConfig = {
        .url = ClientConfig->url,
        .host = ClientConfig->host,
        .path = ClientConfig->path,
        .method = HTTP_METHOD_POST,
        .event_handler = HttpEventHandler,
        .disable_auto_redirect = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&clientConfig);

    if (httpClient == NULL) 
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    // Set headers for authentication and content type
    esp_http_client_set_header(httpClient, "Authorization", Base64Credintials);
    esp_http_client_set_header(httpClient, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_header(httpClient, "Cookie", "__Host-device_id=AQAwmp7jxagopcWw89BjSDAA530mHwIieOZdJ9Im8nI0-70oEsSInx3jkeSO09YQ7sPgPaIUyMEvZ-tct7I6OlshJrzVYOqcgo0; sp_tr=false");

    // Set the request body (POST data)
    char Grand[MEDIUM_BUF] = {0};    
    sprintf(Grand, "grant_type=authorization_code&redirect_uri=%s&%s", ClientConfig->redirectURL, Code);
    esp_http_client_set_post_field(httpClient, Grand, strlen(Grand));

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(httpClient);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
        return;
    } 
    ESP_LOGI(TAG, "HTTP client performed successfully"); 
    // Cleanup HTTP client
    esp_http_client_cleanup(httpClient);
}

/**
 * @brief  This function sends a request to the Service login API 
 *         to exchange an authorization code for an access token.
 * @param  RefreshToken The refresh token.
 * @param  ClientConfig The client configuration.
 * @return This function does not return a value.
 */
void ExchangeTokenWithRefreshToken(char *RefreshToken, HttpClientInfo_t *ClientConfig)
{
    esp_http_client_config_t clientConfig = {
        .url = ClientConfig->url,
        .host = ClientConfig->host,
        .path = ClientConfig->path,
        .method = HTTP_METHOD_POST,
        .event_handler = HttpEventHandler,
        .disable_auto_redirect = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&clientConfig);
    if (httpClient == NULL) 
    {
        ESP_LOGE(TAG, "Failed to refresh the token client");
        return;
    }

    // Set headers for authentication and content type
    esp_http_client_set_header(httpClient, "Authorization", Base64Credintials);
    esp_http_client_set_header(httpClient, "Content-Type", "application/x-www-form-urlencoded");

    // Set the request body (POST data)
    char Grand[SMALL_BUF] = {0};
    sprintf(Grand, "grant_type=refresh_token&refresh_token=%s&redirect_uri=%s", RefreshToken, ClientConfig->redirectURL);
    esp_http_client_set_post_field(httpClient, Grand, strlen(Grand));

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(httpClient);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "HTTP client performed successfully");
    
    esp_http_client_cleanup(httpClient);
}

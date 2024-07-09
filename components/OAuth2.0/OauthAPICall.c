#include "OauthAPICall.h"
#include "authorization.h"

static const char *TAG = "HTTP";
APIBuffer_t *OAuthBuffer;
char Base64Credintials[SMALL_BUF] = {0};

void APICallInit(APIBuffer_t *APIBuffer)
{
    OAuthBuffer = APIBuffer;
    sprintf(Base64Credintials, "Basic %s", BASE64_CREDINTIALS);
}

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
            if (totalLen + evt->data_len < SUPER_BUF)                                       // check if receved data is not larger than buffer size 
            {   
                memcpy(OAuthBuffer->MessageBuffer + totalLen, evt->data, evt->data_len);  // copy received data to the end of previous received data
                totalLen += evt->data_len;                                                  // update pointer to the end of copied data
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            OAuthBuffer->MessageBuffer[totalLen] = '\0';
                                            // write 0 to the end of string
            break;
        case HTTP_EVENT_DISCONNECTED:
            xSemaphoreGive(OAuthBuffer->ResponseReadyFlag);                        // give semaphore to notify that data is ready  
            totalLen = 0;                                                                   // reset contect length counter
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "redirected");
            break;
    }
    return ESP_OK;
}

/**
 * @brief This function sends a request to the Service login API to exchange an authorization code for an access token.
 * @param[in] Code is parameter that we give it before .
 * @param[in] SizeCode The size of the authorization code.
 * @return This function does not return a value.
 */
void SendTokenRequest(char *Code, esp_http_client_config_t *ClientConfig)
{
    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(ClientConfig);

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
    sprintf(Grand, "grant_type=authorization_code&redirect_uri=%s&%s", REDIRECT_URI, Code);
    esp_http_client_set_post_field(httpClient, Grand, strlen(Grand));
ESP_LOGE(TAG, "state4");
    // Enable detailed logging for debugging
    // esp_http_client_set_debug(client, true);

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(httpClient);
ESP_LOGE(TAG, "state5");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
        return;
    } 
    ESP_LOGI(TAG, "HTTP client performed successfully");
//ESP_LOGE(TAG, "state6");    
    // Cleanup HTTP client
    esp_http_client_cleanup(httpClient);
//ESP_LOGE(TAG, "state7");
}

/**
 * @brief This function sends a request to the Service login API to exchange an authorization code for an access token.
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *RefreshToken, esp_http_client_config_t ClientConfig)
{
    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&ClientConfig);
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
    sprintf(Grand, "grant_type=refresh_token&refresh_token=%s&redirect_uri=%s", RefreshToken, REDIRECT_URI);
    esp_http_client_set_post_field(httpClient, Grand, strlen(Grand));

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(httpClient);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "HTTP client performed successfully");
    
    esp_http_client_cleanup(httpClient);                                                        // close all connection releated to this client object 
}

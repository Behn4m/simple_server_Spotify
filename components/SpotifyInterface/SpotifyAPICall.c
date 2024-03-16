#include "SpotifyAPICall.h"
#include "SpotifyTypedef.h"

static const char *TAG = "HTTP";
SpotifyAPIBuffer_t *SpotifyBuffer;
char Base64Credintials[SMALL_BUF] = {0};

void SpotifyAPICallInit(SpotifyAPIBuffer_t *SpotifyAPIBuffer)
{
    SpotifyBuffer = SpotifyAPIBuffer;   
    sprintf(Base64Credintials, "Basic %s", BASE64_CREDINTIALS);
}

esp_err_t HttpEventHandler(esp_http_client_event_t *evt) 
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
                memcpy(SpotifyBuffer->MessageBuffer + totalLen, evt->data, evt->data_len);  // copy received data to the end of previous received data
                totalLen += evt->data_len;                                                  // update pointer to the end of copied data
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            SpotifyBuffer->MessageBuffer[totalLen] = '\0';
                                            // write 0 to the end of string
            break;
        case HTTP_EVENT_DISCONNECTED:
            xSemaphoreGive(SpotifyBuffer->SpotifyResponseReadyFlag);                        // give semaphore to notify that data is ready  
            totalLen = 0;                                                                   // reset contect length counter
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "redirected");
            break;
    }
    return ESP_OK;
}

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in] Code is parameter that we give it before .
 * @param[in] SizeCode The size of the authorization code.
 * @return This function does not return a value.
 */
void Spotify_SendTokenRequest(char *Code)
{  
    esp_http_client_config_t clientConfig = {
        .url = "https://accounts.spotify.com/api/token",                            
        .host = "accounts.spotify.com",
        .path = "/api/token",
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
    sprintf(Grand, "grant_type=authorization_code&redirect_uri=%s&%s", REDIRECT_URI, Code);
    esp_http_client_set_post_field(httpClient, Grand, strlen(Grand));

    // Enable detailed logging for debugging
    // esp_http_client_set_debug(client, true);

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
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *RefreshToken)
{
    esp_http_client_config_t clientConfig = {
        .url = "https://accounts.spotify.com/api/token",
        .host = "accounts.spotify.com",
        .path = "/api/token",
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

/**
* @brief This function sends a request to the Spotify API to perform a play/pause/next/previous commands.
* @param[in] Command_ The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[in] AccessToken given by authorization step.
* @return This function does not return a value.
*/
void Spotify_ControlPlayback(int Command, char *AccessToken)
{
    // fill clientPath variables based on the Spotify API document
    char clientPath[30] = {};
    esp_http_client_method_t clientMethod = HTTP_METHOD_POST;
    if (Command == Play)
    {
        sprintf(clientPath, "/v1/me/player/play");
        clientMethod = HTTP_METHOD_PUT;
    }
    else if(Command == Pause)
    {
        sprintf(clientPath, "/v1/me/player/pause");
        clientMethod = HTTP_METHOD_PUT;
    }
    else if(Command == PlayNext)
    {
        sprintf(clientPath, "/v1/me/player/next");
        clientMethod = HTTP_METHOD_POST;
    }
    else if(Command == PlayPrev)
    {
        sprintf(clientPath, "/v1/me/player/previous");
        clientMethod = HTTP_METHOD_POST;
    }
    else
    {
        ESP_LOGE(TAG, "Incorrect function call - incompatible command code for 'Spotify_ControlPlayback'");
    }
    
    // Configure client object  
    esp_http_client_config_t clientConfig = {
        .host = "api.spotify.com",                                                              // host for spotify api call
        .path = clientPath,                                                                     // clientPath already filled based in API
        .method = clientMethod,                                                                 // method variable already filled based on API 
        .event_handler = HttpEventHandler,                                                      // Event handler function
        .disable_auto_redirect = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&clientConfig);                  // apply configuration to the client object

    if (httpClient == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create the HTTP client");
        return;
    }

    // Set headers for authentication
    char authorizationHeader[SMALL_BUF];
    snprintf(authorizationHeader, sizeof(authorizationHeader), "Bearer %s", AccessToken);
    esp_http_client_set_header(httpClient, "Authorization", authorizationHeader);               // authorization set based on Spotify API
    esp_http_client_set_header(httpClient, "Content-Length", "0");                              // thess requests are not going to send any data to the host

    // Perform the GET request
    esp_err_t err = esp_http_client_perform(httpClient) == ESP_OK;                                        // perform http request
    if (!err) 
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return;
    }

    SpotifyBuffer->status = esp_http_client_get_status_code(httpClient);
    SpotifyBuffer->ContentLength = esp_http_client_get_content_length(httpClient);
    ESP_LOGI(TAG, "HTTP GET Status = %lld, content_length = %lld",
            SpotifyBuffer->status,
            SpotifyBuffer->ContentLength);
    // Cleanup
    esp_http_client_cleanup(httpClient);                                                        // close all connection releated to this client object 
}

/**
 * @brief This function sends a request to the Spotify API to give Curent playing information
 */
void Spotify_GetInfo(int Command, char *AccessToken)
{
    // fill clientPath and clientMethod variables based on the Spotify API document
    char clientPath[50] = {};
    if (Command == GetNowPlaying)
    {
        sprintf(clientPath, "/v1/me/player/currently-playing");
    }
    else if(Command == GetUserInfo)
    {
        sprintf(clientPath, "/v1/me");
    }
    else if(Command == GetUserTopItems)
    {
        sprintf(clientPath, "v1/me/top/artists");
    }
    else
    {
        ESP_LOGE(TAG, "Incorrect function call - incompatible command code for 'Spotify_GetInfo'");
    }

   // Configure client object  
    esp_http_client_config_t clientConfig = {
        .host = "api.spotify.com",                                                          // host for spotify api call
        .path = clientPath,                                                                 // clientPath already filled based in API
        .method = HTTP_METHOD_GET,                                                          // Get for all of this category requests  
        .event_handler = HttpEventHandler,                                                  // Event handler function
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t httpClient = esp_http_client_init(&clientConfig);                  // apply configuration to the client object
    if (httpClient == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create the HTTP client");
        return;
    }

    // Set headers for authentication
    char authorizationHeader[SMALL_BUF];
    snprintf(authorizationHeader, sizeof(authorizationHeader), "Bearer %s", AccessToken);
    esp_http_client_set_header(httpClient, "Authorization", authorizationHeader);               // authorization set based on Spotify API
    esp_http_client_set_header(httpClient, "Content-Length", "0");                              // thess requests are not going to send any data to the host

    // Perform the GET request
    esp_err_t err = esp_http_client_perform(httpClient) == ESP_OK;                                        // perform http request
    if (!err) 
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        return;
    }
    SpotifyBuffer->status = esp_http_client_get_status_code(httpClient);
    SpotifyBuffer->ContentLength = esp_http_client_get_content_length(httpClient);
    ESP_LOGI(TAG, "HTTP GET Status = %lld, content_length = %lld",
            SpotifyBuffer->status,
            SpotifyBuffer->ContentLength);
    // Cleanup
    esp_http_client_cleanup(httpClient);                                                        // close all connection releated to this client object 
}

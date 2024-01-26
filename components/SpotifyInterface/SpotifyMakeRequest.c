#include "SpotifyMakeRequest.h"


static const char *TAG = "HTTP";

// Include the server root certificate data
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[] asm("_binary_server_root_cert_pem_end");

// Include the local server certificate and private key data
extern const uint8_t local_server_cert_pem_start[] asm("_binary_local_server_cert_pem_start");
extern const uint8_t local_server_cert_pem_end[] asm("_binary_local_server_cert_pem_end");
extern const uint8_t local_server_key_pem_start[] asm("_binary_local_server_key_pem_start");
extern const uint8_t local_server_key_pem_end[] asm("_binary_local_server_key_pem_end");

/**
 * @brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was Found.
 * @param[in] Response The character array to search within, and Response is response from first stage from spotify athurisiation
 * @param[in] SizeRes The size of the character array.
 * @return Returns true if either the 'code' or 'state' pattern was Found, and false otherwise.
 */
bool Spotify_FindCode(char *Response, uint16_t SizeRes)
{
    char *CodeString = {"code"};
    uint16_t CodeLength = strlen(CodeString);

    if (Response == NULL || SizeRes < CodeLength)
    {
        // Invalid input, either null pointer or insufficient buffer size
        return false;
    }

    for (uint16_t i = 0; i <= SizeRes - CodeLength; ++i)
    {
        bool Found = true;
        for (uint16_t j = 0; j < CodeLength; ++j)
        {
            if (Response[i + j] != CodeString[j])
            {
                Found = false;
                break;
            }
        }
        if (Found)
        {
            return true;    // Found the access token substring
        }
    }
    return false;           // Access token substring not Found
}

static esp_err_t HttpEventHandler(esp_http_client_event_t *evt) 
{
    static char receivedData[MEDIUM_BUF] = {};
    static int totalLen = 0;
    static bool dataToRead = false;
    // Create the queue and semaphore

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
            dataToRead = true;                                                      // set flag true if server set some data
            memcpy(receivedData + totalLen, evt->data, evt->data_len);              // copy received data to the end of previous received data
            totalLen += evt->data_len;                                              // update pointer to the end of copied data
            break;
        case HTTP_EVENT_ON_FINISH:
            receivedData[totalLen] = '\0';                                          // write 0 to the end of string
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            ESP_LOGI(TAG, "### %s ###", receivedData); 
            // if any data received, send a queue so other tasks can receive and process it
            if (dataToRead == true)                             
            {
                if (xQueueSend(httpToSpotifyDataQueue, receivedData, pdMS_TO_TICKS(SEC * 15)) == pdTRUE)
                {
                    ESP_LOGI(TAG, "Data sent from HTTP to Spotify Interface Handler");
                }
                else
                {
                    ESP_LOGE(TAG, "Sending Received Data by Queue failed"); 
                }
                dataToRead = false;     // reset flag tp prepare it for next packets
            }
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "redirected");
            break;
    }
    return ESP_OK;
}

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in] code is parameter that we give it before .
 * @param[in] SizeCode The size of the authorization code.
 * @return This function does not return a value.
 */
void Spotify_SendTokenRequest(char *code, size_t SizeCode)
{  
    esp_http_client_config_t custom_config = {
        .url = "https://accounts.spotify.com/api/token",                            
        .host = "accounts.spotify.com",
        .path = "/api/token",
        .method = HTTP_METHOD_POST,
        .event_handler = HttpEventHandler,
        .disable_auto_redirect = false,
        .cert_pem = (const char *)server_root_cert_pem_start,                           // Server root certificate
        .cert_len = server_root_cert_pem_end - server_root_cert_pem_start,              // Length of server root certification
        .client_cert_pem = (const char *)local_server_cert_pem_start,                   // Local server certificate
        .client_cert_len = local_server_cert_pem_end - local_server_cert_pem_start,     // Length of local server certificate
        .client_key_pem = (const char *)local_server_key_pem_start,                     // Local server private key
        .client_key_len = local_server_key_pem_end - local_server_key_pem_start,        // Length of local server private key
    };

    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t client = esp_http_client_init(&custom_config);

    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    // Set headers for authentication and content type
    esp_http_client_set_header(client, "Authorization", "Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=");
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_header(client, "Cookie", "__Host-device_id=AQAwmp7jxagopcWw89BjSDAA530mHwIieOZdJ9Im8nI0-70oEsSInx3jkeSO09YQ7sPgPaIUyMEvZ-tct7I6OlshJrzVYOqcgo0; sp_tr=false");


    // Set the request body (POST data)
    char Grand[MEDIUM_BUF] = {0};
    sprintf(Grand, "grant_type=authorization_code&redirect_uri=%s&%s", ReDirectUri, code);
    esp_http_client_set_post_field(client, Grand, strlen(Grand));

    // Enable detailed logging for debugging
    // esp_http_client_set_debug(client, true);

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(client);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "HTTP client performed successfully");
    }
    
    // Cleanup HTTP client
    esp_http_client_cleanup(client);

}

/**
* @brief This function sends a request to the Spotify API to perform a play/pause/next/previous commands.
* @param[in] Command_ The specific player command to be executed (e.g., "next", "previous", "play", "pause").
* @param[in] AccessToken given by authorization step.
* @return This function does not return a value.
*/
void Spotify_ControlPlayback(int Command, char *AccessToken)
{
    // fill path and method variables based on the Spotify API document
    char path[30] = {};
    esp_http_client_method_t method = HTTP_METHOD_POST;
    if (Command == Play)
    {
        sprintf(path, "/v1/me/player/play");
        method = HTTP_METHOD_PUT;
    }
    else if(Command == Pause)
    {
        sprintf(path, "/v1/me/player/pause");
        method = HTTP_METHOD_PUT;
    }
    else if(Command == PlayNext)
    {
        sprintf(path, "/v1/me/player/next");
        method = HTTP_METHOD_POST;
    }
    else if(Command == PlayPrev)
    {
        sprintf(path, "/v1/me/player/previous");
        method = HTTP_METHOD_POST;
    }
    
    // Configure client object  
    esp_http_client_config_t custom_config = {
    .host = "api.spotify.com",                                                          // host for spotify api call
    .path = path,                                                                       // path already filled based in API
    .method = method,                                                                   // method variable already filled based on API 
    .event_handler = HttpEventHandler,                                                  // Event handler function
    .disable_auto_redirect = false,
        .cert_pem = (const char *)server_root_cert_pem_start,                           // Server root certificate
        .cert_len = server_root_cert_pem_end - server_root_cert_pem_start,              // Length of server root certification
        .client_cert_pem = (const char *)local_server_cert_pem_start,                   // Local server certificate
        .client_cert_len = local_server_cert_pem_end - local_server_cert_pem_start,     // Length of local server certificate
        .client_key_pem = (const char *)local_server_key_pem_start,                     // Local server private key
        .client_key_len = local_server_key_pem_end - local_server_key_pem_start,        // Length of local server private key
};

// Initialize HTTP client with custom configuration
esp_http_client_handle_t client = esp_http_client_init(&custom_config);                 // apply configuration to the client object

if (client == NULL) {
    ESP_LOGE(TAG, "Failed to create the HTTP client");
    return;
}

// Set headers for authentication
char authorizationHeader[MEDIUM_BUF];
snprintf(authorizationHeader, sizeof(authorizationHeader), "Bearer %s", AccessToken);
esp_http_client_set_header(client, "Authorization", authorizationHeader);               // authorization set based on Spotify API
esp_http_client_set_header(client, "Content-Length", "0");                              // thess requests are not going to send any data to the host

// Perform the GET request
esp_err_t err = esp_http_client_perform(client);                                        // perform http request

if (err == ESP_OK) 
{
    ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
             esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));
} 
else 
{
    ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
}

// Cleanup
esp_http_client_cleanup(client);                                                        // close all connection releated to this client object 
}

/**
 * @brief This function sends a request to the Spotify API to retrieve the user's profile information.
 * @return This function does not return a value.
 */
void Spotify_GetUserStatus(char *AccessToken)
{
    char Buf[LONG_BUF];
    sprintf(Buf, "GET /v1/me/ HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetUserStatus\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    //Https_GetRequest(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to retrieve the user's top item.
 * @return This function does not return a value.
 */
void Spotify_GetUserTopItems(char *AccessToken)
{
    char Buf[LONG_BUF];
    sprintf(Buf, "GET /v1/me/top/artists HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetUserTopItems\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    //Https_GetRequest(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to retrieve the profile information of a specific user.
 * @param[in] UserId_t The ID of the user whose profile information is to be retrieved.
 * @return This function does not return a value.
 */
void Spotify_GetUserProfile(char *UserId_t, char *Token)
{
    char Buf[LONG_BUF];
    char UserId[50];
    strcpy(UserId, UserId_t);
    sprintf(Buf, "GET /v1/users/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            UserId, Token);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetUserProfile\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    //Https_GetRequest(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify API to give Curent playing information
 */
void Spotify_GetCurrentPlaying(Token_t *Token)
{
    // return json is so long and we need longer buffer
    char Buf[LONG_BUF + MEDIUM_BUF+SMALL_BUF];
    sprintf(Buf, "GET /v1/me/player/currently-playing HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Connection: close\r\n\r\n",
            Token->AccessToken);
    ESP_LOGI(TAG, "\n\nrequest for Spotify_GetCurrentPlaying\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char URL[SMALL_BUF] = "https://api.spotify.com";
    char Server[SMALL_BUF] = "api.spotify.com";
    //Https_GetRequest(Buf, sizeof(Buf), URL, sizeof(URL), Server, sizeof(Server));
}

/**
 * @brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 * @param[in,out] Buf The character buffer to store the request and receive the response.
 * @param[in] SizeBuf The size of the character buffer.
 * @param[in] code is parameter that we give it before .
 * @return This function does not return a value.
 */
void SendRequest_ExchangeTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *refreshToken)
{
    esp_http_client_config_t custom_config = {
        .url = "https://accounts.spotify.com/api/token",
        .host = "accounts.spotify.com",
        .path = "/api/token",
        .method = HTTP_METHOD_POST,
        .event_handler = HttpEventHandler,
        .disable_auto_redirect = false,
        .cert_pem = (const char *)server_root_cert_pem_start,                           // Server root certificate
        .cert_len = server_root_cert_pem_end - server_root_cert_pem_start,              // Length of server root certification
        .client_cert_pem = (const char *)local_server_cert_pem_start,                   // Local server certificate
        .client_cert_len = local_server_cert_pem_end - local_server_cert_pem_start,     // Length of local server certificate
        .client_key_pem = (const char *)local_server_key_pem_start,                     // Local server private key
        .client_key_len = local_server_key_pem_end - local_server_key_pem_start,        // Length of local server private key
    };

    // Initialize HTTP client with custom configuration
    esp_http_client_handle_t client = esp_http_client_init(&custom_config);

    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to refresh the token client");
        return;
    }

    // Set headers for authentication and content type
    esp_http_client_set_header(client, "Authorization", "Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=");
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");


    // Set the request body (POST data)
    char Grand[SMALL_BUF] = {0};
    sprintf(Grand, "grant_type=refresh_token&refresh_token=%s&redirect_uri=%s", refreshToken, ReDirectUri);
    esp_http_client_set_post_field(client, Grand, strlen(Grand));

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(client);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP client perform failed: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "HTTP client performed successfully");
    }
    
    esp_http_client_cleanup(client);                                                    // close all connection releated to this client object 
}
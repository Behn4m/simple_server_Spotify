#include "OauthHttpLocalServer.h"
#include "authorization.h"

static const char *TAG = "OauthTask";
// ******************************

QueueHandle_t SendCodeFromHttpToTask = NULL;

/**
 * @brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was Found.
 * @param[in] Response The character array to search within, and Response is response from first stage from Service athurisiation
 * @param[in] SizeRes The size of the character array.
 * @return Returns true if either the 'code' or 'state' pattern was Found, and false otherwise.
 */
bool FindCode(char *Response, uint16_t SizeRes)
{
    char *codeString = {"code"};
    uint16_t codeLength = strlen(codeString);

    if (Response == NULL || SizeRes < codeLength)
    {
        // Invalid input, either null pointer or insufficient buffer size
        return false;
    }

    for (uint16_t i = 0; i <= SizeRes - codeLength; ++i)
    {
        bool Found = true;
        for (uint16_t j = 0; j < codeLength; ++j)
        {
            if (Response[i + j] != codeString[j])
            {
                Found = false;
                break;
            }
        }
        if (Found)
        {
            return true; // Found the access token substring
        }
    }
    return false; // Access token substring not Found
}

/**
 * @brief This function handles the first HTTPS request to Service and redirects the user to the authorization page.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t RequestDataAccess(httpd_req_t *HttpdRequest)
{
    char *localURL;
    localURL = (char *)malloc((2 * SMALL_BUF) * sizeof(char));
    if (localURL == NULL)
    {
        ESP_LOGI(TAG, "Failed to allocate memory for the array.\n\n");
    }
    memset(localURL, 0x0, SMALL_BUF * 2);
    ESP_LOGI(TAG, "Starting authorization, sending request for TOKEN");
    sprintf(localURL, "http://accounts.spotify.com/authorize/?client_id=%s&response_type=code&redirect_uri=%s&scope=user-read-private%%20user-read-currently-playing%%20user-read-playback-state%%20user-modify-playback-state", CLIENT_ID, REDIRECT_URI);
    httpd_resp_set_hdr(HttpdRequest, "Location", localURL);
    httpd_resp_set_type(HttpdRequest, "text/plain");
    httpd_resp_set_status(HttpdRequest, "302");
    httpd_resp_send(HttpdRequest, "", HTTPD_RESP_USE_STRLEN);
    free(localURL);
    return ESP_OK;
}

/**
 * @brief This function handles the callback HTTPS request from Service and processes the response data.
 * @param[in] req The HTTP request object.
 * @return Returns ESP_OK if the request is processed successfully.
 */
static esp_err_t HttpsCallbackHandler(httpd_req_t *HttpdRequest)
{
    int bufferSize = httpd_req_get_url_query_len(HttpdRequest) + 1;     // +1 for null terminator
    char queryBuffer[bufferSize];                                       // Buffer to store the query string
    esp_err_t err = httpd_req_get_url_query_str(HttpdRequest, queryBuffer, sizeof(queryBuffer));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get the query string from the URL");
        return ESP_FAIL;
    }

    bool isCodeFound = FindCode(queryBuffer, sizeof(queryBuffer));
    if (!isCodeFound)
    {
        ESP_LOGE(TAG, "response does not include code at the beginning ");
        return ESP_FAIL;
    }

    bool isCodeSent = xQueueSend(SendCodeFromHttpToTask, queryBuffer,  pdMS_TO_TICKS(SEC*15));
    if (!isCodeSent)
    {
        ESP_LOGE(TAG, "Sent data with queue failed!");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "the CODE found in response");
    return ESP_OK;
}

/**
 * this strcut is http URL handler if receive "/" RequestDataAccess getting run
 */
static const httpd_uri_t Request_Access_URI = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = RequestDataAccess};

/**
 * this strcut is http URL handler if receive "/callback" HttpsUserCallBackFunc getting run
 */
static const httpd_uri_t Response_Access_URI = {
    .uri = "/callback/",
    .method = HTTP_GET,
    .handler = HttpsCallbackHandler};

/**
 * @brief This function starts the web HttpdServerHandler for handling HTTPS requests.
 * @return Returns the HTTP HttpdServerHandler handle if it is started successfully, or NULL otherwise.
 */
httpd_handle_t StartWebServer()
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
esp_err_t StopWebServer(httpd_handle_t HttpdServerHandler)
{
    return httpd_stop(HttpdServerHandler);
}

/**
 * @brief This function starts the mDNS service.
 */
bool StartMDNSService()
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

    err = mdns_instance_name_set("spotify");
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

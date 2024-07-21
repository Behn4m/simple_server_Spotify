#include "OauthHttpLocalServer.h"
#include "Authorization.h"

HttpClientInfo_t AuthClientInterface;
static const char *TAG = "OauthHTTPServer";
// ******************************

QueueHandle_t SendCodeFromHttpToTask = NULL;

/**
 * @brief This function searches for specific patterns ('code' and 'state') within a character 
 *        array and returns a boolean value indicating if either pattern was Found.
 * @param[in] Response The character array to search within, 
 *            and Response is response from first stage from Service athurisiation
 * @param[in] SizeRes The size of the character array.
 * @return Returns true if either the 'code' or 'state' pattern was Found, and false otherwise.
 */
static bool FindCode(char *Response, uint16_t SizeRes)
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
 * @brief This function handles the first HTTPS request to Service and 
 *        redirects the user to the authorization page.
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
    sprintf(localURL, AuthClientInterface.requestURL, 
            AuthClientInterface.clientID, AuthClientInterface.redirectURL);
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
 * @brief Initialize and return an httpd_uri_t structure for the request URI.
 * @param none
 * @return An initialized httpd_uri_t structure with the specified URI, method, and handler.
*/
static httpd_uri_t RequestURIInit(char *requestURI)
{
    const httpd_uri_t Request_Access_URI = {
        .uri = requestURI,
        .method = HTTP_GET,
        .handler = RequestDataAccess
    };

    return Request_Access_URI;
}

/**
 * @brief Initialize and return an httpd_uri_t structure for the response URI.
 * @param none
 * @return An initialized httpd_uri_t structure with the specified URI, method, and handler.
 */
static httpd_uri_t ResponseURIInit(char *responseURI)
{
    const httpd_uri_t Response_Access_URI = {
        .uri = responseURI,
        .method = HTTP_GET,
        .handler = HttpsCallbackHandler
    };

    return Response_Access_URI;
}

/**
 * @brief This function starts the web HttpdServerHandler for handling HTTPS requests.
 * @param Request_Access_URI The URI handler for the request access.
 * @param Response_Access_URI The URI handler for the response access.
 * @return Returns the HTTP HttpdServerHandler handle if it is started successfully, or NULL otherwise.
 */
static httpd_handle_t StartWebServer(httpd_uri_t Request_Access_URI, httpd_uri_t Response_Access_URI)
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
 * @param HttpdServerHandler The URI handler for the request access.
 * @return Returns the HTTP HttpdServerHandler handle if it is started successfully, or NULL otherwise.
 */
esp_err_t StopOauthWebServer(httpd_handle_t HttpdServerHandler)
{
    return httpd_stop(HttpdServerHandler);
}

/**
 * @brief This function starts the mDNS service.
 * @param hostname The hostname of the device.
 * @return Returns true if the mDNS service is started successfully, or false otherwise.
 */
static bool StartMDNSService(char *hostnameMDNS)
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

    err = mdns_instance_name_set(hostnameMDNS);
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
 * @param hostname The hostname of the device.
 * @param Request_Access_URI The URI handler for the request access.
 * @param Response_Access_URI The URI handler for the response access.
 * @return Returns true if the Http local service is started successfully, or false otherwise.
 */
bool HttpServerServiceInit(HttpClientInfo_t ClientConfig)
{
    SendCodeFromHttpToTask = 
            xQueueCreate(1, sizeof(char) * sizeof(char[MEDIUM_BUF]));

    AuthClientInterface = ClientConfig;
    httpd_uri_t Request_Access_URI = RequestURIInit(ClientConfig.requestURI);
    httpd_uri_t Response_Access_URI = ResponseURIInit(ClientConfig.responseURI);
    httpd_handle_t serviceLocalServer = StartWebServer(Request_Access_URI, Response_Access_URI);
    if (serviceLocalServer == NULL)
    {
        ESP_LOGE(TAG, "Creating Service local server failed!");
        return false;
    }
    
    bool IsMdnsStarted = StartMDNSService(ClientConfig.hostnameMDNS);
    if (!IsMdnsStarted)
    {
        ESP_LOGE(TAG, "Running mDNS failed!");
        return false;
    };

    ESP_LOGI(TAG, "** local server created, %s is running! **", ClientConfig.hostnameMDNS);
    return true;
}
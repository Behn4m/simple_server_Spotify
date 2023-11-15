#include "WiFiConfig.h"
struct UserWifi_ UserWifi
{
    char PassWord[64];
    char SSID[32];
};
static const char *TAG = "wifi AP Mode";
static esp_err_t DetectFileType(httpd_req_t *req, const char *FileName);
static esp_err_t ReadFromFileSystemAndSendIt(httpd_req_t *req, char *FileName_);

/**
 * @brief Retrieves the WiFi parameters from the HTTP request and performs necessary actions.
 * @param req The HTTP request object.
 * @returns ESP_OK if successful, ESP_FAIL otherwise.
 */
static esp_err_t GetWifiParam(httpd_req_t *req)
{
    char Buf[100];
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        ESP_LOGI(TAG, "\n\n\n%s\n\n", Buf);
        if (httpd_query_key_value(Buf, "login_username", UserWifi.SSID, sizeof(UserWifi.SSID)) == ESP_OK && httpd_query_key_value(Buf, "login_password", UserWifi.PassWord, sizeof(UserWifi.PassWord)) == ESP_OK)
        {
            httpd_resp_set_hdr(req, "Location", "http://wificonfig.local/successful");
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_set_status(req, "302");
            httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
            ESP_LOGI(TAG, "\n\n\nssid=%s\tpass%s\n\n", UserWifi.SSID, UserWifi.PassWord);
            xSemaphoreGive(WaitSemaphore);
            return ESP_OK;
        }
        else
        {
            httpd_resp_set_hdr(req, "Location", "http://wificonfig.local/unsuccessful");
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_set_status(req, "302");
            httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    }
    else
    {
        sprintf(Buf, "BAD ARGS");
        httpd_resp_set_hdr(req, "Location", "http://wificonfig.local/unsuccessful");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_set_status(req, "302");
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
        ESP_LOGI(TAG, "bad arguments - the response does not include correct structure");
        return ESP_FAIL;
    }
}

/**
 * @brief Handles a request for a SecPage page.
 * This function processes an HTTP request for a SecPage page. It reads the contents of the "SecPage.html" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t RequestWifiPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/SecPage.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * @brief Handles a request for a logo file.
 * This function processes an HTTP request for a logo file. It reads the contents of the "logo.png" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t RequestLogo(httpd_req_t *req)
{
    char FileName[] = "spiffs/logo.png";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * @brief Handles a request for a user-solid SVG file.
 * This function processes an HTTP request for a user-solid SVG file. It reads the contents of the "user-solid.svg" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t RequestUserSolidSvg(httpd_req_t *req)
{
    char FileName[] = "spiffs/user-solid.svg";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * @brief Handles a request for a lock-solid SVG file.
 * This function processes an HTTP request for a lock-solid SVG file. It reads the contents of the "lock-solid.svg" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t RequestLockSolidSvg(httpd_req_t *req)
{
    char FileName[] = "spiffs/lock-solid.svg";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * @brief Handles a request for an unsuccessful page.
 * This function processes an HTTP request for an unsuccessful page. It reads the contents of the "UNSuccessfull.html" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t RequestSuccessfulPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/Successfull.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * @brief Handles a request for the Font Awesome CSS file.
 * This function processes an HTTP request for the Font Awesome CSS file. It reads the contents of the "font-awesome.min.css" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t FontAweSomeMinCss(httpd_req_t *req)
{
    char FileName[] = "spiffs/css/font-awesome.min.css";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * @brief Handles a request for an unsuccessful page.
 * This function processes an HTTP request for an unsuccessful page. It reads the contents of the "UNSuccessfull.html" file from the SPIFFS file system and sends it as the response to the client.
 * @param[in] req The HTTP request object.
 * @returns An esp_err_t indicating the success or failure of the request.
 */
static esp_err_t RequestUNSuccessfulPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/UNSuccessfull.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}

/**
 * Detects the file type based on the file extension and sets the appropriate content type in the HTTP response.
 * @param req The HTTP request object.
 * @param FileName The name of the file.
 * @returns An esp_err_t indicating the success or failure of detecting the file type.
 */
static esp_err_t DetectFileType(httpd_req_t *req, const char *FileName)
{
    if (IS_FILE_EXT(FileName, ".pdf"))
    {
        return httpd_resp_set_type(req, "application/pdf");
    }
    else if (IS_FILE_EXT(FileName, ".html"))
    {
        return httpd_resp_set_type(req, "text/html");
    }
    else if (IS_FILE_EXT(FileName, ".jpeg"))
    {
        return httpd_resp_set_type(req, "image/jpeg");
    }
    else if (IS_FILE_EXT(FileName, ".ico"))
    {
        return httpd_resp_set_type(req, "image/x-icon");
    }
    else if (IS_FILE_EXT(FileName, ".svg"))
    {
        return httpd_resp_set_type(req, "image/svg+xml");
    }
    else if (IS_FILE_EXT(FileName, ".css"))
    {
        return httpd_resp_set_type(req, "text/css");
    }
    return httpd_resp_set_type(req, "text/plain");
}

/**
 * Reads a file from the file system and sends it as an HTTP response.
 * @param req The HTTP request object.
 * @param FileName_ The name of the file to read and send.
 * @returns An esp_err_t indicating the success or failure of reading and sending the file.
 */
static esp_err_t ReadFromFileSystemAndSendIt(httpd_req_t *req, char *FileName_)
{
    FILE *fd = NULL;
    char FileName[100];
    strcpy(FileName, FileName_);
    sprintf(FileName, "/%s", FileName_);
    fd = fopen(FileName, "r");
    if (!fd)
    {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }
    DetectFileType(req, FileName);
    char Buf[1000];
    memset(Buf, 0x0, sizeof(Buf));
    size_t BufSize;
    do
    {
        BufSize = fread(Buf, 1, sizeof(Buf), fd);
        if (BufSize > 0)
        {
            /* Send the buffer contents as HTTP response Buf */
            if (httpd_resp_send_chunk(req, Buf, BufSize) != ESP_OK)
            {
                fclose(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (BufSize != 0);
    fclose(fd);
    ESP_LOGI(TAG, "File sending complete");
/* Respond with an empty Buf to signal HTTP response completion */
#ifdef CONFIG_EXAMPLE_HTTPD_CONN_CLOSE_HEADER
    httpd_resp_set_hdr(req, "Connection", "close");
#endif
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/**
 * @brief Starts a web server locally.
 * This function starts a web server locally using the default configuration. It registers various URI handlers for different endpoints and returns the server handle.
 * @returns The handle to the started web server.
 */
httpd_handle_t StartWebServerLocally(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting Server_ on port: '%d'", config.server_port);
    httpd_handle_t Server_ = NULL;
    httpd_uri_t SecPage_ = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = RequestWifiPage,
        .user_ctx = Server_};
    httpd_uri_t Successful_ = {
        .uri = "/successful",
        .method = HTTP_GET,
        .handler = RequestSuccessfulPage,
        .user_ctx = Server_};
    httpd_uri_t UNSuccessful_ = {
        .uri = "/unsuccessful",
        .method = HTTP_GET,
        .handler = RequestUNSuccessfulPage,
        .user_ctx = Server_};
    httpd_uri_t GetWifiParam_ = {
        .uri = "/get",
        .method = HTTP_GET,
        .handler = GetWifiParam,
        .user_ctx = Server_};
    httpd_uri_t logo_ = {
        .uri = "/logo.png",
        .method = HTTP_GET,
        .handler = RequestLogo,
        .user_ctx = Server_};
    httpd_uri_t user_solid_svg = {
        .uri = "/user-solid.svg",
        .method = HTTP_GET,
        .handler = RequestUserSolidSvg,
        .user_ctx = Server_};
    httpd_uri_t lock_solid_svg = {
        .uri = "/lock-solid.svg",
        .method = HTTP_GET,
        .handler = RequestLockSolidSvg,
        .user_ctx = Server_};
    httpd_uri_t font_awesome_min_css = {
        .uri = "/css/font-awesome.min.css",
        .method = HTTP_GET,
        .handler = FontAweSomeMinCss,
        .user_ctx = Server_};
    if (httpd_start(&Server_, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(Server_, &SecPage_);
        httpd_register_uri_handler(Server_, &logo_);
        httpd_register_uri_handler(Server_, &GetWifiParam_);
        httpd_register_uri_handler(Server_, &font_awesome_min_css);
        httpd_register_uri_handler(Server_, &Successful_);
        httpd_register_uri_handler(Server_, &UNSuccessful_);
        httpd_register_uri_handler(Server_, &user_solid_svg);
        httpd_register_uri_handler(Server_, &lock_solid_svg);
        return Server_;
    }
    return Server_;
}

/**
 * @brief Stops a running web server.
 * This function stops a running web server identified by the provided server handle.
 * @param[in] server The handle to the web server.
 * @returns An esp_err_t indicating the success or failure of stopping the web server.
 */
esp_err_t StopWebServer(httpd_handle_t server)
{
    return httpd_stop(server);
}

/**
 * @brief Initializes and starts the mDNS service.
 * This function initializes and starts the mDNS (Multicast DNS) service. It sets the hostname and instance name for mDNS.
 */
void StartMDNSService()
{
    esp_err_t err = mdns_init();
    if (err)
    {
        ESP_LOGI(TAG, "MDNS Init failed: %d\n", err);
        return;
    }
    mdns_hostname_set("wificonfig");
    mdns_instance_name_set("Behnam's ESP32 Thing");
}

/**
 * @brief Initializes the SPIFFS (SPI Flash File System).
 * This function initializes the SPIFFS file system with the provided configuration. It registers the SPIFFS file system and checks the partition information.
 */
void SpiffsInit()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 10,
        .format_if_mount_failed = false};
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}
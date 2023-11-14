#include "WiFiConfig.h"
char UserWifiPassWord[64];
char UserWifiSSID[32];
bool ForFirstTimeFlag = 0;
static httpd_handle_t server_ = NULL;
static void WifiConnectionTask(void *pvparameters);
static const char *TAG = "wifi_AP_WEBserver";
static esp_err_t DetectFileType(httpd_req_t *req, const char *FileName);
static esp_err_t ReadFromFileSystemAndSendIt(httpd_req_t *req, char *FileName_);
/* An HTTP GET handler */
static esp_err_t GetWifiParam(httpd_req_t *req)
{
    char Buf[100];
    if (httpd_req_get_url_query_str(req, Buf, sizeof(Buf)) == ESP_OK)
    {
        ESP_LOGI(TAG, "\n\n\n%s\n\n", Buf);
        if (httpd_query_key_value(Buf, "login_username", UserWifiSSID, sizeof(UserWifiSSID)) == ESP_OK && httpd_query_key_value(Buf, "login_password", UserWifiPassWord, sizeof(UserWifiPassWord)) == ESP_OK)
        {
            httpd_resp_set_hdr(req, "Location", "http://wificonfig.local/successful");
            httpd_resp_set_type(req, "text/plain");
            httpd_resp_set_status(req, "302");
            httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
            ESP_LOGI(TAG, "\n\n\nssid=%s\tpass%s\n\n", UserWifiSSID, UserWifiPassWord);
            xSemaphoreGive(Wait);
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
static esp_err_t RequestWifiPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/SecPage.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestLogo(httpd_req_t *req)
{
    char FileName[] = "spiffs/logo.png";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestExclaim(httpd_req_t *req)
{
    char FileName[] = "spiffs/Exclam.png";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestUserSolidSvg(httpd_req_t *req)
{
    char FileName[] = "spiffs/user-solid.svg";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestLockSolidSvg(httpd_req_t *req)
{
    char FileName[] = "spiffs/lock-solid.svg";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestSuccessfulPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/Successfull.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t FontAweSomeMinCss(httpd_req_t *req)
{
    char FileName[] = "spiffs/css/font-awesome.min.css";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t FontAweSomeCss(httpd_req_t *req)
{
    char FileName[] = "spiffs//css/font-awesome.css";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestUNSuccessfulPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/UNSuccessfull.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
static esp_err_t RequestWaitPage(httpd_req_t *req)
{
    char FileName[] = "spiffs/Wait.html";
    return ReadFromFileSystemAndSendIt(req, FileName);
}
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
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}
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

static httpd_handle_t StartWebServerLocally(void)
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
    httpd_uri_t WaitPage = {
        .uri = "/wait",
        .method = HTTP_GET,
        .handler = RequestWaitPage,
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
    httpd_uri_t Exclam = {
        .uri = "/Exclam.png",
        .method = HTTP_GET,
        .handler = RequestExclaim,
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

    httpd_uri_t font_wesome_css = {
        .uri = "/css/font-awesome.css",
        .method = HTTP_GET,
        .handler = FontAweSomeCss,
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
        httpd_register_uri_handler(Server_, &font_wesome_css);
        httpd_register_uri_handler(Server_, &Exclam);
        httpd_register_uri_handler(Server_, &WaitPage);
        return Server_;
    }
    return Server_;
}
static esp_err_t StopWebServer(httpd_handle_t server)
{
    return httpd_stop(server);
}

/**
 * @brief This function starts the mDNS service.
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

//------------------------------------------------------------------------------
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

void wifiConnectionTaskCreation()
{
    ESP_LOGI(TAG, "creat wifi task");
    xTaskCreate(&WifiConnectionTask, "WifiConnectionTask", 10000, NULL, 1, NULL);
}

void WifiConnectionTask()
{
    ESP_LOGI(TAG, "NVS init");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    SpiffsInit();
    ESP_LOGI(TAG, "Eventloop create");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "init softAP");
    ESP_ERROR_CHECK(WifiSoftAccessPointMode(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS));
    StartMDNSService();
    server_ = StartWebServerLocally();
    ForFirstTimeFlag = 1;
    Wait = xSemaphoreCreateBinary();
    ExitFromApMode = xSemaphoreCreateBinary();
    StayInApModeSemaphore = xSemaphoreCreateBinary();
    while (1)
    {
        ESP_LOGI(TAG, "wait \n");
        if (xSemaphoreTake(Wait, portMAX_DELAY) == pdTRUE)
        {
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            WifiStationMode(UserWifiSSID, UserWifiPassWord);
            // wifi_sta_handler();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            while (1)
            {
                if (xSemaphoreTake(ExitFromApMode, 10 / portTICK_PERIOD_MS) == pdTRUE)
                {
                    ESP_LOGI(TAG, "\nExitFromApMode");
                    StopWebServer(server_);
                    server_ = NULL;
                    mdns_free();
                    vTaskDelete(NULL);
                }
                if (xSemaphoreTake(StayInApModeSemaphore, 10 / portTICK_PERIOD_MS) == pdTRUE)
                {
                    ESP_LOGI(TAG, "\nStayInApModeSemaphore");
                    ESP_ERROR_CHECK(WifiSoftAccessPointMode(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS));
                    break;
                }
            }
        }
    }
}
/**
 * @brief This function handles Wi-Fi events and prints corresponding messages based on the event ID.
 * @param[in] event_handler_arg The event handler argument (not used in this function).
 * @param[in] event_base The event base.
 * @param[in] event_id The event ID.
 * @param[in] event_data The event data (not used in this function).
 */
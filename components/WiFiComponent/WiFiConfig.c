#include "WiFiConfig.h"
#include "HttpLocalServer.h"
#define MAXIMUM_RETRY_TO_CONNECT 1
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static EventGroupHandle_t StationModeEventGroup;
extern SemaphoreHandle_t FinishWifiConfig;
SemaphoreHandle_t WaitSemaphore;
SemaphoreHandle_t ExitFromApModeSemaphore;
SemaphoreHandle_t StayInApModeSemaphore;
extern SemaphoreHandle_t FinishWifiConfig;
esp_netif_t *NetifAccessPointStruct;
esp_netif_t *NetifStationStruct;
httpd_handle_t server_ = NULL;
static const char *TAG = "wifi config mode";
static int RetryTime = 0;
bool IsFirstTimeFlag = 0;
bool IsThereSaveFlag = 0;
void WifiConnectionTask();

/**
 * @brief handler for WiFi and IP events.
 * @param[in]Arg Pointer to user-defined data.
 * @param[in] EventBase The event base of the event.
 * @param[in] EventId The event ID.
 * @param[in] EventData Pointer to the event data.
 * @returns None
 */
static void EventStationModeHandler(void *Arg, esp_event_base_t EventBase,
                                    int32_t EventId, void *EventData)
{
    if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (RetryTime < MAXIMUM_RETRY_TO_CONNECT)
        {
            esp_wifi_connect();
            RetryTime++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(StationModeEventGroup, WIFI_FAIL_BIT);
            xSemaphoreGive(StayInApModeSemaphore);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG, "WiFi connected ... \n");
        // check if the ExitFromApModeSemaphore is defined in AP mode, then give it
        if (ExitFromApModeSemaphore != NULL) 
        {
            xSemaphoreGive(ExitFromApModeSemaphore);
        }
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_FAILED)
    {
        ESP_LOGW(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_FAILED config  failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_TIMEOUT)
    {
        ESP_LOGW(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_TIMEOUT config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_PIN)
    {
        ESP_LOGW(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_PIN config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP)
    {
        ESP_LOGW(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_PBC_OVERLAPconfig failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_BEACON_TIMEOUT)
    {
        ESP_LOGW(TAG, "WiFi WIFI_EVENT_STA_BEACON_TIMEOUT config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == IP_EVENT && EventId == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)EventData;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        RetryTime = 0;
        xEventGroupSetBits(StationModeEventGroup, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief Sets up WiFi station mode.
 * @param[in] ssid      SSID of the access point.
 * @param[in] password  Password of the access point.
 * @return esp_err_t
 */
esp_err_t WifiStationMode(char *UserWifiSSID_, char *UserWifiPassWord_)
{
    #ifndef WIFI_INIT_STA_MODE
    if (IsThereSaveFlag == 0)
    {
        esp_netif_deinit();
        if (IsFirstTimeFlag == 0)
        {
            esp_netif_destroy(NetifAccessPointStruct);
            esp_netif_destroy(NetifStationStruct);
        }
        esp_wifi_stop();
        esp_wifi_deinit();
    }
    #endif
    StationModeEventGroup = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    NetifStationStruct = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &EventStationModeHandler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &EventStationModeHandler, NULL, &instance_got_ip));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };
    strcpy((char *)wifi_config.sta.ssid, UserWifiSSID_);
    strcpy((char *)wifi_config.sta.password, UserWifiPassWord_);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_init_sta finished.");
    EventBits_t bits = xEventGroupWaitBits(StationModeEventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", UserWifiSSID_, UserWifiPassWord_);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", UserWifiSSID_, UserWifiPassWord_);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    return ESP_OK;
}

/**
 * @brief Event handler for WiFi access point events.
 * @param[in] arg Pointer to user-defined data.
 * @param[in] EventBase The event base associated with the event.
 * @param[in] EventID The ID of the event.
 * @param[in] EventData Pointer to the event data.
 * @returns None
 */
static void WifiAccessPointEvenHandler(void *arg, esp_event_base_t EventBase, int32_t EventID, void *EventData)
{
    if (EventID == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)EventData;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    else if (EventID == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)EventData;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

/**
 * @brief Configures the ESP32 to operate in Soft Access Point mode.
 * @param[in] WifiAccessPointSSID The SSID (network name) of the access point.
 * @param[in] WifiAccessPointPassWord The password for the access point.
 * @returns ESP_OK if the operation is successful, otherwise an error code.
 */
esp_err_t WifiSoftAccessPointMode(char *WifiAccessPointSSID, char *WifiAccessPointPassWord)
{
    if (IsFirstTimeFlag == 1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        esp_netif_deinit();
        esp_wifi_stop();
        esp_wifi_deinit();
        esp_netif_destroy(NetifStationStruct);
        esp_netif_destroy(NetifAccessPointStruct);
        ESP_ERROR_CHECK(esp_netif_init());
        esp_netif_create_default_wifi_ap();
    }
    else
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        NetifAccessPointStruct = esp_netif_create_default_wifi_ap();
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiAccessPointEvenHandler, NULL));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "",
            .ssid_len = strlen(WifiAccessPointSSID),
            .password = "",
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    strcpy((char *)wifi_config.ap.ssid, WifiAccessPointSSID);
    strcpy((char *)wifi_config.ap.password, WifiAccessPointPassWord);
    if (strlen(WifiAccessPointPassWord) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WifiSoftAccessPointMode finished. SSID:%s password:%s",
             WifiAccessPointSSID, WifiAccessPointPassWord);
    IsFirstTimeFlag = 0;
    return ESP_OK;
}

/**
 * @brief Creates a task for handling Wi-Fi connection.
 * This function creates a task for handling Wi-Fi connection. It creates a task with the `WifiConnectionTask` function as the entry point.
 */
void wifiConnectionModule()
{
    ESP_LOGI(TAG, "creat wifi task");
    xTaskCreate(&WifiConnectionTask, "WifiConnectionTask", WifiModuleTaskStackSize, NULL, 1, NULL);
    if (xSemaphoreTake(FinishWifiConfig, portMAX_DELAY) == pdTRUE)
    {
        ESP_LOGI(TAG, "wifi configuration get finish !");
    }
}

/**
 * @brief Entry point for the Wi-Fi connection task.
 * This function is the entry point for the Wi-Fi connection task. It initializes necessary components, sets up the SPIFFS, starts the mDNS service, starts the web server, and waits for Wi-Fi connection events.
 */
void WifiConnectionTask()
{
    ESP_LOGI(TAG, "NVS init");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ExitFromApModeSemaphore = xSemaphoreCreateBinary();
    if (xSemaphoreTake(WifiParamExistenceCheckerSemaphore, 1) == pdTRUE)
    {
        ESP_LOGI(TAG, "we have save file ! ");
        ReadTxtFileFromSpiffs(WifiConfigDirectoryAddressInSpiffs, "SSID", UserWifi.SSID, "PASS", UserWifi.PassWord, NULL, NULL);
        IsThereSaveFlag = 1;
        WifiStationMode(UserWifi.SSID, UserWifi.PassWord);
        if (xSemaphoreTake(ExitFromApModeSemaphore, 30 * SEC / portTICK_PERIOD_MS) == pdTRUE)
        {
            ESP_LOGI(TAG, "FinishWifiConfig semaphore ");
            vTaskDelay(3 * SEC / portTICK_PERIOD_MS);
            xSemaphoreGive(FinishWifiConfig);
            vTaskDelete(NULL);
        }
        else
        {
            ESP_LOGE(TAG, "SSID or Password or wifi have problem !");
        }
    }
    else
    {
        ESP_LOGE(TAG, "we does not have save file ! ");
    }
    ESP_LOGI(TAG, "init softAP");
    ESP_ERROR_CHECK(WifiSoftAccessPointMode(ESP_WIFI_SSID, ESP_WIFI_PASS));
    StartMDNSServiceForWifi();
    server_ = StartWebServerLocally();
    IsFirstTimeFlag = 1;
    WaitSemaphore = xSemaphoreCreateBinary();
    StayInApModeSemaphore = xSemaphoreCreateBinary();
    while (1)
    {
        ESP_LOGI(TAG, "wait \n");
        if (xSemaphoreTake(WaitSemaphore, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "give ssid and password but , we do'nt test them");
            vTaskDelay(3*SEC / portTICK_PERIOD_MS);
            WifiStationMode(UserWifi.SSID, UserWifi.PassWord);
            vTaskDelay(10 * SEC / portTICK_PERIOD_MS);
            while (1)
            {
                if (xSemaphoreTake(ExitFromApModeSemaphore, 10 / portTICK_PERIOD_MS) == pdTRUE)
                {
                    ESP_LOGI(TAG, "\nExitFromApMode");
                    StopWebServer(server_);
                    server_ = NULL;
                    mdns_free();
                    SaveFileInSpiffsWithTxtFormat(WifiConfigDirectoryAddressInSpiffs, "SSID", UserWifi.SSID, "PASS", UserWifi.PassWord, NULL, NULL);
                    xSemaphoreGive(FinishWifiConfig);
                    vTaskDelete(NULL);
                }
                if (xSemaphoreTake(StayInApModeSemaphore, 10 / portTICK_PERIOD_MS) == pdTRUE)
                {
                    ESP_LOGI(TAG, "\nStayInApModeSemaphore");
                    ESP_ERROR_CHECK(WifiSoftAccessPointMode(ESP_WIFI_SSID, ESP_WIFI_PASS));
                    break;
                }
            }
        }
    }
}
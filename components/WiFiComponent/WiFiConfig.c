#include "WiFiConfig.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#define MAXIMUMRETRYTOCONNECT 1
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static EventGroupHandle_t s_wifi_event_group;
SemaphoreHandle_t Wait;
SemaphoreHandle_t ExitFromApMode;
SemaphoreHandle_t StayInApModeSemaphore;
esp_netif_t *NetifAccessPointStruct;
static const char *TAG = "go to wifi station mode";
static int RetryTime = 0;
static void EventAPHandler(void *Arg, esp_event_base_t EventBase,
                           int32_t EventId, void *EventData)
{
    if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (RetryTime < MAXIMUMRETRYTOCONNECT)
        {
            esp_wifi_connect();
            RetryTime++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            xSemaphoreGive(StayInApModeSemaphore);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG, "WiFi connected ... \n");
        xSemaphoreGive(ExitFromApMode);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_FAILED)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_FAILED config  failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_TIMEOUT)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_TIMEOUT config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_PIN)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_PIN config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_PBC_OVERLAPconfig failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && EventId == WIFI_EVENT_STA_BEACON_TIMEOUT)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_BEACON_TIMEOUT config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == IP_EVENT && EventId == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)EventData;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        RetryTime = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
/**
 * @brief This function handles the Wi-Fi Station Mode connection process.
 * @return return ESP error
 */
esp_err_t WifiStationMode(char *UserWifiSSID_, char *UserWifiPassWord_)
{
    esp_netif_deinit();
    if (ForFirstTimeFlag == 0)
    {
    esp_netif_destroy(NetifAccessPointStruct);
    }
    esp_wifi_stop();
    esp_wifi_deinit();
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &EventAPHandler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &EventAPHandler, NULL, &instance_got_ip));
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
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
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

esp_err_t WifiSoftAccessPointMode(char *WifiAccessPointSSID, char *WifiAccessPointPassWord)
{
    if (ForFirstTimeFlag == 1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        esp_netif_deinit();
        esp_wifi_stop();
        esp_wifi_deinit();
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
            .max_connection = EXAMPLE_MAX_STA_CONN,
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
    return ESP_OK;
}
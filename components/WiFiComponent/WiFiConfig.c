#include "WiFiConfig.h"
extern char UserWifiPassWord[64];
extern char UserWifiSSID[32];
bool FindWifiSSIDAndPassword(char *Res, uint16_t SizeRes)
{
    uint8_t flg_findSSID = 0;
    uint8_t flg_findPassword = 0;
    for (uint16_t i = 0; i < SizeRes; i++)
    {
        if (Res[i] == 'u')
        {
            if (Res[i + 1] == 's' && Res[i + 2] == 'e' && Res[i + 3] == 'r' && Res[i + 4] == 'n' && Res[i + 5] == 'a' && Res[i + 6] == 'm' && Res[i + 7] == 'e')
            {

                for (int j = i + 9; j < SizeRes; j++)
                {
                    UserWifiSSID[j - (i + 9)] = Res[j];
                    if (Res[j] == '&')
                    {
                        break;
                        flg_findSSID = 1;
                        printf("\twe find SSID !\n");
                    }
                }
            }
        }
        if (flg_findSSID == 1)
        {
            if (Res[i] == 'l')
            {
                if (Res[i + 1] == 'o' && Res[i + 2] == 'g' && Res[i + 3] == 'i' && Res[i + 4] == 'n' && Res[i + 5] == '_')
                {
                    for (int j = i + 15; j < SizeRes; j++)
                    {
                        UserWifiPassWord[j - (i + 9)] = Res[j];
                        break;
                        printf("\twe find Password!\n");
                        flg_findPassword = 1;
                    }
                }
            }
        }
    }
    if (flg_findSSID == 1 || flg_findPassword == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
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
#define EXAMPLE_ESP_MAXIMUM_RETRY 50
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define EXAMPLE_H2E_IDENTIFIER ""
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static EventGroupHandle_t s_wifi_event_group;
static const char *TAG = "go to wifi station mode";
static int RetryNumber = 0;
static void EventHandler(void *arg, esp_event_base_t EventBase,
                         int32_t event_id, void *EventData)
{
    if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (RetryNumber < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            RetryNumber++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG, "WiFi connected ... \n");
        xSemaphoreGive(ExitFromApMode);
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_WPS_ER_FAILED)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_FAILED config  failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_WPS_ER_TIMEOUT)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_TIMEOUT config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_WPS_ER_PIN)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_PIN config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_WPS_ER_PBC_OVERLAPconfig failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == WIFI_EVENT && event_id == WIFI_EVENT_STA_BEACON_TIMEOUT)
    {
        ESP_LOGI(TAG, "WiFi WIFI_EVENT_STA_BEACON_TIMEOUT config failed... \n");
        xSemaphoreGive(StayInApModeSemaphore);
    }
    else if (EventBase == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)EventData;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        RetryNumber = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
/**
 * @brief This function handles the Wi-Fi connection process.
 */
void WifiSTAMode(char *UserWifiSSID_, char *UserWifiPassWord_)
{
    esp_netif_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &EventHandler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &EventHandler, NULL, &instance_got_ip));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
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
}
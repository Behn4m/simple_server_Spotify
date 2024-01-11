idf_component_register(SRCS "HttpLocalServer.c" "WiFiConfig.c"
                    INCLUDE_DIRS "include"
                    REQUIRES  esp_wifi esp_eth protocol_examples_common
                    esp_http_client nvs_flash GlobalInit spiffs nvsFlash
                    esp_psram heap)

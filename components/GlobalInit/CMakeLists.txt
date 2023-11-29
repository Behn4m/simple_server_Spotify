idf_component_register(SRCS "GlobalInit.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mdns esp_wifi esp_eth protocol_examples_common
                    esp_https_server esp_http_client nvs_flash  esp_timer  nvsFlash)

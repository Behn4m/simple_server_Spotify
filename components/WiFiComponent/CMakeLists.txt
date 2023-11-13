idf_component_register(SRCS "WiFiConfig.c"
                    INCLUDE_DIRS "include"
                    REQUIRES  esp_wifi esp_eth protocol_examples_common
                    esp_http_client nvs_flash GlobalInit
                    )
# I need main component ,because have some global variable ,without main component dose not work
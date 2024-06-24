idf_component_register(SRCS "authorization.c" "OauthAPICall.c" "OauthHttpLocalServer.c"
                    INCLUDE_DIRS "include"
                    
                    REQUIRES mdns esp_event esp-tls
                    esp_http_server esp_http_client nvs_flash json
                    GlobalInit esp_psram heap) 
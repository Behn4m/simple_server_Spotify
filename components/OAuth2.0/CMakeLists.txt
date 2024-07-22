idf_component_register(SRCS "Authorization.c" "OauthAPICall.c" "OauthHttpLocalServer.c"
                    INCLUDE_DIRS "include"
                    
                    REQUIRES esp_psram mdns heap esp_event esp-tls
                    esp_http_server esp_http_client nvs_flash json
                    nvsFlash) 
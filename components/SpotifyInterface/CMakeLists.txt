idf_component_register(SRCS "JsonExtraction.c" "SpotifyInterface.c" 
                    INCLUDE_DIRS "include"
                    
                    REQUIRES mdns esp_event mbedtls
                    esp_http_server esp_http_client nvs_flash json
                    esp_psram heap) 

#                     # REQUIRES Utilities mdns esp_jpeg esp_event mbedtls 
#                     # esp_http_server esp_http_client nvs_flash json
#                     # esp_psram heap) 

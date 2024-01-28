idf_component_register(SRCS "SpotifyAPICall.c" "JsonExtraction.c" "SpotifyInterface.c" 
                            "SpotifyHttpLocalServer.c"
                    INCLUDE_DIRS "include" "lib"
                    EMBED_TXTFILES server_root_cert.pem local_server_cert.pem local_server_key.pem

                    REQUIRES mdns esp_wifi esp_event esp_eth protocol_examples_common
                    esp_https_server esp_http_client nvs_flash json
                    GlobalInit esp_psram heap) 

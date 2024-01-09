idf_component_register(SRCS "JsonExtraction.c" "SpotifyInterface.c" "SpotifyMakeRequest.c" 
                            "SpotifyEventHandler.c" "SpotifyHttpLocalServer.c"
                    INCLUDE_DIRS "include" "lib"
                    EMBED_TXTFILES "certs/servercert.pem"
                                   "certs/prvtkey.pem"
                    REQUIRES mdns esp_wifi esp_event esp_eth protocol_examples_common
                    esp_https_server esp_http_client nvs_flash HttpsComponent json
                    GlobalInit esp_psram heap) 
                    
idf_component_register(SRCS "JsonExtraction.c" "SpotifyAuthorization.c" "MakeSpotifyRequest.c" 
                    INCLUDE_DIRS "include" 
                    EMBED_TXTFILES "certs/servercert.pem"
                                   "certs/prvtkey.pem"
                    REQUIRES mdns esp_wifi esp_eth protocol_examples_common
                    esp_https_server esp_http_client nvs_flash HttpsComponent json
                    main) 
                    

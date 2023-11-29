# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_component_register(SRCS "HttpsRequests.c" "time_sync.c"
                    INCLUDE_DIRS "include"
                    EMBED_TXTFILES server_root_cert.pem local_server_cert.pem
                    
                    REQUIRES mdns esp_wifi esp_eth protocol_examples_common
                    esp_https_server esp_http_client nvs_flash  esp_timer GlobalInit 
                    nvsFlash) 

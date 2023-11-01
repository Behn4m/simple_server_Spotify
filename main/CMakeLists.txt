idf_component_register(SRCS "main.c" "GlobalInit.c"
                    INCLUDE_DIRS "include" 
                    # REQUIRES SpotifyAuthorization nvs_flash WiFiComponent HttpsCoponent_ nvsFlash 
                    )


idf_component_register(SRCS "GlobalInit.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_event
                    nvs_flash  esp_timer  nvsFlash)

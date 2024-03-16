idf_component_register(SRCS "Setup_GPIO.c"
                    INCLUDE_DIRS "include"
                    REQUIRES    driver esp_timer GlobalInit driver espressif__button)

idf_component_register(SRCS "main.c" "GlobalInit.c"
                    INCLUDE_DIRS "include" 
                    )

spiffs_create_partition_image(
                        storage 
                        ../Data
                        FLASH_IN_PROJECT)
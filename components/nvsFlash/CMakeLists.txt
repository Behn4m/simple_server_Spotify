
idf_component_register(
        SRCS "SpiffsManger.c" "nvsFlash.c"
        INCLUDE_DIRS "include"
        REQUIRES nvs_flash json spiffs GlobalInit
        ) 
        
        #PRIV_REQUIRES 
                         

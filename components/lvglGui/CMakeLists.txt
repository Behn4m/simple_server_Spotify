idf_component_register(
    SRCS    "lvglGui.c"            
    INCLUDE_DIRS "include"         
    REQUIRES lvgl lvgl_esp32_drivers lv_demos driver esp_timer GlobalInit) 

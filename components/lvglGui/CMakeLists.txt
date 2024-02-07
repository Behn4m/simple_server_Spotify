idf_component_register(
    SRCS    "lvglGui.c"            
    INCLUDE_DIRS "include"         
    REQUIRES lvgl lvgl_esp32_drivers  driver esp_timer GlobalInit) 

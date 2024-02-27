idf_component_register(
    SRCS    "lvglGui.c"            
    INCLUDE_DIRS "include"         
    REQUIRES lvgl__lvgl lvgl_esp32_drivers  driver esp_timer GlobalInit driver espressif__button) 

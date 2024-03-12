idf_component_register(
    SRCS    "lvglGui.c"      "screens/MatterScreen.c"    "screens/SpotifyScreen.c" "screens/MenuScreen.c" 
    INCLUDE_DIRS "include"    "data"     "lib" 
    REQUIRES lvgl__lvgl lvgl_esp32_drivers  driver esp_timer GlobalInit driver espressif__button) 

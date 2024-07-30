idf_component_register(
    SRCS
    
"srcs/lvglTimer.c"
"srcs/lvglGui.c"
"generated/widgets_init.c"
"generated/events_init.c"
"generated/setup_scr_screen.c"
"generated/gui_guider.c"
"generated/images/_matterIcon_alpha_184x166.c"
 INCLUDE_DIRS 

"generated"
"generated/guider_customer_fonts"
"include"
     REQUIRES lvgl__lvgl lvgl_esp32_drivers  driver esp_timer GlobalInit driver espressif__button) 

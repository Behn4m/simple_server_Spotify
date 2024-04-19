idf_component_register(
    SRCS
    
"generated/guider_fonts/lv_font_arial_20.c"
"generated/guider_fonts/lv_font_montserratMedium_12.c"
"generated/guider_fonts/lv_font_montserratMedium_16.c"
"generated/guider_fonts/lv_font_montserratMedium_18.c"
"generated/guider_fonts/lv_font_Adventpro_regular_16.c"
"generated/guider_fonts/lv_font_Alatsi_Regular_16.c"
"generated/guider_fonts/lv_font_arial_16.c"
"generated/images/_Spotify_Logo_RGB_White_alpha_120x35.c"
"generated/images/_song_cover_alpha_150x150.c"
"generated/images/_matter_logo_alpha_64x64.c"
"generated/events_init.c"
"generated/gui_guider.c"
"generated/setup_scr_Spotify_Page.c"
"srcs/lvglTimer.c"
"srcs/lvglGui.c"
 INCLUDE_DIRS 

"generated"
"generated/guider_customer_fonts"
"include"
     REQUIRES lvgl__lvgl lvgl_esp32_drivers  driver esp_timer GlobalInit driver espressif__button)
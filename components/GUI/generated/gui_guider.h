/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *Main_menu_page;
	bool Main_menu_page_del;
	lv_obj_t *Main_menu_page_Matter_Bottom;
	lv_obj_t *Main_menu_page_Matter_Bottom_label;
	lv_obj_t *Main_menu_page_Spotify_Bottom;
	lv_obj_t *Main_menu_page_Spotify_Bottom_label;
	lv_obj_t *Matter_connected_page;
	bool Matter_connected_page_del;
	lv_obj_t *Matter_connected_page_img_matterlogo;
	lv_obj_t *Matter_connected_page_img_lamp;
	lv_obj_t *Matter_connected_page_slider_1;
	lv_obj_t *Matter_connected_page_sw_1;
	lv_obj_t *Matter_connected_page_label_1;
	lv_obj_t *Matter_connected_page_label_2;
	lv_obj_t *Matter_disconnect_page;
	bool Matter_disconnect_page_del;
	lv_obj_t *Matter_disconnect_page_img_matterlogo;
	lv_obj_t *Matter_disconnect_page_img_lamp;
	lv_obj_t *Matter_disconnect_page_label_2;
	lv_obj_t *Spotify_Page;
	bool Spotify_Page_del;
	lv_obj_t *Spotify_Page_cont_Spotify;
	lv_obj_t *Spotify_Page_label_time;
	lv_obj_t *Spotify_Page_label_artist;
	lv_obj_t *Spotify_Page_label_song;
	lv_obj_t *Spotify_Page_label_album;
	lv_obj_t *Spotify_Page_img_song;
	lv_obj_t *Spotify_Page_img_artist;
	lv_obj_t *Spotify_Page_img_2;
	lv_obj_t *Spotify_Page_bar_progress;
	lv_obj_t *Spotify_Page_Album_name;
	lv_obj_t *Spotify_Page_Song_name;
	lv_obj_t *Spotify_Page_Artist_name;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_move_animation(void * var, int32_t duration, int32_t delay, int32_t x_end, int32_t y_end, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_scale_animation(void * var, int32_t duration, int32_t delay, int32_t width, int32_t height, lv_anim_path_cb_t path_cb,
                        uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                        lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_img_zoom_animation(void * var, int32_t duration, int32_t delay, int32_t zoom, lv_anim_path_cb_t path_cb,
                           uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                           lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_img_rotate_animation(void * var, int32_t duration, int32_t delay, lv_coord_t x, lv_coord_t y, int32_t rotate,
                   lv_anim_path_cb_t path_cb, uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time,
                   uint32_t playback_delay, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);


extern lv_ui guider_ui;

void setup_scr_Main_menu_page(lv_ui *ui);
void setup_scr_Matter_connected_page(lv_ui *ui);
void setup_scr_Matter_disconnect_page(lv_ui *ui);
void setup_scr_Spotify_Page(lv_ui *ui);
LV_IMG_DECLARE(_matter_logo_alpha_167x37);
LV_IMG_DECLARE(_lamp_alpha_152x134);
LV_IMG_DECLARE(_matter_logo_alpha_155x35);
LV_IMG_DECLARE(_lamp_alpha_128x128);
LV_IMG_DECLARE(_song_cover_alpha_192x192);
LV_IMG_DECLARE(_artists_alpha_64x64);
LV_IMG_DECLARE(_Spotify_Logo_RGB_White_alpha_120x35);

LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_arial_16)
LV_FONT_DECLARE(lv_font_arial_20)
LV_FONT_DECLARE(lv_font_Adventpro_regular_16)


#ifdef __cplusplus
}
#endif
#endif

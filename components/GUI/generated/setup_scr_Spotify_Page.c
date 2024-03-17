/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_Spotify_Page(lv_ui *ui)
{
	//Write codes Spotify_Page
	ui->Spotify_Page = lv_obj_create(NULL);
	lv_obj_set_size(ui->Spotify_Page, 536, 240);
	lv_obj_set_scrollbar_mode(ui->Spotify_Page, LV_SCROLLBAR_MODE_OFF);

	//Write style for Spotify_Page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Spotify_Page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_cont_Spotify
	ui->Spotify_Page_cont_Spotify = lv_obj_create(ui->Spotify_Page);
	lv_obj_set_pos(ui->Spotify_Page_cont_Spotify, 0, 0);
	lv_obj_set_size(ui->Spotify_Page_cont_Spotify, 536, 240);
	lv_obj_set_scrollbar_mode(ui->Spotify_Page_cont_Spotify, LV_SCROLLBAR_MODE_OFF);

	//Write style for Spotify_Page_cont_Spotify, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_cont_Spotify, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->Spotify_Page_cont_Spotify, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->Spotify_Page_cont_Spotify, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->Spotify_Page_cont_Spotify, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_cont_Spotify, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_cont_Spotify, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Spotify_Page_cont_Spotify, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Spotify_Page_cont_Spotify, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_cont_Spotify, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_cont_Spotify, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_cont_Spotify, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_cont_Spotify, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_cont_Spotify, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_label_time
	ui->Spotify_Page_label_time = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_label_time, "00:00");
	lv_label_set_long_mode(ui->Spotify_Page_label_time, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->Spotify_Page_label_time, 235, 185);
	lv_obj_set_size(ui->Spotify_Page_label_time, 73, 18);

	//Write style for Spotify_Page_label_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_label_time, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_label_time, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_label_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_label_time, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_label_time, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_label_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_label_artist
	ui->Spotify_Page_label_artist = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_label_artist, "Artist:");
	lv_label_set_long_mode(ui->Spotify_Page_label_artist, LV_LABEL_LONG_CLIP);
	lv_obj_set_pos(ui->Spotify_Page_label_artist, 235, 155);
	lv_obj_set_size(ui->Spotify_Page_label_artist, 48, 15);

	//Write style for Spotify_Page_label_artist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_label_artist, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_label_artist, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_label_artist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_label_artist, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_label_artist, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_label_artist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_label_song
	ui->Spotify_Page_label_song = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_label_song, "Song:");
	lv_label_set_long_mode(ui->Spotify_Page_label_song, LV_LABEL_LONG_CLIP);
	lv_obj_set_pos(ui->Spotify_Page_label_song, 235, 125);
	lv_obj_set_size(ui->Spotify_Page_label_song, 49, 14);

	//Write style for Spotify_Page_label_song, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_label_song, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_label_song, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_label_song, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_label_song, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_label_song, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_label_album
	ui->Spotify_Page_label_album = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_label_album, "Album:");
	lv_label_set_long_mode(ui->Spotify_Page_label_album, LV_LABEL_LONG_CLIP);
	lv_obj_set_pos(ui->Spotify_Page_label_album, 235, 100);
	lv_obj_set_size(ui->Spotify_Page_label_album, 48, 19);

	//Write style for Spotify_Page_label_album, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_label_album, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_label_album, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_label_album, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_label_album, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_label_album, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_label_album, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_img_song
	ui->Spotify_Page_img_song = lv_img_create(ui->Spotify_Page_cont_Spotify);
	lv_obj_add_flag(ui->Spotify_Page_img_song, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Spotify_Page_img_song, &_song_cover_alpha_192x192);
	lv_img_set_pivot(ui->Spotify_Page_img_song, 50,50);
	lv_img_set_angle(ui->Spotify_Page_img_song, 0);
	lv_obj_set_pos(ui->Spotify_Page_img_song, 6, 22);
	lv_obj_set_size(ui->Spotify_Page_img_song, 192, 192);

	//Write style for Spotify_Page_img_song, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->Spotify_Page_img_song, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_recolor(ui->Spotify_Page_img_song, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->Spotify_Page_img_song, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_img_artist
	ui->Spotify_Page_img_artist = lv_img_create(ui->Spotify_Page_cont_Spotify);
	lv_obj_add_flag(ui->Spotify_Page_img_artist, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Spotify_Page_img_artist, &_artists_alpha_64x64);
	lv_img_set_pivot(ui->Spotify_Page_img_artist, 50,50);
	lv_img_set_angle(ui->Spotify_Page_img_artist, 0);
	lv_obj_set_pos(ui->Spotify_Page_img_artist, 442, 25);
	lv_obj_set_size(ui->Spotify_Page_img_artist, 64, 64);

	//Write style for Spotify_Page_img_artist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->Spotify_Page_img_artist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_recolor(ui->Spotify_Page_img_artist, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->Spotify_Page_img_artist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_img_2
	ui->Spotify_Page_img_2 = lv_img_create(ui->Spotify_Page_cont_Spotify);
	lv_obj_add_flag(ui->Spotify_Page_img_2, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Spotify_Page_img_2, &_Spotify_Logo_RGB_White_alpha_120x35);
	lv_img_set_pivot(ui->Spotify_Page_img_2, 50,50);
	lv_img_set_angle(ui->Spotify_Page_img_2, 0);
	lv_obj_set_pos(ui->Spotify_Page_img_2, 233, 35);
	lv_obj_set_size(ui->Spotify_Page_img_2, 120, 35);

	//Write style for Spotify_Page_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->Spotify_Page_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_recolor(ui->Spotify_Page_img_2, lv_color_hex(0x00ff39), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->Spotify_Page_img_2, 166, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_bar_progress
	ui->Spotify_Page_bar_progress = lv_bar_create(ui->Spotify_Page_cont_Spotify);
	lv_obj_set_style_anim_time(ui->Spotify_Page_bar_progress, 1000, 0);
	lv_bar_set_mode(ui->Spotify_Page_bar_progress, LV_BAR_MODE_NORMAL);
	lv_bar_set_range(ui->Spotify_Page_bar_progress, 0, 100);
	lv_bar_set_value(ui->Spotify_Page_bar_progress, 20, LV_ANIM_OFF);
	lv_obj_set_pos(ui->Spotify_Page_bar_progress, 235, 210);
	lv_obj_set_size(ui->Spotify_Page_bar_progress, 275, 6);

	//Write style for Spotify_Page_bar_progress, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Spotify_Page_bar_progress, 107, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Spotify_Page_bar_progress, lv_color_hex(0x393c41), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Spotify_Page_bar_progress, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_bar_progress, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_bar_progress, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for Spotify_Page_bar_progress, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Spotify_Page_bar_progress, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Spotify_Page_bar_progress, lv_color_hex(0xf34c94), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Spotify_Page_bar_progress, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_bar_progress, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_Album_name
	ui->Spotify_Page_Album_name = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_Album_name, "Album test");
	lv_label_set_long_mode(ui->Spotify_Page_Album_name, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->Spotify_Page_Album_name, 299, 100);
	lv_obj_set_size(ui->Spotify_Page_Album_name, 182, 18);

	//Write style for Spotify_Page_Album_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_Album_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_Album_name, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_Album_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_Album_name, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_Album_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_Album_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_Song_name
	ui->Spotify_Page_Song_name = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_Song_name, "song test");
	lv_label_set_long_mode(ui->Spotify_Page_Song_name, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->Spotify_Page_Song_name, 298, 125);
	lv_obj_set_size(ui->Spotify_Page_Song_name, 182, 18);

	//Write style for Spotify_Page_Song_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_Song_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_Song_name, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_Song_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_Song_name, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_Song_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_Song_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Spotify_Page_Artist_name
	ui->Spotify_Page_Artist_name = lv_label_create(ui->Spotify_Page_cont_Spotify);
	lv_label_set_text(ui->Spotify_Page_Artist_name, "artist test");
	lv_label_set_long_mode(ui->Spotify_Page_Artist_name, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->Spotify_Page_Artist_name, 297, 155);
	lv_obj_set_size(ui->Spotify_Page_Artist_name, 182, 18);

	//Write style for Spotify_Page_Artist_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Spotify_Page_Artist_name, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Spotify_Page_Artist_name, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Spotify_Page_Artist_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Spotify_Page_Artist_name, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Spotify_Page_Artist_name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Spotify_Page_Artist_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of Spotify_Page.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->Spotify_Page);

	//Init events for screen.
	events_init_Spotify_Page(ui);
}

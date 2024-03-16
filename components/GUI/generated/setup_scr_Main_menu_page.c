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



void setup_scr_Main_menu_page(lv_ui *ui)
{
	//Write codes Main_menu_page
	ui->Main_menu_page = lv_obj_create(NULL);
	lv_obj_set_size(ui->Main_menu_page, 536, 240);
	lv_obj_set_scrollbar_mode(ui->Main_menu_page, LV_SCROLLBAR_MODE_OFF);

	//Write style for Main_menu_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Main_menu_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Main_menu_page, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Main_menu_page, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Main_menu_page_Matter_Bottom
	ui->Main_menu_page_Matter_Bottom = lv_btn_create(ui->Main_menu_page);
	ui->Main_menu_page_Matter_Bottom_label = lv_label_create(ui->Main_menu_page_Matter_Bottom);
	lv_label_set_text(ui->Main_menu_page_Matter_Bottom_label, "1.Matter");
	lv_label_set_long_mode(ui->Main_menu_page_Matter_Bottom_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->Main_menu_page_Matter_Bottom_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->Main_menu_page_Matter_Bottom, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->Main_menu_page_Matter_Bottom, 58, 59);
	lv_obj_set_size(ui->Main_menu_page_Matter_Bottom, 213, 53);

	//Write style for Main_menu_page_Matter_Bottom, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Main_menu_page_Matter_Bottom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Main_menu_page_Matter_Bottom, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Main_menu_page_Matter_Bottom, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->Main_menu_page_Matter_Bottom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Main_menu_page_Matter_Bottom, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Main_menu_page_Matter_Bottom, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(ui->Main_menu_page_Matter_Bottom, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_opa(ui->Main_menu_page_Matter_Bottom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_spread(ui->Main_menu_page_Matter_Bottom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_x(ui->Main_menu_page_Matter_Bottom, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_y(ui->Main_menu_page_Matter_Bottom, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Main_menu_page_Matter_Bottom, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Main_menu_page_Matter_Bottom, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Main_menu_page_Matter_Bottom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Main_menu_page_Matter_Bottom, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Main_menu_page_Spotify_Bottom
	ui->Main_menu_page_Spotify_Bottom = lv_btn_create(ui->Main_menu_page);
	ui->Main_menu_page_Spotify_Bottom_label = lv_label_create(ui->Main_menu_page_Spotify_Bottom);
	lv_label_set_text(ui->Main_menu_page_Spotify_Bottom_label, "2.Spotify");
	lv_label_set_long_mode(ui->Main_menu_page_Spotify_Bottom_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->Main_menu_page_Spotify_Bottom_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->Main_menu_page_Spotify_Bottom, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->Main_menu_page_Spotify_Bottom, 266, 146);
	lv_obj_set_size(ui->Main_menu_page_Spotify_Bottom, 213, 53);

	//Write style for Main_menu_page_Spotify_Bottom, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Main_menu_page_Spotify_Bottom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Main_menu_page_Spotify_Bottom, lv_color_hex(0x009ea9), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Main_menu_page_Spotify_Bottom, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->Main_menu_page_Spotify_Bottom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Main_menu_page_Spotify_Bottom, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Main_menu_page_Spotify_Bottom, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(ui->Main_menu_page_Spotify_Bottom, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_opa(ui->Main_menu_page_Spotify_Bottom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_spread(ui->Main_menu_page_Spotify_Bottom, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_x(ui->Main_menu_page_Spotify_Bottom, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_y(ui->Main_menu_page_Spotify_Bottom, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Main_menu_page_Spotify_Bottom, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Main_menu_page_Spotify_Bottom, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Main_menu_page_Spotify_Bottom, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Main_menu_page_Spotify_Bottom, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of Main_menu_page.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->Main_menu_page);

	//Init events for screen.
	events_init_Main_menu_page(ui);
}

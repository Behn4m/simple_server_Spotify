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



void setup_scr_Matter_disconnect_page(lv_ui *ui)
{
	//Write codes Matter_disconnect_page
	ui->Matter_disconnect_page = lv_obj_create(NULL);
	lv_obj_set_size(ui->Matter_disconnect_page, 536, 240);
	lv_obj_set_scrollbar_mode(ui->Matter_disconnect_page, LV_SCROLLBAR_MODE_OFF);

	//Write style for Matter_disconnect_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_disconnect_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_disconnect_page, lv_color_hex(0xc1c1c1), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_disconnect_page, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_disconnect_page_img_matterlogo
	ui->Matter_disconnect_page_img_matterlogo = lv_img_create(ui->Matter_disconnect_page);
	lv_obj_add_flag(ui->Matter_disconnect_page_img_matterlogo, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Matter_disconnect_page_img_matterlogo, &_matter_logo_alpha_155x35);
	lv_img_set_pivot(ui->Matter_disconnect_page_img_matterlogo, 50,50);
	lv_img_set_angle(ui->Matter_disconnect_page_img_matterlogo, 0);
	lv_obj_set_pos(ui->Matter_disconnect_page_img_matterlogo, 14, 14);
	lv_obj_set_size(ui->Matter_disconnect_page_img_matterlogo, 155, 35);

	//Write style for Matter_disconnect_page_img_matterlogo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->Matter_disconnect_page_img_matterlogo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_disconnect_page_img_lamp
	ui->Matter_disconnect_page_img_lamp = lv_img_create(ui->Matter_disconnect_page);
	lv_obj_add_flag(ui->Matter_disconnect_page_img_lamp, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Matter_disconnect_page_img_lamp, &_lamp_alpha_128x128);
	lv_img_set_pivot(ui->Matter_disconnect_page_img_lamp, 50,50);
	lv_img_set_angle(ui->Matter_disconnect_page_img_lamp, 0);
	lv_obj_set_pos(ui->Matter_disconnect_page_img_lamp, 392, 96);
	lv_obj_set_size(ui->Matter_disconnect_page_img_lamp, 128, 128);

	//Write style for Matter_disconnect_page_img_lamp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->Matter_disconnect_page_img_lamp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_disconnect_page_label_2
	ui->Matter_disconnect_page_label_2 = lv_label_create(ui->Matter_disconnect_page);
	lv_label_set_text(ui->Matter_disconnect_page_label_2, "Is not connect !");
	lv_label_set_long_mode(ui->Matter_disconnect_page_label_2, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_pos(ui->Matter_disconnect_page_label_2, 42, 117);
	lv_obj_set_size(ui->Matter_disconnect_page_label_2, 242, 43);

	//Write style for Matter_disconnect_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Matter_disconnect_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Matter_disconnect_page_label_2, &lv_font_arial_20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Matter_disconnect_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Matter_disconnect_page_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Matter_disconnect_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Matter_disconnect_page_label_2, 240, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_disconnect_page_label_2, lv_color_hex(0xf10c0c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_disconnect_page_label_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Matter_disconnect_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of Matter_disconnect_page.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->Matter_disconnect_page);

	//Init events for screen.
	events_init_Matter_disconnect_page(ui);
}

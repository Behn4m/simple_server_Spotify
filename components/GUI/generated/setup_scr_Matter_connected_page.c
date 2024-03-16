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



void setup_scr_Matter_connected_page(lv_ui *ui)
{
	//Write codes Matter_connected_page
	ui->Matter_connected_page = lv_obj_create(NULL);
	lv_obj_set_size(ui->Matter_connected_page, 536, 240);
	lv_obj_set_scrollbar_mode(ui->Matter_connected_page, LV_SCROLLBAR_MODE_OFF);

	//Write style for Matter_connected_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_connected_page, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_connected_page_img_matterlogo
	ui->Matter_connected_page_img_matterlogo = lv_img_create(ui->Matter_connected_page);
	lv_obj_add_flag(ui->Matter_connected_page_img_matterlogo, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Matter_connected_page_img_matterlogo, &_matter_logo_alpha_167x37);
	lv_img_set_pivot(ui->Matter_connected_page_img_matterlogo, 50,50);
	lv_img_set_angle(ui->Matter_connected_page_img_matterlogo, 0);
	lv_obj_set_pos(ui->Matter_connected_page_img_matterlogo, 22, 28);
	lv_obj_set_size(ui->Matter_connected_page_img_matterlogo, 167, 37);

	//Write style for Matter_connected_page_img_matterlogo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->Matter_connected_page_img_matterlogo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_connected_page_img_lamp
	ui->Matter_connected_page_img_lamp = lv_img_create(ui->Matter_connected_page);
	lv_obj_add_flag(ui->Matter_connected_page_img_lamp, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->Matter_connected_page_img_lamp, &_lamp_alpha_152x134);
	lv_img_set_pivot(ui->Matter_connected_page_img_lamp, 50,50);
	lv_img_set_angle(ui->Matter_connected_page_img_lamp, 0);
	lv_obj_set_pos(ui->Matter_connected_page_img_lamp, 378, 89);
	lv_obj_set_size(ui->Matter_connected_page_img_lamp, 152, 134);

	//Write style for Matter_connected_page_img_lamp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->Matter_connected_page_img_lamp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_connected_page_slider_1
	ui->Matter_connected_page_slider_1 = lv_slider_create(ui->Matter_connected_page);
	lv_slider_set_range(ui->Matter_connected_page_slider_1, 0, 100);
	lv_slider_set_mode(ui->Matter_connected_page_slider_1, LV_SLIDER_MODE_NORMAL);
	lv_slider_set_value(ui->Matter_connected_page_slider_1, 20, LV_ANIM_OFF);
	lv_obj_set_pos(ui->Matter_connected_page_slider_1, 271, 186);
	lv_obj_set_size(ui->Matter_connected_page_slider_1, 128, 16);

	//Write style for Matter_connected_page_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_slider_1, 130, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_connected_page_slider_1, lv_color_hex(0x393c41), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_slider_1, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(ui->Matter_connected_page_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Matter_connected_page_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for Matter_connected_page_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_connected_page_slider_1, lv_color_hex(0xfffe00), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_slider_1, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for Matter_connected_page_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_connected_page_slider_1, lv_color_hex(0xfffe00), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_slider_1, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes Matter_connected_page_sw_1
	ui->Matter_connected_page_sw_1 = lv_switch_create(ui->Matter_connected_page);
	lv_obj_set_pos(ui->Matter_connected_page_sw_1, 273, 144);
	lv_obj_set_size(ui->Matter_connected_page_sw_1, 48, 24);

	//Write style for Matter_connected_page_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_sw_1, 81, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_connected_page_sw_1, lv_color_hex(0x41485a), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page_sw_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->Matter_connected_page_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_sw_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Matter_connected_page_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for Matter_connected_page_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_sw_1, 133, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->Matter_connected_page_sw_1, lv_color_hex(0xfffe00), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page_sw_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->Matter_connected_page_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for Matter_connected_page_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_sw_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Matter_connected_page_sw_1, lv_color_hex(0xfbfa45), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->Matter_connected_page_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->Matter_connected_page_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_sw_1, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes Matter_connected_page_label_1
	ui->Matter_connected_page_label_1 = lv_label_create(ui->Matter_connected_page);
	lv_label_set_text(ui->Matter_connected_page_label_1, "80%");
	lv_label_set_long_mode(ui->Matter_connected_page_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->Matter_connected_page_label_1, 349, 152);
	lv_obj_set_size(ui->Matter_connected_page_label_1, 39, 16);

	//Write style for Matter_connected_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Matter_connected_page_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Matter_connected_page_label_1, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Matter_connected_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Matter_connected_page_label_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Matter_connected_page_label_1, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Matter_connected_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Matter_connected_page_label_2
	ui->Matter_connected_page_label_2 = lv_label_create(ui->Matter_connected_page);
	lv_label_set_text(ui->Matter_connected_page_label_2, "Connected");
	lv_label_set_long_mode(ui->Matter_connected_page_label_2, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_pos(ui->Matter_connected_page_label_2, 40, 117);
	lv_obj_set_size(ui->Matter_connected_page_label_2, 148, 32);

	//Write style for Matter_connected_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->Matter_connected_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->Matter_connected_page_label_2, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->Matter_connected_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->Matter_connected_page_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->Matter_connected_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Matter_connected_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of Matter_connected_page.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->Matter_connected_page);

	//Init events for screen.
	events_init_Matter_connected_page(ui);
}

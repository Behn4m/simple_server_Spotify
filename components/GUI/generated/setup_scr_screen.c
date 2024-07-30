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
#include"lvglTimer.h"
//#include "custom.h"



void setup_scr_screen(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen, 536, 240);
	lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen, 136, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x2200ff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_img_1
	ui->screen_img_1 = lv_img_create(ui->screen);
	lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_1, &_matterIcon_alpha_184x166);
	lv_img_set_pivot(ui->screen_img_1, 50,50);
	lv_img_set_angle(ui->screen_img_1, 0);
	lv_obj_set_pos(ui->screen_img_1, 176, 37);
	lv_obj_set_size(ui->screen_img_1, 184, 166);

	//Write style for screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of screen.
		lv_obj_update_layout(ui->screen);

	/*overlay*/
	lv_ui *overlay = lv_obj_create(NULL);
	lv_obj_set_size(overlay, 536, 240);
	static lv_style_t style_overlay;
    lv_style_init(&style_overlay);

    // Set the background color and opacity for the overlay
    lv_style_set_bg_color(&style_overlay, lv_color_black());
    lv_style_set_bg_opa(&style_overlay, LV_OPA_50); // 50% opacity

    // Apply the style to the overlay
    lv_obj_add_style(overlay, &style_overlay, 0);
	lv_obj_update_layout(overlay);
	/*end of overlay*/
// while (1) {
//         lv_task_handler();
//         vTaskDelay(5000); // Sleep for 5 ms
// 	}
// 	//Update current screen layout.
// 	lv_obj_update_layout(ui->screen);

}

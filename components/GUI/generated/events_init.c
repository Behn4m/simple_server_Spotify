/*
 * Copyright 2024 NXP
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include <esp_log.h>
#if LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

static void Spotify_Page_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code)
	{
	default:
		break;
	}
}
static void Spotify_Page_label_time_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *object = lv_event_get_current_target(e);
	char *inputText = (char *)lv_event_get_param(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(object, &lv_font_arial_16, 0);
		lv_label_set_text(object, inputText);
		lv_refr_now(NULL);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_bar_progress_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *object = lv_event_get_current_target(e);
	int inputNum = (int)lv_event_get_param(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_bar_set_value(object, inputNum, LV_ANIM_OFF);
		lv_refr_now(NULL);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_Album_name_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *object = lv_event_get_current_target(e);
	char *inputText = (char *)lv_event_get_param(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(object, &lv_font_arial_16, 0);
		lv_label_set_text(object, inputText);
		lv_refr_now(NULL);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_Song_name_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *object = lv_event_get_current_target(e);
	char *inputText = (char *)lv_event_get_param(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(object, &lv_font_arial_16, 0);
		lv_label_set_text(object, inputText);
		lv_refr_now(NULL);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_Artist_name_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *object = lv_event_get_current_target(e);
	char *inputText = (char *)lv_event_get_param(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(object, &lv_font_arial_16, 0);
		lv_label_set_text(object, inputText);
		lv_refr_now(NULL);
		break;
	}
	default:
		break;
	}
}
void events_init_Spotify_Page(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->Spotify_Page, Spotify_Page_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_label_time, Spotify_Page_label_time_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_bar_progress, Spotify_Page_bar_progress_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_Album_name, Spotify_Page_Album_name_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_Song_name, Spotify_Page_Song_name_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_Artist_name, Spotify_Page_Artist_name_event_handler, LV_EVENT_ALL, NULL);
}

void events_init(lv_ui *ui)
{
}

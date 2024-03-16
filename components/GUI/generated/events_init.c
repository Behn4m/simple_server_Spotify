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

#if LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static void Main_menu_page_Matter_Bottom_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.Matter_connected_page, guider_ui.Matter_connected_page_del, &guider_ui.Main_menu_page_del, setup_scr_Matter_connected_page, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
	default:
		break;
	}
}
static void Main_menu_page_Spotify_Bottom_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.Spotify_Page, guider_ui.Spotify_Page_del, &guider_ui.Main_menu_page_del, setup_scr_Spotify_Page, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
	default:
		break;
	}
}
void events_init_Main_menu_page(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->Main_menu_page_Matter_Bottom, Main_menu_page_Matter_Bottom_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Main_menu_page_Spotify_Bottom, Main_menu_page_Spotify_Bottom_event_handler, LV_EVENT_ALL, NULL);
}
static void Matter_connected_page_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.Main_menu_page, guider_ui.Main_menu_page_del, &guider_ui.Matter_connected_page_del, setup_scr_Main_menu_page, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
	default:
		break;
	}
}
static void Matter_connected_page_sw_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	default:
		break;
	}
}
static void Matter_connected_page_label_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(guider_ui.Matter_connected_page_label_1, &lv_font_arial_16, 0);
		lv_label_set_text(guider_ui.Matter_connected_page_label_1, "default");
		break;
	}
	default:
		break;
	}
}
void events_init_Matter_connected_page(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->Matter_connected_page, Matter_connected_page_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Matter_connected_page_sw_1, Matter_connected_page_sw_1_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Matter_connected_page_label_1, Matter_connected_page_label_1_event_handler, LV_EVENT_ALL, NULL);
}
static void Matter_disconnect_page_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.Main_menu_page, guider_ui.Main_menu_page_del, &guider_ui.Matter_disconnect_page_del, setup_scr_Main_menu_page, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
	default:
		break;
	}
}
void events_init_Matter_disconnect_page(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->Matter_disconnect_page, Matter_disconnect_page_event_handler, LV_EVENT_ALL, NULL);
}
static void Spotify_Page_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.Main_menu_page, guider_ui.Main_menu_page_del, &guider_ui.Spotify_Page_del, setup_scr_Main_menu_page, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_cont_Spotify_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.Main_menu_page, guider_ui.Main_menu_page_del, &guider_ui.Spotify_Page_del, setup_scr_Main_menu_page, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_label_time_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(guider_ui.Spotify_Page_label_time, &lv_font_montserratMedium_12, 0);
		lv_label_set_text(guider_ui.Spotify_Page_label_time, "SAlam");
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_bar_progress_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_add_state(guider_ui.Spotify_Page_bar_progress, LV_STATE_EDITED);
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_Album_name_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(guider_ui.Spotify_Page_Album_name, &lv_font_arial_16, 0);
		lv_label_set_text(guider_ui.Spotify_Page_Album_name, "default");
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_Song_name_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(guider_ui.Spotify_Page_Song_name, &lv_font_arial_16, 0);
		lv_label_set_text(guider_ui.Spotify_Page_Song_name, "default");
		break;
	}
	default:
		break;
	}
}
static void Spotify_Page_Artist_name_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		lv_obj_set_style_text_font(guider_ui.Spotify_Page_label_artist, &lv_font_arial_16, 0);
		lv_label_set_text(guider_ui.Spotify_Page_label_artist, "default");
		break;
	}
	default:
		break;
	}
}
void events_init_Spotify_Page(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->Spotify_Page, Spotify_Page_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_cont_Spotify, Spotify_Page_cont_Spotify_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_label_time, Spotify_Page_label_time_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_bar_progress, Spotify_Page_bar_progress_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_Album_name, Spotify_Page_Album_name_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_Song_name, Spotify_Page_Song_name_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->Spotify_Page_Artist_name, Spotify_Page_Artist_name_event_handler, LV_EVENT_ALL, NULL);
}

void events_init(lv_ui *ui)
{

}

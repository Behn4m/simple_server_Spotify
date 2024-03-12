#include "Typedef.h"



// void MainMenu(void)
// {
//     MenuPage = lv_obj_create(lv_scr_act());
//     lv_obj_remove_style_all(MenuPage);
//     lv_obj_set_size(MenuPage, LV_HOR_RES, LV_VER_RES);
//     lv_obj_set_align(MenuPage, LV_ALIGN_DEFAULT);
//     lv_obj_clear_flag(MenuPage, LV_OBJ_FLAG_SCROLLABLE); /// Flags
//     lv_obj_set_style_bg_color(MenuPage, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_bg_opa(MenuPage, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
//     /*A base style*/
//     static lv_style_t parentStyle;
//     lv_style_init(&parentStyle);
//     lv_style_set_border_width(&parentStyle, 2);
//     lv_style_set_radius(&parentStyle, 10);
//     lv_style_set_shadow_width(&parentStyle, 10);
//     lv_style_set_shadow_ofs_y(&parentStyle, 5);
//     lv_style_set_shadow_opa(&parentStyle, LV_OPA_50);
//     lv_style_set_text_color(&parentStyle, lv_color_white());
//     lv_style_set_text_font(&parentStyle, &lv_font_montserrat_18); // Set the font
//     lv_style_set_width(&parentStyle, LV_HOR_RES * 0.4);
//     lv_style_set_height(&parentStyle, LV_VER_RES / 3);

//     /*Set only the properties that should be different*/
//     static lv_style_t matterStyle;
//     lv_style_init(&matterStyle);
//     lv_style_set_bg_color(&matterStyle, lv_palette_main(LV_PALETTE_RED));
//     lv_style_set_border_color(&matterStyle, lv_palette_darken(LV_PALETTE_RED, 3));
//     /*Create an object with the base style only*/
//     matterObject = lv_btn_create(MenuPage);
//     // lv_obj_t *matterObject = lv_btn_create(MenuPage);
//     lv_obj_add_style(matterObject, &parentStyle, 0);
//     lv_obj_add_style(matterObject, &matterStyle, 0);
//     lv_obj_align(matterObject, LV_ALIGN_LEFT_MID, 20, -35);

//     lv_obj_t *matterLabel = lv_label_create(matterObject);
//     lv_label_set_text(matterLabel, "1.Matter");
//     lv_obj_center(matterLabel);
//     lv_obj_add_event_cb(matterObject, ui_event_Button2, LV_EVENT_ALL, NULL);
//     /*Set only the properties that should be different*/
//     static lv_style_t spotifyStyle;
//     lv_style_init(&spotifyStyle);
//     lv_style_set_bg_color(&spotifyStyle, lv_palette_main(LV_PALETTE_GREEN));
//     lv_style_set_border_color(&spotifyStyle, lv_palette_darken(LV_PALETTE_GREEN, 3));

//     /*Create another object with the base style and earnings style too*/
//     lv_obj_t *spotifyObject = lv_btn_create(MenuPage);
//     // lv_obj_t *spotifyObject = lv_btn_create(MenuPage);
//     lv_obj_add_style(spotifyObject, &parentStyle, 0);
//     lv_obj_add_style(spotifyObject, &spotifyStyle, 0);
//     lv_obj_align(spotifyObject, LV_ALIGN_RIGHT_MID, -20, 35);

//     lv_obj_t *spotifyLabel = lv_label_create(spotifyObject);
//     lv_label_set_text(spotifyLabel, "2.Spotify");
//     lv_obj_center(spotifyLabel);
// }

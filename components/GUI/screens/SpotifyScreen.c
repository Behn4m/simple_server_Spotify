#include "SpotifyScreen.h"
#include "image_test.h"
#include "GUIEvent.h"
static const char *TAG = "GUI_SpotifyScreen";


void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}
void RailBar(lv_obj_t *BarObject)
{
    static lv_style_t style_bg;
    static lv_style_t style_indic;

    lv_style_init(&style_bg);
    lv_style_set_border_color(&style_bg, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_border_width(&style_bg, 2);
    lv_style_set_pad_all(&style_bg, 6); /*To make the indicator smaller*/
    lv_style_set_radius(&style_bg, 6);
    lv_style_set_anim_time(&style_bg, 1000);

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_radius(&style_indic, 3);

    BarObject = lv_bar_create(lv_scr_act());
    lv_obj_remove_style_all(BarObject); /*To have a clean start*/
    lv_obj_add_style(BarObject, &style_bg, 0);
    lv_obj_add_style(BarObject, &style_indic, LV_PART_INDICATOR);

    lv_obj_set_size(BarObject, 200, 20);
    lv_obj_center(BarObject);
    lv_obj_add_event_cb(BarObject, bar_event_cb, LV_EVENT_ALL, NULL);
}

/**
 * @brief Function to create the main LVGL user interface
 */
void SpotifyPageFunc(lv_obj_t *SpotifyPage)
{
    /*Create a SpotifyPage object*/
    SpotifyPage = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(SpotifyPage);
    lv_obj_set_size(SpotifyPage, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_align(SpotifyPage, LV_ALIGN_DEFAULT);
    lv_obj_clear_flag(SpotifyPage, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    
    // Base style for MusicBox
    lv_style_t MusicBox;
    lv_style_init(&MusicBox);
    lv_color_t musicBoxColor;
    musicBoxColor.full = 0x39e9;
    lv_style_set_bg_color(&MusicBox, musicBoxColor);

    // Create an object for the music display with the MusicBox style
    lv_obj_t *musicObject;
    musicObject = lv_obj_create(SpotifyPage);
    lv_obj_add_style(musicObject, &MusicBox, 0);
    lv_obj_align(musicObject, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(musicObject, LV_HOR_RES, LV_VER_RES / 2); // Set size to cover entire horizontal, half vertical

    // Base style for TitleBox
    lv_style_t TitleBox;
    lv_color_t titleBoxColor = lv_color_make(0x0, 0x0, 0x00);
    lv_style_init(&TitleBox);
    lv_style_set_bg_color(&TitleBox, titleBoxColor);

    /*Create another object with the base style and earnings style too*/
    lv_obj_t *textObject = lv_obj_create(SpotifyPage);
    lv_obj_add_style(textObject, &TitleBox, 0);
    lv_obj_align(textObject, LV_ALIGN_BOTTOM_MID, 0, -100);  //  shift it in Y axis
    lv_obj_set_size(textObject, LV_HOR_RES, LV_VER_RES / 4); // Set size to cover entire horizontal, 0.25 vertical

    /*Circular scroll style*/
    static lv_style_t circular;
    lv_style_init(&circular);
    lv_style_set_bg_color(&circular, lv_color_make(0xff, 0x0, 0x00));
    lv_style_set_text_color(&circular, lv_color_white());
    lv_style_set_text_font(&circular, &lv_font_montserrat_18); // Set the font

    // Create a circular scrolling text object
    lv_obj_t *circularObject = lv_label_create(SpotifyPage);
    lv_obj_add_style(circularObject, &circular, 0);
    lv_label_set_long_mode(circularObject, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(circularObject, 150);
    lv_label_set_text(circularObject, "It is a circularly scrolling text. ");
    lv_obj_align(circularObject, LV_ALIGN_CENTER, 0, 0);

    /* Create an image object for a music bottom picture*/
    lv_obj_t *imageObject = lv_img_create(SpotifyPage);
    lv_img_set_src(imageObject, &music);
    lv_obj_align(imageObject, LV_ALIGN_BOTTOM_MID, 0, -10);
}

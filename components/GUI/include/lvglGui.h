/*
 * a component for create indipendent test for gui
 */
#ifndef LVGL_GUI_H_
#define LVGL_GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include"LVGLInterface.h"
#include"lvglTimer.h"
// #include "styles/lv_example_style.h"
// #include "get_started/lv_example_get_started.h"
// #include "widgets/lv_example_widgets.h"
// #include "layouts/lv_example_layout.h"
// #include "scroll/lv_example_scroll.h"
// #include "anim/lv_example_anim.h"
// #include "event/lv_example_event.h"
// #include "styles/lv_example_style.h"
// #include "others/lv_example_others.h"
// #include "libs/lv_example_libs.h"
// #include "lvgl__lvgl/src/font/lv_font.h"
// #include "lvgl__lvgl/examples/scroll/lv_example_scroll.h"
// #include "lvgl__lvgl/src/core/lv_obj.h"


/**
 * @brief      create guiTask and its global method
 */
void GUI_TaskInit(void);

/**
 * @brief Function to update the LVGL screen
 * @param Artist: Artist name
 * @param Title: Title of the song
 * @param Album: Album name
 * @param ProgressMS: Progress of the song in milliseconds
 * @return void
 */
void GUI_UpdateSpotifyScreen(char *Artist, char *Song, char *Album, int DurationMS, int ProgressMS);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

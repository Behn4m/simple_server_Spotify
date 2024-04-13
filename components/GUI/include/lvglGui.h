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
void GUI_UpdateSpotifyScreen(bool songUpdated, char *Artist, char *Song, char *Album, int DurationMS, int ProgressMS, uint8_t *coverPhoto);
#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

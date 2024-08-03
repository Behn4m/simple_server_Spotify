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

void send_event_test();

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

#ifndef LVGL_EVENT_H_
#define LVGL_EVENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "GUITypedef.h"
#include"lvglGui.h"


void ui_event_Button2(lv_event_t *e);
void bar_event_cb(lv_event_t *e);
void button_event_cb(void *arg, void *data);

#ifdef __cplusplus
}
#endif

#endif

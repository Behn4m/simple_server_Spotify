#include "Typedef.h"


 void bar_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    int32_t percentOfFillingBar = 0;
    percentOfFillingBar = *((int32_t *)e->param);
    if (percentOfFillingBar <= 100)
    {
        lv_bar_set_value(cont, i, LV_ANIM_OFF);
    }
}

void ui_event_Button2(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        _ui_screen_change(&target, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &SpotifyPageFunc);
    }
    lv_obj_del(MenuPage);
    SpotifyPageFunc();
    lv_disp_load_scr(SpotifyPage);
}
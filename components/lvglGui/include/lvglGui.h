/*
 * SEZ@Done(ehsan.ziyaee@gmail.com) 
 * init 2023.10.14
 *
 * a component for create indipendent tast for gui
 */
#ifndef LVGL_GUI_H_
#define LVGL_GUI_H_

#ifdef __cplusplus
extern "C" {
#endif


/* #ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    #if defined CONFIG_LV_USE_DEMO_WIDGETS
        #include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
    #elif defined CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #include "lv_examples/src/lv_demo_keypad_and_encoder/lv_demo_keypad_and_encoder.h"
    #elif defined CONFIG_LV_USE_DEMO_BENCHMARK
        #include "lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
    #elif defined CONFIG_LV_USE_DEMO_STRESS
        #include "lv_examples/src/lv_demo_stress/lv_demo_stress.h"
    #else
        #error "No demo application selected."
    #endif
#endif */

/**
 * @brief      create guiTask and its global method
 * @param[in]  void   nothing
 * @param[out] void   nothing
 * @return nothing
 */

void lvglGui(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_GUI_H_ */

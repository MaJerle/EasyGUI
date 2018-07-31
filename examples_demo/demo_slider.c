#include "demo.h"

static uint8_t  slider_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_slider(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);
    
    h = gui_slider_create(0, 10, 10, 300, 30, parent, slider_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_slider_setmin(h, 0);
    gui_slider_setmax(h, 100);
    gui_slider_setvalue(h, 25);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
slider_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        default: break;
    }
    return ret;
}

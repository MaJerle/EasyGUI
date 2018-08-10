#include "demo.h"

static uint8_t  slider_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_slider(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);
    
    /* Create slider */
    h = gui_slider_create(0, 10, 10, 300, 30, parent, slider_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_slider_setmin(h, 0);
    gui_slider_setmax(h, 100);
    gui_slider_setvalue(h, 25);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create slider */
    h = gui_slider_create(0, 10, 50, 50, 300, parent, slider_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_slider_setmin(h, 0);
    gui_slider_setmax(h, 100);
    gui_slider_setvalue(h, 25);
    gui_slider_setmode(h, GUI_SLIDER_MODE_TOP_BOTTOM);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
slider_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

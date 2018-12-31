#include "demo.h"

static uint8_t  template_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* param, gui_evt_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_template(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    GUI_UNUSED(h);

    gui_protect(protect);
    
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
template_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* param, gui_evt_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

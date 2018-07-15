#include "demo.h"

static uint8_t  edit_text_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_edit_text(gui_handle_p parent) {
    gui_handle_p h;

    /* Create edit text */
    h = gui_edittext_create(0, 10, 10, 300, 50, parent, edit_text_callback, 0);
    gui_widget_alloctextmemory(h, 255);
}

/**
 * \brief           Event callback
 */
static uint8_t
edit_text_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    return gui_widget_processdefaultcallback(h, wc, param, result);
}

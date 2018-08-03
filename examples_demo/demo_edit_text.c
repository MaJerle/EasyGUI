#include "demo.h"

static uint8_t  edit_text_callback(gui_handle_p h, gui_we_t wc, gui_evt_param_t* param, gui_evt_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_edit_text(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);

    /* Create edit text */
    h = gui_edittext_create(0, 10, 10, 300, 50, parent, edit_text_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_alloctextmemory(h, 255);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
edit_text_callback(gui_handle_p h, gui_we_t wc, gui_evt_param_t* param, gui_evt_result_t* result) {
    return gui_widget_processdefaultcallback(h, wc, param, result);
}

#include "demo.h"

static uint8_t  edit_text_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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
    gui_widget_setpositionpercent(h, 1, 1);
    gui_widget_setsizepercent(h, 98, 6);
    gui_widget_alloctextmemory(h, 255);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create edit text */
    h = gui_edittext_create(0, 10, 10, 300, 50, parent, edit_text_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 1, 8);
    gui_widget_setsizepercent(h, 98, 91);
    gui_widget_alloctextmemory(h, 255);
    gui_edittext_setmultiline(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
edit_text_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    return gui_widget_processdefaultcallback(h, evt, param, result);
}

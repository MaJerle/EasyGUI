#include "demo.h"

static uint8_t  radio_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_radio(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);
    
    /* Create radio */
    h = gui_radio_create(0, 10, 10, 300, 40, parent, radio_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Radio box option 1\r\n"));
    gui_radio_setgroup(h, 1);
    gui_radio_setvalue(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create radio */
    h = gui_radio_create(0, 10, 60, 300, 40, parent, radio_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Radio box option 2\r\n"));
    gui_radio_setgroup(h, 1);
    gui_radio_setvalue(h, 2);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create radio */
    h = gui_radio_create(0, 10, 110, 300, 40, parent, radio_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Radio box option 3\r\n"));
    gui_radio_setgroup(h, 1);
    gui_radio_setvalue(h, 3);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
radio_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        default: break;
    }
    return ret;
}

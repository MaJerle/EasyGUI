#include "demo.h"

static uint8_t  button_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature window widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_button(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);
    
    h = gui_button_create(0, 10, 10, 100, 40, parent, button_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setposition(h, 0, 0);
    gui_widget_setsizepercent(h, 100, 10);
    gui_widget_settext(h, _GT("Button with 100% width"));
    gui_widget_setignoreinvalidate(h, 0, 1);

    h = gui_button_create(0, 10, 100, 100, 40, parent, button_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setsize(h, 300, 50);
    gui_widget_setposition(h, 10, 100);
    gui_button_set3dstyle(h, 1);
    gui_widget_settext(h, _GT("Default 3D button"));
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    h = gui_button_create(0, 10, 300, 100, 40, parent, button_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setsize(h, 300, 50);
    gui_widget_setposition(h, 10, 200);
    gui_button_set3dstyle(h, 1);
    gui_widget_settext(h, _GT("Default flat button"));
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    gui_unprotect(protect);
}

/**
 * \brief           Button event callback
 */
static uint8_t
button_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

#include "demo.h"

static uint8_t  button_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature window widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_button(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    h = gui_button_create(0, 10, 10, 100, 40, parent, button_callback, 0, protect);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setposition(h, 0, 0, protect);
    gui_widget_setsizepercent(h, 100, 10, protect);
    gui_widget_settext(h, _GT("Button with 100% width"), protect);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    h = gui_button_create(0, 10, 100, 100, 40, parent, button_callback, 0, protect);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setsize(h, 300, 50, protect);
    gui_widget_setposition(h, 10, 100, protect);
    gui_widget_set3dstyle(h, 1, protect);
    gui_widget_settext(h, _GT("Default 3D button"), protect);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);
    
    h = gui_button_create(0, 10, 300, 100, 40, parent, button_callback, 0, protect);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setsize(h, 300, 50, protect);
    gui_widget_setposition(h, 10, 200, protect);
    gui_widget_set3dstyle(h, 0, protect);
    gui_widget_settext(h, _GT("Default flat button"), protect);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);
}

/**
 * \brief           Window event callback
 */
static uint8_t
button_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        case GUI_WC_Click: {
            gui_dialog_create(0, 250, 100, 300, 200, gui_container_create, NULL, 0, 0);
            break;
        }
        default: break;
    }
    return ret;
}

#include "demo.h"

static uint8_t  button_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature window widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_button(gui_handle_p parent) {
    gui_handle_p h;

    h = gui_button_create(0, 10, 10, 100, 40, parent, button_callback, 0);
    gui_widget_setposition(h, 0, 0);
    gui_widget_setsizepercent(h, 100, 10);
    gui_widget_settext(h, _GT("Button with 100% width"));

    h = gui_button_create(0, 10, 100, 100, 40, parent, button_callback, 0);
    gui_widget_setsize(h, 300, 50);
    gui_widget_setposition(h, 10, 100);
    gui_widget_set3dstyle(h, 1);
    gui_widget_settext(h, _GT("Default 3D button"));
    
    h = gui_button_create(0, 10, 300, 100, 40, parent, button_callback, 0);
    gui_widget_setsize(h, 300, 50);
    gui_widget_setposition(h, 10, 200);
    gui_widget_set3dstyle(h, 0);
    gui_widget_settext(h, _GT("Default flat button"));
}

/**
 * \brief           Window event callback
 */
static uint8_t
button_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    return gui_widget_processdefaultcallback(h, wc, param, result);
}

#include "demo.h"

static uint8_t  progbar_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_progbar(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);

    /* First progbar */
    h = gui_progbar_create(0, 0, 0, 0, 0, parent, progbar_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setposition(h, 10, 10, 0);
    gui_widget_setsize(h, 300, 40, 0);
    gui_widget_settext(h, _GT("Progress bar"), 0);
    gui_progbar_setmin(h, 0, 0);
    gui_progbar_setmax(h, 100, 0);
    gui_progbar_setvalue(h, 56, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Second progbar with percent mode */
    h = gui_progbar_create(0, 0, 0, 0, 0, parent, progbar_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setsize(h, 300, 40, 0);
    gui_widget_setposition(h, 10, 60, 0);
    gui_progbar_setmin(h, 0, 0);
    gui_progbar_setmax(h, 100, 0);
    gui_progbar_setvalue(h, 0, 0);
    gui_progbar_setanimation(h, 1, 0);
    gui_progbar_setpercentmode(h, 1, 0);
    gui_progbar_setvalue(h, 89, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
progbar_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    return gui_widget_processdefaultcallback(h, wc, param, result);
}

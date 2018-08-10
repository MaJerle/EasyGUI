#include "demo.h"

static uint8_t  progbar_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_progbar(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    size_t i;

    gui_protect(protect);

    /* First progbar */
    h = gui_progbar_create(0, 0, 0, 0, 0, parent, progbar_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setsizepercent(h, 98, 8.5f);
    gui_widget_setpositionpercent(h, 1, 1);
    gui_progbar_setmin(h, 0);
    gui_progbar_setmax(h, 100);
    gui_progbar_setvalue(h, 56);
    gui_widget_settext(h, _GT("Progress bar"));
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Second progbar with percent mode */
    for (i = 0; i < 9; i++) {
        h = gui_progbar_create(0, 0, 0, 0, 0, parent, progbar_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
        gui_widget_setsizepercent(h, 98, 8.5f);
        gui_widget_setpositionpercent(h, 1, GUI_FLOAT(10 + 10 * i));

        gui_progbar_setmin(h, 0);
        gui_progbar_setmax(h, 100);
        gui_progbar_setvalue(h, 0);
        gui_progbar_setanimation(h, 1);
        gui_progbar_setpercentmode(h, 1);
        gui_progbar_setvalue(h, 89);
        gui_widget_setignoreinvalidate(h, 0, 1);
    }

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
progbar_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    return gui_widget_processdefaultcallback(h, evt, param, result);
}

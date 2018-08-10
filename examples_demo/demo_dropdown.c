#include "demo.h"

static uint8_t  dropdown_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

static const gui_char* const 
strings[] = {
    _GT("Item 1"),
    _GT("Item 2"),
    _GT("Item 3"),
    _GT("Item 4"),
    _GT("Item 5"),
    _GT("Item 6"),
};

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_dropdown(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    size_t i;

    gui_protect(protect);

    /* Create dropdown */
    h = gui_dropdown_create(0, 10, 10, 10, 10, parent, dropdown_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setxpositionpercent(h, 1);
    gui_widget_setyposition(h, 10);
    gui_widget_setsizepercent(h, 48, 8);
    for (i = 0; i < GUI_ARRAYSIZE(strings); i++) {
        gui_dropdown_addstring(h, strings[i]);
    }
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create dropdown */
    h = gui_dropdown_create(0, 10, 10, 10, 10, parent, dropdown_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setxpositionpercent(h, 51);
    gui_widget_setyposition(h, 300);
    gui_widget_setsizepercent(h, 48, 8);
    gui_dropdown_setopendirection(h, GUI_DROPDOWN_OPENDIR_UP);
    for (i = 0; i < GUI_ARRAYSIZE(strings); i++) {
        gui_dropdown_addstring(h, strings[i]);
    }
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
dropdown_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

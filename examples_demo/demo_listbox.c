#include "demo.h"

static uint8_t  listbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           List of strings
 */
static const gui_char* const
strings[] = {
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
    _GT("String 1"),
    _GT("String 2"),
    _GT("String 3"),
    _GT("String 4"),
};

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_listbox(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    size_t i;

    gui_protect(protect);
    
    /* Create listbox */
    h = gui_listbox_create(0, 0, 0, 200, 300, parent, listbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 10, 10);
    gui_widget_setsizepercent(h, 80, 80);
    
    /* Add strings to listbox */
    for (i = 0; i < GUI_ARRAYSIZE(strings); i++) {
        gui_listbox_addstring(h, strings[i]);
    }
    
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
listbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

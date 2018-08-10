#include "demo.h"

static uint8_t  listview_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

static const gui_char* const
strings[][3] = {
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
    { _GT("Meggan"),    _GT("Klann"),   _GT("megan@klann") },
    { _GT("Natasha"),   _GT("Grim"),    _GT("natasha@grim") },
    { _GT("John"),      _GT("Doe"),     _GT("john@doe.com") },
    { _GT("Jane"),      _GT("Doe"),     _GT("jane@doe.com") },
};

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_listview(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    gui_listview_row_p row;
    size_t i;

    gui_protect(protect);
    
    /* Create listview */
    h = gui_listview_create(0, 0, 0, 1, 1, parent, listview_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 2, 2);
    gui_widget_setsizepercent(h, 96, 96);
    
    /* Add columns */
    gui_listview_addcolumn(h, _GT("FName"), 120);
    gui_listview_addcolumn(h, _GT("LName"), 120);
    gui_listview_addcolumn(h, _GT("Email"), 300);
    
    /* Write rows */
    for (i = 0; i < GUI_ARRAYSIZE(strings); i++) {
        row = gui_listview_addrow(h);
        
        /* Apply item strings */
        gui_listview_setitemstring(h, row, 0, strings[i][0]);
        gui_listview_setitemstring(h, row, 1, strings[i][1]);
        gui_listview_setitemstring(h, row, 2, strings[i][2]);
    }

    gui_widget_setignoreinvalidate(h, 0, 1);
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
listview_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

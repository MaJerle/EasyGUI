#include "demo.h"

static uint8_t  checkbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_checkbox(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);
    
    /* Create checkbox */
    h = gui_checkbox_create(0, 10, 10, 300, 40, parent, checkbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Default checkbox"));
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_FG, GUI_COLOR_DARKRED);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTRED);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create checkbox */
    h = gui_checkbox_create(0, 10, 60, 300, 40, parent, checkbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Default checkbox checked"));
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_FG, GUI_COLOR_DARKGREEN);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTCYAN);
    gui_checkbox_setchecked(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create checkbox */
    h = gui_checkbox_create(0, 10, 110, 300, 40, parent, checkbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Default checkbox disabled"));
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_FG, GUI_COLOR_DARKBLUE);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTBLUE);
    gui_checkbox_setdisabled(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create checkbox */
    h = gui_checkbox_create(0, 10, 160, 300, 40, parent, checkbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Default checkbox checked disabled"));
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_FG, GUI_COLOR_DARKCYAN);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTCYAN);
    gui_checkbox_setchecked(h, 1);
    gui_checkbox_setdisabled(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
checkbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

#include "demo.h"

static uint8_t  debugbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_debugbox(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);
    
    /* Create debug box */
    h = gui_debugbox_create(0, 0, 0, 1, 1, parent, debugbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 1, 1);
    gui_widget_setsizepercent(h, 98, 98);
    gui_debugbox_setmaxitems(h, 100);
    gui_debugbox_addstring(h, _GT("Debug item 1"));
    gui_debugbox_addstring(h, _GT("Debug item 2"));
    gui_debugbox_addstring(h, _GT("Debug item 3"));
    gui_debugbox_addstring(h, _GT("Debug item 4"));
    gui_debugbox_addstring(h, _GT("Debug item 5"));
    gui_debugbox_addstring(h, _GT("Debug item 6"));
    gui_debugbox_addstring(h, _GT("Debug item 7"));
    gui_debugbox_addstring(h, _GT("Debug item 8"));
    gui_debugbox_addstring(h, _GT("Debug item 9"));
    gui_debugbox_addstring(h, _GT("Debug item 0"));
    gui_debugbox_addstring(h, _GT("Debug item 1"));
    gui_debugbox_addstring(h, _GT("Debug item 2"));
    gui_debugbox_addstring(h, _GT("Debug item 3"));
    gui_debugbox_addstring(h, _GT("Debug item 4"));
    gui_debugbox_addstring(h, _GT("Debug item 5"));
    gui_debugbox_addstring(h, _GT("Debug item 6"));
    gui_debugbox_addstring(h, _GT("Debug item 7"));
    gui_debugbox_addstring(h, _GT("Debug item 8"));
    gui_debugbox_addstring(h, _GT("Debug item 9"));
    gui_debugbox_addstring(h, _GT("Debug item 0"));
    gui_debugbox_addstring(h, _GT("Debug item 1"));
    gui_debugbox_addstring(h, _GT("Debug item 2"));
    gui_debugbox_addstring(h, _GT("Debug item 3"));
    gui_debugbox_addstring(h, _GT("Debug item 4"));
    gui_debugbox_addstring(h, _GT("Debug item 5"));
    gui_debugbox_addstring(h, _GT("Debug item 6"));
    gui_debugbox_addstring(h, _GT("Debug item 7"));
    gui_debugbox_addstring(h, _GT("Debug item 8"));
    gui_debugbox_addstring(h, _GT("Debug item 9"));
    gui_debugbox_addstring(h, _GT("Debug item 0"));
    gui_debugbox_addstring(h, _GT("Debug item 1"));
    gui_debugbox_addstring(h, _GT("Debug item 2"));
    gui_debugbox_addstring(h, _GT("Debug item 3"));
    gui_debugbox_addstring(h, _GT("Debug item 4"));
    gui_debugbox_addstring(h, _GT("Debug item 5"));
    gui_debugbox_addstring(h, _GT("Debug item 6"));
    gui_debugbox_addstring(h, _GT("Debug item 7"));
    gui_debugbox_addstring(h, _GT("Debug item 8"));
    gui_debugbox_addstring(h, _GT("Debug item 9"));
    gui_debugbox_addstring(h, _GT("Debug item 0"));
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
debugbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

/**
 * \file            gui_callbacks.c
 * \brief           GUI callback functions for widgets
 */

#include "app.h"

/**
 * \brief           Graph event function
 */
uint8_t
gui_graph_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t res;
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);

    return res;
}

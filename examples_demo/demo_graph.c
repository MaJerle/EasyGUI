#include "demo.h"

static uint8_t  graph_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);
static uint8_t  checkbox_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_graph(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    gui_graph_data_p data;
    size_t i;

    gui_protect(protect);

    /* Create graph */
    h = gui_graph_create(ID_GRAPH, 10, 10, 400, 300, parent, graph_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setpositionpercent(h, 1, 1, 0);
    gui_widget_setsizepercent(h, 98, 98, 0);
    gui_widget_setzindex(h, 0, 0);
    gui_graph_setaxes(h, 0, 200, -50, 200, 0);
    gui_graph_zoomreset(h, 0);

    /* Create graph data */
    data = gui_graph_data_create(0, GUI_GRAPH_TYPE_YT, 200, 0);
    gui_graph_data_setcolor(data, GUI_COLOR_GREEN, 0);

    for (i = 0; i < 25; i++) {
        gui_graph_data_addvalue(data, 0, 100, 0);
        gui_graph_data_addvalue(data, 0, 1, 0);
        gui_graph_data_addvalue(data, 0, 4, 0);
        gui_graph_data_addvalue(data, 0, 0, 0);
        gui_graph_data_addvalue(data, 0, 3, 0);
        gui_graph_data_addvalue(data, 0, 0, 0);
        gui_graph_data_addvalue(data, 0, 40, 0);
        gui_graph_data_addvalue(data, 0, 0, 0);
    }

    /* Attach with graph */
    gui_graph_attachdata(h, data, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create checkbox for maximize function */
    h = gui_checkbox_create(0, 10, 10, 300, 30, parent, checkbox_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_settext(h, _GT("Expand graph"), 0);
    gui_widget_setzindex(h, 1, 0);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTRED, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
graph_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    return gui_widget_processdefaultcallback(h, wc, param, result);
}

/**
 * \brief           Event callback
 */
static uint8_t
checkbox_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        case GUI_WC_Click: {
            gui_handle_p graph;

            graph = gui_widget_getbyid(ID_GRAPH, 0);
            if (graph != NULL) {
                gui_widget_setexpanded(graph, gui_checkbox_ischecked(h, 0), 0);
            }
        }
    }
    return ret;
}

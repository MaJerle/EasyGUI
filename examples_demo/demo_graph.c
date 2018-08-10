#include "demo.h"

static uint8_t  graph_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);
static uint8_t  checkbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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
    h = gui_graph_create(ID_GRAPH, 10, 10, 400, 300, parent, graph_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 1, 10);
    gui_widget_setsizepercent(h, 98, 98);
    gui_widget_setzindex(h, 0);
    gui_graph_setaxes(h, 0, 200, -50, 200);
    gui_graph_zoomreset(h);

    /* Create graph data */
    data = gui_graph_data_create(0, GUI_GRAPH_TYPE_YT, 200);
    gui_graph_data_setcolor(data, GUI_COLOR_GREEN);

    for (i = 0; i < 25; i++) {
        gui_graph_data_addvalue(data, 0, 100);
        gui_graph_data_addvalue(data, 0, 1);
        gui_graph_data_addvalue(data, 0, 4);
        gui_graph_data_addvalue(data, 0, 0);
        gui_graph_data_addvalue(data, 0, 3);
        gui_graph_data_addvalue(data, 0, 0);
        gui_graph_data_addvalue(data, 0, 40);
        gui_graph_data_addvalue(data, 0, 0);
    }

    /* Attach with graph */
    gui_graph_attachdata(h, data);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create checkbox for maximize function */
    h = gui_checkbox_create(0, 10, 10, 300, 30, parent, checkbox_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_settext(h, _GT("Expand graph"));
    gui_widget_setzindex(h, 1);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTRED);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
graph_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    return gui_widget_processdefaultcallback(h, evt, param, result);
}

/**
 * \brief           Event callback
 */
static uint8_t
checkbox_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        case GUI_EVT_CLICK: {
            gui_handle_p graph;

            graph = gui_widget_getbyid(ID_GRAPH);
            if (graph != NULL) {
                gui_widget_setexpanded(graph, gui_checkbox_ischecked(h));
            }
        }
        default: break;
    }
    return ret;
}

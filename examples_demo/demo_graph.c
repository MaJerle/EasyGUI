#include "demo.h"

static uint8_t  graph_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);
static uint8_t  checkbox_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_graph(gui_handle_p parent) {
    gui_handle_p h;
    gui_graph_data_p data;
    size_t i;

    /* Create graph */
    h = gui_graph_create(ID_GRAPH, 10, 10, 400, 300, parent, graph_callback, 0);
    gui_widget_setpositionpercent(h, 1, 1);
    gui_widget_setsizepercent(h, 98, 98);
    gui_widget_setzindex(h, 0);
    gui_graph_setminy(h, -100);
    gui_graph_setmaxy(h, 100);
    gui_graph_setminx(h, 0);
    gui_graph_setmaxx(h, 800);
    gui_graph_zoomreset(h);

    /* Create graph data */
    data = gui_graph_data_create(0, GUI_GRAPH_TYPE_YT, 800);
    gui_graph_data_setcolor(data, GUI_COLOR_GREEN);

    for (i = 0; i < 100; i++) {
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

    /* Create checkbox for maximize function */
    h = gui_checkbox_create(0, 10, 10, 300, 30, parent, checkbox_callback, 0);
    gui_widget_settext(h, _GT("Expand graph"));
    gui_widget_setzindex(h, 1);
    gui_checkbox_setcolor(h, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_LIGHTRED);
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

            graph = gui_widget_getbyid(ID_GRAPH);
            if (graph != NULL) {
                gui_widget_setexpanded(graph, gui_checkbox_ischecked(h));
            }
        }
    }
    return ret;
}

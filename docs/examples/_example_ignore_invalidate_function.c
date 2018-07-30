gui_handle_p h;
gui_graph_data_p data;
size_t i;

/* Create graph with ignore invalidate flag (GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE) */
h = gui_graph_create(ID_GRAPH, 10, 10, 400, 300, parent, graph_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE, 0);

/* If GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE is not used on create call */
/* user may manually call gui_widget_setignoreinvalidate to enable ignore invalidate */
gui_widget_setignoreinvalidate(h, 1, 0, 0);

/* Now set all parameters for widget */
/* Each of them might want to invalidate widget */
/* which means we may waste time to call the same thing 5 times */
gui_widget_setpositionpercent(h, 1, 1, 0);
gui_widget_setsizepercent(h, 98, 98, 0);
gui_widget_setzindex(h, 0, 0);
gui_graph_setaxes(h, 0, 200, -50, 200, 0);
gui_graph_zoomreset(h, 0);

/* Now disable ignore invalidation and force invalidation */
gui_widget_setignoreinvalidate(h, 0, 1, 0);
handle = gui_graph_create(ID_GRAPH_MAIN, 10, 10, 400, 220, h, 0, 0, 0);

/* Create data objects */
if (graphdata1 == NULL) {
    graphdata1 = gui_graph_data_create(GUI_GRAPH_TYPE_XY, len, 0);
    graphdata1->Color = GUI_COLOR_RED;
}
if (graphdata2 == NULL) {
    graphdata2 = gui_graph_data_create(GUI_GRAPH_TYPE_YT, len / 2, 0);
    graphdata2->Color = GUI_COLOR_GREEN;
}

/* Fill data objects */
for (i = 0; i <= 360; i += 360 / len) {
    x = cos((float)i * (PI / 180.0f));
    y = sin((float)i * (PI / 180.0f));
    gui_graph_data_addvalue(graphdata1, x * radius, y * radius, 0);
    gui_graph_data_addvalue(graphdata2, x * radius / 3, y * radius / 4, 0);
}

/* Attach data objects to plot */
gui_graph_attachdata(handle, graphdata1, 0);
gui_graph_attachdata(handle, graphdata2, 0);
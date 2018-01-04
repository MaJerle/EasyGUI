/* First checkbox widget */
handle = gui_checkbox_create(0, 10, 10, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box default"));

/* Second checkbox widget */
handle = gui_checkbox_create(0, 10, 50, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box checked"));
gui_checkbox_setchecked(handle, 1);

/* Third checkbox widget */
handle = gui_checkbox_create(0, 10, 90, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box disabled"));
gui_checkbox_setdisabled(handle, 1);

/* Forth checkbox widget */
handle = gui_checkbox_create(0, 10, 130, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box checked disabled"));
gui_checkbox_setchecked(handle, 1);
gui_checkbox_setdisabled(handle, 1);
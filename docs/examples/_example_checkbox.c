/* First checkbox widget */
handle = gui_checkbox_create(0, 10, 10, 400, 40, 0, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box default"), 0);

/* Second checkbox widget */
handle = gui_checkbox_create(0, 10, 50, 400, 40, 0, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box checked"), 0);
gui_checkbox_setchecked(handle, 1, 0);

/* Third checkbox widget */
handle = gui_checkbox_create(0, 10, 90, 400, 40, 0, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box disabled"), 0);
gui_checkbox_setdisabled(handle, 1, 0);

/* Forth checkbox widget */
handle = gui_checkbox_create(0, 10, 130, 400, 40, 0, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box checked disabled"), 0);
gui_checkbox_setchecked(handle, 1, 0);
gui_checkbox_setdisabled(handle, 1, 0);
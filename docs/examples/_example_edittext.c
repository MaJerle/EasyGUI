/* Create widget and allocate memory for text edit process */
handle = gui_edittext_create(1, 10, 10, 400, 40, h, 0, 0, 0);
gui_widget_alloctextmemory(handle, 255, 0);
gui_widget_settext(handle, _GT("Edit text"), 0);
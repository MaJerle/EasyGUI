handle = gui_progbar_create(2, 10, 10, 400, 40, h, 0, 0, 0);
gui_widget_settext(handle, _GT("Progress bar with custom text"));

handle = gui_progbar_create(2, 10, 60, 400, 40, h, 0, 0, 0);
gui_progbar_enablepercentages(handle, 0);
gui_progbar_setmin(handle, 20, 0);
gui_progbar_setmax(handle, 30, 0);
gui_progbar_setvalue(handle, 22, 0); 
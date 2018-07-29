size_t len;
GUI_LISTVIEW_ROW_p row;

/* Create widget */
handle = gui_listview_create(0, 10, 10, 100, 40, 0, 0, 0, 0);

/* Create columns */
gui_listview_addcolumn(handle, _GT("Name"), 120, 0);
gui_listview_addcolumn(handle, _GT("LastName"), 150, 0);
gui_listview_addcolumn(handle, _GT("Email"), 220, 0);
gui_listview_addcolumn(handle, _GT("Phone"), 160, 0);

/* Create rows and add values for columns */
for (len = 0; len < 10; len++) {
    row = gui_listview_addrow(handle);
    gui_listview_setitemstring(handle, row, 0, _GT("Name 1"), 0);
    gui_listview_setitemstring(handle, row, 1, _GT("Last name 2"), 0);
    gui_listview_setitemstring(handle, row, 2, _GT("info@example.com"), 0);
    gui_listview_setitemstring(handle, row, 3, _GT("0123456789"), 0);
}
size_t i;
GUI_Char* listboxtexts[] = {
    _GT("Item 0"),
    _GT("Item 1"),
    _GT("Item 2"),
    _GT("Item 3"),
    _GT("Item 4"),
    _GT("Item 5"),
    _GT("Item 6"),
    _GT("Item 7"),
    _GT("Item 8"),
    _GT("Item 9"),
    _GT("Item 10"),
    _GT("Item 11"),
    _GT("Item 12"),
};

/* Create left dropdown */
handle = gui_dropdown_create(0, 10, 10, 200, 40, h, 0, 0, 0);
for (i = 0; i < COUNT_OF(listboxtexts); i++) {
    gui_dropdown_addstring(handle, listboxtexts[i]);
}
/* Force slider to be always visible */
gui_dropdown_setsliderauto(handle, 0, 0);
gui_dropdown_setslidervisibility(handle, 1, 0);

/* Create right widget */
handle = gui_dropdown_create(0, 220, 180, 200, 40, h, 0, 0, 0);
gui_dropdown_setopendirection(handle, GUI_DROPDOWN_OPENDIR_UP, 0);
for (i = 0; i < COUNT_OF(listboxtexts); i++) {
    gui_dropdown_addstring(handle, listboxtexts[i], 0);
}
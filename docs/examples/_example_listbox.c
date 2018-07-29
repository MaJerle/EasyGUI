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
size_t i;
    
/* Create left listbox */
handle = gui_listbox_create(1, 10, 10, 190, 195, h, 0, 0, 0);
for (i = 0; i < COUNT_OF(listboxtexts); i++) {
    gui_listbox_addstring(handle, listboxtexts[i], 0);
}

/* Force visible slider */
gui_listbox_setsliderauto(handle, 0, 0);
gui_listbox_setslidervisibility(handle, 1, 0);

/* Create right list box */
handle = gui_listbox_create(1, 210, 10, 200, 195, h, 0, 0, 0);
for (i = 0; i < COUNT_OF(listboxtexts); i++) {
    gui_listbox_addstring(handle, listboxtexts[i], 0);
}
gui_listbox_setsliderauto(handle, 0, 0);
gui_listbox_setslidervisibility(handle, 0, 0);
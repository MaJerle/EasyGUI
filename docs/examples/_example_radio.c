uint8_t i;
for (i = 0; i < 4; i++) {
    handle = gui_radio_create(10, 10, 10 + (i * 30), 300, 25, h, 0, 0);
    switch (i) {
        case 0: gui_widget_settext(handle, _GT("Radio box default")); break;
        case 1: gui_widget_settext(handle, _GT("Radio box selected")); break;
        case 2: gui_widget_settext(handle, _GT("Radio box disabled")); break;
        case 3: gui_widget_settext(handle, _GT("Radio box disabled checked")); break;
        default: break;
    }
    gui_radio_setgroup(handle, i < 2 ? 1 : 2);
    gui_radio_setvalue(handle, i);
    if (i % 2) {
        gui_radio_setselected(handle);
    }
    gui_radio_setdisabled(handle, i / 2);
}
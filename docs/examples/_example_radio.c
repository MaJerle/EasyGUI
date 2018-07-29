uint8_t i;
for (i = 0; i < 4; i++) {
    handle = gui_radio_create(10, 10, 10 + (i * 30), 300, 25, h, 0, 0, 0);
    switch (i) {
        case 0: gui_widget_settext(handle, _GT("Radio box default"), 0); break;
        case 1: gui_widget_settext(handle, _GT("Radio box selected"), 0); break;
        case 2: gui_widget_settext(handle, _GT("Radio box disabled"), 0); break;
        case 3: gui_widget_settext(handle, _GT("Radio box disabled checked"), 0); break;
        default: break;
    }
    gui_radio_setgroup(handle, i < 2 ? 1 : 2, 0);
    gui_radio_setvalue(handle, i, 0);
    if (i % 2) {
        gui_radio_setselected(handle, 0);
    }
    gui_radio_setdisabled(handle, i / 2, 0);
}
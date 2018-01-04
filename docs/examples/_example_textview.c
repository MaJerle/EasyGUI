size_t i = 0;
static const GUI_Char* texts[] = {
    _GT("Text view with top left alignment on screen"),
    _GT("Text view with top center alignment on screen"),
    _GT("Text view with top right alignment on screen"),
    _GT("Text view with middle left alignment on screen"),
    _GT("Text view with middle center alignment on screen"),
    _GT("Text view with middle right alignment on screen"),
    _GT("Text view with bottom left alignment on screen"),
    _GT("Text view with bottom center alignment on screen"),
    _GT("Text view with bottom right alignment on screen"),
};
for (i = 0; i < 9; i++) {
    handle = gui_textview_create(0, 1, 1, 1, 1, 0, 0, 0);
    gui_widget_setsizepercent(handle, 30, 30);
    gui_widget_setpositionpercent(handle, 3 + (i % 3) * 33, 3 + (i / 3) * 33);
    gui_widget_settext(handle, texts[i]);
    gui_widget_setfont(handle, &GUI_Font_Roboto_Italic_14);
    switch (i % 4) {
        case 0: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_WHITE); break;
        case 1: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_YELLOW); break;
        case 2: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_GRAY); break;
        case 3: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_LIGHTGREEN); break;
    }
    switch (i % 3) {
        case 0: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_LEFT); break;
        case 1: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_CENTER); break;
        case 2: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_RIGHT); break;
    }
    switch (i / 3) {
        case 0: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_TOP); break;
        case 1: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_CENTER); break;
        case 2: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_BOTTOM); break;
    }
}
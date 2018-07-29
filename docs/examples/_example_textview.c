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
    handle = gui_textview_create(0, 1, 1, 1, 1, 0, 0, 0, 0);
    gui_widget_setsizepercent(handle, 30, 30, 0);
    gui_widget_setpositionpercent(handle, 3 + (i % 3) * 33, 3 + (i / 3) * 33, 0);
    gui_widget_settext(handle, texts[i], 0);
    gui_widget_setfont(handle, &GUI_Font_Roboto_Italic_14, 0);
    switch (i % 4) {
        case 0: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_WHITE, 0); break;
        case 1: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_YELLOW, 0); break;
        case 2: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_GRAY, 0); break;
        case 3: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_LIGHTGREEN, 0); break;
    }
    switch (i % 3) {
        case 0: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_LEFT, 0); break;
        case 1: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_CENTER, 0); break;
        case 2: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_RIGHT, 0); break;
    }
    switch (i / 3) {
        case 0: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_TOP, 0); break;
        case 1: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_CENTER, 0); break;
        case 2: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_BOTTOM, 0); break;
    }
}
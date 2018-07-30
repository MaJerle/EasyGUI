#include "demo.h"

static uint8_t  text_view_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of strings
 */
static const gui_char* const
strings[] = {
    _GT("Top-left aligned text view"),
    _GT("Top-center aligned text view"),
    _GT("Top-right aligned text view"),
    _GT("Center-left aligned text view"),
    _GT("Center-center aligned text view"),
    _GT("Center-right aligned text view"),
    _GT("Bottom-left aligned text view"),
    _GT("Bottom-center aligned text view"),
    _GT("Bottom-right aligned text view"),
};

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_text_view(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    size_t i;

    gui_protect(protect);
    
    /* Create strings */
    for (i = 0; i < GUI_ARRAYSIZE(strings); i++) {
        h = gui_textview_create(0, 0, 0, 1, 1, parent, text_view_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE, 0);
        gui_widget_setsizepercent(h, 30, 30, 0);
        gui_widget_setpositionpercent(h, 2.5f + 32.5f * (i % 3), 2.5f + (i / 3) * 32.5f, 0);
        gui_widget_settext(h, strings[i], 0);
        gui_textview_setcolor(h, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_LIGHTBLUE, 0);
        switch (i % 3) {
            case 0:
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_LEFT, 0);
                break;
            case 1:
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_CENTER, 0);
                break;
            case 2:
            default:
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_RIGHT, 0);
                break;
        }
        switch (i / 3) {
            case 0:
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_TOP, 0);
                break;
            case 1:
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_CENTER, 0);
                break;
            case 2:
            default:
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_BOTTOM, 0);
                break;
        }
        gui_widget_setignoreinvalidate(h, 0, 1, 0);
    }
    
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
text_view_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        default: break;
    }
    return ret;
}

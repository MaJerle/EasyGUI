#include "demo.h"

static uint8_t  text_view_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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

const gui_char* text = _GT("THISISONE_LONGWORD_                  FOR_WHICH_WE_DONTKNOW _IF_IT_WILL_FIT_INTO_SI NGLE_LINE_WHAT_TO_DO_NOW?");

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
        h = gui_textview_create(0, 0, 0, 1, 1, parent, text_view_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
        gui_widget_setsizepercent(h, 30, 30);
        gui_widget_setpositionpercent(h, 2.5f + 32.5f * (i % 3), 2.5f + (i / 3) * 32.5f);
        gui_widget_settext(h, text);
        gui_widget_setfont(h, &GUI_Font_Calibri_Bold_8);
        gui_textview_setcolor(h, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_LIGHTBLUE);
        switch (i % 3) {
            case 0:
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_LEFT);
                break;
            case 1:
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_CENTER);
                break;
            case 2:
            default:
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_RIGHT);
                break;
        }
        switch (i / 3) {
            case 0:
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_TOP);
                break;
            case 1:
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_CENTER);
                break;
            case 2:
            default:
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_BOTTOM);
                break;
        }
        gui_widget_setignoreinvalidate(h, 0, 1);
    }
    
    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
text_view_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        default: break;
    }
    return ret;
}

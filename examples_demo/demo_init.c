#include "demo.h"

/* Define list of all buttons on main screen */
typedef struct {
    gui_id_t id;
    const uint8_t* label;
    gui_id_t win_id;
} main_win_btn_t;

#define BTN_ENTRY(vid, vlabel, vwin_id)            { .id = vid, .label = vlabel, .win_id = vwin_id },

/* Create list of all buttons */
static main_win_btn_t
btns[] = {
    BTN_ENTRY(ID_BTN_MAINWIN_BUTTONS, "Buttons", ID_WIN_BUTTONS)
    BTN_ENTRY(ID_BTN_MAINWIN_EDITTEXT, "Edit text", ID_WIN_EDITTEXT)
    BTN_ENTRY(ID_BTN_MAINWIN_GRAPH, "Graph", ID_WIN_GRAPH)
    BTN_ENTRY(ID_BTN_MAINWIN_PROGBAR, "Progress bar", ID_WIN_PROGBAR)
    BTN_ENTRY(ID_BTN_MAINWIN_LED, "Led", ID_WIN_LED)
};

static uint8_t  main_win_btn_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Initialize demonstration
 */
void
demo_init(void) {
    size_t i;
    gui_handle_p h;
    gui_dim_t lcd_w;
                                                
    gui_widget_setfontdefault(&GUI_Font_Arial_Bold_18); /* Set default font */
    gui_keyboard_create();

    lcd_w = gui_lcd_getwidth();                 /* Get LCD width */

    /* Print all buttons on screen */
    for (i = 0; i < GUI_COUNT_OF(btns); i++) {
        h = gui_button_create(btns[i].id,
            GUI_FLOAT((i % 3) * lcd_w / 3 + 5), GUI_FLOAT((i / 3) * 40 + 5),
            GUI_FLOAT(lcd_w / 3 - 10), GUI_FLOAT(35),
            NULL, main_win_btn_callback, 0
        );
        gui_widget_settext(h, _GT(btns[i].label));
        gui_widget_setuserdata(h, (void *)btns[i].win_id);
    }
}

/**
 * \brief           Callback function for main window buttons
 */
static uint8_t
main_win_btn_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        case GUI_WC_Click: {
            gui_id_t win_id = (gui_id_t)gui_widget_getuserdata(h);  /* Get user data */
            demo_create_feature_window(win_id);
            break;
        }
        default: break;
    }
    return ret;
}

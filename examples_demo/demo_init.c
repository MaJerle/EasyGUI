#include "demo.h"

/* Define list of all buttons on main screen */
typedef struct {
    gui_id_t id;
    const uint8_t* label;
    gui_id_t win_id;
    win_data_t data;
} main_win_btn_t;

#define BTN_ENTRY(vid, vlabel, vwin_id, create_fn)            { .id = vid, .label = vlabel, .win_id = vwin_id, .data = { .feature_create_fn = create_fn } },

/* Create list of all buttons */
static main_win_btn_t
btns[] = {
    BTN_ENTRY(ID_BTN_MAINWIN_WINDOW, _GT("Window"), ID_WIN_WINDOW, demo_create_feature_windows)
    BTN_ENTRY(ID_BTN_MAINWIN_BUTTONS, _GT("Buttons"), ID_WIN_BUTTONS, demo_create_feature_button)
    BTN_ENTRY(ID_BTN_MAINWIN_EDITTEXT, _GT("Edit text"), ID_WIN_EDITTEXT, demo_create_feature_edit_text)
    BTN_ENTRY(ID_BTN_MAINWIN_GRAPH, _GT("Graph"), ID_WIN_GRAPH, demo_create_feature_graph)
    BTN_ENTRY(ID_BTN_MAINWIN_PROGBAR, _GT("Progress bar"), ID_WIN_PROGBAR, demo_create_feature_progbar)
    BTN_ENTRY(ID_BTN_MAINWIN_LED, _GT("Led"), ID_WIN_LED, NULL)
    BTN_ENTRY(ID_BTN_MAINWIN_LIST_CONTAINER, _GT("ListContainer"), ID_WIN_LIST_CONTAINER, demo_create_feature_list_container)
};

static uint8_t  main_win_btn_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Initialize demonstration
 */
void
demo_init(void) {
    size_t i;
    gui_handle_p h, parent;
    gui_dim_t lcd_w;
                                                
    gui_widget_setfontdefault(&GUI_Font_Arial_Bold_18); /* Set default font */
    gui_keyboard_create();

    lcd_w = gui_lcd_getwidth();                 /* Get LCD width */
    parent = gui_window_getdesktop();

    /* Print all buttons on screen */
    for (i = 0; i < GUI_COUNT_OF(btns); i++) {
        h = gui_button_create(btns[i].id,
            GUI_FLOAT((i % 4) * (lcd_w - 10) / 4 + 10), GUI_FLOAT((i / 4) * 40 + 5),
            GUI_FLOAT(lcd_w / 4 - 10), GUI_FLOAT(35),
            parent, main_win_btn_callback, 0
        );
        gui_widget_settext(h, _GT(btns[i].label));
        btns[i].data.id = btns[i].win_id;
        btns[i].data.title = btns[i].label;
        gui_widget_setuserdata(h, (void *)&btns[i].data);
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
            demo_create_feature_window(gui_widget_getuserdata(h));
            break;
        }
        default: break;
    }
    return ret;
}

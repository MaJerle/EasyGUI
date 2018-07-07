#include "demo.h"

static uint8_t  main_win_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature window widget
 */
void
demo_create_feature_window(gui_id_t win_id) {
    gui_handle_p base, h;
    base = gui_window_getdesktop();             /* Get desktop handle */
  
    h = gui_widget_getbyid(win_id);             /* Get widget by ID first */
    if (h == NULL) {                            /* If it does not exists yet */
        h = gui_window_create(win_id, 10, 10, gui_lcd_getwidth() - 20, gui_lcd_getheight() - 20, base, main_win_callback, 0);
        gui_widget_settransparency(h, 0x7F);
        gui_widget_settext(h, _GT("Window"));   /* Set label */
    }
    gui_widget_show(h);                         /* Show widget */
}

/**
 * \brief           Window event callback
 */
static uint8_t
main_win_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    gui_id_t id;
    gui_handle_p tmp;
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);

    id = gui_widget_getid(h);

    switch (wc) {
        case GUI_WC_Init: {

            if (id == ID_WIN_EDITTEXT) {
                tmp = gui_edittext_create(0, 10, 10, 300, 50, h, NULL, 0);
                gui_widget_alloctextmemory(tmp, 255);
            }
            break;
        }
    }
    return ret;
}
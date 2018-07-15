#include "demo.h"

static uint8_t  main_win_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature window widget
 */
void
demo_create_feature_window(win_data_t* data) {
    gui_handle_p base, h;
    base = gui_window_getdesktop();             /* Get desktop handle */
  
    h = gui_widget_getbyid(data->id);           /* Get widget by ID first */
    if (h == NULL) {                            /* If it does not exists yet */
        h = gui_window_create(data->id, 10, 10, gui_lcd_getwidth() - 20, gui_lcd_getheight() - 20, base, main_win_callback, 0);
        gui_widget_settext(h, data->title);     /* Set label */
        if (data->feature_create_fn != NULL) {
            data->feature_create_fn(h);         /* Call create feature function */
        }
    }
    gui_widget_show(h);                         /* Show widget */
}

/**
 * \brief           Window event callback
 */
static uint8_t
main_win_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    return ret;
}
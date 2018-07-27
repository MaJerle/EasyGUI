#include "demo.h"

static uint8_t  main_win_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);
static uint8_t  window_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature window widget
 */
void
demo_create_feature_window(win_data_t* data) {
    gui_handle_p base, h;
    base = gui_window_getdesktop();             /* Get desktop handle */
  
    h = gui_widget_getbyid(data->id);           /* Get widget by ID first */
    if (h == NULL) {                            /* If it does not exists yet */
        h = gui_window_create(data->id, 50, 50, gui_lcd_getwidth() - 100, gui_lcd_getheight() - 100, base, main_win_callback, 0);
        gui_widget_settext(h, data->title);     /* Set label */
        if (data->feature_create_fn != NULL) {
            data->feature_create_fn(h);         /* Call create feature function */
        }
    }
    gui_widget_show(h);                         /* Show widget */
}
/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_windows(gui_handle_p parent) {
    gui_handle_p h;
    uint8_t alpha = 0xC0;

    gui_widget_setposition(parent, 0, 0);
    gui_widget_setsize(parent, 800, 480);
    gui_widget_setalpha(parent, alpha);

    /* Create first window */
    h = gui_window_create(0, 1, 1, 1, 1, parent, window_callback, 0);
    gui_widget_setposition(h, 20, 20);
    gui_widget_setsize(h, 750, 410);
    gui_widget_setalpha(h, alpha);

    /* Create second window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, 0);
    gui_widget_setposition(h, 20, 20);
    gui_widget_setsize(h, 700, 340);
    gui_widget_setalpha(h, alpha);

    /* Create third window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, 0);
    gui_widget_setposition(h, 20, 20);
    gui_widget_setsize(h, 650, 270);
    gui_widget_setalpha(h, alpha);

    /* Create 4th window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, 0);
    gui_widget_setposition(h, 20, 20);
    gui_widget_setsize(h, 600, 200);
    gui_widget_setalpha(h, alpha);
}

/**
 * \brief           Window event callback
 */
static uint8_t
main_win_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    return ret;
}

/**
 * \brief           Event callback
 */
static uint8_t
window_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    return ret;
}


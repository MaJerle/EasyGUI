#include "demo.h"

static uint8_t  main_cont_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);
static uint8_t  window_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature window widget
 */
void
demo_create_feature(win_data_t* data, uint8_t protect) {
    gui_handle_p base, h;
    base = gui_window_getdesktop();             /* Get desktop handle */
  
    gui_protect(protect);

    h = gui_widget_getbyid_ex(data->id, base, 0, 0);/* Get widget by ID first */
    if (h != NULL) {
        gui_widget_remove(&h, 0);
        h = NULL;
    }
    if (h == NULL) {                            /* If it does not exists yet */
        h = gui_container_create(data->id,
            GUI_FLOAT(50), GUI_FLOAT(50),
            GUI_FLOAT(gui_lcd_getwidth() - 100), GUI_FLOAT(gui_lcd_getheight() - 100),
            base, main_cont_callback, 0, 0);

        gui_widget_setignoreinvalidate(h, 1, 0, 0);
        gui_container_setcolor(h, GUI_CONTAINER_COLOR_BG, GUI_COLOR_DARKGRAY, 0);
        gui_widget_setsizepercent(h, 68, 98, 0);
        gui_widget_setpositionpercent(h, 31, 1, 0);
        gui_widget_setignoreinvalidate(h, 0, 1, 0);

        gui_widget_settext(h, data->title, 0);  /* Set label */
        if (data->feature_create_fn != NULL) {
            data->feature_create_fn(h, 0);      /* Call create feature function */
        }
    } else {
        gui_widget_putonfront(h, 0, 0);         /* Move widget to front */
    }
    gui_widget_show(h, 0);                      /* Show widget */

    gui_unprotect(protect);
}

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_window(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;
    uint8_t alpha = 0xFF;

    gui_protect(protect);

    /* Create first window */
    h = gui_window_create(0, 1, 1, 1, 1, parent, window_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setpositionpercent(h, 5, 5, 0);
    gui_widget_setsizepercent(h, 90, 90, 0);
    gui_widget_setalpha(h, alpha, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create second window */ 
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setpositionpercent(h, 5, 5, 0);
    gui_widget_setsizepercent(h, 90, 90, 0);
    gui_widget_setalpha(h, alpha, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create third window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setpositionpercent(h, 5, 5, 0);
    gui_widget_setsizepercent(h, 90, 90, 0);
    gui_widget_setalpha(h, alpha, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create forth window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_widget_setpositionpercent(h, 5, 5, 0);
    gui_widget_setsizepercent(h, 90, 90, 0);
    gui_widget_setalpha(h, alpha, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    gui_unprotect(protect);
}

/**
 * \brief           Window event callback
 */
static uint8_t
main_cont_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
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


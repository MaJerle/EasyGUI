#include "demo.h"

static uint8_t  main_cont_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);
static uint8_t  window_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature window widget
 */
void
demo_create_feature(win_data_t* data, uint8_t protect) {
    gui_handle_p base, h;
    base = gui_window_getdesktop();             /* Get desktop handle */
  
    gui_protect(protect);

    h = gui_widget_getbyid_ex(data->id, base, 0);   /* Get widget by ID first */
    if (h != NULL) {
        gui_widget_remove(&h);
        h = NULL;
    }
    if (h == NULL) {                            /* If it does not exists yet */
        h = gui_container_create(data->id,
            GUI_FLOAT(50), GUI_FLOAT(50),
            GUI_FLOAT(gui_lcd_getwidth() - 100), GUI_FLOAT(gui_lcd_getheight() - 100),
            base, main_cont_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);

        gui_container_setcolor(h, GUI_CONTAINER_COLOR_BG, GUI_COLOR_DARKGRAY);
        gui_widget_setsizepercent(h, 68, 98);
        gui_widget_setpositionpercent(h, 31, 1);
        gui_widget_setignoreinvalidate(h, 0, 1);

        gui_widget_settext(h, data->title);
        if (data->feature_create_fn != NULL) {
            data->feature_create_fn(h, 0);      /* Call create feature function */
        }
    } else {
        gui_widget_putonfront(h, 0);            /* Move widget to front */
    }
    gui_widget_show(h);

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
    h = gui_window_create(0, 1, 1, 1, 1, parent, window_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 5, 5);
    gui_widget_setsizepercent(h, 90, 90);
    gui_widget_setalpha(h, alpha);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create second window */ 
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 5, 5);
    gui_widget_setsizepercent(h, 90, 90);
    gui_widget_setalpha(h, alpha);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create third window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 5, 5);
    gui_widget_setsizepercent(h, 90, 90);
    gui_widget_setalpha(h, alpha);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create forth window */
    h = gui_window_create(0, 1, 1, 1, 1, h, window_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 5, 5);
    gui_widget_setsizepercent(h, 90, 90);
    gui_widget_setalpha(h, alpha);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Window event callback
 */
static uint8_t
main_cont_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    return ret;
}

/**
 * \brief           Event callback
 */
static uint8_t
window_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    return ret;
}


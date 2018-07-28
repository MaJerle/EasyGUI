#include "demo.h"

static uint8_t  led_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_led(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);

    /* Create led */
    h = gui_led_create(0, 10, 10, 40, 40, parent, led_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_led_settype(h, GUI_LED_TYPE_RECT, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_ON, GUI_COLOR_LIGHTRED, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_ON_BORDER, GUI_COLOR_LIGHTGRAY, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF, GUI_COLOR_DARKRED, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF_BORDER, GUI_COLOR_DARKGRAY, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create led */
    h = gui_led_create(0, 60, 10, 40, 40, parent, led_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_led_settype(h, GUI_LED_TYPE_CIRCLE, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_ON, GUI_COLOR_LIGHTRED, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_ON_BORDER, GUI_COLOR_LIGHTGRAY, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF, GUI_COLOR_DARKRED, 0);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF_BORDER, GUI_COLOR_DARKGRAY, 0);
    gui_led_set(h, 1, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create led */
    h = gui_led_create(0, 10, 60, 40, 40, parent, led_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_led_settype(h, GUI_LED_TYPE_RECT, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    /* Create led */
    h = gui_led_create(0, 60, 60, 40, 40, parent, led_callback, 0, 0);
    gui_widget_setignoreinvalidate(h, 1, 0, 0);
    gui_led_settype(h, GUI_LED_TYPE_CIRCLE, 0);
    gui_led_set(h, 1, 0);
    gui_widget_setignoreinvalidate(h, 0, 1, 0);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
led_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        case GUI_WC_Click: {
            gui_led_toggle(h, 0);
            break;
        }
        default: break;
    }
    return ret;
}

#include "demo.h"

static uint8_t  led_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Create and open feature widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_led(gui_handle_p parent, uint8_t protect) {
    gui_handle_p h;

    gui_protect(protect);

    /* Create led */
    h = gui_led_create(0, 10, 10, 40, 40, parent, led_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_led_settype(h, GUI_LED_TYPE_RECT);
    gui_led_setcolor(h, GUI_LED_COLOR_ON, GUI_COLOR_LIGHTRED);
    gui_led_setcolor(h, GUI_LED_COLOR_ON_BORDER, GUI_COLOR_LIGHTGRAY);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF, GUI_COLOR_DARKRED);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF_BORDER, GUI_COLOR_DARKGRAY);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create led */
    h = gui_led_create(0, 60, 10, 40, 40, parent, led_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_led_settype(h, GUI_LED_TYPE_CIRCLE);
    gui_led_setcolor(h, GUI_LED_COLOR_ON, GUI_COLOR_LIGHTRED);
    gui_led_setcolor(h, GUI_LED_COLOR_ON_BORDER, GUI_COLOR_LIGHTGRAY);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF, GUI_COLOR_DARKRED);
    gui_led_setcolor(h, GUI_LED_COLOR_OFF_BORDER, GUI_COLOR_DARKGRAY);
    gui_led_set(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create led */
    h = gui_led_create(0, 10, 60, 40, 40, parent, led_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_led_settype(h, GUI_LED_TYPE_RECT);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create led */
    h = gui_led_create(0, 60, 60, 40, 40, parent, led_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_led_settype(h, GUI_LED_TYPE_CIRCLE);
    gui_led_set(h, 1);
    gui_widget_setignoreinvalidate(h, 0, 1);

    gui_unprotect(protect);
}

/**
 * \brief           Event callback
 */
static uint8_t
led_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        case GUI_EVT_CLICK: {
            gui_led_toggle(h);
            break;
        }
        default: break;
    }
    return ret;
}

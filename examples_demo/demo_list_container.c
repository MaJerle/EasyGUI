#include "demo.h"

static uint8_t  list_container_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of buttons on list-container widget
 */
static 
const char * const buttons[] = {
    "Btn 1", "Btn 2", "Btn 3", "Btn 4",
    "Btn 5", "Btn 6", "Btn 7", "Btn 8",
    "Btn 9", "Btn 10", "Btn 11", "Btn 12"
};

/**
 * \brief           Create and open feature window widget
 * \param[in]       parent: Parent widget for new widgets
 */
void
demo_create_feature_list_container(gui_handle_p parent) {
    gui_handle_p btn, h;
    size_t i;

    /* Create list container */
    h = gui_listcontainer_create(0, 10, 10, 300, 50, parent, list_container_callback, 0);
    gui_widget_setpositionpercent(h, 5, 5);
    gui_widget_setsizepercent(h, 90, 90);

    /* Create buttons and put them to container */
    for (i = 0; i < GUI_ARRAYSIZE(buttons); i++) {
        btn = gui_button_create(0, 10, 10, 10, 10, h, NULL, 0);
        gui_widget_setpositionpercent(btn, 5.0f, 5.0f + i * 20.0f);
        gui_widget_setsizepercent(btn, 90, 17);
        gui_widget_settext(btn, _GT(buttons[i]));
    }
}

/**
 * \brief           Window event callback
 */
static uint8_t
list_container_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    return gui_widget_processdefaultcallback(h, wc, param, result);
}

#include "demo.h"

static uint8_t  list_container_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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
demo_create_feature_list_container(gui_handle_p parent, uint8_t protect) {
    gui_handle_p btn, h;
    size_t i;

    gui_protect(protect);

    /* Create left list container */
    h = gui_listcontainer_create(0, 10, 10, 300, 50, parent, list_container_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 1, 1);
    gui_widget_setsizepercent(h, 48, 98);
    gui_listcontainer_setcolor(h, GUI_LISTCONTAINER_COLOR_BG, GUI_COLOR_LIGHTBLUE);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create buttons and put them to container */
    for (i = 0; i < GUI_ARRAYSIZE(buttons); i++) {
        btn = gui_button_create(0, 10, 10, 10, 10, h, NULL, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
        gui_widget_setpositionpercent(btn, 5.0f, 5.0f + i * 20.0f);
        gui_widget_setsizepercent(btn, 90, 17);
        gui_widget_settext(btn, _GT(buttons[i]));
        gui_widget_setignoreinvalidate(btn, 0, 1);
    }

    /* Create right list container */
    h = gui_listcontainer_create(0, 10, 10, 300, 50, parent, list_container_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 51, 1);
    gui_widget_setsizepercent(h, 48, 98);
    gui_listcontainer_setcolor(h, GUI_LISTCONTAINER_COLOR_BG, GUI_COLOR_ORANGE);
    gui_widget_setignoreinvalidate(h, 0, 1);
    
    /* Create container inside */
    h = gui_listcontainer_create(0, 10, 10, 300, 50, h, list_container_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(h, 0, 0);
    gui_widget_setsizepercent(h, 100, 200);
    gui_listcontainer_setcolor(h, GUI_LISTCONTAINER_COLOR_BG, GUI_COLOR_LIGHTGREEN);
    gui_widget_setignoreinvalidate(h, 0, 1);

    /* Create buttons and put them to container */
    for (i = 0; i < GUI_ARRAYSIZE(buttons); i++) {
        btn = gui_button_create(0, 10, 10, 10, 10, h, NULL, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
        gui_widget_setpositionpercent(btn, 5.0f, i * 20.0f);
        gui_widget_setsizepercent(btn, 90, 17);
        gui_widget_settext(btn, _GT(buttons[i]));
        gui_widget_setignoreinvalidate(btn, 0, 1);
    }
    gui_unprotect(protect);
}

/**
 * \brief           Window event callback
 */
static uint8_t
list_container_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    return gui_widget_processdefaultcallback(h, evt, param, result);
}

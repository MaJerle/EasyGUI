/* Get all widgets of parent widget from last to first */
/* Parent object must have GUI_HANDLE_ROOT_t data type */
void LoopWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;         /* Value holding widgets of parent */

    for (h = gui_linkedlist_widgetgetprev((GUI_HANDLE_ROOT_t *)parent, NULL); h;
    	h = gui_linkedlist_widgetgetprev(NULL, h)) {
        /* Loop through each widget of parent */
    }
}
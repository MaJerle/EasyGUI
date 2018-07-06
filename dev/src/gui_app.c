/**
 * \file            gui_app.c
 * \brief           GUI application
 */

#include "app.h"

/* Touch data structure */
TM_TOUCH_t TS;

gui_dim_t lcd_width, lcd_height;

/**
 * \brief           Creates desktop with GUI
 */
void
create_desktop(void) {
    gui_handle_p winbase;
    
    lcd_width = gui_lcd_getwidth();
    lcd_height = gui_lcd_getheight();
    
    gui_widget_setfontdefault(&GUI_Font_Arial_Narrow_Italic_22);
    
    winbase = gui_window_getdesktop();          /* Get desktop window */
    if (winbase != NULL) {
        /* Create status bar */
        gui_container_create(GUI_ID_CONTAINER_STATUS, 0, 0, lcd_width, 40, NULL, gui_container_callback, 0);
    }
    
    /* Fake opening containers to create them */
    open_container(GUI_ID_CONTAINER_CONSOLE);
    open_container(GUI_ID_CONTAINER_LOG);
    open_container(GUI_ID_CONTAINER_WIFI);      /* Open WiFi container as last one */
}

/**
 * \brief           Open container with specific ID on main page
 * \param[in]       id: Container ID to open
 */
void
open_container(gui_id_t cont_id) {
    gui_handle_p h;
    
    gui_widget_hidechildren(gui_window_getdesktop());   /* Hide all widgets on desktop first */
    gui_widget_show(gui_widget_getbyid(GUI_ID_CONTAINER_STATUS));
    
    h = gui_widget_getbyid(cont_id);            /* Get widget by ID */
    if (h == NULL) {
        h = gui_container_create(cont_id, 0, 40, lcd_width, lcd_height - 40, NULL, gui_container_callback, 0);
    }
    gui_widget_show(h);                         /* Show widget */
    gui_widget_putonfront(h);                   /* Put widget to most visible area */
}

/**
 * \brief           Read touch from screen and notify GUI
 */
void
read_touch(void) {
    static gui_touch_data_t p = {0}, t = {0};
    uint8_t i, update = 0, diffx, diffy;
    TM_TOUCH_Read(&TS);                         /* Read touch data */
    
    memset((void *)&t, 0x00, sizeof(t));
    t.status = TS.NumPresses ? GUI_TOUCH_STATE_PRESSED : GUI_TOUCH_STATE_RELEASED;
    t.count = TS.NumPresses > GUI_CFG_TOUCH_MAX_PRESSES ? GUI_CFG_TOUCH_MAX_PRESSES : TS.NumPresses;
    for (i = 0; i < t.count; i++) {
        t.x[i] = TS.X[i];
        t.y[i] = TS.Y[i];
    }
    if (t.count != p.count) {
        update = 1;
    } else {
        for (i = 0; i < t.count; i++) {
            diffx = GUI_ABS(p.x[i] - t.x[i]);
            diffy = GUI_ABS(p.y[i] - t.y[i]);
            if (diffx > 2 || diffy > 2) {
                update = 1;
                break;
            }
        }
    }
    
    /* Check differences */
    if (update || (t.status == GUI_TOUCH_STATE_RELEASED && p.status != GUI_TOUCH_STATE_RELEASED)) {
        gui_input_touchadd(&t);
    }
    memcpy(&p, &t, sizeof(p));
}

/**
 * \file            gui_app.c
 * \brief           GUI application
 */

#include "app.h"

/* Touch data structure */
TM_TOUCH_t TS;

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

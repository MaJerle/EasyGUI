/**	
 * \file            gui.c
 * \brief           Core file
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui.h"
#include "system/gui_sys.h"

/**
 * \brief           GUI global structure
 */
gui_t GUI;

/**
 * \brief           Clips are required to draw widget
 * \param[in]       h: Widget handle
 */
static void
check_disp_clipping(gui_handle_p h) {
#if !GUI_CFG_USE_POS_SIZE_CACHE
    gui_dim_t x, y, wi, hi;
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
    
    /* Copy current setup */
    memcpy(&GUI.display_temp, &GUI.display, sizeof(GUI.display_temp));
    
#if GUI_CFG_USE_POS_SIZE_CACHE
    if (GUI.display_temp.x1 == GUI_DIM_MAX || GUI.display_temp.x1 < h->abs_visible_x1) {
        GUI.display_temp.x1 = h->abs_visible_x1;
    }
    if (GUI.display_temp.y1 == GUI_DIM_MAX || GUI.display_temp.y1 < h->abs_visible_y1) {
        GUI.display_temp.y1 = h->abs_visible_y1;
    }
    if (GUI.display_temp.x2 == GUI_DIM_MIN || GUI.display_temp.x2 > h->abs_visible_x2) {
        GUI.display_temp.x2 = h->abs_visible_x2;
    }
    if (GUI.display_temp.y2 == GUI_DIM_MIN || GUI.display_temp.y2 > h->abs_visible_y2) {
        GUI.display_temp.y2 = h->abs_visible_y2;
    }
#else /* GUI_CFG_USE_POS_SIZE_CACHE */

    /* Set widget itself first */
    x = gui_widget_getabsolutex(h); 
    y = gui_widget_getabsolutey(h); 
    wi = gui_widget_getwidth(h);   
    hi = gui_widget_getheight(h);   
    
    /* Step 1: Set active clipping area only for current widget */
    if (GUI.display_temp.x1 == GUI_DIM_MAX || GUI.display_temp.x1 < x) {
        GUI.display_temp.x1 = x;
    }
    if (GUI.display_temp.y1 == GUI_DIM_MAX || GUI.display_temp.y1 < y) {
        GUI.display_temp.y1 = y;
    }
    if (GUI.display_temp.x2 == GUI_DIM_MIN || GUI.display_temp.x2 > (x + wi)) {
        GUI.display_temp.x2 = x + wi;
    }
    if (GUI.display_temp.y2 == GUI_DIM_MIN || GUI.display_temp.y2 > (y + hi)) {
        GUI.display_temp.y2 = y + hi;
    }
    
    /*
     * Step 2: Set active clipping area, combining all parent together
     * 
     * Process all parent widgets and check what is the position of them
     * Make sure that on current widget we only draw actual visible area
     */
    for (; h != NULL; h = guii_widget_getparent(h)) {
        x = guii_widget_getparentabsolutex(h);     /* Parent absolute X position for inner widgets */
        y = guii_widget_getparentabsolutey(h);     /* Parent absolute Y position for inner widgets */
        wi = guii_widget_getparentinnerwidth(h);   /* Get parent inner width */
        hi = guii_widget_getparentinnerheight(h);  /* Get parent inner height */

        GUI.display_temp.x1 = GUI_MAX(GUI.display_temp.x1, x);
        GUI.display_temp.x2 = GUI_MIN(GUI.display_temp.x2, x + wi);
        GUI.display_temp.y1 = GUI_MAX(GUI.display_temp.y1, y);
        GUI.display_temp.y2 = GUI_MIN(GUI.display_temp.y2, y + hi);
    }
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
}

/**
 * \brief           Redraw all widgets of selected parent
 * \param[in]       parent: Parent widget handle to draw widgets on
 * \param[in]       force_redraw: Set to 1 to force drawing all widgets on linked list
 * \return          Number of widgets redrawn
 */
static uint32_t
redraw_widgets(gui_handle_p parent, uint8_t force_redraw) {
    gui_handle_p h;
    uint32_t cnt = 0;
    static uint32_t level = 0;

    /* Go through all elements of parent */
    GUI_LINKEDLIST_WIDGETSLISTNEXT(parent, h) {
        if (!guii_widget_isvisible(h)) {            /* Check if visible */
            guii_widget_clrflag(h, GUI_FLAG_REDRAW);/* Clear flag to be sure */
            continue;                               /* Ignore hidden elements */
        }
        if (guii_widget_isinsideclippingregion(h, 1)) { /* If widget is inside clipping region and not fully covered by any of its siblings */
            /* Draw main widget if required */
            if (guii_widget_getflag(h, GUI_FLAG_REDRAW) || force_redraw) {    /* Check if redraw required */
#if GUI_CFG_USE_ALPHA
                gui_layer_t* layerPrev = GUI.lcd.drawing_layer; /* Save drawing layer */
                uint8_t transparent = 0;
#endif /* GUI_CFG_USE_ALPHA */
                
                guii_widget_clrflag(h, GUI_FLAG_REDRAW);    /* Clear flag for drawing on widget */
                
                /* Prepare clipping region for this widget drawing */
                check_disp_clipping(h);             /* Check coordinates for drawings only particular widget */

#if GUI_CFG_USE_ALPHA
                /* Check alpha and check if blending function exists to merge layers later together */
                if (guii_widget_hasalpha(h) /* && GUI.ll.CopyBlend != NULL */) {
                    gui_dim_t width = GUI.display_temp.x2 - GUI.display_temp.x1;
                    gui_dim_t height = GUI.display_temp.y2 - GUI.display_temp.y1;
                    
                    /* Try to allocate memory for new virtual layer for temporary usage */
                    GUI.lcd.drawing_layer = GUI_MEMALLOC(sizeof(*GUI.lcd.drawing_layer) + (size_t)width * (size_t)height * (size_t)GUI.lcd.pixel_size);
                    
                    if (GUI.lcd.drawing_layer != NULL) {/* Check if allocation was successful */
                        GUI.lcd.drawing_layer->width = width;
                        GUI.lcd.drawing_layer->height = height;
                        GUI.lcd.drawing_layer->x_pos = GUI.display_temp.x1;
                        GUI.lcd.drawing_layer->y_pos = GUI.display_temp.y1;
                        GUI.lcd.drawing_layer->start_address = ((uint8_t *)GUI.lcd.drawing_layer) + sizeof(*GUI.lcd.drawing_layer);
                        transparent = 1;            /* We are going to transparent drawing mode */
                    } else {
                        GUI.lcd.drawing_layer = layerPrev;  /* Reset layer back */
                    }
                }
#endif /* GUI_CFG_USE_ALPHA */
                
                /* Draw widget itself normally, don't care on layer offset and size */
                GUI_EVT_PARAMTYPE_DISP(&GUI.evt_param) = &GUI.display_temp;
                guii_widget_callback(h, GUI_EVT_DRAW, &GUI.evt_param, &GUI.evt_result);
                
                /* Check if there are children widgets in this widget */
                if (guii_widget_haschildren(h)) {   /* Check if widget has children */
                    /* ...now call function for actual redrawing process */
                    /* Force children redraw operation, even if no redraw flag set */
                    level++;
                    cnt += redraw_widgets(h, 1);    /* Redraw children widgets */
                    level--;
                }
                
                /* TODO: Copy previous temporary variables instead of calling function again */
                /* Prepare clipping region for this widget drawing */
                check_disp_clipping(h);             /* Check coordinates for drawings only particular widget */
                
                /* Draw widget itself normally, don't care on layer offset and size */
                GUI_EVT_PARAMTYPE_DISP(&GUI.evt_param) = &GUI.display_temp;
                guii_widget_callback(h, GUI_EVT_DRAWAFTER, &GUI.evt_param, &GUI.evt_result);
                
#if GUI_CFG_USE_ALPHA
                /* If transparent mode is used on widget, copy content back */
                if (transparent) {                  /* If we are in transparent mode */
                    /* Copy layers with blending */
                    if (GUI.ll.CopyBlend != NULL) { /* Hardware way */
                        GUI.ll.CopyBlend(&GUI.lcd, GUI.lcd.drawing_layer,
                            (void *)(((uint8_t *)layerPrev->start_address) +
                                GUI.lcd.pixel_size * (layerPrev->width * (GUI.lcd.drawing_layer->y_pos - layerPrev->y_pos) + (GUI.lcd.drawing_layer->x_pos - layerPrev->x_pos))),
                            (void *)GUI.lcd.drawing_layer->start_address,
                            gui_widget_getalpha(h), 0xFF,
                            GUI.lcd.drawing_layer->width, GUI.lcd.drawing_layer->height,
                            layerPrev->width - GUI.lcd.drawing_layer->width, 0
                        );
                    } else {                        /* Software way, ugly and slow way */
                        gui_dim_t x, y, dxo, dyo;
                        gui_color_t fg, bg;
                        uint8_t r, g, b;
                        float a;

                        /* Get difference in offset */
                        dxo = GUI.lcd.drawing_layer->x_pos - layerPrev->x_pos;
                        dyo = GUI.lcd.drawing_layer->y_pos - layerPrev->y_pos;;

                        a = GUI_FLOAT(gui_widget_getalpha(h)) / GUI_FLOAT(0xFF);
                        for (y = 0; y < GUI.lcd.drawing_layer->height; y++) {
                            for (x = 0; x < GUI.lcd.drawing_layer->width; x++) {
                                fg = GUI.ll.GetPixel(&GUI.lcd, GUI.lcd.drawing_layer, x, y);
                                bg = GUI.ll.GetPixel(&GUI.lcd, layerPrev, dxo + x, dyo + y);

                                r = GUI_U8(((fg >> 16) & 0xFF) * a + (1.0f - a) * ((bg >> 16) & 0xFF));
                                g = GUI_U8(((fg >> 8) & 0xFF) * a + (1.0f - a) * ((bg >> 8) & 0xFF));
                                b = GUI_U8(((fg >> 0) & 0xFF) * a + (1.0f - a) * ((bg >> 0) & 0xFF));
                                
                                fg = (gui_color_t)(0xFF000000UL | (uint8_t)r << 16 | (uint8_t)g << 8 | (uint8_t)b);
                                
                                GUI.ll.SetPixel(&GUI.lcd, layerPrev, dxo + x, dyo + y, fg);
                            }
                        }                        
                    }
                    
                    GUI_MEMFREE(GUI.lcd.drawing_layer); /* Free memory for virtual layer */
                    GUI.lcd.drawing_layer = layerPrev;  /* Reset layer pointer */
                }
#endif /* GUI_CFG_USE_ALPHA */

                cnt++;
            /* Check if any child widget needs drawing */
            } else if (guii_widget_haschildren(h)) {
                cnt += redraw_widgets(h, 0);        /* Redraw children widgets */
            }
        }
    }
    return cnt;                                     /* Return number of redrawn objects */
}

#if GUI_CFG_USE_TOUCH

/**
 * \brief           Set relative coordinate of touch on widget
 *
 *					Relative coordinates are calculated based on widget position on screen
 *					and actual absolute X and Y values from touch event
 * \param[in,out]   ts: Raw touch data with X and Y position
 * \param[in]       h: Widget handle. Set to `NULL` to use default value
 */
static void
set_relative_coordinate(guii_touch_data_t* ts, gui_touch_data_t* old, gui_handle_p h) {
    uint8_t i = 0;

    /* Get absolute position */
    if (h != NULL) {
        ts->widget_x = gui_widget_getabsolutex(h);
        ts->widget_y = gui_widget_getabsolutey(h);
        ts->widget_width = gui_widget_getwidth(h);
        ts->widget_height = gui_widget_getheight(h);
    } else {
        ts->widget_x = 0;
        ts->widget_y = 0;
        ts->widget_width = gui_lcd_getwidth();
        ts->widget_height = gui_lcd_getheight();
    }
    
    /* Calculate values */
    for (i = 0; i < ts->ts.count; i++) {
        /* Get previous values  */
        ts->x_old[i] = old->x[i];
        ts->y_old[i] = old->y[i];

        /* Calculate difference new value versus old */
        ts->x_diff[i] = ts->ts.x[i] - ts->x_old[i];
        ts->y_diff[i] = ts->ts.y[i] - ts->y_old[i];

        /* Calculate widget relative coordinate */
        ts->x_rel[i] = ts->ts.x[i] - ts->widget_x;
        ts->y_rel[i] = ts->ts.y[i] - ts->widget_y;
    }

#if GUI_CFG_TOUCH_MAX_PRESSES > 1
    if (ts->ts.count == 2) {                        /* 2 points detected */
        ts->distance_old = ts->distance;            /* Save old distance */
        gui_math_distancebetweenxy(ts->x_rel[0], ts->y_rel[0], ts->x_rel[1], ts->y_rel[1], &ts->distance);  /* Calculate distance between 2 points */
    }
#endif /* GUI_CFG_TOUCH_MAX_PRESSES > 1 */
}

/*
 * How touch events work
 *
 * When using touch, 3 events are supported on widget:
 *  - TouchStart: Triggered when touch is going from released status to pressed status (touch just pressed)
 *  - TouchMove: Triggered when touch is in pressed status and new pressed event is received (finger is moving on widget pressed on touch panel)
 *  - TouchEnd: Triggered when touch is going from pressed status to released. Finger has been just released from touch panel
 *
 * Besides, with these 3 basic touch events, it is possible to handle 3 more events,
 *         which may not be used just with touch but can also be used with mouse
 *  - Click: Triggered when pressed state is detected and after that released state, still on widget coordinates
 *  - LongClick: Triggered when pressed state is detected for x amount of time (usually 2 seconds)
 *  - DblClick: Triggered when 2 Click events are detected in range of defined time
 *
 * To be able to detect DblClick, 2 Click events must be valid in certain time.
 *
 * Since second events are based on first 3 events for touch, first events must be respected for proper handling second ones.
 *
 * Each widget receives a callback for TouchStart, TouchMove and TouchEnd events.
 * Callback function can return status if event is processed or not. On the returned value 2 things are possible.
 *
 * Consider using button and dropdown. In button, everything what matters is TouchStart and TouchEnd.
 * If TouchMove happens, it is not necessary to handle because only press is needed for detection.
 * In this case, if TouchStart happen and TouchMove too (finger moved inside widget),
 * click event will happen after TouchEnd event, because TouchMove command was ignored and 0 was returned on callback.
 *
 * In dropdown mode it is different because TouchMove is required to detect sliding on items.
 * If TouchMove is detected and 1 is returned from callback, Click event won't be called after TouchEnd event,
 * even if release was inside widget. This is not valid Click because TouchMove was required for something.
 * If Click event would be also triggered in this case, after sliding, item value would be automatically selected which is wrong.
 * In this case, Click event is detected only if TouchMove is not detected (TouchStart and after immediatelly TouchEnd).
 *
 * This is all handled by this "thread" function
 */

/**
 * \brief           Touch event proto thread 
 *
 *                  Designed as non-blocking event thread for processing input touch events.
 * \param[in]       ts: Touch information with X and Y positions
 * \param[in]       old: Old touch information
 * \param[in]       v: Is input value valid or is just "check" call
 * \param[out]      result: Result of event, if any
 * \return          PT thread result
 */
static
PT_THREAD(__TouchEvents_Thread(guii_touch_data_t* const ts, gui_touch_data_t* const old, uint8_t v, gui_widget_evt_t* const result)) {
    static volatile uint32_t time;
    static uint8_t i = 0;
    static gui_dim_t x[2], y[2];

    *result = (gui_widget_evt_t)0;                  /* Reset widget control variable */          

    PT_BEGIN(&ts->pt);                              /* Start thread execution */

    memset(x, 0x00, sizeof(x));                     /* Reset X values */
    memset(y, 0x00, sizeof(y));                     /* Reset Y values */
    for (i = 0; i < 2;) {                           /* Allow up to 2 touch presses */

        /* Wait for valid input with pressed state */
        PT_WAIT_UNTIL(&ts->pt, v && ts->ts.status && !old->status && ts->ts.count == 1);

        time = ts->ts.time;                         /* Get start time of this touch */
        x[i] = ts->x_rel[0];                        /* Save X value */
        y[i] = ts->y_rel[0];                        /* Save Y value */
        
        /* Either wait for released status or timeout */
        do {
            PT_YIELD(&ts->pt);                      /* Stop thread for now and wait next call */

            /* Wait for new data */
            /* Wait touch with released state or timeout */
            PT_WAIT_UNTIL(&ts->pt, v || (gui_sys_now() - time) > GUI_CFG_LONG_CLICK_TIMEOUT);

            /* We have new touch entry, but we do
               not yet if it is "pressed" or "released" */
            if (v) {
                /*
                 * If touch is still pressed (touch move) and we have active widget and touch move event was not processed
                 * then we can use click events also after touch move (for example, button is that widget) where in
                 * some cases, click event should not be processed after touch move (slider, dropdown, etc)
                 */
                if (ts->ts.status && GUI.active_widget != NULL && !guii_widget_getflag(GUI.active_widget, GUI_FLAG_TOUCH_MOVE)) {
                    time = ts->ts.time;             /* Get start time of this touch */
                    x[i] = ts->x_rel[0];            /* Update X value */
                    y[i] = ts->y_rel[0];            /* Update Y value */
                    continue;                       /* Continue and wait for next (released) event */
                } else {                            /* Released status received */
                    break;                          /* Stop execution, continue later */
                }
            }
            else {
                break;                              /* Stop while loop execution */
            }
        } while (1);

        /* Check what was the reason for thread to continue */
        if (v) {                                    /* New touch event occurred */
            if (!ts->ts.status) {                   /* We received released state */

                /* Try to get second click, check difference for double click */
                /* Difference was too big, reset and act like normal click */
                if (i && (GUI_ABS(x[0] - x[1]) > 30 || GUI_ABS(y[0] - y[1]) > 30)) {
                    i = 0;
                }
                set_relative_coordinate(ts, old, GUI.active_widget);
                if (
                    x[0] < 0 || x[0] > ts->widget_width ||
                    y[0] < 0 || y[0] > ts->widget_height ||
                    x[1] < 0 || x[1] > ts->widget_width ||
                    y[1] < 0 || y[1] > ts->widget_height
                    ) {
                    PT_EXIT(&ts->pt);               /* Exit thread, invalid coordinate for touch click or double click */
                }
                if (!i) {                           /* On first call, this is click event */
                    *result = GUI_EVT_CLICK;        /* Click event occurred */

                    time = ts->ts.time;             /* Save last time */
                    PT_YIELD(&ts->pt);              /* Stop thread for now and wait next call with new touch event */

                    /* Wait for valid input with pressed state */
                    PT_WAIT_UNTIL(&ts->pt, (v && ts->ts.status) || (gui_sys_now() - time) > 300);
                    if ((gui_sys_now() - time) > 300) { /* Check timeout for new pressed state */
                        PT_EXIT(&ts->pt);           /* Exit protothread */
                    }
                } else {
                    *result = GUI_EVT_DBLCLICK;     /* Double click event */
                    PT_EXIT(&ts->pt);               /* Reset protothread */
                }
            }
        } else {
            if (!i) {                               /* Timeout occurred with no touch data, long click */
                *result = GUI_EVT_LONGCLICK;        /* Click event occurred */
            }
            PT_EXIT(&ts->pt);                       /* Exit protothread here */
        }
        i++;
    }
    PT_END(&ts->pt);                                /* Stop thread execution */
}

/**
 * \brief           Process input touch event
 *                  
 *                  Scan all widgets from top to bottom which will be first on valid 
 *                  position for touch and call callback function to this widget
 * \param[in]       touch: Touch data info
 * \param[in]       parent: Parent widget where to check for touch
 * \return          Member of \ref guii_touch_status_t enumeration about success
 */
static guii_touch_status_t
process_touch(guii_touch_data_t* const touch, gui_touch_data_t* const touch_old, gui_handle_p parent) {
    gui_handle_p h;
    static uint8_t deep = 0;
    static uint8_t isKeyboard = 0;
    uint8_t dialogOnly = 0;
    guii_touch_status_t tStat = touchCONTINUE;
    
    /*
     * To handle touch events, process widgets in reverse order,
     * starting from widget on most deep level.
     *
     * This is due to the fact that widget with most deep level,
     * is displayed on top of screen = should be detected first
     */
    GUI_LINKEDLIST_WIDGETSLISTPREV(parent, h) {
        if (guii_widget_ishidden(h)) {             /* Ignore hidden widget */
            continue;
        }
        
        /*
         * Dialogs are placed as children of main window
         * If level 1 is current and dialog is detected,
         * stop processsing other widgets except if widget is inside dialog
         */
        if (deep == 1) {                            /* On base elements list = children of base window element */
            if (guii_widget_isdialogbase(h)) {      /* We found dialog element */
                dialogOnly = 1;                     /* Check only widgets which are dialog based */
            }
        }
        
        /* When we should only check dialogs and previous element is not dialog anymore */
        if (dialogOnly && !guii_widget_isdialogbase(h)) {
            break;
        }
        
        /* Check for keyboard mode */
        if (h->id == GUI_ID_KEYBOARD_BASE) {
            isKeyboard = 1;                         /* Set keyboard mode as 1 */
        }
        
        /*
         * Before we check if touch position matches widget coordinates
         * we have to check if this widget has any direct children
         */
        if (guii_widget_haschildren(h)) {           /* Check if widget has children */
            deep++;                                 /* Go deeper in level */
            tStat = process_touch(touch, touch_old, h); /* Process touch on widget elements first */
            deep--;                                 /* Go back to normal level */
        }
        
        /* hildren widgets were not detected */
        if (tStat == touchCONTINUE) {               /* Do we still have to check this widget? */
            check_disp_clipping(h);                 /* Check display region where widget is placed */
        
            /* Check if widget is in touch area */
            if (touch->ts.x[0] >= GUI.display_temp.x1 && touch->ts.x[0] <= GUI.display_temp.x2 && 
                touch->ts.y[0] >= GUI.display_temp.y1 && touch->ts.y[0] <= GUI.display_temp.y2) {
                set_relative_coordinate(touch, touch_old, h);
                    
                /* Call touch start callback to see if widget accepts touches */
                GUI_EVT_PARAMTYPE_TOUCH(&GUI.evt_param) = touch;
                guii_widget_callback(h, GUI_EVT_TOUCHSTART, &GUI.evt_param, &GUI.evt_result);
                tStat = GUI_EVT_RESULTTYPE_TOUCH(&GUI.evt_result);
                if (tStat == touchCONTINUE) {       /* Check result status */
                    tStat = touchHANDLED;           /* If command is processed, touchCONTINUE can't work */
                }
                
                /*
                 * Move widget down on parent linked list and do the same with all of its parents,
                 * no matter of touch focus or not
                 */
                guii_widget_movedowntree(h);
                
                if (tStat == touchHANDLED) {        /* Touch handled for widget completely */
                    /*
                     * Set active widget and set flag for it
                     * Set focus widget and set flag for it but only do this if widget is not related to keyboard
                     *
                     * This allows us to click keyboard items but not to lose focus on main widget
                     */
                    if (!isKeyboard) {
                        guii_widget_focus_set(h);
                    }
                    guii_widget_active_set(h);
                    
                    /*
                     * Invalidate actual handle object
                     * Already invalidated in guii_widget_active_set function
                     */
                    //gui_widget_invalidate(h);   
                } else {                            /* Touch handled with no focus */
                    /*
                     * When touch was handled without focus,
                     * process only clearing currently focused and active widgets and clear them
                     */
                    if (!isKeyboard) {
                        guii_widget_focus_clear();
                    }
                    guii_widget_active_clear();
                }
            }
        }
        
        /* Check for keyboard mode */
        if (gui_widget_getid(h) == GUI_ID_KEYBOARD_BASE) {
            isKeyboard = 0;                         /* Set keyboard mode as 1 */
        }
        
        if (tStat != touchCONTINUE) {               /* Return status if necessary */
            return tStat;
        }
    }
    return touchCONTINUE;                           /* Try with another widget */
}

#define __ProcessAfterTouchEventsThread() do {\
    if (rresult != 0) {                             /* Valid event occurred */\
        uint8_t ret;                                \
        GUI_EVT_PARAMTYPE_TOUCH(&param) = &GUI.touch;    \
        ret = guii_widget_callback(GUI.active_widget, rresult, &param, NULL);\
        if (rresult == GUI_EVT_DBLCLICK && !ret) {   /* If double click was not recorded, proceed with normal click again */\
            guii_widget_callback(GUI.active_widget, GUI_EVT_CLICK, &param, NULL);/* Check for normal click now */\
        }\
    }\
} while (0)

/**
 * \brief           Process touch inputs
 * 
 *                  Reads all input touches written so far to buffer 
 *                  and goes one by one
 */
static void
gui_process_touch(void) {
    gui_evt_param_t param = {0};
    gui_evt_result_t result = {0};
    gui_widget_evt_t rresult;
    
    if (guii_input_touchavailable()) {              /* Check if any touch available */
        while (guii_input_touchread(&GUI.touch.ts)) {   /* Process all touch events possible */
            /* Set relative coordinates for new widget directly */
            if (GUI.active_widget != NULL && GUI.touch.ts.status) {
                set_relative_coordinate(&GUI.touch, &GUI.touch_old, GUI.active_widget);
            }
            
            /*
             * Old status: pressed
             * New status: pressed
             * Action: Touch move on active element
             */
            if (GUI.touch.ts.status && GUI.touch_old.status) {
                if (GUI.active_widget != NULL) {    /* If active widget exists */
                    if (GUI.touch.ts.count == GUI.touch_old.count) {
                        gui_handle_p aw = GUI.active_widget;/* Temporary set active widget */
                        do {
                            uint8_t r;
                            GUI_EVT_PARAMTYPE_TOUCH(&param) = &GUI.touch;
                            GUI_EVT_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                            if (aw != GUI.active_widget) {
                                r = guii_widget_callback(aw, GUI_EVT_TOUCHSTART, &param, &result);   /* The same amount of touch events currently */
                                GUI_EVT_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                            }
                            r = guii_widget_callback(aw, GUI_EVT_TOUCHMOVE, &param, &result);   /* The same amount of touch events currently */
                            if (r) {                /* Check if touch move processed */
                                guii_widget_setflag(aw, GUI_FLAG_TOUCH_MOVE);   /* Touch move has been processed */
                            } else {
                                guii_widget_clrflag(aw, GUI_FLAG_TOUCH_MOVE);   /* Touch move has not been processed */
                            }
                            if (GUI_EVT_RESULTTYPE_TOUCH(&result) != touchCONTINUE) {
                                break;
                            }
                            
                            /*
                             * TODO: Handle relative coordinates on new object
                             *
                             * If widget does not detect touch start, then forward touch start to parent widget.
                             * With this approach, you can achieve slider on parent widget
                             */
                            aw = guii_widget_getparent(aw);    /* Get parent widget */
                            if (aw != NULL) {
                                set_relative_coordinate(&GUI.touch, &GUI.touch_old, aw);
                            }
                        } while (aw != NULL);
                        
                        /*
                         * In case touch move widget was detected on another widget,
                         * set this new widget to active from now on
                         */
                        if (aw != NULL) {
                            if (aw != GUI.active_widget) {
                                guii_widget_active_set(aw);  /* Set new active widget */
                            }
                        }
                    } else {
                        GUI_EVT_PARAMTYPE_TOUCH(&param) = &GUI.touch;
                        GUI_EVT_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                        guii_widget_callback(GUI.active_widget, GUI_EVT_TOUCHSTART, &param, &result);    /* New amount of touch elements happened */
                    }
                }
            }
            
            /*
             * Old status: released
             * New status: pressed
             * Action: Touch down on element, find element
             */
            if (GUI.touch.ts.status && !GUI.touch_old.status) {
                process_touch(&GUI.touch, &GUI.touch_old, NULL);
                if (GUI.active_widget != GUI.active_widget_prev) {  /* If new active widget is not the same as previous */
                    PT_INIT(&GUI.touch.pt)          /* Reset thread, otherwise process with double click event */
                }
            }
            
            /* Periodical check for events on active widget */
            if (GUI.active_widget != NULL) {
                __TouchEvents_Thread(&GUI.touch, &GUI.touch_old, 1, &rresult);   /* Call thread for touch process */
                __ProcessAfterTouchEventsThread();  /* Process after event macro */
            }
            
            /*
             * Old status: pressed
             * New status: released
             * Action: Touch up on active element
             */
            if (!GUI.touch.ts.status && GUI.touch_old.status) {
                if (GUI.active_widget != NULL) {    /* Check if active widget */
                    GUI_EVT_PARAMTYPE_TOUCH(&param) = &GUI.touch;
                    GUI_EVT_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                    guii_widget_callback(GUI.active_widget, GUI_EVT_TOUCHEND, &param, &result);  /* Process callback function */
                    guii_widget_active_clear();     /* Clear active widget */
                }
            }
            
            memcpy((void *)&GUI.touch_old, (void *)&GUI.touch.ts, sizeof(GUI.touch_old));   /* Copy current touch to last touch status */
        }
    } else {                                        /* No new touch events, periodically call touch event thread */
        __TouchEvents_Thread(&GUI.touch, &GUI.touch_old, 0, &rresult);   /* Call thread for touch process periodically, handle long presses or timeouts */
        __ProcessAfterTouchEventsThread();          /* Process after event macro */
    }
}

#endif /* GUI_CFG_USE_TOUCH */

#if GUI_CFG_USE_KEYBOARD || __DOXYGEN__

/**
 * \brief           Process input keyboard data
 * 
 *                  Check all input keyboard data and process entry by entry to relevant widget
 */
static void
process_keyboard(void) {
    guii_keyboard_data_t key;
    
    gui_evt_param_t param = {0};
    gui_evt_result_t result = {0};
    
    while (guii_input_keyread(&key.kb)) {           /* Read all keyboard entires */
        if (GUI.focused_widget != NULL) {           /* Check if any widget is in focus already */
            GUI_EVT_PARAMTYPE_KEYBOARD(&param) = &key;
            GUI_EVT_RESULTTYPE_KEYBOARD(&result) = keyCONTINUE;
            guii_widget_callback(GUI.focused_widget, GUI_EVT_KEYPRESS, &param, &result);
            if (GUI_EVT_RESULTTYPE_KEYBOARD(&result) != keyHANDLED) {
                if (key.kb.keys[0] == GUI_KEY_TAB) {/* Tab key pressed, set next widget as focused */
                    gui_handle_p h = gui_linkedlist_widgetgetnext(NULL, GUI.active_widget); /* Get next widget if possible */
                    if (h != NULL && guii_widget_ishidden(h)) {/* Ignore hidden widget */
                        h = NULL;
                    }
                    if (h == NULL) {                /* There is no next widget */
                        for (h = gui_linkedlist_widgetgetnext(guii_widget_getparent(GUI.focused_widget), NULL); 
                            h != NULL; h = gui_linkedlist_widgetgetnext(NULL, h)) {
                            if (guii_widget_isvisible(h)) {    /* Check if widget is visible */
                                break;
                            }
                        }
                    }
                    if (h != NULL) {                /* We have next widget */
                        gui_linkedlist_widgetmovetobottom(h); /* Set widget to the down of list */
                        guii_widget_focus_set(h);   /* Set focus to new widget */
                    }
                }
            }
        }
    }
}
#endif /* GUI_CFG_USE_KEYBOARD || __DOXYGEN__ */

/**
 * \brief           Process redraw of all widgets
 */
static void
process_redraw(void) {
    gui_layer_t* active = GUI.lcd.active_layer;
    gui_layer_t* drawing = GUI.lcd.drawing_layer;
    uint8_t result = 1;
    gui_display_t* dispA = &active->display;
    
    if ((GUI.lcd.flags & GUI_FLAG_LCD_WAIT_LAYER_CONFIRM) || !(GUI.flags & GUI_FLAG_REDRAW)) {  /* Check if anything to draw first */
        return;
    }
    
    GUI.flags &= ~GUI_FLAG_REDRAW;                  /* Clear redraw flag */

    /* Copy from currently active layer to drawing layer only changes on layer */
    if (dispA->x1 < GUI.lcd.width && dispA->x2 >= 0 && dispA->y1 < GUI.lcd.height && dispA->y2 >= 0) {
        GUI.ll.Copy(&GUI.lcd, drawing, 
            (void *)(((uint8_t *)drawing->start_address) + GUI.lcd.pixel_size * (dispA->y1 * drawing->width + dispA->x1)),   /* Destination address */
            (void *)(((uint8_t *)active->start_address) + GUI.lcd.pixel_size * (dispA->y1 * active->width + dispA->x1)), /* Source address */
            dispA->x2 - dispA->x1,                  /* Area width */
            dispA->y2 - dispA->y1,                  /* Area height */
            drawing->width - (dispA->x2 - dispA->x1),   /* Offline destination */
            active->width - (dispA->x2 - dispA->x1) /* Offline source */
        );
    }
    
    redraw_widgets(NULL, 0);                        /* Redraw all widgets now on drawing layer */
    drawing->pending = 1;                           /* Set drawing layer as pending */

    /* Draw clipping area rectangle on screen for debug */
    //gui_draw_rectangle(&GUI.display, GUI.display.x1, GUI.display.y1, GUI.display.x2, GUI.display.y2, GUI_COLOR_RED);
    
    /* Notify low-level about layer change */
    GUI.lcd.flags |= GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;
    gui_ll_control(&GUI.lcd, GUI_LL_Command_SetActiveLayer, &drawing, &result); /* Set new active layer to low-level driver */
    
    /* Swap active and drawing layers */
    /* New drawings won't be affected until confirmation from low-level is not received */
    GUI.lcd.active_layer = drawing;
    GUI.lcd.drawing_layer = active;
    
    memcpy(&GUI.lcd.active_layer->display, &GUI.display, sizeof(GUI.display));  /* Copy clipping data to region */
    
    /* Invalid clipping region(s) for next drawing process */
    GUI.display.x1 = GUI_DIM_MAX;
    GUI.display.y1 = GUI_DIM_MAX;
    GUI.display.x2 = GUI_DIM_MIN;
    GUI.display.y2 = GUI_DIM_MIN;
}

/**
 * \brief           Default global callback function
 */
static void
default_event_cb(void) {

}

#if GUI_CFG_OS || __DOXYGEN__
/**
 * \brief           GUI main thread for RTOS
 * \param[in]       argument: Pointer to user specific argument
 */
static void
gui_thread(void * const argument) {
    if (argument != NULL) {                         /* If argument exists */
        *((volatile uint8_t *)argument) = 0x01;     /* Set it to 1, indicate thread started */
    }
    while (1) {
        gui_process();                              /* Process graphical update */
    }
}
#endif /* GUI_CFG_OS || __DOXYGEN__ */

/**
 * \brief           Initializes GUI stack.
 *                    In addition, it prepares memory for work with widgets on later usage and
 *                    calls low-layer functions to initialize LCD or custom driver for LCD
 * \return          Member of \ref guir_t enumeration
 */
guir_t
gui_init(void) {
    uint8_t result;

    memset((void *)&GUI, 0x00, sizeof(GUI));        /* Reset GUI structure */
    
    gui_seteventcallback(NULL);                     /* Set event callback */
    
#if GUI_CFG_OS
    /* Init system */
    gui_sys_init();                                 /* Init low-level system */
    gui_sys_mbox_create(&GUI.OS.mbox, 32);          /* Message box for 10 elements */
#endif /* GUI_CFG_OS */
    
    /* Call LCD low-level function */
    result = 1;
    gui_ll_control(&GUI.lcd, GUI_LL_Command_Init, &GUI.ll, &result);/* Call low-level initialization */
    GUI.ll.Init(&GUI.lcd);                          /* Call user LCD driver function */
    
    /* Check situation with layers */
    if (GUI.lcd.layer_count >= 1) {
        size_t i;
        /* Set default values for all layers */
        /* User layers use full screen */
        for (i = 0; i < GUI.lcd.layer_count; i++) {
            GUI.lcd.layers[i].x_pos = 0;
            GUI.lcd.layers[i].y_pos = 0;
            GUI.lcd.layers[i].width = GUI.lcd.width;
            GUI.lcd.layers[i].height = GUI.lcd.height;
        }
        GUI.lcd.active_layer = &GUI.lcd.layers[0];
        GUI.lcd.drawing_layer = &GUI.lcd.layers[0];
        GUI.ll.Fill(&GUI.lcd, GUI.lcd.drawing_layer, (void *)GUI.lcd.drawing_layer->start_address, GUI.lcd.width, GUI.lcd.height, 0, GUI_COLOR_LIGHTGRAY);
        if (GUI.lcd.layer_count > 1) {
            GUI.lcd.drawing_layer = &GUI.lcd.layers[1];
        }
    } else {
        return guiERROR;
    }
    
    guii_input_init();                              /* Init input devices */
    GUI.initialized = 1;                            /* GUI is initialized */
    guii_widget_init();                             /* Init widgets */
    
#if GUI_CFG_OS
    /* Create graphical thread */
    if (GUI.OS.thread_id == NULL) {
        volatile uint8_t started = 0;
        if (gui_sys_thread_create(&GUI.OS.thread_id, "gui_thread", gui_thread, (void *)&started, GUI_SYS_THREAD_SS, GUI_SYS_THREAD_PRIO)) {
            while (!started) {                      /* Wait for thread to start */

            }
        }
    }
#endif /* GUI_CFG_OS */
    
    return guiOK;
}

/**
 * \brief           Processes all drawing operations for GUI
 * \note            When `GUI_CFG_OS = 0`, user has to call this function in main loop,
 *                     otherwise it is processed in separated thread by GUI `GUI_CFG_OS != 0`
 * \return          Number of jobs done in current call
 */
int32_t
gui_process(void) {
#if GUI_CFG_OS
    gui_mbox_msg_t* msg;
    uint32_t time;
    uint32_t tmr_cnt = guii_timer_getactivecount(); /* Get number of active timers in system */
    
    time = gui_sys_mbox_get(&GUI.OS.mbox, (void **)&msg, tmr_cnt ? 1 : 20); /* Get value from message queue */
    
    GUI_UNUSED(time);
    GUI_UNUSED(msg);
#endif /* GUI_CFG_OS */
   
    GUI_CORE_PROTECT(1);
    guii_timer_process();                           /* Process all timers */
    guii_widget_executeremove();                    /* Delete widgets */
#if GUI_CFG_USE_TOUCH
    gui_process_touch();                            /* Process touch inputs */
#endif /* GUI_CFG_USE_TOUCH */
#if GUI_CFG_USE_KEYBOARD
    process_keyboard();                             /* Process keyboard inputs */
#endif /* GUI_CFG_USE_KEYBOARD */
    process_redraw();                               /* Redraw widgets */
    GUI_CORE_UNPROTECT(1);
    
    return 0;                                       /* Return number of elements updated on GUI */
}

/**
 * \brief           Set callback for global events from GUI
 * \param[in]       evt_fn: Callback function
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_seteventcallback(gui_eventcallback_t cb) {
    if (cb != NULL) {
        GUI.evt_cb = cb;                            /* Set user callback */
    } else {
        GUI.evt_cb = default_event_cb;              /* Set default callback */
    }
    return 1;
}

#if GUI_CFG_OS || __DOXYGEN__

/**
 * \brief           Protect core from multiple thread access
 * \param[in]       protect: Set to `1` to protect, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_protect(const uint8_t protect) {
    if (protect) {
        GUI_CORE_PROTECT(1);
    }
    return 1;
}

/**
 * \brief           Unprotect core from multiple thread access
 * \param[in]       unprotect: Set to `1` to unprotect, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_unprotect(const uint8_t unprotect) {
    if (unprotect) {
        GUI_CORE_UNPROTECT(1);
        gui_sys_mbox_putnow(&GUI.OS.mbox, 0x00);    /* Wakeup processing thread */
    }
    return 1;
}

/**
 * \brief           Delay GUI in units of milliseconds
 * \note            Available only when \ref GUI_CFG_OS is enabled
 * \param[in]       ms: Time to delay in units of milliseconds
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_delay(const uint32_t ms) {
    gui_sys_sem_t sem;
    if (!ms) {
        return 1;
    }
    if (gui_sys_sem_create(&sem, 0)) {
        gui_sys_sem_wait(&sem, ms);
        gui_sys_sem_release(&sem);
        gui_sys_sem_delete(&sem);
        return 1;
    }
    return 0;
}

#endif /* GUI_CFG_OS || __DOXYGEN__ */

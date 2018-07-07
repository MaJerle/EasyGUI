/**	
 * \file            gui.c
 * \brief           Core file
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
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
 * Author:          Tilen Majerle <tilen@majerle.eu>
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
 * \brief           Clip are required to draw widget
 * \param[in]       h: Widget handle
 */
static void
check_disp_clipping(gui_handle_p h) {
#if !GUI_CFG_USE_POS_SIZE_CACHE
    gui_dim_t x, y, wi, hi;
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
    
    /* Copy current setup */
    memcpy(&GUI.DisplayTemp, &GUI.Display, sizeof(GUI.DisplayTemp));
    
#if GUI_CFG_USE_POS_SIZE_CACHE
    if (GUI.DisplayTemp.x1 == (gui_dim_t)0x7FFF || GUI.DisplayTemp.x1 < h->abs_visible_x1) {
        GUI.DisplayTemp.x1 = h->abs_visible_x1;
    }
    if (GUI.DisplayTemp.y1 == (gui_dim_t)0x7FFF || GUI.DisplayTemp.y1 < h->abs_visible_y1) {
        GUI.DisplayTemp.y1 = h->abs_visible_y1;
    }
    if (GUI.DisplayTemp.x2 == (gui_dim_t)0x8000 || GUI.DisplayTemp.x2 > h->abs_visible_x2) {
        GUI.DisplayTemp.x2 = h->abs_visible_x2;
    }
    if (GUI.DisplayTemp.y2 == (gui_dim_t)0x8000 || GUI.DisplayTemp.y2 > h->abs_visible_y2) {
        GUI.DisplayTemp.y2 = h->abs_visible_y2;
    }
#else /* GUI_CFG_USE_POS_SIZE_CACHE */

    /* Set widget itself first */
    x = guii_widget_getabsolutex(h);
    y = guii_widget_getabsolutey(h);
    wi = guii_widget_getwidth(h);
    hi = guii_widget_getheight(h);container
    
    
    /*
     * Step 1: Set active clipping area only for current widget
     */
    if (GUI.DisplayTemp.x1 == (gui_dim_t)0x7FFF)   { GUI.DisplayTemp.x1 = x; }
    if (GUI.DisplayTemp.y1 == (gui_dim_t)0x7FFF)   { GUI.DisplayTemp.y1 = y; }
    if (GUI.DisplayTemp.x2 == (gui_dim_t)0x8000)   { GUI.DisplayTemp.x2 = GUI.lcd.width; }
    if (GUI.DisplayTemp.y2 == (gui_dim_t)0x8000)   { GUI.DisplayTemp.y2 = GUI.lcd.height; }
    
    if (GUI.DisplayTemp.x1 < x)             { GUI.DisplayTemp.x1 = x; }
    if (GUI.DisplayTemp.x2 > x + wi)        { GUI.DisplayTemp.x2 = x + wi; }
    if (GUI.DisplayTemp.y1 < y)             { GUI.DisplayTemp.y1 = y; }
    if (GUI.DisplayTemp.y2 > y + hi)        { GUI.DisplayTemp.y2 = y + hi; }
    
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
        
        if (GUI.DisplayTemp.x1 < x)         { GUI.DisplayTemp.x1 = x; }
        if (GUI.DisplayTemp.x2 > x + wi)    { GUI.DisplayTemp.x2 = x + wi; }
        if (GUI.DisplayTemp.y1 < y)         { GUI.DisplayTemp.y1 = y; }
        if (GUI.DisplayTemp.y2 > y + hi)    { GUI.DisplayTemp.y2 = y + hi; }
    }
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
}

/**
 * \brief           Redraw all widgets of selected parent
 * \param[in]       parent: Parent widget handle to draw widgets on
 * \return          Number of widgets redrawn
 */
static uint32_t
redraw_widgets(gui_handle_p parent) {
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
            if (guii_widget_getflag(h, GUI_FLAG_REDRAW)) {  /* Check if redraw required */
#if GUI_CFG_USE_TRANSPARENCY
                gui_layer_t* layerPrev = GUI.lcd.drawing_layer; /* Save drawing layer */
                uint8_t transparent = 0;
#endif /* GUI_CFG_USE_TRANSPARENCY */
                
                guii_widget_clrflag(h, GUI_FLAG_REDRAW);    /* Clear flag for drawing on widget */
                
                /* Prepare clipping region for this widget drawing */
                check_disp_clipping(h);             /* Check coordinates for drawings only particular widget */

#if GUI_CFG_USE_TRANSPARENCY
                /*
                 * Check transparency and check if blending function exists to merge layers later together
                 */
                if (guii_widget_istransparent(h) && GUI.ll.CopyBlend != NULL) {
                    gui_dim_t width = GUI.DisplayTemp.x2 - GUI.DisplayTemp.x1;
                    gui_dim_t height = GUI.DisplayTemp.y2 - GUI.DisplayTemp.y1;
                    
                    /* Try to allocate memory for new virtual layer for temporary usage */
                    GUI.lcd.drawing_layer = GUI_MEMALLOC(sizeof(*GUI.lcd.drawing_layer) + (size_t)width * (size_t)height * (size_t)GUI.lcd.pixel_size);
                    
                    if (GUI.lcd.drawing_layer != NULL) {/* Check if allocation was successful */
                        GUI.lcd.drawing_layer->width = width;
                        GUI.lcd.drawing_layer->height = height;
                        GUI.lcd.drawing_layer->x_offset = GUI.DisplayTemp.x1;
                        GUI.lcd.drawing_layer->y_offset = GUI.DisplayTemp.y1;
                        GUI.lcd.drawing_layer->start_address = ((uint8_t *)GUI.lcd.drawing_layer) + sizeof(*GUI.lcd.drawing_layer);
                        transparent = 1;            /* We are going to transparent drawing mode */
                    } else {
                        GUI.lcd.drawing_layer = layerPrev;  /* Reset layer back */
                    }
                }
#endif /* GUI_CFG_USE_TRANSPARENCY */
                
                /* Draw widget itself normally, don't care on layer offset and size */
                GUI_WIDGET_PARAMTYPE_DISP(&GUI.WidgetParam) = &GUI.DisplayTemp;  /* Set parameter */
                guii_widget_callback(h, GUI_WC_Draw, &GUI.WidgetParam, &GUI.WidgetResult); /* Draw widget */
                
                /* Check if there are children widgets in this widget */
                if (guii_widget_allowchildren(h)) {
                    gui_handle_p tmp;
                    
                    /* Set drawing flag to all widgets  first... */
                    GUI_LINKEDLIST_WIDGETSLISTNEXT(h, tmp) {
                        guii_widget_setflag(tmp, GUI_FLAG_REDRAW); /* Set redraw bit to all children elements */
                    }
                            
                    /* ...now call function for actual redrawing process */
                    level++;
                    cnt += redraw_widgets(h);       /* Redraw children widgets */
                    level--;
                }
                
#if GUI_CFG_USE_TRANSPARENCY
                /*
                 * If transparent mode is used on widget, copy content back
                 */
                if (transparent) {                  /* If we were in transparent mode */
                    /* Copy layers with blending */
                    if (GUI.ll.CopyBlend != NULL) { /* Hardware way */
                        GUI.ll.CopyBlend(&GUI.lcd, GUI.lcd.drawing_layer,
                            (void *)(((uint8_t *)layerPrev->start_address) +
                                GUI.lcd.pixel_size * (layerPrev->width * (GUI.lcd.drawing_layer->y_offset - layerPrev->y_offset) + (GUI.lcd.drawing_layer->x_offset - layerPrev->x_offset))),
                            (void *)GUI.lcd.drawing_layer->start_address,
                            guii_widget_gettransparency(h), 0xFF,
                            GUI.lcd.drawing_layer->width, GUI.lcd.drawing_layer->height,
                            layerPrev->width - GUI.lcd.drawing_layer->width, 0
                        );
                    } else {                        /* Software way, ugly and slow way */
                        gui_dim_t x, y;
                        gui_color_t fg, bg;
                        uint8_t r, g, b;
                        float a;

                        a = GUI_FLOAT(guii_widget_gettransparency(h)) / GUI_FLOAT(0xFF);
                        for (y = 0; y < GUI.lcd.drawing_layer->height; y++) {
                            for (x = 0; x < GUI.lcd.drawing_layer->width; x++) {
                                fg = GUI.ll.GetPixel(&GUI.lcd, GUI.lcd.drawing_layer, x, y);
                                bg = GUI.ll.GetPixel(&GUI.lcd, layerPrev, GUI.lcd.drawing_layer->x_offset + x, GUI.lcd.drawing_layer->y_offset + y);

                                r = GUI_U8(((fg >> 16) & 0xFF) * a + (1.0f - a) * ((bg >> 16) & 0xFF));
                                g = GUI_U8(((fg >> 8) & 0xFF) * a + (1.0f - a) * ((bg >> 8) & 0xFF));
                                b = GUI_U8(((fg >> 0) & 0xFF) * a + (1.0f - a) * ((bg >> 0) & 0xFF));
                                
                                fg = (gui_color_t)(0xFF000000UL | (uint8_t)r << 16 | (uint8_t)g << 8 | (uint8_t)b);
                                
                                GUI.ll.SetPixel(&GUI.lcd, layerPrev, GUI.lcd.drawing_layer->x_offset + x, GUI.lcd.drawing_layer->y_offset + y, fg);
                            }
                        }                        
                    }
                    
                    GUI_MEMFREE(GUI.lcd.drawing_layer); /* Free memory for virtual layer */
                    GUI.lcd.drawing_layer = layerPrev;  /* Reset layer pointer */
                }
#endif /* GUI_CFG_USE_TRANSPARENCY */

                cnt++;
            /*
             * Check if any widget from children should be redrawn
             */
            } else if (guii_widget_allowchildren(h)) {
                cnt += redraw_widgets(h);           /* Redraw children widgets */
            }
        }
    }
    if (level == 0) {
        GUI_DEBUG("Number of widgets: %d\r\n", (int)cnt);
    }
    return cnt;                                     /* Return number of redrawn objects */
}

#if GUI_CFG_USE_TOUCH
/*
 * How touch events works
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
PT_THREAD(__TouchEvents_Thread(guii_touch_data_t* ts, guii_touch_data_t* old, uint8_t v, gui_wc_t* result)) {
    static volatile uint32_t time;
    static uint8_t i = 0;
    static gui_dim_t x[2], y[2];

    *result = (gui_wc_t)0;                          /* Reset widget control variable */          

    PT_BEGIN(&ts->pt);                              /* Start thread execution */

    memset(x, 0x00, sizeof(x));                     /* Reset X values */
    memset(y, 0x00, sizeof(y));                     /* Reset Y values */
    for (i = 0; i < 2;) {                           /* Allow up to 2 touch presses */
        /*
         * Wait for valid input with pressed state
         */
        PT_WAIT_UNTIL(&ts->pt, v && ts->ts.status && !old->ts.status && ts->ts.count == 1);
        
        time = ts->ts.time;                         /* Get start time of this touch */
        x[i] = ts->x_rel[0];                        /* Save X value */
        y[i] = ts->y_rel[0];                        /* Save Y value */
        
        /* Either wait for released status or timeout */
        do {
            PT_YIELD(&ts->pt);                      /* Stop thread for now and wait next call */
            PT_WAIT_UNTIL(&ts->pt, v || (gui_sys_now() - time) > 2000); /* Wait touch with released state or timeout */
            
            if (v) {                                /* New valid touch entry received, either released or pressed again */
                /*
                 * If touch is still pressed (touch move) and we have active widget and touch move event was not processed
                 * then we can use click events also after touch move (for example, button is that widget) where in
                 * some cases, click event should not be processed after touch move (slider, dropdown, etc)
                 */
                if (ts->ts.status && GUI.ActiveWidget != NULL && !guii_widget_getflag(GUI.ActiveWidget, GUI_FLAG_TOUCH_MOVE)) {
                    time = ts->ts.time;             /* Get start time of this touch */
                    x[i] = ts->x_rel[0];            /* Update X value */
                    y[i] = ts->y_rel[0];            /* Update Y value */
                    continue;                       /* Continue and wait for next (released) event */
                } else {                            /* Released status received */
                    break;                          /* Stop execution, continue later */
                }
            } else {
                break;                              /* Stop while loop execution */
            }
        } while (1);
        
        /*
         * Check what was the reason for thread to continue
         */
        if (v) {                                    /* New touch event occurred */
            if (!ts->ts.status) {                   /* We received released state */
                if (i) {                            /* Try to get second click, check difference for double click */
                    if (GUI_ABS(x[0] - x[1]) > 30 || GUI_ABS(y[0] - y[1]) > 30) {
                        i = 0;                      /* Difference was too big, reset and act like normal click */
                    }
                }
                if (
                    x[0] < 0 || x[0] > ts->widget_width ||
                    y[0] < 0 || y[0] > ts->widget_height ||
                    x[1] < 0 || x[1] > ts->widget_width ||
                    y[1] < 0 || y[1] > ts->widget_height
                ) {
                    PT_EXIT(&ts->pt);               /* Exit thread, invalid coordinate for touch click or double click */
                }
                if (!i) {                           /* On first call, this is click event */
                    *result = GUI_WC_Click;         /* Click event occurred */
                    
                    time = ts->ts.time;             /* Save last time */
                    PT_YIELD(&ts->pt);              /* Stop thread for now and wait next call with new touch event */
                    
                    /*
                     * Wait for valid input with pressed state
                     */
                    PT_WAIT_UNTIL(&ts->pt, (v && ts->ts.status) || (gui_sys_now() - time) > 300);
                    if ((gui_sys_now() - time) > 300) { /* Check timeout for new pressed state */
                        PT_EXIT(&ts->pt);           /* Exit protothread */
                    }
                } else {
                    *result = GUI_WC_DblClick;      /* Double click event */
                    PT_EXIT(&ts->pt);               /* Reset protothread */
                }
            }
        } else {
            if (!i) {                               /* Timeout occurred with no touch data, long click */
                *result = GUI_WC_LongClick;         /* Click event occurred */
            }
            PT_EXIT(&ts->pt);                       /* Exit protothread here */
        }
        i++;
    }
    PT_END(&ts->pt);                                /* Stop thread execution */
}

/**
 * \brief           Set relative coordinate of touch on widget
 * 					
 *					Relative coordinates are calculated based on widget position on screen
 *					and actual absolute X and Y values from touch event
 * \param[in,out]   ts: Raw touch data with X and Y position
 * \param[in]       x: Absolute X position of widget
 * \param[in]       y: Absolute Y position of widget
 * \param[in]       width: Width of widget
 * \param[in]       height: height of widget
 */
static void
set_relative_coordinate(guii_touch_data_t* ts, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height) {
    uint8_t i = 0;
    for (i = 0; i < ts->ts.count; i++) {
        ts->x_rel_old[i] = ts->x_rel[i];            /* Save old on X value */
        ts->y_rel_old[i] = ts->y_rel[i];            /* Save old on Y value */
        ts->x_rel[i] = ts->ts.x[i] - x;             /* Get relative coordinate on widget */
        ts->y_rel[i] = ts->ts.y[i] - y;             /* Get relative coordinate on widget */
    }
    
    ts->widget_width = width;                       /* Set widget width */
    ts->widget_height = height;                     /* Set widget height */

#if GUI_CFG_TOUCH_MAX_PRESSES > 1
    if (ts->ts.count == 2) {                        /* 2 points detected */
        ts->distance_old = ts->distance;            /* Save old distance */
        gui_math_distancebetweenxy(ts->x_rel[0], ts->y_rel[0], ts->x_rel[1], ts->y_rel[1], &ts->distance);  /* Calculate distance between 2 points */
    }
#endif /* GUI_CFG_TOUCH_MAX_PRESSES > 1 */
}

/**
 * \brief           Process input touch event
 *                  
 *                  Scan all widgets from top to bottom which will be first on valid 
 *                  position for touch and call callback function to this widget
 *
 * \param[in]       touch: Touch data info
 * \param[in]       parent: Parent widget where to check for touch
 * \return          Member of \ref guii_touch_status_t enumeration about success
 */
static guii_touch_status_t
process_touch(guii_touch_data_t* touch, gui_handle_p parent) {
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
         * stop process of other widgets except if widget is inside dialog
         */
        if (deep == 1) {                            /* On base elements list = children of base window element */
            if (guii_widget_isdialogbase(h)) {     /* We found dialog element */
                dialogOnly = 1;                     /* Check only widgets which are dialog based */
            }
        }
        
        /* When we should only check dialogs and previous element is not dialog anymore */
        if (dialogOnly && !guii_widget_isdialogbase(h)) {
            break;
        }
        
        /* Check for keyboard mode */
        if (guii_widget_getid(h) == GUI_ID_KEYBOARD_BASE) {
            isKeyboard = 1;                         /* Set keyboard mode as 1 */
        }
        
        /*
         * Before we check if touch position matches widget coordinates
         * we have to check if this widget has any direct children
         */
        if (guii_widget_allowchildren(h)) {        /* If children widgets are allowed */
            deep++;                                 /* Go deeper in level */
            tStat = process_touch(touch, h);        /* Process touch on widget elements first */
            deep--;                                 /* Go back to normal level */
        }
        
        /*
         * Children widgets were not detected
         */
        if (tStat == touchCONTINUE) {               /* Do we still have to check this widget? */
            check_disp_clipping(h);                 /* Check display region where widget is placed */
        
            /* Check if widget is in touch area */
            if (touch->ts.x[0] >= GUI.DisplayTemp.x1 && touch->ts.x[0] <= GUI.DisplayTemp.x2 && 
                touch->ts.y[0] >= GUI.DisplayTemp.y1 && touch->ts.y[0] <= GUI.DisplayTemp.y2) {
                set_relative_coordinate(touch,      /* Set relative coordinate */
                    guii_widget_getabsolutex(h), guii_widget_getabsolutey(h), 
                    guii_widget_getwidth(h), guii_widget_getheight(h)
                ); 
            
                /* Call touch start callback to see if widget accepts touches */
                GUI_WIDGET_PARAMTYPE_TOUCH(&GUI.WidgetParam) = touch;
                guii_widget_callback(h, GUI_WC_TouchStart, &GUI.WidgetParam, &GUI.WidgetResult);
                tStat = GUI_WIDGET_RESULTTYPE_TOUCH(&GUI.WidgetResult);
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
                     * Already invalidated in __GUI_ACTIVE_SET function
                     */
                    //guii_widget_invalidate(h);
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
        if (guii_widget_getid(h) == GUI_ID_KEYBOARD_BASE) {
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
        GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;    \
        ret = guii_widget_callback(GUI.ActiveWidget, rresult, &param, NULL);\
        if (rresult == GUI_WC_DblClick && !ret) {   /* If double click was not recorded, proceed with normal click again */\
            guii_widget_callback(GUI.ActiveWidget, GUI_WC_Click, &param, NULL);    /* Check for normal click now */\
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
    gui_widget_param_t param = {0};
    gui_widget_result_t result = {0};
    gui_wc_t rresult;
    
    if (gui_input_touchavailable()) {               /* Check if any touch available */
        while (gui_input_touchread(&GUI.Touch.ts)) {/* Process all touch events possible */
            if (GUI.ActiveWidget && GUI.Touch.ts.status) {  /* Check active widget for touch and pressed status */
                set_relative_coordinate(&GUI.Touch, /* Set relative touch (for widget) from current touch */
                    guii_widget_getabsolutex(GUI.ActiveWidget), guii_widget_getabsolutey(GUI.ActiveWidget), 
                    guii_widget_getwidth(GUI.ActiveWidget), guii_widget_getheight(GUI.ActiveWidget)
                );
            }
            
            /**
             * Old status: pressed
             * New status: pressed
             * Action: Touch move on active element
             */
            if (GUI.Touch.ts.status && GUI.TouchOld.ts.status) {
                if (GUI.ActiveWidget != NULL) {     /* If active widget exists */
                    if (GUI.Touch.ts.count == GUI.TouchOld.ts.count) {
                        gui_handle_p aw = GUI.ActiveWidget; /* Temporary set active widget */
                        do {
                            uint8_t r;
                            GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;
                            GUI_WIDGET_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                            r = guii_widget_callback(aw, GUI_WC_TouchMove, &param, &result);   /* The same amount of touch events currently */
                            if (r) {                /* Check if touch move processed */
                                guii_widget_setflag(aw, GUI_FLAG_TOUCH_MOVE);    /* Touch move has been processed */
                            } else {
                                guii_widget_clrflag(aw, GUI_FLAG_TOUCH_MOVE);    /* Touch move has not been processed */
                            }
                            if (GUI_WIDGET_RESULTTYPE_TOUCH(&result) != touchCONTINUE) {
                                break;
                            }
                            /**
                             * If widget does not detect touch start, then forward touch start to parent widget.
                             * With this approach, you can achieve slider on parent widget
                             */
                            aw = guii_widget_getparent(aw);    /* Get parent widget */
                            if (aw != NULL) {
                                set_relative_coordinate(&GUI.Touch, /* Set relative touch (for widget) from current touch */
                                    guii_widget_getabsolutex(aw), guii_widget_getabsolutey(aw), 
                                    guii_widget_getwidth(aw), guii_widget_getheight(aw)
                                );
                                /* Reset relative coordinates here! */
                                memcpy(GUI.Touch.x_rel_old, GUI.Touch.x_rel, sizeof(GUI.Touch.x_rel_old));
                                memcpy(GUI.Touch.y_rel_old, GUI.Touch.y_rel, sizeof(GUI.Touch.y_rel_old));
                            }
                        } while (aw);
                        /**
                         * In case touch move widget was detected on another widget,
                         * set this new widget to active from now on
                         */
                        if (aw && aw != GUI.ActiveWidget) {
                            guii_widget_active_set(aw);  /* Set new active widget */
                        }
                    } else {
                        GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;
                        GUI_WIDGET_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                        guii_widget_callback(GUI.ActiveWidget, GUI_WC_TouchStart, &param, &result);    /* New amount of touch elements happened */
                    }
                }
            }
            
            /*
             * Old status: released
             * New status: pressed
             * Action: Touch down on element, find element
             */
            if (GUI.Touch.ts.status && !GUI.TouchOld.ts.status) {
                process_touch(&GUI.Touch, NULL);
                if (GUI.ActiveWidget != GUI.ActiveWidgetPrev) { /* If new active widget is not the same as previous */
                    PT_INIT(&GUI.Touch.pt)          /* Reset thread, otherwise process with double click event */
                }
            }
            
            /**
             * Periodical check for events on active widget
             */
            if (GUI.ActiveWidget) {
                __TouchEvents_Thread(&GUI.Touch, &GUI.TouchOld, 1, &rresult);   /* Call thread for touch process */
                __ProcessAfterTouchEventsThread();  /* Process after event macro */
            }
            
            /*
             * Old status: pressed
             * New status: released
             * Action: Touch up on active element
             */
            if (!GUI.Touch.ts.status && GUI.TouchOld.ts.status) {
                if (GUI.ActiveWidget) {             /* Check if active widget */
                    GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;
                    GUI_WIDGET_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                    guii_widget_callback(GUI.ActiveWidget, GUI_WC_TouchEnd, &param, &result);  /* Process callback function */
                    guii_widget_active_clear();     /* Clear active widget */
                }
            }
            
            memcpy((void *)&GUI.TouchOld, (void *)&GUI.Touch, sizeof(GUI.Touch));   /* Copy current touch to last touch status */
        }
    } else {                                        /* No new touch events, periodically call touch event thread */
        __TouchEvents_Thread(&GUI.Touch, &GUI.TouchOld, 0, &rresult);   /* Call thread for touch process periodically, handle long presses or timeouts */
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
    
    gui_widget_param_t param = {0};
    gui_widget_result_t result = {0};
    
    while (gui_input_keyread(&key.kb)) {            /* Read all keyboard entires */
        if (GUI.FocusedWidget != NULL) {            /* Check if any widget is in focus already */
            GUI_WIDGET_PARAMTYPE_KEYBOARD(&param) = &key;
            GUI_WIDGET_RESULTTYPE_KEYBOARD(&result) = keyCONTINUE;
            guii_widget_callback(GUI.FocusedWidget, GUI_WC_KeyPress, &param, &result);
            if (GUI_WIDGET_RESULTTYPE_KEYBOARD(&result) != keyHANDLED) {
                if (key.kb.keys[0] == GUI_KEY_TAB) {/* Tab key pressed, set next widget as focused */
                    gui_handle_p h = gui_linkedlist_widgetgetnext(NULL, GUI.ActiveWidget);  /* Get next widget if possible */
                    if (h != NULL && guii_widget_ishidden(h)) {/* Ignore hidden widget */
                        h = NULL;
                    }
                    if (h == NULL) {                /* There is no next widget */
                        for (h = gui_linkedlist_widgetgetnext(guii_widget_getparent(GUI.FocusedWidget), NULL); 
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
    
    redraw_widgets(NULL);                           /* Redraw all widgets now on drawing layer */
    drawing->pending = 1;                           /* Set drawing layer as pending */
    
    /* Notify low-level about layer change */
    GUI.lcd.flags |= GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;
    gui_ll_control(&GUI.lcd, GUI_LL_Command_SetActiveLayer, &drawing, &result); /* Set new active layer to low-level driver */
    
    /* Swap active and drawing layers */
    /* New drawings won't be affected until confirmation from low-level is not received */
    GUI.lcd.active_layer = drawing;
    GUI.lcd.drawing_layer = active;
    
    memcpy(&GUI.lcd.active_layer->display, &GUI.Display, sizeof(GUI.Display));  /* Copy clipping data to region */
    
    /* Invalid clipping region(s) for next drawing process */
    GUI.Display.x1 = 0x7FFF;
    GUI.Display.y1 = 0x7FFF;
    GUI.Display.x2 = 0x8000;
    GUI.Display.y2 = 0x8000;
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
        for (i = 0; i < GUI.lcd.layer_count; i++) {
            GUI.lcd.layers[i].x_offset = 0;
            GUI.lcd.layers[i].y_offset = 0;
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
    
    gui_input_init();                               /* Init input devices */
    GUI.Initialized = 1;                            /* GUI is initialized */
    guii_widget_init();                              /* Init widgets */
    
#if GUI_CFG_OS
    /* Create graphical thread */
    if (GUI.OS.thread_id == NULL) {
        gui_sys_thread_create(&GUI.OS.thread_id, "gui_thread", gui_thread, NULL, GUI_SYS_THREAD_SS, GUI_SYS_THREAD_PRIO);
    }
#endif
    
    return guiOK;
}

/**
 * \brief           Processes all drawing operations for GUI
 * \note            When GUI_CFG_OS is set to 0, then user has to call this function in main loop, otherwise it is processed in separated thread by GUI (GUI_CFG_OS != 0)
 * \return          Number of jobs done in current call
 */
int32_t
gui_process(void) {
#if GUI_CFG_OS
    gui_mbox_msg_t* msg;
    uint32_t time;
    uint32_t tmr_cnt = guii_timer_getactivecount(); /* Get number of active timers in system */
    
    time = gui_sys_mbox_get(&GUI.OS.mbox, (void **)&msg, tmr_cnt ? 1 : 0); /* Get value from message queue */
    
    GUI_UNUSED(time);                               /* Unused variable */
    GUI_UNUSED(msg);                                /* Unused variable */
#endif /* GUI_CFG_OS */
   
    __GUI_SYS_PROTECT();                            /* Protect from multiple access */
    
    /*
     * Periodically process everything
     */
    guii_timer_process();                           /* Process all timers */
    guii_widget_executeremove();                    /* Delete widgets */
#if GUI_CFG_USE_TOUCH
    gui_process_touch();                            /* Process touch inputs */
#endif /* GUI_CFG_USE_TOUCH */
#if GUI_CFG_USE_KEYBOARD
    process_keyboard();                             /* Process keyboard inputs */
#endif /* GUI_CFG_USE_KEYBOARD */
    process_redraw();                               /* Redraw widgets */
    
    __GUI_SYS_UNPROTECT();                          /* Release protection */
    return 0;                                       /* Return number of elements updated on GUI */
}

/**
 * \brief           Set callback for global events from GUI
 * \param[in]       cb: Callback function
 * \return          0 on failure, non-zero otherwise
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

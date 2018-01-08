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

GUI_t GUI;

/**
 * \brief           Clip are required to draw widget
 * \param[in]       h: Widget handle
 */
static void
check_disp_clipping(GUI_HANDLE_p h) {
    GUI_iDim_t x, y;
    GUI_Dim_t wi, hi;
    
    /* Set widget itself first */
    x = gui_widget_getabsolutex__(h);
    y = gui_widget_getabsolutey__(h);
    wi = gui_widget_getwidth__(h);
    hi = gui_widget_getheight__(h);
    
    memcpy(&GUI.DisplayTemp, &GUI.Display, sizeof(GUI.DisplayTemp));
    if (GUI.DisplayTemp.X1 == (GUI_iDim_t)0x7FFF)   { GUI.DisplayTemp.X1 = x; }
    if (GUI.DisplayTemp.Y1 == (GUI_iDim_t)0x7FFF)   { GUI.DisplayTemp.Y1 = y; }
    if (GUI.DisplayTemp.X2 == (GUI_iDim_t)0x8000)   { GUI.DisplayTemp.X2 = (GUI_iDim_t)GUI.LCD.Width; }
    if (GUI.DisplayTemp.Y2 == (GUI_iDim_t)0x8000)   { GUI.DisplayTemp.Y2 = (GUI_iDim_t)GUI.LCD.Height; }
    
    if (GUI.DisplayTemp.X1 < x)             { GUI.DisplayTemp.X1 = x; }
    if (GUI.DisplayTemp.X2 > x + wi)        { GUI.DisplayTemp.X2 = x + wi; }
    if (GUI.DisplayTemp.Y1 < y)             { GUI.DisplayTemp.Y1 = y; }
    if (GUI.DisplayTemp.Y2 > y + hi)        { GUI.DisplayTemp.Y2 = y + hi; }
    
    /*
     * Check all parent widgets of current and make sure
     * widget is not drawn over any of parent if scrolling is enabled
     */
    for (; h != NULL; h = __GH(h)->Parent) {
        x = gui_widget_getparentabsolutex__(h);     /* Parent absolute X position for inner widgets */
        y = gui_widget_getparentabsolutey__(h);     /* Parent absolute Y position for inner widgets */
        wi = gui_widget_getparentinnerwidth__(h);   /* Get parent inner width */
        hi = gui_widget_getparentinnerheight__(h);  /* Get parent inner height */
        
        if (GUI.DisplayTemp.X1 < x)         { GUI.DisplayTemp.X1 = x; }
        if (GUI.DisplayTemp.X2 > x + wi)    { GUI.DisplayTemp.X2 = x + wi; }
        if (GUI.DisplayTemp.Y1 < y)         { GUI.DisplayTemp.Y1 = y; }
        if (GUI.DisplayTemp.Y2 > y + hi)    { GUI.DisplayTemp.Y2 = y + hi; }
    }
}

/**
 * \brief           Redraw all widgets of selected parent
 * \param[in]       parent: Parent widget handle to draw widgets on
 * \return          Number of widgets redrawn
 */
static uint32_t
redraw_widgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    uint32_t cnt = 0;
    static uint32_t level = 0;

    /* Go through all elements of parent */
    for (h = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)parent, NULL); h != NULL; 
            h = gui_linkedlist_widgetgetnext(NULL, h)) {
        if (!gui_widget_isvisible__(h)) {           /* Check if visible */
            gui_widget_clrflag__(h, GUI_FLAG_REDRAW);   /* Clear flag to be sure */
            continue;                               /* Ignore hidden elements */
        }
        if (gui_widget_isinsideclippingregion(h)) { /* If widget is inside clipping region */
            /* Draw main widget if required */
            if (gui_widget_getflag__(h, GUI_FLAG_REDRAW)) { /* Check if redraw required */
#if GUI_CFG_USE_TRANSPARENCY
                GUI_Layer_t* layerPrev = GUI.LCD.DrawingLayer;  /* Save drawing layer */
                uint8_t transparent = 0;
#endif /* GUI_CFG_USE_TRANSPARENCY */
                
                gui_widget_clrflag__(h, GUI_FLAG_REDRAW);   /* Clear flag for drawing on widget */
                
                /*
                 * Prepare clipping region for this widget drawing
                 */
                check_disp_clipping(h);             /* Check coordinates for drawings only particular widget */

#if GUI_CFG_USE_TRANSPARENCY
                /*
                 * Check transparency and check if blending function exists to merge layers later together
                 */
                if (gui_widget_istransparent__(h) && GUI.LL.CopyBlend) {
                    GUI_iDim_t width = GUI.DisplayTemp.X2 - GUI.DisplayTemp.X1;
                    GUI_iDim_t height = GUI.DisplayTemp.Y2 - GUI.DisplayTemp.Y1;
                    
                    /**
                     * Try to allocate memory for new virtual layer for temporary usage
                     */
                    GUI.LCD.DrawingLayer = GUI_MEMALLOC(sizeof(*GUI.LCD.DrawingLayer) + (size_t)width * (size_t)height * (size_t)GUI.LCD.PixelSize);
                    
                    if (GUI.LCD.DrawingLayer != NULL) { /* Check if allocation was successful */
                        GUI.LCD.DrawingLayer->Width = width;
                        GUI.LCD.DrawingLayer->Height = height;
                        GUI.LCD.DrawingLayer->OffsetX = GUI.DisplayTemp.X1;
                        GUI.LCD.DrawingLayer->OffsetY = GUI.DisplayTemp.Y1;
                        GUI.LCD.DrawingLayer->StartAddress = (uint32_t)((char *)GUI.LCD.DrawingLayer) + sizeof(*GUI.LCD.DrawingLayer);
                        transparent = 1;            /* We are going to transparent drawing mode */
                    } else {
                        GUI.LCD.DrawingLayer = layerPrev;   /* Reset layer back */
                    }
                }
#endif /* GUI_CFG_USE_TRANSPARENCY */
                
                /*
                 * Draw widget itself normally, don't care on layer offset and size
                 */
                GUI_WIDGET_PARAMTYPE_DISP(&GUI.WidgetParam) = &GUI.DisplayTemp;  /* Set parameter */
                gui_widget_callback__(h, GUI_WC_Draw, &GUI.WidgetParam, &GUI.WidgetResult); /* Draw widget */
                
                /* Check if there are children widgets in this widget */
                if (gui_widget_allowchildren__(h)) {
                    GUI_HANDLE_p tmp;
                    
                    /* Set drawing flag to all widgets  first... */
                    for (tmp = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)h, NULL); tmp != NULL; 
                            tmp = gui_linkedlist_widgetgetnext(NULL, tmp)) {
                        gui_widget_setflag__(tmp, GUI_FLAG_REDRAW); /* Set redraw bit to all children elements */
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
                    GUI.LL.CopyBlend(&GUI.LCD, GUI.LCD.DrawingLayer,
                        (void *)GUI.LCD.DrawingLayer->StartAddress, 
                        (void *)(layerPrev->StartAddress + 
                            GUI.LCD.PixelSize * (layerPrev->Width * (GUI.LCD.DrawingLayer->OffsetY - layerPrev->OffsetY) + (GUI.LCD.DrawingLayer->OffsetX - layerPrev->OffsetX))),
                        gui_widget_gettransparency__(h), 0xFF,
                        GUI.LCD.DrawingLayer->Width, GUI.LCD.DrawingLayer->Height,
                        0, layerPrev->Width - GUI.LCD.DrawingLayer->Width
                    );
                    
                    GUI_MEMFREE(GUI.LCD.DrawingLayer);  /* Free memory for virtual layer */
                    GUI.LCD.DrawingLayer = layerPrev;   /* Reset layer pointer */
                }
#endif /* GUI_CFG_USE_TRANSPARENCY */
                
            /*
             * Check if any widget from children should be redrawn
             */
            } else if (gui_widget_allowchildren__(h)) {
                cnt += redraw_widgets(h);           /* Redraw children widgets */
            }
        }
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
 * Besides, with these 3 basic touch events, it is possible to handle 3 more events, which may not be just used with touch but can also be used with mouse
 *  - Click: Triggered when pressed state is detected and after that released state, still on widget coordinates
 *  - LongClick: Triggered when pressed state is detected for x amount of time (usually 2 seconds)
 *  - DblClick: Triggered when 2 Click events are detected in range of some milliseconds
 *
 * To be able to detect DblClick, 2 Click events must be valid in certain time.
 *
 * Since second events are based on first 3 events for touch, first events must be respected for proper handling second ones.
 *
 * Each widget receives a callback for TouchStart, TouchMove and TouchEnd events.
 * Callback function can return status if event is processed or not. On the returned value 2 things are possible.
 *
 * Consider using button and dropdown. In button, everything what matters is TouchStart and TouchEnd. If TouchMove happens, it is not necessary to handle because only press is needed for detect.
 * In this case, if TouchStart happen and TouchMove too (finger moved inside widget), click event will happen after TouchEnd event, because TouchMove command was ignored and 0 was returned on callback.
 *
 * In dropdown mode it is different because TouchMove is required to detect sliding on items. If TouchMove is detected and 1 is returned from callback,
 * Click event won't be called after TouchEnd event, even if release was inside widget. This is not valid Click because TouchMove was required for something.
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
PT_THREAD(__TouchEvents_Thread(__GUI_TouchData_t* ts, __GUI_TouchData_t* old, uint8_t v, GUI_WC_t* result)) {
    static volatile uint32_t Time;
    static uint8_t i = 0;
    static GUI_iDim_t x[2], y[2];
    
    *result = (GUI_WC_t)0;                          /* Reset widget control variable */          
    
    PT_BEGIN(&ts->pt);                              /* Start thread execution */
    
    memset(x, 0x00, sizeof(x));                     /* Reset X values */
    memset(y, 0x00, sizeof(y));                     /* Reset Y values */
    for (i = 0; i < 2;) {                           /* Allow up to 2 touch presses */
        /**
         * Wait for valid input with pressed state
         */
        PT_WAIT_UNTIL(&ts->pt, v && ts->TS.Status && !old->TS.Status && ts->TS.Count == 1);
        
        Time = ts->TS.Time;                         /* Get start time of this touch */
        x[i] = ts->RelX[0];                         /* Save X value */
        y[i] = ts->RelY[0];                         /* Save Y value */
        
        /*
         * Either wait for released status or timeout
         */
        do {
            PT_YIELD(&ts->pt);                      /* Stop thread for now and wait next call */
            PT_WAIT_UNTIL(&ts->pt, v || (gui_sys_now() - Time) > 2000); /* Wait touch with released state or timeout */
            
            if (v) {                                /* New valid touch entry received, either released or pressed again */
                /*
                 * If touch is still pressed (touch move) and we have active widget and touch move event was not processed
                 * then we can use click events also after touch move (for example, button is that widget) where in
                 * some cases, click event should not be processed after touch move (slider, dropdown, etc)
                 */
                if (ts->TS.Status && GUI.ActiveWidget && !gui_widget_getflag__(GUI.ActiveWidget, GUI_FLAG_TOUCH_MOVE)) {
                    Time = ts->TS.Time;             /* Get start time of this touch */
                    x[i] = ts->RelX[0];             /* Update X value */
                    y[i] = ts->RelY[0];             /* Update Y value */
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
            if (!ts->TS.Status) {                   /* We received released state */
                if (i) {                            /* Try to get second click, check difference for double click */
                    if (GUI_ABS(x[0] - x[1]) > 30 || GUI_ABS(y[0] - y[1]) > 30) {
                        i = 0;                      /* Difference was too big, reset and act like normal click */
                    }
                }
                if (
                    x[0] < 0 || x[0] > ts->WidgetWidth ||
                    y[0] < 0 || y[0] > ts->WidgetHeight ||
                    x[1] < 0 || x[1] > ts->WidgetWidth ||
                    y[1] < 0 || y[1] > ts->WidgetHeight
                ) {
                    PT_EXIT(&ts->pt);               /* Exit thread, invalid coordinate for touch click or double click */
                }
                if (!i) {                           /* On first call, this is click event */
                    *result = GUI_WC_Click;         /* Click event occurred */
                    
                    Time = ts->TS.Time;             /* Save last time */
                    PT_YIELD(&ts->pt);              /* Stop thread for now and wait next call with new touch event */
                    
                    /*
                     * Wait for valid input with pressed state
                     */
                    PT_WAIT_UNTIL(&ts->pt, (v && ts->TS.Status) || (gui_sys_now() - Time) > 300);
                    if ((gui_sys_now() - Time) > 300) { /* Check timeout for new pressed state */
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
 * \param[in,out]   ts: Raw touch data with X and Y position
 * \param[in]       x: Absolute X position of widget
 * \param[in]       y: Absolute Y position of widget
 * \param[in]       width: Width of widget
 * \param[in]       height: Height of widget
 */
static void
set_relative_coordinate(__GUI_TouchData_t* ts, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height) {
    uint8_t i = 0;
    for (i = 0; i < ts->TS.Count; i++) {
        ts->RelOldX[i] = ts->RelX[i];               /* Save old on X value */
        ts->RelOldY[i] = ts->RelY[i];               /* Save old on Y value */
        ts->RelX[i] = ts->TS.X[i] - x;              /* Get relative coordinate on widget */
        ts->RelY[i] = ts->TS.Y[i] - y;              /* Get relative coordinate on widget */
    }
    
    ts->WidgetWidth = width;                        /* Get widget width */
    ts->WidgetHeight = height;                      /* Get widget height */

#if GUI_CFG_TOUCH_MAX_PRESSES > 1
    if (ts->TS.Count == 2) {                        /* 2 points detected */
        ts->DistanceOld = ts->Distance;             /* Save old distance */
        gui_math_distancebetweenxy(ts->RelX[0], ts->RelY[0], ts->RelX[1], ts->RelY[1], &ts->Distance);  /* Calculate distance between 2 points */
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
 * \return          Member of \ref __GUI_TouchStatus_t enumeration about success
 */
static __GUI_TouchStatus_t
process_touch(__GUI_TouchData_t* touch, GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    static uint8_t deep = 0;
    static uint8_t isKeyboard = 0;
    uint8_t dialogOnly = 0;
    __GUI_TouchStatus_t tStat = touchCONTINUE;
    
    /* Check touches if any matches, go reverse on linked list */
    for (h = gui_linkedlist_widgetgetprev((GUI_HANDLE_ROOT_t *)parent, NULL); h != NULL; 
            h = gui_linkedlist_widgetgetprev(NULL, h)) {
        if (gui_widget_ishidden__(h)) {             /* Ignore hidden widget */
            continue;
        }
        
        /*
         * Dialogs are placed as children of main window
         * If level 1 is current and dialog is detected,
         * stop process of other widgets except if widget is inside dialog
         */
        if (deep == 1) {                            /* On base elements list = children of base window element */
            if (gui_widget_isdialogbase__(h)) {     /* We found dialog element */
                dialogOnly = 1;                     /* Check only widgets which are dialog based */
            }
        }
        
        /* When we should only check dialogs and previous element is not dialog anymore */
        if (dialogOnly && !gui_widget_isdialogbase__(h)) {
            break;
        }
        
        /* Check for keyboard mode */
        if (gui_widget_getid__(h) == GUI_ID_KEYBOARD_BASE) {
            isKeyboard = 1;                         /* Set keyboard mode as 1 */
        }
        
        /* Check children elements first */
        if (gui_widget_allowchildren__(h)) {        /* If children widgets are allowed */
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
            if (touch->TS.X[0] >= GUI.DisplayTemp.X1 && touch->TS.X[0] <= GUI.DisplayTemp.X2 && 
                touch->TS.Y[0] >= GUI.DisplayTemp.Y1 && touch->TS.Y[0] <= GUI.DisplayTemp.Y2) {
                set_relative_coordinate(touch,      /* Set relative coordinate */
                    gui_widget_getabsolutex__(h), gui_widget_getabsolutey__(h), 
                    gui_widget_getwidth__(h), gui_widget_getheight__(h)
                ); 
            
                /* Call touch start callback to see if widget accepts touches */
                GUI_WIDGET_PARAMTYPE_TOUCH(&GUI.WidgetParam) = touch;
                gui_widget_callback__(h, GUI_WC_TouchStart, &GUI.WidgetParam, &GUI.WidgetResult);
                tStat = GUI_WIDGET_RESULTTYPE_TOUCH(&GUI.WidgetResult);
                if (tStat == touchCONTINUE) {       /* Check result status */
                    tStat = touchHANDLED;           /* If command is processed, touchCONTINUE can't work */
                }
                
                /*
                 * Move widget down on parent linked list and do the same with all of its parents,
                 * no matter of touch focus or not
                 */
                gui_widget_movedowntree(h);
                
                if (tStat == touchHANDLED) {        /* Touch handled for widget completely */
                    /*
                     * Set active widget and set flag for it
                     * Set focus widget and set flag for it but only do this if widget is not related to keyboard
                     *
                     * This allows us to click keyboard items but not to lose focus on main widget
                     */
                    if (!isKeyboard) {
                        gui_widget_focus_set(h);
                    }
                    gui_widget_active_set(h);
                    
                    /*
                     * Invalidate actual handle object
                     * Already invalidated in __GUI_ACTIVE_SET function
                     */
                    //gui_widget_invalidate__(h);
                } else {                            /* Touch handled with no focus */
                    /*
                     * When touch was handled without focus,
                     * process only clearing currently focused and active widgets and clear them
                     */
                    if (!isKeyboard) {
                        gui_widget_focus_clear();
                    }
                    gui_widget_active_clear();
                }
            }
        }
        
        /* Check for keyboard mode */
        if (gui_widget_getid__(h) == GUI_ID_KEYBOARD_BASE) {
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
        ret = gui_widget_callback__(GUI.ActiveWidget, rresult, &param, NULL);\
        if (rresult == GUI_WC_DblClick && !ret) {   /* If double click was not recorded, proceed with normal click again */\
            gui_widget_callback__(GUI.ActiveWidget, GUI_WC_Click, &param, NULL);    /* Check for normal click now */\
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
    GUI_WIDGET_PARAM_t param = {0};
    GUI_WIDGET_RESULT_t result = {0};
    GUI_WC_t rresult;
    
    if (gui_input_touchavailable()) {               /* Check if any touch available */
        while (gui_input_touchread(&GUI.Touch.TS)) {/* Process all touch events possible */
            if (GUI.ActiveWidget && GUI.Touch.TS.Status) {  /* Check active widget for touch and pressed status */
                set_relative_coordinate(&GUI.Touch, /* Set relative touch (for widget) from current touch */
                    gui_widget_getabsolutex__(GUI.ActiveWidget), gui_widget_getabsolutey__(GUI.ActiveWidget), 
                    gui_widget_getwidth__(GUI.ActiveWidget), gui_widget_getheight__(GUI.ActiveWidget)
                );
            }
            
            /**
             * Old status: pressed
             * New status: pressed
             * Action: Touch move on active element
             */
            if (GUI.Touch.TS.Status && GUI.TouchOld.TS.Status) {
                if (GUI.ActiveWidget) {             /* If active widget exists */
                    if (GUI.Touch.TS.Count == GUI.TouchOld.TS.Count) {
                        GUI_HANDLE_p aw = GUI.ActiveWidget; /* Temporary set active widget */
                        do {
                            uint8_t r;
                            GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;
                            GUI_WIDGET_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                            r = gui_widget_callback__(aw, GUI_WC_TouchMove, &param, &result);   /* The same amount of touch events currently */
                            if (r) {                /* Check if touch move processed */
                                gui_widget_setflag__(aw, GUI_FLAG_TOUCH_MOVE);    /* Touch move has been processed */
                            } else {
                                gui_widget_clrflag__(aw, GUI_FLAG_TOUCH_MOVE);    /* Touch move has not been processed */
                            }
                            if (GUI_WIDGET_RESULTTYPE_TOUCH(&result) != touchCONTINUE) {
                                break;
                            }
                            /**
                             * If widget does not detect touch start, then forward touch start to parent widget.
                             * With this approach, you can achieve slider on parent widget
                             */
                            aw = gui_widget_getparent__(aw);    /* Get parent widget */
                            if (aw) {
                                set_relative_coordinate(&GUI.Touch, /* Set relative touch (for widget) from current touch */
                                    gui_widget_getabsolutex__(aw), gui_widget_getabsolutey__(aw), 
                                    gui_widget_getwidth__(aw), gui_widget_getheight__(aw)
                                );
                                /* Reset relative coordinates here! */
                                memcpy(GUI.Touch.RelOldX, GUI.Touch.RelX, sizeof(GUI.Touch.RelOldX));
                                memcpy(GUI.Touch.RelOldY, GUI.Touch.RelY, sizeof(GUI.Touch.RelOldY));
                            }
                        } while (aw);
                        /**
                         * In case touch move widget was detected on another widget,
                         * set this new widget to active from now on
                         */
                        if (aw && aw != GUI.ActiveWidget) {
                            gui_widget_active_set(aw);  /* Set new active widget */
                        }
                    } else {
                        GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;
                        GUI_WIDGET_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                        gui_widget_callback__(GUI.ActiveWidget, GUI_WC_TouchStart, &param, &result);    /* New amount of touch elements happened */
                    }
                }
            }
            
            /**
             * Old status: released
             * New status: pressed
             * Action: Touch down on element, find element
             */
            if (GUI.Touch.TS.Status && !GUI.TouchOld.TS.Status) {
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
            
            /**
             * Old status: pressed
             * New status: released
             * Action: Touch up on active element
             */
            if (!GUI.Touch.TS.Status && GUI.TouchOld.TS.Status) {
                if (GUI.ActiveWidget) {             /* Check if active widget */
                    GUI_WIDGET_PARAMTYPE_TOUCH(&param) = &GUI.Touch;
                    GUI_WIDGET_RESULTTYPE_TOUCH(&result) = touchCONTINUE;
                    gui_widget_callback__(GUI.ActiveWidget, GUI_WC_TouchEnd, &param, &result);  /* Process callback function */
                    gui_widget_active_clear();      /* Clear active widget */
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
    __GUI_KeyboardData_t key;
    
    GUI_WIDGET_PARAM_t param = {0};
    GUI_WIDGET_RESULT_t result = {0};
    
    while (gui_input_keyread(&key.KB)) {            /* Read all keyboard entires */
        if (GUI.FocusedWidget) {                    /* Check if any widget is in focus already */
            GUI_WIDGET_PARAMTYPE_KEYBOARD(&param) = &key;
            GUI_WIDGET_RESULTTYPE_KEYBOARD(&result) = keyCONTINUE;
            gui_widget_callback__(GUI.FocusedWidget, GUI_WC_KeyPress, &param, &result);
            if (GUI_WIDGET_RESULTTYPE_KEYBOARD(&result) != keyHANDLED) {
                if (key.KB.Keys[0] == GUI_KEY_TAB) {/* Tab key pressed, set next widget as focused */
                    GUI_HANDLE_p h = gui_linkedlist_widgetgetnext(NULL, GUI.ActiveWidget);  /* Get next widget if possible */
                    if (h && gui_widget_ishidden__(h)) {    /* Ignore hidden widget */
                        h = 0;
                    }
                    if (!h) {                       /* There is no next widget */
                        for (h = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)__GH(GUI.FocusedWidget)->Parent, NULL); 
                            h; h = gui_linkedlist_widgetgetnext(NULL, h)) {
                            if (gui_widget_isvisible__(h)) {    /* Check if widget is visible */
                                break;
                            }
                        }
                    }
                    if (h) {                        /* We have next widget */
                        gui_linkedlist_widgetmovetobottom(h); /* Set widget to the down of list */
                        gui_widget_focus_set(h);    /* Set focus to new widget */
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
    GUI_Layer_t* active = GUI.LCD.ActiveLayer;
    GUI_Layer_t* drawing = GUI.LCD.DrawingLayer;
    uint8_t result = 1;
    GUI_Display_t* dispA = &active->Display;
    
    if ((GUI.LCD.Flags & GUI_FLAG_LCD_WAIT_LAYER_CONFIRM) || !(GUI.Flags & GUI_FLAG_REDRAW)) {  /* Check if anything to draw first */
        return;
    }
    
    GUI.Flags &= ~GUI_FLAG_REDRAW;                  /* Clear redraw flag */
    
    /* Copy from currently active layer to drawing layer only changes on layer */
    GUI.LL.Copy(&GUI.LCD, drawing, 
        (void *)(active->StartAddress + GUI.LCD.PixelSize * (dispA->Y1 * active->Width + dispA->X1)),    /* Source address */
        (void *)(drawing->StartAddress + GUI.LCD.PixelSize * (dispA->Y1 * drawing->Width + dispA->X1)),   /* Destination address */
        dispA->X2 - dispA->X1,                      /* Area width */
        dispA->Y2 - dispA->Y1,                      /* Area height */
        active->Width - (dispA->X2 - dispA->X1),    /* Offline source */
        drawing->Width - (dispA->X2 - dispA->X1)    /* Offline destination */
    );
    
    redraw_widgets(NULL);                           /* Redraw all widgets now on drawing layer */
    drawing->Pending = 1;                           /* Set drawing layer as pending */
    
    /* Notify low-level about layer change */
    GUI.LCD.Flags |= GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;
    gui_ll_control(&GUI.LCD, GUI_LL_Command_SetActiveLayer, &drawing, &result); /* Set new active layer to low-level driver */
    
    /* Swap active and drawing layers */
    /* New drawings won't be affected until confirmation from low-level is not received */
    GUI.LCD.ActiveLayer = drawing;
    GUI.LCD.DrawingLayer = active;
    
    memcpy(&GUI.LCD.ActiveLayer->Display, &GUI.Display, sizeof(GUI.Display));   /* Copy clipping data to region */
    
    /* Invalid clipping region(s) for next drawing process */
    GUI.Display.X1 = 0x7FFF;
    GUI.Display.Y1 = 0x7FFF;
    GUI.Display.X2 = 0x8000;
    GUI.Display.Y2 = 0x8000;
}

/**
 * \brief           Default global callback function
 */
static void
gui_default_event_cb(void) {

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
    
    memset((void *)&GUI, 0x00, sizeof(GUI_t));      /* Reset GUI structure */
    
    gui_seteventcallback(NULL);                     /* Set event callback */
    
#if GUI_CFG_OS
    /* Init system */
    gui_sys_init();                                 /* Init low-level system */
    gui_sys_mbox_create(&GUI.OS.mbox, 10);          /* Message box for 10 elements */
#endif /* GUI_CFG_OS */
    
    /* Call LCD low-level function */
    result = 1;
    gui_ll_control(&GUI.LCD, GUI_LL_Command_Init, &GUI.LL, &result);    /* Call low-level initialization */
    GUI.LL.Init(&GUI.LCD);                          /* Call user LCD driver function */
    
    /* Check situation with layers */
    if (GUI.LCD.LayersCount >= 1) {
        size_t i;
        /* Set default values for all layers */
        for (i = 0; i < GUI.LCD.LayersCount; i++) {
            GUI.LCD.Layers[i].OffsetX = 0;
            GUI.LCD.Layers[i].OffsetY = 0;
            GUI.LCD.Layers[i].Width = GUI.LCD.Width;
            GUI.LCD.Layers[i].Height = GUI.LCD.Height;
        }
        GUI.LCD.ActiveLayer = &GUI.LCD.Layers[0];
        GUI.LCD.DrawingLayer = &GUI.LCD.Layers[0];
        GUI.LL.Fill(&GUI.LCD, GUI.LCD.DrawingLayer, (void *)GUI.LCD.DrawingLayer->StartAddress, GUI.LCD.Width, GUI.LCD.Height, 0, GUI_COLOR_LIGHTGRAY);
        if (GUI.LCD.LayersCount > 1) {
            GUI.LCD.DrawingLayer = &GUI.LCD.Layers[1];
        }
    } else {
        return guiERROR;
    }
    
    gui_input_init();                               /* Init input devices */
    GUI.Initialized = 1;                            /* GUI is initialized */
    gui_widget_init();                              /* Init widgets */
    
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
    uint32_t tmr_cnt = gui_timer_getactivecount();  /* Get number of active timers in system */
    
    time = gui_sys_mbox_get(&GUI.OS.mbox, (void **)&msg, tmr_cnt ? 10 : 0); /* Get value from message queue */
    GUI_UNUSED(time);                               /* Unused variable */
    
    __GUI_SYS_PROTECT();                            /* Protect from multiple access */
#endif /* GUI_CFG_OS */
   
    /*
     * Periodically process everything
     */
#if GUI_CFG_USE_TOUCH
    gui_process_touch();                            /* Process touch inputs */
#endif /* GUI_CFG_USE_TOUCH */
#if GUI_CFG_USE_KEYBOARD
    process_keyboard();                             /* Process keyboard inputs */
#endif /* GUI_CFG_USE_KEYBOARD */
    gui_timer_process();                            /* Process all timers */
    gui_widget_executeremove();                     /* Delete widgets */
    process_redraw();                               /* Redraw widgets */
    
#if GUI_CFG_OS
    __GUI_SYS_UNPROTECT();                          /* Release protection */
#endif /* GUI_CFG_OS */
    
    return 0;                                       /* Return number of elements updated on GUI */
}

/**
 * \brief           Set callback for global events from GUI
 * \param[in]       cb: Callback function
 * \return          0 on failure, non-zero otherwise
 */
uint8_t
gui_seteventcallback(GUI_EventCallback_t cb) {
    if (cb) {
        GUI.EventCb = cb;                           /*!< Set user callback */
    } else {
        GUI.EventCb = gui_default_event_cb;         /*!< Set default callback */
    }
    return 1;
}

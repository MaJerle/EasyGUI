/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui.h"
#include "gui_system.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
GUI_t GUI;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
/* Clip widget before draw/touch operation */
static void
__CheckDispClipping(GUI_HANDLE_p h) {
    GUI_iDim_t x, y;
    GUI_Dim_t wi, hi;
    
    /* Set widget itself first */
    x = __GUI_WIDGET_GetAbsoluteX(h);
    y = __GUI_WIDGET_GetAbsoluteY(h);
    wi = __GUI_WIDGET_GetWidth(h);
    hi = __GUI_WIDGET_GetHeight(h);
    
    memcpy(&GUI.DisplayTemp, &GUI.Display, sizeof(GUI.DisplayTemp));
    if (GUI.DisplayTemp.X1 == (GUI_iDim_t)0x7FFF)   { GUI.DisplayTemp.X1 = x; }
    if (GUI.DisplayTemp.Y1 == (GUI_iDim_t)0x7FFF)   { GUI.DisplayTemp.Y1 = y; }
    if (GUI.DisplayTemp.X2 == (GUI_iDim_t)0x8000)   { GUI.DisplayTemp.X2 = (GUI_iDim_t)GUI.LCD.Width; }
    if (GUI.DisplayTemp.Y2 == (GUI_iDim_t)0x8000)   { GUI.DisplayTemp.Y2 = (GUI_iDim_t)GUI.LCD.Height; }
    
    if (GUI.DisplayTemp.X1 < x)             { GUI.DisplayTemp.X1 = x; }
    if (GUI.DisplayTemp.X2 > x + wi)        { GUI.DisplayTemp.X2 = x + wi; }
    if (GUI.DisplayTemp.Y1 < y)             { GUI.DisplayTemp.Y1 = y; }
    if (GUI.DisplayTemp.Y2 > y + hi)        { GUI.DisplayTemp.Y2 = y + hi; }
    
    /* Check that widget is not drawn over any of parent widgets because of scrolling */
    for (; h; h = __GH(h)->Parent) {
        x = __GUI_WIDGET_GetParentAbsoluteX(h);     /* Parent absolute X position for inner widgets */
        y = __GUI_WIDGET_GetParentAbsoluteY(h);     /* Parent absolute Y position for inner widgets */
        wi = __GUI_WIDGET_GetParentInnerWidth(h);   /* Get parent inner width */
        hi = __GUI_WIDGET_GetParentInnerHeight(h);  /* Get parent inner height */
        
        if (GUI.DisplayTemp.X1 < x)         { GUI.DisplayTemp.X1 = x; }
        if (GUI.DisplayTemp.X2 > x + wi)    { GUI.DisplayTemp.X2 = x + wi; }
        if (GUI.DisplayTemp.Y1 < y)         { GUI.DisplayTemp.Y1 = y; }
        if (GUI.DisplayTemp.Y2 > y + hi)    { GUI.DisplayTemp.Y2 = y + hi; }
    }
}

/* Redraw widgets */
static
uint32_t __RedrawWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    uint32_t cnt = 0;
    static uint32_t level = 0;

    /* Go through all elements of parent */
    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, NULL); h; 
            h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
        if (!__GUI_WIDGET_IsVisible(h)) {           /* Check if visible */
            __GUI_WIDGET_ClrFlag(h, GUI_FLAG_REDRAW);   /* Clear flag to be sure */
            continue;                               /* Ignore hidden elements */
        }
        if (__GUI_WIDGET_IsInsideClippingRegion(h)) {   /* If widget is inside clipping region */
            /* Draw main widget if required */
            if (__GUI_WIDGET_GetFlag(h, GUI_FLAG_REDRAW)) { /* Check if redraw required */
                GUI_Layer_t* layerPrev = GUI.LCD.DrawingLayer;  /* Save drawing layer */
                uint8_t transparent = 0;
                
                __GUI_WIDGET_ClrFlag(h, GUI_FLAG_REDRAW);   /* Clear flag for drawing on widget */
                
                /**
                 * Prepare clipping region for this widget drawing
                 */
                __CheckDispClipping(h);             /* Check coordinates for drawings only particular widget */
                
                /**
                 * Check transparency and check if blending function exists to merge layers later together
                 */
                if (__GUI_WIDGET_IsTransparent(h) && GUI.LL.CopyBlend) {
                    GUI_iDim_t width = GUI.DisplayTemp.X2 - GUI.DisplayTemp.X1;
                    GUI_iDim_t height = GUI.DisplayTemp.Y2 - GUI.DisplayTemp.Y1;
                    
                    /**
                     * Try to allocate memory for new virtual layer for temporary usage
                     */
                    GUI.LCD.DrawingLayer = __GUI_MEMALLOC(sizeof(*GUI.LCD.DrawingLayer) + (size_t)width * (size_t)height * (size_t)GUI.LCD.PixelSize);
                    
                    if (GUI.LCD.DrawingLayer) {     /* Check if allocation was successful */
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
                
                /**
                 * Draw widget itself normally, don't care on layer offset and size
                 */
                __GUI_WIDGET_Callback(h, GUI_WC_Draw, &GUI.DisplayTemp, NULL);  /* Draw widget */
                
                /* Check if there are children widgets in this widget */
                if (__GUI_WIDGET_AllowChildren(h)) {
                    GUI_HANDLE_p tmp;
                    
                    /* Set drawing flag to all widgets  first... */
                    for (tmp = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)h, NULL); tmp; 
                            tmp = __GUI_LINKEDLIST_WidgetGetNext(NULL, tmp)) {
                        __GUI_WIDGET_SetFlag(tmp, GUI_FLAG_REDRAW); /* Set redraw bit to all children elements */
                    }
                    /* ...now call function for actual redrawing process */
                    level++;
                    cnt += __RedrawWidgets(h);      /* Redraw children widgets */
                    level--;
                }
                
                /**
                 * If transparent mode is used on widget, copy content back
                 */
                if (transparent) {                  /* If we were in transparent mode */
                    /* Copy layers with blending */
                    GUI.LL.CopyBlend(&GUI.LCD, GUI.LCD.DrawingLayer,
                        (void *)GUI.LCD.DrawingLayer->StartAddress, 
                        (void *)(layerPrev->StartAddress + 
                            GUI.LCD.PixelSize * (layerPrev->Width * (GUI.LCD.DrawingLayer->OffsetY - layerPrev->OffsetY) + (GUI.LCD.DrawingLayer->OffsetX - layerPrev->OffsetX))),
                        __GUI_WIDGET_GetTransparency(h), 0xFF,
                        GUI.LCD.DrawingLayer->Width, GUI.LCD.DrawingLayer->Height,
                        0, layerPrev->Width - GUI.LCD.DrawingLayer->Width
                    );
                    
                    __GUI_MEMFREE(GUI.LCD.DrawingLayer);    /* Free memory for virtual layer */
                    GUI.LCD.DrawingLayer = layerPrev;   /* Reset layer pointer */
                }
                
            /**
             * Check if any widget from children should be redrawn
             */
            } else if (__GUI_WIDGET_AllowChildren(h)) {
                cnt += __RedrawWidgets(h);          /* Redraw children widgets */
            }
        }
    }
    return cnt;                                     /* Return number of redrawn objects */
}

#if GUI_USE_TOUCH
/**
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
        
        /**
         * Either wait for released status or timeout
         */
        do {
            PT_YIELD(&ts->pt);                      /* Stop thread for now and wait next call */
            PT_WAIT_UNTIL(&ts->pt, v || (gui_sys_now() - Time) > 2000); /* Wait touch with released state or timeout */
            
            if (v) {                                /* New valid touch entry received, either released or pressed again */
                /**
                 * If touch is still pressed (touch move) and we have active widget and touch move event was not processed
                 * then we can use click events also after touch move (for example, button is that widget) where in
                 * some cases, click event should not be processed after touch move (slider, dropdown, etc)
                 */
                if (ts->TS.Status && GUI.ActiveWidget && !__GUI_WIDGET_GetFlag(GUI.ActiveWidget, GUI_FLAG_TOUCH_MOVE)) {
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
        
        /**
         * Check what was the reason for thread to continue
         */
        if (v) {                                    /* New touch event occurred */
            if (!ts->TS.Status) {                   /* We received released state */
                if (i) {                            /* Try to get second click, check difference for double click */
                    if (__GUI_ABS(x[0] - x[1]) > 30 || __GUI_ABS(y[0] - y[1]) > 30) {
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
                    
                    /**
                     * Wait for valid input with pressed state
                     */
                    PT_WAIT_UNTIL(&ts->pt, (v && ts->TS.Status) || (gui_sys_now() - Time) > 300);
                    if ((gui_sys_now() - Time) > 200) { /* Check timeout for new pressed state */
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

void __SetRelativeCoordinate(__GUI_TouchData_t* ts, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height) {
    uint8_t i = 0;
    for (i = 0; i < ts->TS.Count; i++) {
        ts->RelX[i] = ts->TS.X[i] - x;              /* Get relative coordinate on widget */
        ts->RelY[i] = ts->TS.Y[i] - y;              /* Get relative coordinate on widget */
    }
    
    ts->WidgetWidth = width;                        /* Get widget width */
    ts->WidgetHeight = height;                      /* Get widget height */

#if GUI_TOUCH_MAX_PRESSES > 1
    if (ts->TS.Count == 2) {                        /* 2 points detected */
        ts->DistanceOld = ts->Distance;             /* Save old distance */
        GUI_MATH_DistanceBetweenXY(ts->RelX[0], ts->RelY[0], ts->RelX[1], ts->RelY[1], &ts->Distance);  /* Calculate distance between 2 points */
    }
#endif /* GUI_TOUCH_MAX_PRESSES > 1 */
}

__GUI_TouchStatus_t __ProcessTouch(__GUI_TouchData_t* touch, GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    static uint8_t deep = 0;
    static uint8_t isKeyboard = 0;
    uint8_t dialogOnly = 0;
    __GUI_TouchStatus_t tStat = touchCONTINUE;
    
    /* Check touches if any matches, go reverse on linked list */
    for (h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, h)) {
        if (__GUI_WIDGET_IsHidden(h)) {             /* Ignore hidden widget */
            continue;
        }
        
        if (deep == 1) {                            /* On base elements list = children of base window element */
            if (__GUI_WIDGET_IsDialogBase(h)) {     /* We found dialog element */
                dialogOnly = 1;                     /* Check only widgets which are dialog based */
            }
        }
        
        /* When we should only check dialogs and previous element is not dialog anymore */
        if (dialogOnly && !__GUI_WIDGET_IsDialogBase(h)) {
            break;
        }
        
        /* Check for keyboard mode */
        if (__GUI_WIDGET_GetId(h) == GUI_ID_KEYBOARD_BASE) {
            isKeyboard = 1;                         /* Set keyboard mode as 1 */
        }
        
        /* Check children elements first */
        if (__GUI_WIDGET_AllowChildren(h)) {        /* If children widgets are allowed */
            deep++;                                 /* Go deeper in level */
            tStat = __ProcessTouch(touch, h);       /* Process touch on widget elements first */
            deep--;                                 /* Go back to normal level */
        }
        
        /**
         * Children widgets were not detected
         */
        if (tStat == touchCONTINUE) {               /* Do we still have to check this widget? */
            __CheckDispClipping(h);                 /* Check display region where widget is placed */
        
            /* Check if widget is in touch area */
            if (touch->TS.X[0] >= GUI.DisplayTemp.X1 && touch->TS.X[0] <= GUI.DisplayTemp.X2 && 
                touch->TS.Y[0] >= GUI.DisplayTemp.Y1 && touch->TS.Y[0] <= GUI.DisplayTemp.Y2) {
                __SetRelativeCoordinate(touch,      /* Set relative coordinate */
                    __GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h), 
                    __GUI_WIDGET_GetWidth(h), __GUI_WIDGET_GetHeight(h)
                ); 
            
                __GUI_WIDGET_Callback(h, GUI_WC_TouchStart, touch, &tStat);
                if (tStat == touchCONTINUE) {       /* Check result status */
                    tStat = touchHANDLED;           /* If command is processed, touchCONTINUE can't work */
                }
                /**
                 * Move widget down on parent linked list and do the same with all of its parents,
                 * no matter of touch focus or not
                 */
                __GUI_WIDGET_MoveDownTree(h);
                
                if (tStat == touchHANDLED) {        /* Touch handled for widget completelly */
                    /**
                     * Set active widget and set flag for it
                     * Set focus widget and set flag for it but only do this if widget is not related to keyboard
                     *
                     * This allows us to click keyboard items but not to lose focus on main widget
                     */
                    if (!isKeyboard) {
                        __GUI_WIDGET_FOCUS_SET(h);
                    }
                    __GUI_WIDGET_ACTIVE_SET(h);
                    
                    /**
                     * Invalidate actual handle object
                     * Already invalidated in __GUI_ACTIVE_SET function
                     */
                    //__GUI_WIDGET_Invalidate(h);
                } else {                            /* Touch handled with no focus */
                    /**
                     * When touch was handled without focus,
                     * process only clearing currently focused and active widgets and clear them
                     */
                    if (!isKeyboard) {
                        __GUI_WIDGET_FOCUS_CLEAR();
                    }
                    __GUI_WIDGET_ACTIVE_CLEAR();
                }
            }
        }
        
        /* Check for keyboard mode */
        if (__GUI_WIDGET_GetId(h) == GUI_ID_KEYBOARD_BASE) {
            isKeyboard = 0;                         /* Set keyboard mode as 1 */
        }
        
        if (tStat != touchCONTINUE) {               /* Return status if necessary */
            return tStat;
        }
    }
    return touchCONTINUE;                           /* Try with another widget */
}

/**
 * \brief           Process touch inputs
 */
static void
__GUI_Process_Touch(void) {
    __GUI_TouchStatus_t tStat;
    GUI_WC_t result;
    
    if (__GUI_INPUT_TouchAvailable()) {             /* Check if any touch available */
        while (__GUI_INPUT_TouchRead(&GUI.Touch.TS)) {  /* Process all touch events possible */
            if (GUI.ActiveWidget && GUI.Touch.TS.Status) {  /* Check active widget for touch and pressed status */
                __SetRelativeCoordinate(&GUI.Touch, /* Set relative touch (for widget) from current touch */
                    __GUI_WIDGET_GetAbsoluteX(GUI.ActiveWidget), __GUI_WIDGET_GetAbsoluteY(GUI.ActiveWidget), 
                    __GUI_WIDGET_GetWidth(GUI.ActiveWidget), __GUI_WIDGET_GetHeight(GUI.ActiveWidget)
                );
            }
            
            if (GUI.Touch.TS.Status && GUI.TouchOld.TS.Status) {
                /**
                 * Old status: pressed
                 * New status: pressed
                 * Action: Touch move on active element
                 */
                if (GUI.ActiveWidget) {             /* If active widget exists */
                    if (GUI.Touch.TS.Count == GUI.TouchOld.TS.Count) {
                        uint8_t result = __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchMove, &GUI.Touch, &tStat); /* The same amount of touch events currently */
                        if (result) {               /* Check if touch move processed */
                            __GUI_WIDGET_SetFlag(GUI.ActiveWidget, GUI_FLAG_TOUCH_MOVE);    /* Touch move has been processed */
                        } else {
                            __GUI_WIDGET_ClrFlag(GUI.ActiveWidget, GUI_FLAG_TOUCH_MOVE);    /* Touch move has not been processed */
                        }
                    } else {
                        __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchStart, &GUI.Touch, &tStat); /* New amount of touch elements happened */
                    }
                }
            } else if (GUI.Touch.TS.Status && !GUI.TouchOld.TS.Status) {
                /**
                 * Old status: released
                 * New status: pressed
                 * Action: Touch down on element, find element
                 */
                __ProcessTouch(&GUI.Touch, NULL);
                if (GUI.ActiveWidget != GUI.ActiveWidgetPrev) { /* If new active widget is not the same as previous */
                    PT_INIT(&GUI.Touch.pt)          /* Reset thread, otherwise process with double click event */
                }
            }
            
            if (GUI.ActiveWidget) {
                /**
                 * Periodical check for events
                 */
                __TouchEvents_Thread(&GUI.Touch, &GUI.TouchOld, 1, &result);    /* Call thread for touch process */
                if (result != 0) {                  /* Valid event occurred */
                    __GUI_WIDGET_Callback(GUI.ActiveWidget, result, &GUI.Touch.TS, NULL);
                }
            }
            
            if (!GUI.Touch.TS.Status && GUI.TouchOld.TS.Status) {
                /**
                 * Old status: pressed
                 * New status: released
                 * Action: Touch up on active element
                 */
                if (GUI.ActiveWidget) {             /* Check if active widget */
                    __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchEnd, &GUI.Touch, &tStat);   /* Process callback function */
                    __GUI_WIDGET_ACTIVE_CLEAR();    /* Clear active widget */
                }
            }
            
            memcpy((void *)&GUI.TouchOld, (void *)&GUI.Touch, sizeof(GUI.Touch));   /* Copy current touch to last touch status */
        }
    } else {                                        /* No new touch events, periodically call touch event thread */
        __TouchEvents_Thread(&GUI.Touch, &GUI.TouchOld, 0, &result);    /* Call thread for touch process periodically, handle long presses or timeouts */
        if (result != 0) {                  /* Valid event occurred */
            __GUI_WIDGET_Callback(GUI.ActiveWidget, result, &GUI.Touch, NULL);
        }
    }
}
#endif /* GUI_USE_TOUCH */

#if GUI_USE_KEYBOARD
/**
 * \brief           Process reads of keyboard
 */
static void
__GUI_Process_Keyboard(void) {
    __GUI_KeyboardData_t key;
    __GUI_KeyboardStatus_t kStat;
    
    while (__GUI_INPUT_KeyRead(&key.KB)) {          /* Read all keyboard entires */
        if (GUI.FocusedWidget) {                    /* Check if any widget is in focus already */
            kStat = keyCONTINUE;
            __GUI_WIDGET_Callback(GUI.FocusedWidget, GUI_WC_KeyPress, &key, &kStat);
            if (kStat != keyHANDLED) {
                if (key.KB.Keys[0] == GUI_KEY_TAB) {/* Tab key pressed, set next widget as focused */
                    GUI_HANDLE_p h = __GUI_LINKEDLIST_WidgetGetNext(NULL, GUI.FocusedWidget);   /* Get next widget if possible */
                    if (h && __GUI_WIDGET_IsHidden(h)) {    /* Ignore hidden widget */
                        h = 0;
                    }
                    if (!h) {                       /* There is no next widget */
                        for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)__GH(GUI.FocusedWidget)->Parent, NULL); 
                            h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
                            if (__GUI_WIDGET_IsVisible(h)) {    /* Check if widget is visible */
                                break;
                            }
                        }
                    }
                    if (h) {                        /* We have next widget */
                        __GUI_LINKEDLIST_WidgetMoveToBottom(h); /* Set widget to the down of list */
                        __GUI_WIDGET_FOCUS_SET(h);  /* Set focus to new widget */
                    }
                }
            }
        }
    }
}
#endif

#include "tm_stm32_general.h"
/**
 * \brief           Process redraw of all widgets
 */
static void
__GUI_Process_Redraw(void) {
    if (!(GUI.LCD.Flags & GUI_FLAG_LCD_WAIT_LAYER_CONFIRM) && (GUI.Flags & GUI_FLAG_REDRAW)) {  /* Check if anything to draw first */
        uint32_t time;
        GUI_Layer_t* active = GUI.LCD.ActiveLayer;
        GUI_Layer_t* drawing = GUI.LCD.DrawingLayer;
        uint8_t result = 1;
        GUI_Display_t* dispA = &active->Display;
        
        GUI.Flags &= ~GUI_FLAG_REDRAW;              /* Clear redraw flag */
        
        time = TM_GENERAL_DWTCounterGetValue();
        
        /* Copy from currently active layer to drawing layer only changes on layer */
        GUI.LL.Copy(&GUI.LCD, drawing, 
            (void *)(active->StartAddress + GUI.LCD.PixelSize * (dispA->Y1 * active->Width + dispA->X1)),    /* Source address */
            (void *)(drawing->StartAddress + GUI.LCD.PixelSize * (dispA->Y1 * drawing->Width + dispA->X1)),   /* Destination address */
            dispA->X2 - dispA->X1,                  /* Area width */
            dispA->Y2 - dispA->Y1,                  /* Area height */
            active->Width - (dispA->X2 - dispA->X1),    /* Offline source */
            drawing->Width - (dispA->X2 - dispA->X1)    /* Offline destination */
        );
            
        /* Actually draw new screen based on setup */
        __RedrawWidgets(NULL);                      /* Redraw all widgets now */
            
        /* Get cycles for drawing */
        time = TM_GENERAL_DWTCounterGetValue() - time;
        
        /* Set drawing layer as pending */
        drawing->Pending = 1;
        
        /* Notify low-level about layer change */
        GUI.LCD.Flags |= GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;
        GUI_LL_Control(&GUI.LCD, GUI_LL_Command_SetActiveLayer, &drawing, &result); /* Set new active layer to low-level driver */
        
        /* Swap active and drawing layers */
        /* New drawings won't be affected until confirmation from low-level is not received */
        GUI.LCD.ActiveLayer = drawing;
        GUI.LCD.DrawingLayer = active;
        
        /* Copy clipping data to region */
        memcpy(&GUI.LCD.ActiveLayer->Display, &GUI.Display, sizeof(GUI.Display));
        
        /* Invalid clipping region(s) for next drawing process */
        GUI.Display.X1 = 0x7FFF;
        GUI.Display.Y1 = 0x7FFF;
        GUI.Display.X2 = 0x8000;
        GUI.Display.Y2 = 0x8000;
        
        __GUI_UNUSED(time);                         /* Prevent compiler warnings */
    }
}

#if GUI_OS
/**
 * \brief           GUI main thread for RTOS
 * \param[in]       *argument: Pointer to user specific argument
 */
static void
gui_thread(void * const argument) {
    
    while (1) {
        GUI_Process();                              /* Process graphical update */
    }
}
#endif

/******************************************************************************/
/******************************************************************************/
/***                              Protothreads                               **/
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
#if GUI_OS
char buffer[1024];
#endif /* GUI_OS */
GUI_Result_t GUI_Init(void) {
    uint8_t result;
    
    memset((void *)&GUI, 0x00, sizeof(GUI_t));      /* Reset GUI structure */
    
#if GUI_OS
    /* Init system */
    gui_sys_init();                                 /* Init low-level system */
    gui_sys_mbox_create(&GUI.OS.mbox, 10);          /* Message box for 10 elements */
    GUI.OS.thread_id = gui_sys_thread_create("gui_thread", gui_thread, NULL, SYS_THREAD_SS, SYS_THREAD_PRIO);
#endif /* GUI_OS */
    
    /* Call LCD low-level function */
    result = 1;
    GUI_LL_Control(&GUI.LCD, GUI_LL_Command_Init, &GUI.LL, &result);    /* Call low-level initialization */
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
        GUI.LL.Fill(&GUI.LCD, GUI.LCD.DrawingLayer, (void *)GUI.LCD.DrawingLayer->StartAddress, GUI.LCD.Width, GUI.LCD.Height, 0, 0xFFFFFFFF);
        if (GUI.LCD.LayersCount > 1) {
            GUI.LCD.DrawingLayer = &GUI.LCD.Layers[1];
        }
    } else {
        return guiERROR;
    }
    
    /* Init input devices */
    __GUI_INPUT_Init();
    
    /* GUI is initialized */
    GUI.Initialized = 1;
    
    /* Init widgets */
    __GUI_WIDGET_Init();
    
    return guiOK;
}
int32_t GUI_Process(void) {
#if GUI_OS
    gui_mbox_msg_t* msg;
    uint32_t time;
    uint32_t tmr_cnt = __GUI_TIMER_GetActiveCount();    /* Get number of timers in system */
    
    time = gui_sys_mbox_get(&GUI.OS.mbox, (void **)&msg, tmr_cnt > 0 ? 5 : 50); /* Get value from message queue */
    if (time != SYS_TIMEOUT) {
        __GUI_DEBUG("mt: %d\r\n", (uint32_t)msg->type);
        gui_sys_protect();                          /* Lock protection */
        switch (msg->type) {                        /* Check about event */
            case GUI_SYS_MBOX_TYPE_TOUCH:
                __GUI_Process_Touch();              /* Process touch event */
                break;
            case GUI_SYS_MBOX_TYPE_KEYBOARD:
                __GUI_Process_Keyboard();           /* Process keyboard event */
                break;
            case GUI_SYS_MBOX_TYPE_REMOVE:
                __GUI_WIDGET_ExecuteRemove();       /* Process remove event */
                break;
            default: 
                break;
        }
        gui_sys_unprotect();                        /* Release protection */
    }
    __GUI_TIMER_Process();                          /* Process timers only */
    __GUI_Process_Redraw();                         /* Redraw widgets */
    
#else
#if GUI_USE_TOUCH
    __GUI_Process_Touch();                          /* Process touch inputs */
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    __GUI_Process_Keyboard();                       /* Process keyboard inputs */
#endif /* GUI_USE_KEYBOARD */
    __GUI_TIMER_Process();                          /* Process all timers */
    __GUI_WIDGET_ExecuteRemove();                   /* Delete widgets */
    __GUI_Process_Redraw();                         /* Redraw widgets */
#endif /* GUI_OS */
    
    return 0;                                       /* Return number of elements updated on GUI */
}

void GUI_LCD_ConfirmActiveLayer(GUI_Byte layer_num) {
    if ((GUI.LCD.Flags & GUI_FLAG_LCD_WAIT_LAYER_CONFIRM)) {/* If we have anything pending */
        GUI.LCD.Layers[layer_num].Pending = 0;
        GUI.LCD.Flags &= ~GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;  /* Clear flag */
    }
}

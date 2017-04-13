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
/* Gets number of widgets waiting for redraw */
uint32_t __GetNumberOfPendingWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    uint32_t cnt = 0;
    
    if (parent && __GH(parent)->Flags & GUI_FLAG_REDRAW) {  /* Check for specific widget */
        return 1;                                   /* We have object to redraw */
    }
    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
        if (__GUI_WIDGET_AllowChildren(h)) {        /* If this widget has children elements */
            cnt += __GetNumberOfPendingWidgets(h);  /* Redraw this widget and all its children if required */
        } else if (__GH(h)->Flags & GUI_FLAG_REDRAW) {  /* Check if we need redraw */
            cnt++;                                  /* Increase number of elements to redraw */
        }
    }
    return cnt;
}

/* Clip widget before draw/touch operation */
static
void __CheckDispClipping(GUI_HANDLE_p h) {
    GUI_iDim_t x, y;
    GUI_Dim_t wi, hi;
    
    /* Set widget itself first */
    x = __GUI_WIDGET_GetAbsoluteX(h);
    y = __GUI_WIDGET_GetAbsoluteY(h);
    wi = __GUI_WIDGET_GetWidth(h);
    hi = __GUI_WIDGET_GetHeight(h);
    
    memcpy(&GUI.DisplayTemp, &GUI.Display, sizeof(GUI.DisplayTemp));
    if (GUI.DisplayTemp.X1 == (GUI_iDim_t)0x7FFF)   { GUI.DisplayTemp.X1 = 0; }
    if (GUI.DisplayTemp.Y1 == (GUI_iDim_t)0x7FFF)   { GUI.DisplayTemp.Y1 = 0; }
    if (GUI.DisplayTemp.X2 == (GUI_iDim_t)0x8000)   { GUI.DisplayTemp.X2 = (GUI_iDim_t)GUI.LCD.Width; }
    if (GUI.DisplayTemp.Y2 == (GUI_iDim_t)0x8000)   { GUI.DisplayTemp.Y2 = (GUI_iDim_t)GUI.LCD.Height; }
    
    if (GUI.DisplayTemp.X1 < x)         { GUI.DisplayTemp.X1 = x; }
    if (GUI.DisplayTemp.X2 > x + wi)    { GUI.DisplayTemp.X2 = x + wi; }
    if (GUI.DisplayTemp.Y1 < y)         { GUI.DisplayTemp.Y1 = y; }
    if (GUI.DisplayTemp.Y2 > y + hi)    { GUI.DisplayTemp.Y2 = y + hi; }
    
#if !GUI_WIDGET_INSIDE_PARENT
    /* Check that widget is not drawn over any of parent widgets because of scrolling */
    for (; h; h = __GH(h)->Parent) {
        x = __GUI_WIDGET_GetParentAbsoluteX(h);         /* Parent absolute X position for inner widgets */
        y = __GUI_WIDGET_GetParentAbsoluteY(h);         /* Parent absolute Y position for inner widgets */
        wi = __GUI_WIDGET_GetParentInnerWidth(h);       /* Get parent inner width */
        hi = __GUI_WIDGET_GetParentInnerHeight(h);      /* Get parent inner height */
        
        if (GUI.DisplayTemp.X1 < x)         { GUI.DisplayTemp.X1 = x; }
        if (GUI.DisplayTemp.X2 > x + wi)    { GUI.DisplayTemp.X2 = x + wi; }
        if (GUI.DisplayTemp.Y1 < y)         { GUI.DisplayTemp.Y1 = y; }
        if (GUI.DisplayTemp.Y2 > y + hi)    { GUI.DisplayTemp.Y2 = y + hi; }
    }
#endif /* !GUI_WIDGET_INSIDE_PARENT */
}

uint32_t __RedrawWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    uint32_t cnt = 0;
    
    if (parent && (__GH(parent)->Flags & GUI_FLAG_REDRAW)) {  /* Check if parent window should redraw operation */
        if (!__GUI_WIDGET_IsVisible(parent)) {      /* Check if visible */
            return 0;                               /* Stop execution if parent is hidden */
        }
        __GH(parent)->Flags &= ~GUI_FLAG_REDRAW;    /* Clear flag */
        for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
            __GH(h)->Flags |= GUI_FLAG_REDRAW;      /* Set redraw bit to all children elements */
        }
        if (__GUI_WIDGET_IsInsideClippingRegion(parent)) {  /* If draw function is set and drawing is inside clipping region */
            __CheckDispClipping(parent);            /* Check coordinates for drawings */
            __GUI_WIDGET_Callback(parent, GUI_WC_Draw, &GUI.DisplayTemp, NULL);
        }
    }

    /* Go through all elements of parent */
    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
        if (!__GUI_WIDGET_IsVisible(h)) {           /* Check if visible */
            continue;                               /* Ignore hidden elements */
        }
        if (__GUI_WIDGET_AllowChildren(h)) {        /* If this widget allows children widgets */
            cnt += __RedrawWidgets(h);              /* Redraw this widget and all its children if required */
        } else {
            if (__GH(h)->Flags & GUI_FLAG_REDRAW) { /* Check if redraw required */
                __GH(h)->Flags &= ~GUI_FLAG_REDRAW; /* Clear flag */
                if (__GUI_WIDGET_IsInsideClippingRegion(h)) {   /* If draw function is set and drawing is inside clipping region */
                    __CheckDispClipping(h);         /* Check coordinates for drawings */
                    __GUI_WIDGET_Callback(h, GUI_WC_Draw, &GUI.DisplayTemp, NULL);  /* Draw widget */
                }
                cnt++;
            }
        }
    }
    
    return cnt;                                     /* Return number of redrawn objects */
}

#if GUI_USE_TOUCH
PT_THREAD(__TouchEvents_Thread(__GUI_TouchData_t* ts, __GUI_TouchData_t* old, uint8_t v, GUI_WC_t* result)) {
    static volatile uint32_t Time;
    static uint8_t i = 0;
    static GUI_iDim_t x[2], y[2];
    
    *result = (GUI_WC_t)0;
    
    PT_BEGIN(&ts->pt);
    
    for (i = 0; i < 2;) {
        /**
         * Wait for valid input with pressed state
         */
        PT_WAIT_UNTIL(&ts->pt, v && ts->TS.Status && !old->TS.Status && ts->TS.Count == 1);
        
        Time = ts->TS.Time;                             /* Get start time of this touch */
        x[i] = ts->RelX[0];                             /* Save X value */
        y[i] = ts->RelY[0];                             /* Save Y value */
        PT_YIELD(&ts->pt);                              /* Stop thread for now and wait next call */
            
        /**
         * Either wait for released status or timeout
         */
        PT_WAIT_UNTIL(&ts->pt, v || (GUI.Time - Time) > 2000); /* Wait touch with released state */
        
        /**
         * Check what was the reason for thread to continue
         */
        if (v) {                                        /* New touch event occurred */
            if (!ts->TS.Status) {                       /* We received released state */
                if (i) {                                /* Try to get second click, check difference for double click */
                    if (__GUI_ABS(x[0] - x[1]) > 10 || __GUI_ABS(y[0] - y[1]) > 10) {
                        i = 0;
                    }
                }
                if (!i) {                               /* On first call, this is click event */
                    *result = GUI_WC_Click;             /* Click event occurred */
                    
                    Time = ts->TS.Time;                 /* Save last time */
                    PT_YIELD(&ts->pt);                  /* Stop thread for now and wait next call */
                    
                    /**
                     * Wait for valid input with pressed state
                     */
                    PT_WAIT_UNTIL(&ts->pt, (v && ts->TS.Status) || (GUI.Time - Time) > 300);
                    if ((GUI.Time - Time) > 300) {      /* Check timeout for new pressed state */
                        PT_EXIT(&ts->pt);               /* Exit protothread */
                    }
                } else {
                    *result = GUI_WC_DblClick;          /* Double click event */
                    PT_EXIT(&ts->pt);                   /* Reset protothread */
                }
            } else {
                
            }
        } else {
            if (!i) {
                *result = GUI_WC_LongClick;         /* Click event occurred */
            }
            PT_EXIT(&ts->pt);                       /* Exit protothread here */
        }
        i++;
    }

    
    PT_END(&ts->pt);
}

void __SetRelativeCoordinate(__GUI_TouchData_t* ts, GUI_iDim_t x, GUI_iDim_t y) {
    uint8_t i = 0;
    for (i = 0; i < ts->TS.Count; i++) {
        ts->RelX[i] = ts->TS.X[i] - x;              /* Get relative coordinate on widget */
        ts->RelY[i] = ts->TS.Y[i] - y;              /* Get relative coordinate on widget */
    }

#if GUI_TOUCH_MAX_PRESSES > 1
    if (ts->TS.Count == 2) {                        /* 2 points detected */
        ts->DistanceOld = ts->Distance;             /* Save old distance */
        GUI_MATH_DistanceBetweenXY(ts->RelX[0], ts->RelY[0], ts->RelX[1], ts->RelY[1], &ts->Distance);  /* Calculate distance between 2 points */
    }
#endif /* GUI_TOUCH_MAX_PRESSES > 1 */
}

__GUI_TouchStatus_t __ProcessTouch(__GUI_TouchData_t* touch, GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    __GUI_TouchStatus_t tStat = touchCONTINUE;
    uint8_t processed;
    
    /* Check touches if any matches, go reverse on linked list */
    for (h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, h)) {
        if (__GUI_WIDGET_IsHidden(h)) {             /* Ignore hidden widget */
            continue;
        }
        
        /* Check children elements first */
        if (__GUI_WIDGET_AllowChildren(h)) {        /* If children widgets are allowed */
            tStat = __ProcessTouch(touch, h);       /* Process touch on widget elements first */
            if (tStat != touchCONTINUE) {           /* If we should not continue */
                return tStat;
            }
        }
        
        __CheckDispClipping(h);                     /* Check display region where widget is placed */
        
        /* Check if widget is in touch area */
        if (touch->TS.X[0] >= GUI.DisplayTemp.X1 && touch->TS.X[0] <= GUI.DisplayTemp.X2 && touch->TS.Y[0] >= GUI.DisplayTemp.Y1 && touch->TS.Y[0] <= GUI.DisplayTemp.Y2) {
            __SetRelativeCoordinate(touch, __GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h)); /* Set relative coordinate */

            processed = __GUI_WIDGET_Callback(h, GUI_WC_TouchStart, touch, &tStat);
            if (processed) {                    /* Check if command has been processed */
                if (tStat != touchCONTINUE) {   /* If touch was handled */
                    /**
                     * Move widget down on parent linked list and do the same with all of its parents,
                     * no matter of touch focus or not
                     */
                    __GUI_WIDGET_MoveDownTree(h);
                    
                    if (tStat == touchHANDLED) {    /* Touch handled for widget completelly */
                        
                        /**
                         * Set active widget and set flag for it
                         * Set focus widget and set flag for iz
                         */
                        __GUI_FOCUS_SET(h);
                        __GUI_ACTIVE_SET(h);
                        
                        /**
                         * Invalidate actual handle object
                         * Already invalidated in __GUI_ACTIVE_SET function
                         */
                        //__GUI_WIDGET_Invalidate(h);
                    } else {                    /* Touch handled with no focus */
                        /**
                         * When touch was handled without focus,
                         * process only clearing currently focused and active widgets and clear them
                         */
                        __GUI_FOCUS_CLEAR();
                        __GUI_ACTIVE_CLEAR();
                    }
                    return tStat;
                }
            }
        }
    }
    return touchCONTINUE;                           /* Try with another widget */
}
#endif /* GUI_USE_TOUCH */

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
GUI_Result_t GUI_Init(void) {
    memset((void *)&GUI, 0x00, sizeof(GUI_t));      /* Reset GUI structure */
    
    /* Call LCD low-level function */
    GUI_LL_Init(&GUI.LCD, &GUI.LL);                 /* Call low-level initialization */
    GUI.LL.Init(&GUI.LCD);                          /* Call user LCD driver function */
    
    /* Draw LCD with default color */
    
    /* Check situation with layers */
    if (GUI.LCD.LayersCount == 1) {
        GUI.LCD.ActiveLayer = 0;
        GUI.LCD.DrawingLayer = 0;
        GUI.LL.Fill(&GUI.LCD, GUI.LCD.DrawingLayer, (void *)GUI.LCD.Layers[GUI.LCD.DrawingLayer].StartAddress, GUI.LCD.Width, GUI.LCD.Height, 0, 0xFFFFFFFF);
    } else if (GUI.LCD.LayersCount > 1) {
        GUI.LCD.ActiveLayer = 0;
        GUI.LCD.DrawingLayer = 0;
        GUI.LL.Fill(&GUI.LCD, GUI.LCD.DrawingLayer, (void *)GUI.LCD.Layers[GUI.LCD.DrawingLayer].StartAddress, GUI.LCD.Width, GUI.LCD.Height, 0, 0xFFFFFFFF);
        GUI.LCD.DrawingLayer = 1;
    } else {
        return guiERROR;
    }
    
    /* Init input devices */
    __GUI_INPUT_Init();
    
    /* Init widgets */
    __GUI_WIDGET_Init();
    
    return guiOK;
}
#include "tm_stm32_general.h"
int32_t GUI_Process(void) {
    int32_t cnt = 0;
#if GUI_USE_TOUCH
    __GUI_TouchStatus_t tStat;
    GUI_WC_t result;
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    __GUI_KeyboardData_t key;
    __GUI_KeyboardStatus_t kStat;
#endif /* GUI_USE_KEYBOARD */

#if GUI_USE_TOUCH
    if (__GUI_INPUT_TouchAvailable()) {             /* Check if any touch available */
        while (__GUI_INPUT_TouchRead(&GUI.Touch.TS)) {  /* Process all touch events possible */
            if (GUI.Touch.TS.Status && GUI.TouchOld.TS.Status) {
                /**
                 * Old status: pressed
                 * New status: pressed
                 * Action: Touch move on active element
                 */
                if (GUI.ActiveWidget) {             /* If active widget exists */
                    __SetRelativeCoordinate(&GUI.Touch, __GUI_WIDGET_GetAbsoluteX(GUI.ActiveWidget), __GUI_WIDGET_GetAbsoluteY(GUI.ActiveWidget));  /* Set relative touch from current touch */
                    if (GUI.Touch.TS.Count == GUI.TouchOld.TS.Count) {
                        __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchMove, &GUI.Touch, &tStat);  /* The same amount of touch events currently */
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
                if (__ProcessTouch(&GUI.Touch, NULL) == touchHANDLED) {
                    if (GUI.ActiveWidget != GUI.ActiveWidgetOld) {  /* If new active widget is not the same as previous */
                        PT_INIT(&GUI.Touch.pt)      /* Reset threads */
                    }
                }
            }
            
            if (GUI.ActiveWidget) {
                /**
                 * Check for events
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
                    __SetRelativeCoordinate(&GUI.Touch, __GUI_WIDGET_GetAbsoluteX(GUI.ActiveWidget), __GUI_WIDGET_GetAbsoluteY(GUI.ActiveWidget));  /* Set relative touch from current touch */
                    __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchEnd, &GUI.Touch, &tStat);   /* Process callback function */
                    __GUI_ACTIVE_CLEAR();           /* Clear active widget */
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
    __GUI_UNUSED(tStat);                            /* Ignore compiler warnings */
#endif /* GUI_USE_TOUCH */
    
#if GUI_USE_KEYBOARD
    /**
     * Keyboard data reads
     */
    __GUI_UNUSED(kStat);
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
                        for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)__GH(GUI.FocusedWidget)->Parent, NULL); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
                            if (__GUI_WIDGET_IsVisible(h)) {    /* Check if widget is visible */
                                break;
                            }
                        }
                    }
                    if (h) {                        /* We have next widget */
                        __GUI_LINKEDLIST_WidgetMoveToBottom(h); /* Set widget to the down of list */
                        __GUI_FOCUS_SET(h);         /* Set focus to new widget */
                    }
                }
            }
        }
    }
#endif /* GUI_USE_KEYBOARD */
    
    /**
     * Timer processing
     */
    __GUI_TIMER_Process();                          /* Process all timers */
    
    /**
     * Redrawing operations
     */
    if (!(GUI.LCD.Flags & GUI_FLAG_LCD_WAIT_LAYER_CONFIRM) && __GetNumberOfPendingWidgets(NULL)) {  /* Check if anything to draw first */
        uint32_t time;
        GUI_Byte active = GUI.LCD.ActiveLayer;
        GUI_Byte drawing = GUI.LCD.DrawingLayer;
        
        /* Copy current status from one layer to another */
        GUI.LL.Copy(&GUI.LCD, drawing, (void *)GUI.LCD.Layers[active].StartAddress, (void *)GUI.LCD.Layers[drawing].StartAddress, GUI.LCD.Width, GUI.LCD.Height, 0, 0);
            
        /* Actually draw new screen based on setup */
        time = TM_GENERAL_DWTCounterGetValue();
        cnt = __RedrawWidgets(NULL);                /* Redraw all widgets now */
        //__GUI_DEBUG("T: %d\r\n", TM_GENERAL_DWTCounterGetValue() - time);
        
        //GUI_DRAW_Rectangle(& GUI.Display, GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2 - GUI.Display.X1, GUI.Display.Y2 - GUI.Display.Y1, GUI_COLOR_CYAN);
        
        /* Invalid clipping region */
        GUI.Display.X1 = 0x7FFF;
        GUI.Display.Y1 = 0x7FFF;
        GUI.Display.X2 = 0x8000;
        GUI.Display.Y2 = 0x8000;
        
        /* Set drawing layer as pending */
        GUI.LCD.Layers[drawing].Pending = 1;
        
        /* Notify low-level about layer change */
        GUI.LCD.Flags |= GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;
        GUI_LL_Control(&GUI.LCD, GUI_LL_Command_SetActiveLayer, &drawing); /* Set new active layer to low-level driver */
        
        /* Swap active and drawing layers */
        /* New drawings won't be affected until confirmation from low-level is not received */
        GUI.LCD.ActiveLayer = drawing;
        GUI.LCD.DrawingLayer = active;
    }
    
    return cnt;                                     /* Return number of elements updated on GUI */
}

void GUI_UpdateTime(uint32_t millis) {
    GUI.Time += millis;                             /* Increase GUI time for amount of milliseconds */
}

void GUI_LCD_ConfirmActiveLayer(GUI_Byte layer_num) {
    if ((GUI.LCD.Flags & GUI_FLAG_LCD_WAIT_LAYER_CONFIRM)) {/* If we have anything pending */
        GUI.LCD.Layers[layer_num].Pending = 0;
        GUI.LCD.Flags &= ~GUI_FLAG_LCD_WAIT_LAYER_CONFIRM;  /* Clear flag */
    }
}

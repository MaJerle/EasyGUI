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
//Gets number of widgets waiting for redraw
uint32_t __GetNumberOfPendingWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    uint32_t cnt = 0;
    
    if (parent && parent->Flags & GUI_FLAG_REDRAW) {    /* Check for specific widget */
        return 1;                                   /* We have object to redraw */
    }
    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, h)) {
        if (__GUI_WIDGET_AllowChildren(h)) {        /* If this widget has children elements */
            cnt += __GetNumberOfPendingWidgets(h);  /* Redraw this widget and all its children if required */
        } else if (h->Flags & GUI_FLAG_REDRAW) {    /* Check if we need redraw */
            cnt++;                                  /* Increase number of elements to redraw */
        }
    }
    return cnt;
}

//Draws widgets
uint32_t __RedrawWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    uint32_t cnt = 0;
    
    if (parent && (parent->Flags & GUI_FLAG_REDRAW)) {  /* Check if parent window should redraw operation */
        if (!__GUI_WIDGET_IsVisible(parent)) {      /* Check if visible */
            return 0;                               /* Stop execution if parent is hidden */
        }
        parent->Flags &= ~GUI_FLAG_REDRAW;          /* Clear flag */
        for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
            h->Flags |= GUI_FLAG_REDRAW;            /* Set redraw bit to all children elements */
        }
        if (__GUI_WIDGET_IsInsideClippingRegion(parent)) {  /* If draw function is set and drawing is inside clipping region */
            __GUI_WIDGET_Callback(parent, GUI_WC_Draw, &GUI.Display, NULL);
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
            if (h->Flags & GUI_FLAG_REDRAW) {       /* Check if redraw required */
                h->Flags &= ~GUI_FLAG_REDRAW;       /* Clear flag */
                if (__GUI_WIDGET_IsInsideClippingRegion(h)) {   /* If draw function is set and drawing is inside clipping region */
                    __GUI_WIDGET_Callback(h, GUI_WC_Draw, &GUI.Display, NULL);  /* Draw widget */
                }
                cnt++;
            }
        }
    }
    
    return cnt;                                     /* Return number of redrawn objects */
}

#if GUI_USE_TOUCH
void __SetRelativeCoordinate(__GUI_TouchData_t* touch, GUI_iDim_t x, GUI_iDim_t y) {
    uint8_t i = 0;
    for (i = 0; i < touch->TS.Count; i++) {
        touch->RelX[i] = touch->TS.X[i] - x;    /* Get relative coordinate on widget */
        touch->RelY[i] = touch->TS.Y[i] - y;    /* Get relative coordinate on widget */
    }
}

__GUI_TouchStatus_t __ProcessTouch(__GUI_TouchData_t* touch, GUI_TouchData_t* touchLast, GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    __GUI_TouchStatus_t tStat = touchCONTINUE;
    GUI_Dim_t x, y, width, height;
    uint8_t processed;
    
    /* Check touches if any matches, go reverse on linked list */
    for (h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, h)) {
        if (h->Flags & GUI_FLAG_HIDDEN) {           /* Ignore hidden widget */
            continue;
        }
        
        /* Check children elements first */
        if (__GUI_WIDGET_AllowChildren(h)) {        /* If children widgets are allowed */
            tStat = __ProcessTouch(touch, touchLast, h);    /* Process touch on widget elements first */
            if (tStat != touchCONTINUE) {           /* If we should not continue */
                return tStat;
            }
        }
        
        x = __GUI_WIDGET_GetAbsoluteX(h);           /* Get actual widget X position */
        y = __GUI_WIDGET_GetAbsoluteY(h);           /* Get actual widget Y position */
        width = __GUI_WIDGET_GetWidth(h);           /* Get widget width */
        height = __GUI_WIDGET_GetHeight(h);         /* Get widget height */
        
        /* Check if widget is in drawing area */
        if (touch->TS.X[0] >= x && touch->TS.X[0] <= (x + width) && touch->TS.Y[0] >= y && touch->TS.Y[0] <= (y + height)) {
            if (touch->TS.Status && !touchLast->Status) {   /* Check for touchdown event */
                __SetRelativeCoordinate(touch, x, y);   /* Set relative coordinate */

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
            } else if (!touch->TS.Status && touchLast->Status) {    /* Check for touchup event */
                __GUI_WIDGET_Callback(h, GUI_WC_TouchEnd, touch, &tStat);    /* Process touch up event */
                if (h == GUI.ActiveWidget) {
                    __GUI_ACTIVE_CLEAR();           /* Clear active */
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

int32_t GUI_Process(void) {
    int32_t cnt = 0;
#if GUI_USE_TOUCH  
    static uint8_t first = 1;
    static GUI_TouchData_t touchLast;
    __GUI_TouchData_t touch;
    __GUI_TouchStatus_t tStat;
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    __GUI_KeyboardData_t key;
    __GUI_KeyboardStatus_t kStat;
#endif /* GUI_USE_KEYBOARD */

#if GUI_USE_TOUCH  
    /**
     * Touch manipulation
     */
    if (first) {                                    /* Process first call */
        first = 0;
        memset(&touchLast, 0x00, sizeof(touchLast));
        touchLast.Status = GUI_TouchState_RELEASED; /* Start with released touch */
    } 

    while (__GUI_INPUT_ReadTouch(&touch.TS)) {      /* Process all touch events possible */
        /* If there is already an active touch */
        if (touchLast.Status && touch.TS.Status) {
            if (GUI.ActiveWidget) {                 /* If active widget exists */
                __SetRelativeCoordinate(&touch, __GUI_WIDGET_GetAbsoluteX(GUI.ActiveWidget), __GUI_WIDGET_GetAbsoluteY(GUI.ActiveWidget));  /* Set relative touch from current touch */
                if (touch.TS.Count == touchLast.Count) {
                    __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchMove, &touch, &tStat);  /* The same amount of touch events currently */
                } else {
                    __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_TouchStart, &touch, &tStat);  /* The same amount of touch events currently */
                }
                __GUI_UNUSED(tStat);
            }                                       /* Wait for touch up */
        } else {
            /* Process other touches */
            if (__ProcessTouch(&touch, &touchLast, NULL) == touchHANDLED) {
                
            }
        }
        
        /* In case odd things happen, make sure there is no active widget */
        if (!touch.TS.Status) {                     /* If touch released, release active widget */
            if (GUI.ActiveWidget) {                 /* If touch is active, clear flag */
                GUI.ActiveWidget->Flags &= ~GUI_FLAG_ACTIVE;    /* Remove active flag, touch/mouse not active */
                __GUI_WIDGET_Invalidate(GUI.ActiveWidget);  /* Invalidate widget */
            }
            GUI.ActiveWidget = 0;                   /* No active widget anymore */
        }
        memcpy((void *)&touchLast, (void *)&touch.TS, sizeof(GUI_TouchData_t)); /* Copy current touch to last touch status */
    }
#endif /* GUI_USE_TOUCH */
    
#if GUI_USE_KEYBOARD
    /**
     * Keyboard data reads
     */
    while (__GUI_INPUT_ReadKey(&key.KB)) {          /* Read all keyboard entires */
        if (GUI.FocusedWidget) {                    /* Check if any widget is in focus already */
            if (key.KB.Keys[0] == GUI_KEY_TAB) {    /* Tab key pressed, set next widget as focused */
                GUI_HANDLE_p h = __GUI_LINKEDLIST_WidgetGetNext(NULL, GUI.FocusedWidget);   /* Get next widget if possible */
                if (h && __GUI_WIDGET_IsHidden(h)) {    /* Ignore hidden widget */
                    h = 0;
                }
                if (!h) {                           /* There is no next widget */
                    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)GUI.FocusedWidget->Parent, NULL); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
                        if (__GUI_WIDGET_IsVisible(h)) {    /* Check if widget is visible */
                            break;
                        }
                    }
                }
                if (h) {                            /* We have next widget */
                    __GUI_LINKEDLIST_WidgetMoveToBottom(h);    /* Set widget to the down of list */
                    __GUI_FOCUS_SET(h);             /* Set focus to new widget */
                }
            } else {                                /* Other key */
                __GUI_WIDGET_Callback(GUI.FocusedWidget, GUI_WC_KeyPress, &key, &kStat);
                __GUI_UNUSED(kStat);
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
        GUI_Byte active = GUI.LCD.ActiveLayer;
        GUI_Byte drawing = GUI.LCD.DrawingLayer;
        
        /* Copy current status from one layer to another */
        GUI.LL.Copy(&GUI.LCD, drawing, (void *)GUI.LCD.Layers[active].StartAddress, (void *)GUI.LCD.Layers[drawing].StartAddress, GUI.LCD.Width, GUI.LCD.Height, 0, 0);
            
        /* Actually draw new screen based on setup */
        cnt = __RedrawWidgets(NULL);                /* Redraw all widgets now */
        
        //GUI_DRAW_Rectangle(& GUI.Display, GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2 - GUI.Display.X1, GUI.Display.Y2 - GUI.Display.Y1, GUI_COLOR_CYAN);
        
        /* Invalid clipping region */
        GUI.Display.X1 = 0xFFFF;
        GUI.Display.Y1 = 0xFFFF;
        GUI.Display.X2 = 0;
        GUI.Display.Y2 = 0;
        
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

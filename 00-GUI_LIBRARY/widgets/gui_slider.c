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
#include "gui_slider.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define __GS(x)             ((GUI_SLIDER_t *)(x))

static
uint8_t GUI_SLIDER_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_BG,                               /*!< Default background color on non-active part */
    GUI_COLOR_WIN_BLUE,                             /*!< Default background color on active part */
    GUI_COLOR_WIN_RED,                              /*!< Default circle (...) color */
    GUI_COLOR_WIN_DARKGRAY,                         /*!< Default border color */
};

const static GUI_WIDGET_t Widget = {
    .Name = _T("SLIDER"),                           /*!< Widget name */
    .Size = sizeof(GUI_SLIDER_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_INVALIDATE_PARENT,     /*!< List of widget flags */
    .Callback = GUI_SLIDER_Callback,                /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o       ((GUI_SLIDER_t *)(h))

/* Check if slider is horizontal */
#define __IsHorizontal(h)   (o->Mode == GUI_SLIDER_MODE_LEFT_RIGHT || o->Mode == GUI_SLIDER_MODE_RIGHT_LEFT)

/* Get delta value from widget */
static
GUI_Dim_t __GetDelta(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t he) {
    return wi > he ? he : wi;
}

/* Set slider value */
static
void __SetValue(GUI_HANDLE_p h, int32_t value) {
    if (value > o->Max) {
        value = o->Max;
    } else if (value < o->Min) {
        value = o->Min;
    }
    if (value != o->Value) {                        /* Check difference in values */
        o->Value = value;                           /* Set new value */
        __GUI_WIDGET_Callback(h, GUI_WC_ValueChanged, 0, 0);    /* Callback process */
    }
}

/* Process touch event */
static 
void __TouchHandle(GUI_HANDLE_p h, __GUI_TouchData_t* ts) {
    GUI_Dim_t pos, delta, deltaH, width, height;
    int32_t value;
            
    width = ts->WidgetWidth;
    height = ts->WidgetHeight;
    delta = __GetDelta(h, ts->WidgetWidth, ts->WidgetHeight);   /* Get delta value */
    deltaH = delta >> 1;
    if (__IsHorizontal(h)) {                        /* Horizontal widget */
        if (o->Mode == GUI_SLIDER_MODE_LEFT_RIGHT) {/* Inverted version to normal */
            pos = ts->RelX[0];                      /* Invert position */
        } else {
            pos = width - ts->RelX[0];
        }
        if (pos >= deltaH) {
            pos -= deltaH;
        } else {
            pos = 0;
        }
        width -= delta;
        if (pos > width) {
            pos = width;
        }
        value = (int32_t)(((float)(o->Max - o->Min)) * (float)pos / (float)width) + o->Min;
    } else {                                        /* Vertical widget */
        if (o->Mode == GUI_SLIDER_MODE_TOP_BOTTOM) {/* Inverted version to normal */
            pos = ts->RelY[0];                      /* Invert position */
        } else {
            pos = height - ts->RelY[0];
        }
        if (pos >= deltaH) {
            pos -= deltaH;
        } else {
            pos = 0;
        }
        height -= delta;
        if (pos > height) {
            pos = height;
        }
        value = (int32_t)(((float)(o->Max - o->Min)) * (float)pos / (float)height) + o->Min;
    }
    
    __SetValue(h, value);                           /* Set new value */
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate with parent */
}

/* Timer callback function for slider widget */
static
void __TimerCallback(GUI_TIMER_t* timer) {
    GUI_HANDLE_p h = (GUI_HANDLE_p)__GUI_TIMER_GetParams(timer);    /* Get user parameters */
    if (__GUI_WIDGET_IsActive(h)) {                 /* Timer is in focus */
        if (__GS(h)->CurrentSize < __GS(h)->MaxSize) {
            __GS(h)->CurrentSize++;                 /* Increase size */
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
        }
    } else {
        if (__GS(h)->CurrentSize > 0) {
            __GS(h)->CurrentSize--;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
        } else {
            __GUI_TIMER_Stop(timer);                /* Stop timer execution */
        }
    }
}

/* Callback function */
static
uint8_t GUI_SLIDER_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) { 
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            o->Min = 0;                             /* Set default minimal value */
            o->Max = 100;                           /* Set default maximal value */
            o->Value = 50;                          /* Set default value */
            
            o->MaxSize = 4;
            o->CurrentSize = 0;
            o->C.Timer = __GUI_TIMER_Create(30, __TimerCallback, o);    /* Create timer for widget, when widget is deleted, timer will be automatically deleted too */
            if (!o->C.Timer) {                      /* Check if timer created */
                *(uint8_t *)result = 0;             /* Failed, widget will be deleted */
            }
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, width, height, delta, deltaH, recParam, offset;
            GUI_Color_t c1, c2;
            GUI_Dim_t circleSize;

            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            delta = __GetDelta(h, width, height);   /* Get delta value */
            deltaH = delta >> 1;                    /* Half of delta */
            recParam = delta >> 2;                  /* Set rectangle height */
            if (recParam == 0) {                    /* Check value */
                recParam = 1;
            }
            
            /* Set color for inverted sliders */
            if (o->Mode == GUI_SLIDER_MODE_BOTTOM_TOP || o->Mode == GUI_SLIDER_MODE_RIGHT_LEFT) {
                c1 = __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BG_NONACTIVE);
                c2 = __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BG_ACTIVE);
            } else {
                c1 = __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BG_ACTIVE);
                c2 = __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BG_NONACTIVE);
            }
            
            circleSize = (deltaH >> 2) + o->CurrentSize * (deltaH - (deltaH >> 2)) / o->MaxSize;    /* Get circle size */
            
            /* Draw bottom rectangle */
            if (__IsHorizontal(h)) {                /* Horizontal slider */
                offset = 0;                         /* Make start offset */
                width -= delta;
                if (o->Mode == GUI_SLIDER_MODE_RIGHT_LEFT) {/* Right left version */
                    offset = width - (GUI_Dim_t)(((float)(width) * (float)(o->Value - o->Min)) / (float)(o->Max - o->Min));
                } else {                            /* Left right version */
                    offset = (GUI_Dim_t)(((float)(width) * (float)(o->Value - o->Min)) / (float)(o->Max - o->Min));
                }
                x += deltaH;
                GUI_DRAW_FilledRoundedRectangle(disp, x, y + ((delta - recParam) >> 1), offset, recParam, (recParam >> 1), c1);   
                GUI_DRAW_FilledRoundedRectangle(disp, x + offset, y + ((delta - recParam) >> 1), width - offset, recParam, (recParam >> 1), c2);
                GUI_DRAW_RoundedRectangle(disp, x, y + ((delta - recParam) >> 1), width, recParam, (recParam >> 1), __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BORDER));
                GUI_DRAW_FilledCircle(disp, x + offset, y + deltaH, circleSize, __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_FG));
                GUI_DRAW_Circle(disp, x + offset, y + deltaH, circleSize, __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BORDER));
            } else {                                /* Vertical slider */
                offset = 0;                         /* Make start offset */
                height -= delta;
                if (o->Mode == GUI_SLIDER_MODE_BOTTOM_TOP) {    /* Bottom top version */
                    offset = height - (GUI_Dim_t)(((float)(height) * (float)o->Value) / (float)(o->Max - o->Min));
                } else {                            /* Top bottom version */
                    offset = (GUI_Dim_t)(((float)(height) * (float)o->Value) / (float)(o->Max - o->Min));
                }
                y += deltaH;
                GUI_DRAW_FilledRoundedRectangle(disp, x + ((delta - recParam) >> 1), y, recParam, offset, (recParam >> 1), c1);   
                GUI_DRAW_FilledRoundedRectangle(disp, x + ((delta - recParam) >> 1), y + offset, recParam, height - offset, (recParam >> 1), c2);
                GUI_DRAW_RoundedRectangle(disp, x + ((delta - recParam) >> 1), y, recParam, height, (recParam >> 1), __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BORDER)); 
                GUI_DRAW_FilledCircle(disp, x + deltaH, y + offset, circleSize, __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_FG));
                GUI_DRAW_Circle(disp, x + deltaH, y + offset, circleSize, __GUI_WIDGET_GetColor(h, GUI_SLIDER_COLOR_BORDER));
            }
            
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {                   /* Touch down event */
            __TouchHandle(h, (__GUI_TouchData_t *)param);   /* Handle touch */
            
            *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Set touch status */
            return 1;
        }
        case GUI_WC_TouchMove: {                    /* Touch move event */            
            __TouchHandle(h, (__GUI_TouchData_t *)param);   /* Handle touch */
            return 1;
        }
        case GUI_WC_TouchEnd:
            return 1;
#endif /* GUI_USE_TOUCH */
        case GUI_WC_ActiveIn: {
            __GUI_TIMER_StartPeriodic(o->C.Timer);  /* Start animation timer */
            return 1;
        }
        case GUI_WC_ActiveOut: {
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_SLIDER_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_SLIDER_t* ptr;
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_SLIDER_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags); /* Allocate memory for basic widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_SLIDER_SetColor(GUI_HANDLE_p h, GUI_SLIDER_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set colors */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_SLIDER_SetMode(GUI_HANDLE_p h, GUI_SLIDER_MODE_t mode) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GS(h)->Mode != mode) {
        __GS(h)->Mode = mode;                       /* Set mode */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_SLIDER_SetValue(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __SetValue(h, val);                             /* Set value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_SLIDER_SetMin(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GS(h)->Min != val && val < __GS(h)->Max) {
        __GS(h)->Min = val;                         /* Set new parameter */
        if (__GS(h)->Value < __GS(h)->Min) {        /* Check current value */
            __GS(h)->Value = __GS(h)->Min;
        }
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_SLIDER_SetMax(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GS(h)->Max != val && val > __GS(h)->Min) {
        __GS(h)->Max = val;                         /* Set new parameter */
        if (__GS(h)->Value > __GS(h)->Max) {        /* Check current value */
            __GS(h)->Value = __GS(h)->Max;
        }
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

int32_t GUI_SLIDER_GetMin(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GS(h)->Min;                             /* Get minimal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

int32_t GUI_SLIDER_GetMax(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GS(h)->Max;                             /* Get maximal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

int32_t GUI_SLIDER_GetValue(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GS(h)->Value;                           /* Get current value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

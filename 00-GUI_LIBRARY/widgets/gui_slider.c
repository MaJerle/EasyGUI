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

#define CFG_MODE            0x01
#define CFG_VALUE           0x02
#define CFG_MIN             0x03
#define CFG_MAX             0x04

static uint8_t gui_slider_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_BG,                               /*!< Default background color on non-active part */
    GUI_COLOR_WIN_BLUE,                             /*!< Default background color on active part */
    GUI_COLOR_WIN_RED,                              /*!< Default circle (...) color */
    GUI_COLOR_WIN_DARKGRAY,                         /*!< Default border color */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("SLIDER"),                          /*!< Widget name */
    .Size = sizeof(GUI_SLIDER_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_INVALIDATE_PARENT,     /*!< List of widget flags */
    .Callback = gui_slider_callback,                /*!< Callback function */
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
static GUI_Dim_t
__GetDelta(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t he) {
    return wi > he ? he : wi;
}

/* Set slider value */
static void 
__SetValue(GUI_HANDLE_p h, int32_t value) {
    if (value > o->Max) {
        value = o->Max;
    } else if (value < o->Min) {
        value = o->Min;
    }
    if (value != o->Value) {                        /* Check difference in values */
        o->Value = value;                           /* Set new value */
        gui_widget_callback__(h, GUI_WC_ValueChanged, 0, 0);    /* Callback process */
    }
}

/* Process touch event */
static void
__TouchHandle(GUI_HANDLE_p h, __GUI_TouchData_t* ts) {
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
    gui_widget_invalidate__(h);                     /* Invalidate with parent */
}

/* Timer callback function for slider widget */
static void
__TimerCallback(GUI_TIMER_t* timer) {
    GUI_HANDLE_p h = (GUI_HANDLE_p)gui_timer_getparams__(timer);    /* Get user parameters */
    if (gui_widget_isactive__(h)) {                 /* Timer is in focus */
        if (__GS(h)->CurrentSize < __GS(h)->MaxSize) {
            __GS(h)->CurrentSize++;                 /* Increase size */
            gui_widget_invalidate__(h);             /* Invalidate widget */
        }
    } else {
        if (__GS(h)->CurrentSize > 0) {
            __GS(h)->CurrentSize--;
            gui_widget_invalidate__(h);             /* Invalidate widget */
        } else {
            gui_timer_stop__(timer);                /* Stop timer execution */
        }
    }
}

/* Callback function */
static uint8_t
gui_slider_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) { 
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            o->Min = 0;                             /* Set default minimal value */
            o->Max = 100;                           /* Set default maximal value */
            o->Value = 50;                          /* Set default value */
            
            o->MaxSize = 4;
            o->CurrentSize = 0;
            o->C.Timer = gui_timer_create__(30, __TimerCallback, o);    /* Create timer for widget, when widget is deleted, timer will be automatically deleted too */
            if (!o->C.Timer) {                      /* Check if timer created */
                *(uint8_t *)result = 0;             /* Failed, widget will be deleted */
            }
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* v = (GUI_WIDGET_Param_t *)param;
            int32_t tmp;
            switch (v->Type) {
                case CFG_MODE:                      /* Set current progress value */
                    o->Mode = *(GUI_SLIDER_MODE_t *)v->Data;
                    break;
                case CFG_VALUE:                     /* Set current progress value */
                    __SetValue(h, *(int32_t *)v->Data);
                    break;
                case CFG_MAX:                       /* Set maximal value */
                    tmp = *(int32_t *)v->Data;
                    if (tmp > o->Min) {
                        o->Max = tmp;
                        if (o->Value > o->Max) {
                            __SetValue(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:                       /* Set minimal value */
                    tmp = *(int32_t *)v->Data;
                    if (tmp < o->Max) {
                        o->Min = tmp;
                        if (o->Value < o->Min) {
                            __SetValue(h, tmp);
                        }
                    }
                    break;
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, width, height, delta, deltaH, recParam, offset;
            GUI_Color_t c1, c2;
            GUI_Dim_t circleSize;

            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            delta = __GetDelta(h, width, height);   /* Get delta value */
            deltaH = delta >> 1;                    /* Half of delta */
            recParam = delta >> 2;                  /* Set rectangle height */
            if (recParam == 0) {                    /* Check value */
                recParam = 1;
            }
            
            /* Set color for inverted sliders */
            if (o->Mode == GUI_SLIDER_MODE_BOTTOM_TOP || o->Mode == GUI_SLIDER_MODE_RIGHT_LEFT) {
                c1 = gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BG_NONACTIVE);
                c2 = gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BG_ACTIVE);
            } else {
                c1 = gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BG_ACTIVE);
                c2 = gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BG_NONACTIVE);
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
                gui_draw_filledroundedrectangle(disp, x, y + ((delta - recParam) >> 1), offset, recParam, (recParam >> 1), c1);   
                gui_draw_filledroundedrectangle(disp, x + offset, y + ((delta - recParam) >> 1), width - offset, recParam, (recParam >> 1), c2);
                gui_draw_roundedrectangle(disp, x, y + ((delta - recParam) >> 1), width, recParam, (recParam >> 1), gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BORDER));
                gui_draw_filledcircle(disp, x + offset, y + deltaH, circleSize, gui_widget_getcolor__(h, GUI_SLIDER_COLOR_FG));
                gui_draw_circle(disp, x + offset, y + deltaH, circleSize, gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BORDER));
            } else {                                /* Vertical slider */
                offset = 0;                         /* Make start offset */
                height -= delta;
                if (o->Mode == GUI_SLIDER_MODE_BOTTOM_TOP) {    /* Bottom top version */
                    offset = height - (GUI_Dim_t)(((float)(height) * (float)o->Value) / (float)(o->Max - o->Min));
                } else {                            /* Top bottom version */
                    offset = (GUI_Dim_t)(((float)(height) * (float)o->Value) / (float)(o->Max - o->Min));
                }
                y += deltaH;
                gui_draw_filledroundedrectangle(disp, x + ((delta - recParam) >> 1), y, recParam, offset, (recParam >> 1), c1);   
                gui_draw_filledroundedrectangle(disp, x + ((delta - recParam) >> 1), y + offset, recParam, height - offset, (recParam >> 1), c2);
                gui_draw_roundedrectangle(disp, x + ((delta - recParam) >> 1), y, recParam, height, (recParam >> 1), gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BORDER)); 
                gui_draw_filledcircle(disp, x + deltaH, y + offset, circleSize, gui_widget_getcolor__(h, GUI_SLIDER_COLOR_FG));
                gui_draw_circle(disp, x + deltaH, y + offset, circleSize, gui_widget_getcolor__(h, GUI_SLIDER_COLOR_BORDER));
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
            gui_timer_startperiodic__(o->C.Timer);  /* Start animation timer */
            return 1;
        }
        case GUI_WC_ActiveOut: {
            gui_widget_invalidate__(h);             /* Invalidate widget */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new slider widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for create procedure
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p
gui_slider_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_SLIDER_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t
gui_slider_setcolor(GUI_HANDLE_p h, GUI_SLIDER_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Set slider mode (orientation)
 * \param[in]       h: Widget handle
 * \param[in]       mode: Slider mode. This parameter can be a value of \ref GUI_SLIDER_MODE_t enumeration
 * \retval          1: Mode was set ok
 * \retval          0: Mode was not set
 */
uint8_t
gui_slider_setmode(GUI_HANDLE_p h, GUI_SLIDER_MODE_t mode) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MODE, &mode, 1, 0); /* Set parameter */
}

/**
 * \brief           Set slider current value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New current value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              gui_slider_setmin, gui_slider_setmax, gui_slider_getvalue, gui_slider_getmin, gui_slider_getmax  
 */
uint8_t
gui_slider_setvalue(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_VALUE, &val, 1, 0); /* Set parameter */
}

/**
 * \brief           Set slider minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New minimal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              gui_slider_setvalue, gui_slider_setmax, gui_slider_getvalue, gui_slider_getmin, gui_slider_getmax         
 */
uint8_t
gui_slider_setmin(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MIN, &val, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set slider maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New maximal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              gui_slider_setmin, gui_slider_setvalue, gui_slider_getvalue, gui_slider_getmin, gui_slider_getmax  
 */
uint8_t
gui_slider_setmax(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MAX, &val, 1, 0);   /* Set parameter */
}

/**
 * \brief           Get slider minimal value
 * \param[in,out]   h: Widget handle
 * \retval          Minimal value
 * \sa              gui_slider_setmin, gui_slider_setvalue, gui_slider_setmax, gui_slider_getvalue, gui_slider_getmax  
 */
int32_t
gui_slider_getmin(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GS(h)->Min;                             /* Get minimal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Get slider maximal value
 * \param[in,out]   h: Widget handle
 * \retval          Maximal value
 * \sa              gui_slider_setmin, gui_slider_setvalue, gui_slider_setmax, gui_slider_getvalue, gui_slider_getmin  
 */
int32_t
gui_slider_getmax(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GS(h)->Max;                             /* Get maximal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Get slider current value
 * \param[in,out]   h: Widget handle
 * \retval          Current value
 * \sa              gui_slider_setmin, gui_slider_setvalue, gui_slider_setmax, gui_slider_getmin, gui_slider_getmax  
 */
int32_t
gui_slider_getvalue(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GS(h)->Value;                           /* Get current value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

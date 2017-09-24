/**	
 * \file            gui_progbar.c
 * \brief           Progress bar widget
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
#include "gui_progbar.h"

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
#define GUI_PROGBAR_FLAG_PERCENT    0x01            /*!< Flag indicating percentages are enabled on view */
#define GUI_PROGBAR_FLAG_ANIMATE    0x02            /*!< Animation for progress bar changes */

#define __GP(x)             ((GUI_PROGBAR_t *)(x))

#define CFG_VALUE           0x01
#define CFG_MIN             0x02
#define CFG_MAX             0x03
#define CFG_PERCENT         0x04
#define CFG_ANIM            0x05

static uint8_t gui_progbar_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_MIDDLEGRAY,                       /*!< Default background color */
    GUI_COLOR_WIN_DARKGRAY,                         /*!< Default freground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("PROGBAR"),                         /*!< Widget name */
    .Size = sizeof(GUI_PROGBAR_t),                  /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_progbar_callback,               /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define p           ((GUI_PROGBAR_t *)h)
#define is_anim(h) (!!(__GP(h)->Flags & GUI_PROGBAR_FLAG_ANIMATE))

/* Set value for widget */
static uint8_t
set_value(GUI_HANDLE_p h, int32_t val) {
    if (p->DesiredValue != val && val >= p->Min && val <= p->Max) { /* Value has changed */
        p->DesiredValue = val;                      /* Set value */
        if (p->CurrentValue < p->Min) {
            p->CurrentValue = p->Min;
        } else if (p->CurrentValue > p->Max) {
            p->CurrentValue = p->Max;
        }
        if (is_anim(h) && __GH(h)->Timer) {         /* In case of animation and timer availability */
            gui_timer_start__(__GH(h)->Timer);      /* Start timer */
        } else {
            p->CurrentValue = p->DesiredValue;      /* Set values to the same */
        }
        gui_widget_invalidate__(h);                 /* Redraw widget */
        gui_widget_callback__(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        return 1;
    }
    return 0;
}

/* Timer callback for widget */
static void
timer_callback(GUI_TIMER_t* t) {
    GUI_HANDLE_p h = gui_timer_getparams__(t);      /* Get timer parameters */
    if (h) {
        if (p->CurrentValue != p->DesiredValue) {   /* Check difference */
            if (p->CurrentValue > p->DesiredValue) {
                p->CurrentValue--;
            } else {
                p->CurrentValue++;
            }
            gui_widget_invalidate__(h);             /* Invalidate widget */
            gui_timer_start__(t);                   /* Start timer */
        }
    }
}

/* Main widget callback */
static
uint8_t gui_progbar_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GP(h)->Min = __GP(h)->CurrentValue = 0;
            __GP(h)->Max = 100;
            set_value(h, 50);
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* v = (GUI_WIDGET_Param_t *)param;
            int32_t tmp;
            switch (v->Type) {
                case CFG_VALUE:                     /* Set current progress value */
                    set_value(h, *(int32_t *)v->Data);
                    break;
                case CFG_MAX:                       /* Set maximal value */
                    tmp = *(int32_t *)v->Data;
                    if (tmp > p->Min) {
                        p->Max = tmp;
                        if (p->DesiredValue > p->Max || p->CurrentValue > p->Max) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:                       /* Set minimal value */
                    tmp = *(int32_t *)v->Data;
                    if (tmp < p->Max) {
                        p->Min = tmp;
                        if (p->DesiredValue < p->Min || p->CurrentValue < p->Min) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_PERCENT:                   /* Set percentage mode */
                    if (*(uint8_t *)v->Data) {
                        p->Flags |= GUI_PROGBAR_FLAG_PERCENT;
                    } else {
                        p->Flags &= ~GUI_PROGBAR_FLAG_PERCENT;
                    }
                    break;
                case CFG_ANIM:
                    if (*(uint8_t *)v->Data) {
                        if (!__GH(h)->Timer) {
                            __GH(h)->Timer = gui_timer_create__(10, timer_callback, h); /* Create animation timer */
                        }
                        if (__GH(h)->Timer) {       /* Check timer response */
                            __GP(h)->Flags |= GUI_PROGBAR_FLAG_ANIMATE; /* Enable animations */
                        }
                    } else {
                        __GP(h)->Flags &= ~GUI_PROGBAR_FLAG_ANIMATE;    /* Disable animation */
                        if (__GH(h)->Timer) {
                            gui_timer_remove__(&__GH(h)->Timer);    /* Remove timer */
                        }
                        set_value(h, p->DesiredValue);  /* Reset value */
                    }
                    break;
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Dim_t x, y, w, width, height;
            GUI_Display_t* disp = (GUI_Display_t *)param;
    
            x = gui_widget_getabsolutex__(h);       /* Get absolute position on screen */
            y = gui_widget_getabsolutey__(h);       /* Get absolute position on screen */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
           
            w = ((width - 4) * (p->CurrentValue - p->Min)) / (p->Max - p->Min); /* Get width for active part */
            
            gui_draw_filledrectangle(disp, x + w + 2, y + 2, width - w - 4, height - 4, gui_widget_getcolor__(h, GUI_PROGBAR_COLOR_BG));
            gui_draw_filledrectangle(disp, x + 2, y + 2, w, height - 4, gui_widget_getcolor__(h, GUI_PROGBAR_COLOR_FG));
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            
            /* Draw text if possible */
            if (__GH(h)->Font) {
                const GUI_Char* text = NULL;
                GUI_Char buff[5];
                
                if (p->Flags & GUI_PROGBAR_FLAG_PERCENT) {
                    sprintf((char *)buff, "%lu%%", (unsigned long)(((p->CurrentValue - p->Min) * 100) / (p->Max - p->Min)));
                    text = buff;
                } else if (gui_widget_isfontandtextset__(h)) {
                    text = gui_widget_gettext__(h);
                }
                
                if (text) {                         /* If text is valid, print it */
                    GUI_DRAW_FONT_t f;
                    gui_draw_font_init(&f);         /* Init structure */
                    
                    f.X = x + 2;
                    f.Y = y + 2;
                    f.Width = width - 4;
                    f.Height = height - 4;
                    f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.Color1Width = w ? w - 1 : 0;
                    f.Color1 = gui_widget_getcolor__(h, GUI_PROGBAR_COLOR_BG);
                    f.Color2 = gui_widget_getcolor__(h, GUI_PROGBAR_COLOR_FG);
                    gui_draw_writetext(disp, gui_widget_getfont__(h), text, &f);
                }
            }
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new progress bar widget
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
gui_progbar_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_PROGBAR_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t
gui_progbar_setcolor(GUI_HANDLE_p h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Set progress bar current value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New current value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              gui_progbar_setmin, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmin, gui_progbar_getmax  
 */
uint8_t
gui_progbar_setvalue(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_VALUE, &val, 1, 0); /* Set parameter */
}

/**
 * \brief           Set progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New minimal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmin, gui_progbar_getmax         
 */
uint8_t
gui_progbar_setmin(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MIN, &val, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New maximal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_getvalue, gui_progbar_getmin, gui_progbar_getmax  
 */
uint8_t
gui_progbar_setmax(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MAX, &val, 1, 0);   /* Set parameter */
}

/**
 * \brief           Sets percent mode. When in this mode, widget text is in percent according to current value between minimum and maximum
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value either to enable or disable mode
 * \retval          1: Percentage mode was disabled ok
 * \retval          0: Percange mode was not disabled
 */
uint8_t
gui_progbar_setpercentmode(GUI_HANDLE_p h, uint8_t enable) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_PERCENT, &enable, 1, 0);    /* Set parameter */
}

/**
 * \brief           Set progress bar to animation mode
 * \param[in,out]   h: Widget handle
 * \param[in]       anim: New animation value either 1 (enable) or 0 (disable)
 * \retval          1: Animation was set ok
 * \retval          0: Animation vas not set
 */
uint8_t
gui_progbar_setanimation(GUI_HANDLE_p h, uint8_t anim) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_ANIM, &anim, 1, 0); /* Set parameter */
}

/**
 * \brief           Get progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \retval          Minimal value
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmax  
 */
int32_t
gui_progbar_getmin(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Min;                             /* Get minimal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Get progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \retval          Maximal value
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmin  
 */
int32_t
gui_progbar_getmax(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Max;                             /* Get maximal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Get progress bar current value
 * \param[in,out]   h: Widget handle
 * \retval          Current value
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getmin, gui_progbar_getmax  
 */
int32_t
gui_progbar_getvalue(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->DesiredValue;                    /* Get current value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

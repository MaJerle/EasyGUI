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
#include "gui/gui_private.h"
#include "widget/gui_progbar.h"

#define __GP(x)             ((GUI_PROGBAR_t *)(x))

#define GUI_PROGBAR_FLAG_PERCENT    0x01            /*!< Flag indicating percentages are enabled on view */
#define GUI_PROGBAR_FLAG_ANIMATE    0x02            /*!< Animation for progress bar changes */

#define CFG_VALUE           0x01
#define CFG_MIN             0x02
#define CFG_MAX             0x03
#define CFG_PERCENT         0x04
#define CFG_ANIM            0x05

static uint8_t gui_progbar_callback(gui_handle_p h, GUI_WC_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WIN_MIDDLEGRAY,                       /*!< Default background color */
    GUI_COLOR_WIN_DARKGRAY,                         /*!< Default freground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("PROGBAR"),                         /*!< Widget name */
    .size = sizeof(GUI_PROGBAR_t),                  /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_progbar_callback,               /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

#define p           ((GUI_PROGBAR_t *)h)
#define is_anim(h)  (!!(__GP(h)->Flags & GUI_PROGBAR_FLAG_ANIMATE))

/* Set value for widget */
static uint8_t
set_value(gui_handle_p h, int32_t val) {
    if (p->DesiredValue != val && val >= p->Min && val <= p->Max) { /* Value has changed */
        p->DesiredValue = val;                      /* Set value */
        if (p->CurrentValue < p->Min) {
            p->CurrentValue = p->Min;
        } else if (p->CurrentValue > p->Max) {
            p->CurrentValue = p->Max;
        }
        if (is_anim(h) && h->timer != NULL) {       /* In case of animation and timer availability */
            guii_timer_start(h->timer);             /* Start timer */
        } else {
            p->CurrentValue = p->DesiredValue;      /* Set values to the same */
        }
        guii_widget_invalidate(h);                  /* Redraw widget */
        guii_widget_callback(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        return 1;
    }
    return 0;
}

/* Timer callback for widget */
static void
timer_callback(gui_timer_t* t) {
    gui_handle_p h = guii_timer_getparams(t);       /* Get timer parameters */
    if (h) {
        if (p->CurrentValue != p->DesiredValue) {   /* Check difference */
            if (p->CurrentValue > p->DesiredValue) {
                p->CurrentValue--;
            } else {
                p->CurrentValue++;
            }
            guii_widget_invalidate(h);              /* Invalidate widget */
            guii_timer_start(t);                    /* Start timer */
        }
    }
}

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       ctr: Callback type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          1 if command processed, 0 otherwise
 */
static uint8_t
gui_progbar_callback(gui_handle_p h, GUI_WC_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GP(h)->Min = __GP(h)->CurrentValue = 0;
            __GP(h)->Max = 100;
            set_value(h, 50);
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            gui_widget_param* v = GUI_WIDGET_PARAMTYPE_WIDGETPARAM(param);
            int32_t tmp;
            switch (v->type) {
                case CFG_VALUE:                     /* Set current progress value */
                    set_value(h, *(int32_t *)v->data);
                    break;
                case CFG_MAX:                       /* Set maximal value */
                    tmp = *(int32_t *)v->data;
                    if (tmp > p->Min) {
                        p->Max = tmp;
                        if (p->DesiredValue > p->Max || p->CurrentValue > p->Max) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:                       /* Set minimal value */
                    tmp = *(int32_t *)v->data;
                    if (tmp < p->Max) {
                        p->Min = tmp;
                        if (p->DesiredValue < p->Min || p->CurrentValue < p->Min) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_PERCENT:                   /* Set percentage mode */
                    if (*(uint8_t *)v->data) {
                        p->Flags |= GUI_PROGBAR_FLAG_PERCENT;
                    } else {
                        p->Flags &= ~GUI_PROGBAR_FLAG_PERCENT;
                    }
                    break;
                case CFG_ANIM:
                    if (*(uint8_t *)v->data) {
                        if (h->timer == NULL) {
                            h->timer = guii_timer_create(10, timer_callback, h);    /* Create animation timer */
                        }
                        if (h->timer != NULL) { /* Check timer response */
                            __GP(h)->Flags |= GUI_PROGBAR_FLAG_ANIMATE; /* Enable animations */
                        }
                    } else {
                        __GP(h)->Flags &= ~GUI_PROGBAR_FLAG_ANIMATE;    /* Disable animation */
                        if (h->timer != NULL) {
                            guii_timer_remove(&h->timer);   /* Remove timer */
                        }
                        set_value(h, p->DesiredValue);  /* Reset value */
                    }
                    break;
                default: break;
            }
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            gui_dim_t x, y, w, width, height;
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
    
            x = guii_widget_getabsolutex(h);        /* Get absolute position on screen */
            y = guii_widget_getabsolutey(h);        /* Get absolute position on screen */
            width = guii_widget_getwidth(h);        /* Get widget width */
            height = guii_widget_getheight(h);      /* Get widget height */
           
            w = ((width - 4) * (p->CurrentValue - p->Min)) / (p->Max - p->Min); /* Get width for active part */
            
            gui_draw_filledrectangle(disp, x + w + 2, y + 2, width - w - 4, height - 4, guii_widget_getcolor(h, GUI_PROGBAR_COLOR_BG));
            gui_draw_filledrectangle(disp, x + 2, y + 2, w, height - 4, guii_widget_getcolor(h, GUI_PROGBAR_COLOR_FG));
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            
            /* Draw text if possible */
            if (h->font != NULL) {
                const gui_char* text = NULL;
                gui_char buff[5];
                
                if (p->Flags & GUI_PROGBAR_FLAG_PERCENT) {
                    sprintf((char *)buff, "%lu%%", (unsigned long)(((p->CurrentValue - p->Min) * 100) / (p->Max - p->Min)));
                    text = buff;
                } else if (guii_widget_isfontandtextset(h)) {
                    text = guii_widget_gettext(h);
                }
                
                if (text != NULL) {                 /* If text is valid, print it */
                    GUI_DRAW_FONT_t f;
                    gui_draw_font_init(&f);         /* Init structure */
                    
                    f.X = x + 2;
                    f.Y = y + 2;
                    f.Width = width - 4;
                    f.Height = height - 4;
                    f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.Color1Width = w ? w - 1 : 0;
                    f.Color1 = guii_widget_getcolor(h, GUI_PROGBAR_COLOR_BG);
                    f.Color2 = guii_widget_getcolor(h, GUI_PROGBAR_COLOR_FG);
                    gui_draw_writetext(disp, guii_widget_getfont(h), text, &f);
                }
            }
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

/**
 * \brief           Create new progress bar widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref gui_widget_callback_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for create procedure
 * \return          \ref gui_handle_p object of created widget on success, NULL otherwise
 */
gui_handle_p
gui_progbar_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_PROGBAR_COLOR_t enumeration
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setcolor(gui_handle_p h, GUI_PROGBAR_COLOR_t index, gui_color_t color) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Set progress bar current value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New current value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_progbar_setmin, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmin, gui_progbar_getmax  
 */
uint8_t
gui_progbar_setvalue(gui_handle_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_VALUE, &val, 1, 0); /* Set parameter */
}

/**
 * \brief           Set progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New minimal value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmin, gui_progbar_getmax         
 */
uint8_t
gui_progbar_setmin(gui_handle_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MIN, &val, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New maximal value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_getvalue, gui_progbar_getmin, gui_progbar_getmax  
 */
uint8_t
gui_progbar_setmax(gui_handle_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MAX, &val, 1, 0);   /* Set parameter */
}

/**
 * \brief           Sets percent mode. When in this mode, widget text is in percent according to current value between minimum and maximum
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value either to enable or disable mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setpercentmode(gui_handle_p h, uint8_t enable) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_PERCENT, &enable, 1, 0);    /* Set parameter */
}

/**
 * \brief           Set progress bar to animation mode
 * \param[in,out]   h: Widget handle
 * \param[in]       anim: New animation value either 1 (enable) or 0 (disable)
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setanimation(gui_handle_p h, uint8_t anim) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_ANIM, &anim, 1, 0); /* Set parameter */
}

/**
 * \brief           Get progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \return          Minimal value
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmax  
 */
int32_t
gui_progbar_getmin(gui_handle_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Min;                             /* Get minimal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Get progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \return          Maximal value
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getvalue, gui_progbar_getmin  
 */
int32_t
gui_progbar_getmax(gui_handle_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Max;                             /* Get maximal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Get progress bar current value
 * \param[in,out]   h: Widget handle
 * \return          Current value
 * \sa              gui_progbar_setmin, gui_progbar_setvalue, gui_progbar_setmax, gui_progbar_getmin, gui_progbar_getmax  
 */
int32_t
gui_progbar_getvalue(gui_handle_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->DesiredValue;                    /* Get current value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

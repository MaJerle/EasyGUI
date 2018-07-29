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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "widget/gui_progbar.h"

#define __GP(x)             ((gui_progbar_t *)(x))

#define GUI_PROGBAR_FLAG_PERCENT    0x01            /*!< Flag indicating percentages are enabled on view */
#define GUI_PROGBAR_FLAG_ANIMATE    0x02            /*!< Animation for progress bar changes */

#define CFG_VALUE           0x01
#define CFG_MIN             0x02
#define CFG_MAX             0x03
#define CFG_PERCENT         0x04
#define CFG_ANIM            0x05

static uint8_t gui_progbar_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

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
    .size = sizeof(gui_progbar_t),                  /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_progbar_callback,               /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

#define p           ((gui_progbar_t *)h)
#define is_anim(h)  (!!(__GP(h)->flags & GUI_PROGBAR_FLAG_ANIMATE))

/* Set value for widget */
static uint8_t
set_value(gui_handle_p h, int32_t val) {
    if (p->desiredvalue != val && val >= p->min && val <= p->max) { /* Value has changed */
        p->desiredvalue = val;                      /* Set value */
        if (p->currentvalue < p->min) {
            p->currentvalue = p->min;
        } else if (p->currentvalue > p->max) {
            p->currentvalue = p->max;
        }
        if (is_anim(h) && h->timer != NULL) {       /* In case of animation and timer availability */
            guii_timer_start(h->timer);             /* Start timer */
        } else {
            p->currentvalue = p->desiredvalue;      /* Set values to the same */
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
        if (p->currentvalue != p->desiredvalue) {   /* Check difference */
            if (p->currentvalue > p->desiredvalue) {
                p->currentvalue--;
            } else {
                p->currentvalue++;
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
gui_progbar_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GP(h)->min = __GP(h)->currentvalue = 0;
            __GP(h)->max = 100;
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
                    if (tmp > p->min) {
                        p->max = tmp;
                        if (p->desiredvalue > p->max || p->currentvalue > p->max) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:                       /* Set minimal value */
                    tmp = *(int32_t *)v->data;
                    if (tmp < p->max) {
                        p->min = tmp;
                        if (p->desiredvalue < p->min || p->currentvalue < p->min) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_PERCENT:                   /* Set percentage mode */
                    if (*(uint8_t *)v->data) {
                        p->flags |= GUI_PROGBAR_FLAG_PERCENT;
                    } else {
                        p->flags &= ~GUI_PROGBAR_FLAG_PERCENT;
                    }
                    break;
                case CFG_ANIM:
                    if (*(uint8_t *)v->data) {
                        if (h->timer == NULL) {
                            h->timer = guii_timer_create(10, timer_callback, h);    /* Create animation timer */
                        }
                        if (h->timer != NULL) { /* Check timer response */
                            __GP(h)->flags |= GUI_PROGBAR_FLAG_ANIMATE; /* Enable animations */
                        }
                    } else {
                        __GP(h)->flags &= ~GUI_PROGBAR_FLAG_ANIMATE;    /* Disable animation */
                        if (h->timer != NULL) {
                            guii_timer_remove(&h->timer);   /* Remove timer */
                        }
                        set_value(h, p->desiredvalue);  /* Reset value */
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
            width = gui_widget_getwidth(h, 0);      /* Get widget width */
            height = gui_widget_getheight(h, 0);    /* Get widget height */
           
            w = ((width - 4) * (p->currentvalue - p->min)) / (p->max - p->min); /* Get width for active part */
            
            gui_draw_filledrectangle(disp, x + w + 2, y + 2, width - w - 4, height - 4, guii_widget_getcolor(h, GUI_PROGBAR_COLOR_BG));
            gui_draw_filledrectangle(disp, x + 2, y + 2, w, height - 4, guii_widget_getcolor(h, GUI_PROGBAR_COLOR_FG));
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            
            /* Draw text if possible */
            if (h->font != NULL) {
                const gui_char* text = NULL;
                gui_char buff[5];
                
                if (p->flags & GUI_PROGBAR_FLAG_PERCENT) {
                    sprintf((char *)buff, "%lu%%", (unsigned long)(((p->currentvalue - p->min) * 100) / (p->max - p->min)));
                    text = buff;
                } else if (guii_widget_isfontandtextset(h)) {
                    text = gui_widget_gettext(h, 0);
                }
                
                if (text != NULL) {                 /* If text is valid, print it */
                    gui_draw_font_t f;
                    gui_draw_font_init(&f);         /* Init structure */
                    
                    f.x = x + 2;
                    f.y = y + 2;
                    f.width = width - 4;
                    f.height = height - 4;
                    f.align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.color1width = w ? w - 1 : 0;
                    f.color1 = guii_widget_getcolor(h, GUI_PROGBAR_COLOR_BG);
                    f.color2 = guii_widget_getcolor(h, GUI_PROGBAR_COLOR_FG);
                    gui_draw_writetext(disp, gui_widget_getfont(h, 0), text, &f);
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
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for create procedure
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_progbar_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags, protect);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setcolor(gui_handle_p h, gui_progbar_color_t index, gui_color_t color, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color, protect); /* Set color */
}

/**
 * \brief           Set progress bar current value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New current value
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setvalue(gui_handle_p h, int32_t val, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_VALUE, &val, 1, 0, protect); /* Set parameter */
}

/**
 * \brief           Set progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New minimal value
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise 
 */
uint8_t
gui_progbar_setmin(gui_handle_p h, int32_t val, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MIN, &val, 1, 0, protect);   /* Set parameter */
}

/**
 * \brief           Set progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New maximal value
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setmax(gui_handle_p h, int32_t val, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MAX, &val, 1, 0, protect);   /* Set parameter */
}

/**
 * \brief           Sets percent mode. When in this mode, widget text is in percent according to current value between minimum and maximum
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value either to enable or disable mode
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setpercentmode(gui_handle_p h, uint8_t enable, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_PERCENT, &enable, 1, 0, protect);/* Set parameter */
}

/**
 * \brief           Set progress bar to animation mode
 * \param[in,out]   h: Widget handle
 * \param[in]       anim: New animation value either `1` (enable) or `0` (disable)
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setanimation(gui_handle_p h, uint8_t anim, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_ANIM, &anim, 1, 0, protect); /* Set parameter */
}

/**
 * \brief           Get progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Minimal value
 */
int32_t
gui_progbar_getmin(gui_handle_p h, const uint8_t protect) {
    int32_t val;

    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    val = __GP(h)->min;                             /* Get minimal value */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return val;
}

/**
 * \brief           Get progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Maximal value
 */
int32_t
gui_progbar_getmax(gui_handle_p h, const uint8_t protect) {
    int32_t val;

    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    val = __GP(h)->max;                             /* Get maximal value */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return val;
}

/**
 * \brief           Get progress bar current value
 * \param[in,out]   h: Widget handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Current value
 */
int32_t
gui_progbar_getvalue(gui_handle_p h, const uint8_t protect) {
    int32_t val;

    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    val = __GP(h)->desiredvalue;                    /* Get current value */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return val;
}

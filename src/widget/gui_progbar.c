/**	
 * \file            gui_progbar.c
 * \brief           Progress bar widget
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "widget/gui_progbar.h"

/**
 * \ingroup         GUI_PROGBAR
 * \name            GUI_PROGBAR_FLAGS
 * \anchor          GUI_PROGBAR_FLAGS
 * \{
 */

#define GUI_FLAG_PROGBAR_PERCENT            0x01    /*!< Flag indicating percentages are enabled on view */
#define GUI_FLAG_PROGBAR_ANIMATE            0x02    /*!< Animation for progress bar changes */
    
/**
 * \}
 */

/**
 * \ingroup         GUI_PROGBAR
 * \brief           Progress bar object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    int32_t min;                                    /*!< Low value for progress bar */
    int32_t max;                                    /*!< High value for progress bar */
    int32_t currentvalue;                           /*!< Current value for progress bar */
    int32_t desiredvalue;                           /*!< Desired value, set by used */
    uint8_t flags;                                  /*!< flags variable */
} gui_progbar_t;

#define CFG_VALUE           0x01
#define CFG_MIN             0x02
#define CFG_MAX             0x03
#define CFG_PERCENT         0x04
#define CFG_ANIM            0x05

static uint8_t gui_progbar_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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

#define is_anim(o)  ((((gui_progbar_t *)(o))->flags & GUI_FLAG_PROGBAR_ANIMATE) == GUI_FLAG_PROGBAR_ANIMATE)

/* Set value for widget */
static uint8_t
set_value(gui_handle_p h, int32_t val) {
    gui_progbar_t* o = GUI_VP(h);
    if (o->desiredvalue != val) {                   /* Value has changed */
        o->desiredvalue = val;                      /* Set value */
        if (o->currentvalue < o->min) {
            o->currentvalue = o->min;
        } else if (o->currentvalue > o->max) {
            o->currentvalue = o->max;
        }
        if (is_anim(h) && h->timer != NULL) {       /* In case of animation and timer availability */
            guii_timer_start(h->timer);             /* Start timer */
        } else {
            o->currentvalue = o->desiredvalue;      /* Set values to the same */
        }
        gui_widget_invalidate(h);
        guii_widget_callback(h, GUI_EVT_VALUECHANGED, NULL, NULL);  /* Process callback */
        return 1;
    }
    return 0;
}

/* Timer callback for widget */
static void
timer_callback(gui_timer_t* t) {
    gui_handle_p h = guii_timer_getparams(t);       /* Get timer parameters */
    gui_progbar_t* o = GUI_VP(h);
    if (h != NULL) {
        if (o->currentvalue != o->desiredvalue) {   /* Check difference */
            if (o->currentvalue > o->desiredvalue) {
                o->currentvalue--;
            } else {
                o->currentvalue++;
            }
            gui_widget_invalidate(h);
            guii_timer_start(t);
        }
    }
}

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       evt: Event type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_progbar_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_progbar_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            o->min = o->currentvalue = 0;
            o->max = 100;
            set_value(h, 50);
            return 1;
        }
        case GUI_EVT_SETPARAM: {
            gui_widget_param* v = GUI_EVT_PARAMTYPE_WIDGETPARAM(param);
            int32_t tmp;
            switch (v->type) {
                case CFG_VALUE:
                    set_value(h, *(int32_t *)v->data);
                    break;
                case CFG_MAX:
                    tmp = *(int32_t *)v->data;
                    if (tmp > o->min) {
                        o->max = tmp;
                        if (o->desiredvalue > o->max || o->currentvalue > o->max) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:
                    tmp = *(int32_t *)v->data;
                    if (tmp < o->max) {
                        o->min = tmp;
                        if (o->desiredvalue < o->min || o->currentvalue < o->min) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_PERCENT:
                    if (*(uint8_t *)v->data) {
                        o->flags |= GUI_FLAG_PROGBAR_PERCENT;
                    } else {
                        o->flags &= ~GUI_FLAG_PROGBAR_PERCENT;
                    }
                    break;
                case CFG_ANIM:
                    if (*(uint8_t *)v->data) {
                        if (h->timer == NULL) {
                            h->timer = guii_timer_create(10, timer_callback, h);    /* Create animation timer */
                        }
                        if (h->timer != NULL) { /* Check timer response */
                            o->flags |= GUI_FLAG_PROGBAR_ANIMATE;   /* Enable animations */
                        }
                    } else {
                        o->flags &= ~GUI_FLAG_PROGBAR_ANIMATE;  /* Disable animation */
                        if (h->timer != NULL) {
                            guii_timer_remove(&h->timer);   /* Remove timer */
                        }
                        set_value(h, o->desiredvalue);  /* Reset value */
                    }
                    break;
                default: break;
            }
            GUI_EVT_RESULTTYPE_U8(result) = 1;      /* Save result */
            break;
        }
        case GUI_EVT_DRAW: {
            gui_dim_t x, y, w, width, height;
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
    
            x = gui_widget_getabsolutex(h);         /* Get absolute position on screen */
            y = gui_widget_getabsolutey(h);         /* Get absolute position on screen */
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
           
            w = ((width - 4) * (o->currentvalue - o->min)) / (o->max - o->min); /* Get width for active part */
            
            gui_draw_filledrectangle(disp, x + w + 2, y + 2, width - w - 4, height - 4, guii_widget_getcolor(h, GUI_PROGBAR_COLOR_BG));
            gui_draw_filledrectangle(disp, x + 2, y + 2, w, height - 4, guii_widget_getcolor(h, GUI_PROGBAR_COLOR_FG));
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            
            /* Draw text if possible */
            if (h->font != NULL) {
                const gui_char* text = NULL;
                gui_char buff[5];
                
                if (o->flags & GUI_FLAG_PROGBAR_PERCENT) {
                    sprintf((char *)buff, "%lu%%", (unsigned long)(((o->currentvalue - o->min) * 100) / (o->max - o->min)));
                    text = buff;
                } else if (gui_widget_isfontandtextset(h)) {
                    text = gui_widget_gettext(h);
                }
                
                if (text != NULL) {                 /* If text is valid, print it */
                    gui_draw_text_t f;
                    gui_draw_text_init(&f);         /* Init structure */
                    
                    f.x = x + 2;
                    f.y = y + 2;
                    f.width = width - 4;
                    f.height = height - 4;
                    f.align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.color1width = w ? w - 1 : 0;
                    f.color1 = guii_widget_getcolor(h, GUI_PROGBAR_COLOR_BG);
                    f.color2 = guii_widget_getcolor(h, GUI_PROGBAR_COLOR_FG);
                    gui_draw_writetext(disp, gui_widget_getfont(h), text, &f);
                }
            }
            break;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
    return 1;
}

/**
 * \brief           Create new progress bar widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       evt_fn: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for create procedure
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_progbar_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, evt_fn, flags);
}

/**
 * \brief           Set color to specific part of widget
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setcolor(gui_handle_p h, gui_progbar_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Set progress bar current value
 * \param[in]       h: Widget handle
 * \param[in]       val: New current value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setvalue(gui_handle_p h, int32_t val) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_VALUE, &val, 1, 0);
}

/**
 * \brief           Set progress bar minimal value
 * \param[in]       h: Widget handle
 * \param[in]       val: New minimal value
 * \return          `1` on success, `0` otherwise 
 */
uint8_t
gui_progbar_setmin(gui_handle_p h, int32_t val) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MIN, &val, 1, 0);
}

/**
 * \brief           Set progress bar maximal value
 * \param[in]       h: Widget handle
 * \param[in]       val: New maximal value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setmax(gui_handle_p h, int32_t val) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MAX, &val, 1, 0);
}

/**
 * \brief           Sets percent mode. When in this mode, widget text is in percent according to current value between minimum and maximum
 * \param[in]       h: Widget handle
 * \param[in]       enable: Value either to enable or disable mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setpercentmode(gui_handle_p h, uint8_t enable) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_PERCENT, &enable, 1, 0);
}

/**
 * \brief           Set progress bar to animation mode
 * \param[in]       h: Widget handle
 * \param[in]       anim: New animation value either `1` (enable) or `0` (disable)
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_progbar_setanimation(gui_handle_p h, uint8_t anim) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_ANIM, &anim, 1, 0);
}

/**
 * \brief           Get progress bar minimal value
 * \param[in]       h: Widget handle
 * \return          Minimal value
 */
int32_t
gui_progbar_getmin(gui_handle_p h) {
    gui_progbar_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->min;
}

/**
 * \brief           Get progress bar maximal value
 * \param[in]       h: Widget handle
 * \return          Maximal value
 */
int32_t
gui_progbar_getmax(gui_handle_p h) {
    gui_progbar_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->max;
}

/**
 * \brief           Get progress bar current value
 * \param[in]       h: Widget handle
 * \return          Current value
 */
int32_t
gui_progbar_getvalue(gui_handle_p h) {
    gui_progbar_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->desiredvalue;
}

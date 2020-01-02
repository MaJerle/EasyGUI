/**	
 * \file            gui_slider.c
 * \brief           Slider widget
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
#include "widget/gui_slider.h"

/**
 * \ingroup         GUI_SLIDER
 * \brief           Slider object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    int32_t min;                                    /*!< Low value for slider */
    int32_t max;                                    /*!< High value for slider */
    int32_t value;                                  /*!< Current value for slider */
    gui_slider_mode_t mode;                         /*!< Slider mode */
    
    uint8_t max_size;                               /*!< Max size for animation */
    uint8_t current_size;                           /*!< Current size for animation */
    
    uint8_t flags;                                  /*!< Widget flags */
} gui_slider_t;

#define CFG_MODE            0x01
#define CFG_VALUE           0x02
#define CFG_MIN             0x03
#define CFG_MAX             0x04

static uint8_t gui_slider_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WIN_BG,                               /*!< Default background color on non-active part */
    GUI_COLOR_WIN_BLUE,                             /*!< Default background color on active part */
    GUI_COLOR_WIN_RED,                              /*!< Default circle (...) color */
    GUI_COLOR_WIN_DARKGRAY,                         /*!< Default border color */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("SLIDER"),                          /*!< Widget name */
    .size = sizeof(gui_slider_t),                   /*!< Size of widget for memory allocation */
    .flags = GUI_FLAG_WIDGET_INVALIDATE_PARENT,     /*!< List of widget flags */
    .callback = gui_slider_callback,                /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Define number of colors */
};

/* Check if slider is horizontal */
#define is_horizontal(h)   (o->mode == GUI_SLIDER_MODE_LEFT_RIGHT || o->mode == GUI_SLIDER_MODE_RIGHT_LEFT)

/* Get delta value from widget */
static gui_dim_t
get_delta(gui_handle_p h, gui_dim_t wi, gui_dim_t he) {
    return wi > he ? he : wi;
}

/* Set slider value */
static uint8_t 
set_value(gui_handle_p h, int32_t value) {
    gui_slider_t* o = GUI_VP(h);
    if (value > o->max) {
        value = o->max;
    } else if (value < o->min) {
        value = o->min;
    }
    if (value != o->value) {                        /* Check difference in values */
        o->value = value;                           /* Set new value */
        guii_widget_callback(h, GUI_EVT_VALUECHANGED, NULL, NULL);  /* Callback process */
        return 1;
    }
    return 0;
}

/* Process touch event */
static uint8_t
touch_handle(gui_handle_p h, guii_touch_data_t* ts) {
    gui_slider_t* o = GUI_VP(h);
    gui_dim_t pos, delta, deltaH, width, height;
    int32_t value;
            
    width = ts->widget_width;
    height = ts->widget_height;
    delta = get_delta(h, ts->widget_width, ts->widget_height);  /* Get delta value */
    deltaH = delta >> 1;
    if (is_horizontal(h)) {                         /* Horizontal widget */
        if (o->mode == GUI_SLIDER_MODE_LEFT_RIGHT) {/* Inverted version to normal */
            pos = ts->x_rel[0];                     /* Invert position */
        } else {
            pos = width - ts->x_rel[0];
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
        value = (int32_t)(((float)(o->max - o->min)) * (float)pos / (float)width) + o->min;
    } else {                                        /* Vertical widget */
        if (o->mode == GUI_SLIDER_MODE_TOP_BOTTOM) {/* Inverted version to normal */
            pos = ts->y_rel[0];                     /* Invert position */
        } else {
            pos = height - ts->y_rel[0];
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
        value = (int32_t)(((float)(o->max - o->min)) * (float)pos / (float)height) + o->min;
    }
    
    gui_widget_invalidate(h);                       /* Redraw widget */
    return set_value(h, value);                     /* Set new value */
}

/* Timer callback function for slider widget */
static void
timer_callback(gui_timer_t* timer) {
    gui_slider_t* o;
    gui_handle_p h = guii_timer_getparams(timer);   /* Get user parameters */
    
    o = GUI_VP(h);
    
    if (guii_widget_isactive(o)) {                  /* Timer is in focus */
        if (o->current_size < o->max_size) {
            o->current_size++;
            gui_widget_invalidate(h);
        }
    } else {
        if (o->current_size > 0) {
            o->current_size--;
            gui_widget_invalidate(h);
        } else {
            guii_timer_stop(timer);
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
gui_slider_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_slider_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            o->min = 0;                             /* Set default minimal value */
            o->max = 100;                           /* Set default maximal value */
            o->value = 50;                          /* Set default value */
            
            o->max_size = 4;
            o->current_size = 0;
            h->timer = guii_timer_create(30, timer_callback, o);    /* Create timer for widget, when widget is deleted, timer will be automatically deleted too */
            if (h->timer == NULL) {                 /* Check if timer created */
                GUI_EVT_RESULTTYPE_U8(result) = 0;   /* Failed, widget will be deleted */
            }
            return 1;
        }
        case GUI_EVT_SETPARAM: {                     /* Set parameter for widget */
            gui_widget_param* v = GUI_EVT_PARAMTYPE_WIDGETPARAM(param);
            int32_t tmp;
            switch (v->type) {
                case CFG_MODE:                      /* Set current progress value */
                    o->mode = *(gui_slider_mode_t *)v->data;
                    break;
                case CFG_VALUE:                     /* Set current progress value */
                    set_value(h, *(int32_t *)v->data);
                    break;
                case CFG_MAX:                       /* Set maximal value */
                    tmp = *(int32_t *)v->data;
                    if (tmp > o->min) {
                        o->max = tmp;
                        if (o->value > o->max) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:                       /* Set minimal value */
                    tmp = *(int32_t *)v->data;
                    if (tmp < o->max) {
                        o->min = tmp;
                        if (o->value < o->min) {
                            set_value(h, tmp);
                        }
                    }
                    break;
                default: break;
            }
            GUI_EVT_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height, delta, deltaH, recParam, offset;
            gui_color_t c1, c2;
            gui_dim_t circleSize;

            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            delta = get_delta(h, width, height);    /* Get delta value */
            deltaH = delta >> 1;                    /* Half of delta */
            recParam = delta >> 2;                  /* Set rectangle height */
            if (recParam == 0) {                    /* Check value */
                recParam = 1;
            }
            
            /* Set color for inverted sliders */
            if (o->mode == GUI_SLIDER_MODE_BOTTOM_TOP || o->mode == GUI_SLIDER_MODE_RIGHT_LEFT) {
                c1 = guii_widget_getcolor(h, GUI_SLIDER_COLOR_BG_NONACTIVE);
                c2 = guii_widget_getcolor(h, GUI_SLIDER_COLOR_BG_ACTIVE);
            } else {
                c1 = guii_widget_getcolor(h, GUI_SLIDER_COLOR_BG_ACTIVE);
                c2 = guii_widget_getcolor(h, GUI_SLIDER_COLOR_BG_NONACTIVE);
            }
            
            circleSize = (deltaH >> 2) + o->current_size * (deltaH - (deltaH >> 2)) / o->max_size;    /* Get circle size */
            
            /* Draw bottom rectangle */
            if (is_horizontal(h)) {                 /* Horizontal slider */
                offset = 0;                         /* Make start offset */
                width -= delta;
                if (o->mode == GUI_SLIDER_MODE_RIGHT_LEFT) {/* Right left version */
                    offset = width - (gui_dim_t)(((float)(width) * (float)(o->value - o->min)) / (float)(o->max - o->min));
                } else {                            /* Left right version */
                    offset = (gui_dim_t)(((float)(width) * (float)(o->value - o->min)) / (float)(o->max - o->min));
                }
                x += deltaH;
                gui_draw_filledroundedrectangle(disp, x, y + ((delta - recParam) >> 1), offset, recParam, (recParam >> 1), c1);   
                gui_draw_filledroundedrectangle(disp, x + offset, y + ((delta - recParam) >> 1), width - offset, recParam, (recParam >> 1), c2);
                gui_draw_roundedrectangle(disp, x, y + ((delta - recParam) >> 1), width, recParam, (recParam >> 1), guii_widget_getcolor(h, GUI_SLIDER_COLOR_BORDER));
                gui_draw_filledcircle(disp, x + offset, y + deltaH, circleSize, guii_widget_getcolor(h, GUI_SLIDER_COLOR_FG));
                gui_draw_circle(disp, x + offset, y + deltaH, circleSize, guii_widget_getcolor(h, GUI_SLIDER_COLOR_BORDER));
            } else {                                /* Vertical slider */
                offset = 0;                         /* Make start offset */
                height -= delta;
                if (o->mode == GUI_SLIDER_MODE_BOTTOM_TOP) {    /* Bottom top version */
                    offset = height - (gui_dim_t)(((float)(height) * (float)o->value) / (float)(o->max - o->min));
                } else {                            /* Top bottom version */
                    offset = (gui_dim_t)(((float)(height) * (float)o->value) / (float)(o->max - o->min));
                }
                y += deltaH;
                gui_draw_filledroundedrectangle(disp, x + ((delta - recParam) >> 1), y, recParam, offset, (recParam >> 1), c1);   
                gui_draw_filledroundedrectangle(disp, x + ((delta - recParam) >> 1), y + offset, recParam, height - offset, (recParam >> 1), c2);
                gui_draw_roundedrectangle(disp, x + ((delta - recParam) >> 1), y, recParam, height, (recParam >> 1), guii_widget_getcolor(h, GUI_SLIDER_COLOR_BORDER)); 
                gui_draw_filledcircle(disp, x + deltaH, y + offset, circleSize, guii_widget_getcolor(h, GUI_SLIDER_COLOR_FG));
                gui_draw_circle(disp, x + deltaH, y + offset, circleSize, guii_widget_getcolor(h, GUI_SLIDER_COLOR_BORDER));
            }
            
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            touch_handle(h, GUI_EVT_PARAMTYPE_TOUCH(param));    /* Handle touch */
            
            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;  /* Set touch status */
            return 1;
        }
        case GUI_EVT_TOUCHMOVE: {
            GUI_EVT_RESULTTYPE_TOUCH(result) = touch_handle(h, GUI_EVT_PARAMTYPE_TOUCH(param)) ? touchHANDLED : touchCONTINUE;
            return 1;
        }
        case GUI_EVT_TOUCHEND:
            return 1;
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_EVT_ACTIVEIN: {
            guii_timer_startperiodic(h->timer);     /* Start animation timer */
            return 1;
        }
        case GUI_EVT_ACTIVEOUT: {
            gui_widget_invalidate(h);               /* Invalidate widget */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new slider widget
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
gui_slider_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
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
gui_slider_setcolor(gui_handle_p h, gui_slider_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Set slider mode (orientation)
 * \param[in]       h: Widget handle
 * \param[in]       mode: Slider mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_slider_setmode(gui_handle_p h, gui_slider_mode_t mode) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MODE, &mode, 1, 0);
}

/**
 * \brief           Set slider current value
 * \param[in]       h: Widget handle
 * \param[in]       val: New current value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_slider_setvalue(gui_handle_p h, int32_t val) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_VALUE, &val, 1, 0);
}

/**
 * \brief           Set slider minimal value
 * \param[in]       h: Widget handle
 * \param[in]       val: New minimal value
 * \return          `1` on success, `0` otherwise        
 */
uint8_t
gui_slider_setmin(gui_handle_p h, int32_t val) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MIN, &val, 1, 0);
}

/**
 * \brief           Set slider maximal value
 * \param[in]       h: Widget handle
 * \param[in]       val: New maximal value
 * \return          `1` on success, `0` otherwise 
 */
uint8_t
gui_slider_setmax(gui_handle_p h, int32_t val) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MAX, &val, 1, 0);
}

/**
 * \brief           Get slider minimal value
 * \param[in]       h: Widget handle
 * \return          Minimal value 
 */
int32_t
gui_slider_getmin(gui_handle_p h) {
    gui_slider_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->min;
}

/**
 * \brief           Get slider maximal value
 * \param[in]       h: Widget handle
 * \return          Maximal value  
 */
int32_t
gui_slider_getmax(gui_handle_p h) {
    gui_slider_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->max;
}

/**
 * \brief           Get slider current value
 * \param[in]       h: Widget handle
 * \return          Current value 
 */
int32_t
gui_slider_getvalue(gui_handle_p h) {
    gui_slider_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->value;
}

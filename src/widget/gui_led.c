/**	
 * \file            gui_led.c
 * \brief           LED widget
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
#include "widget/gui_led.h"

/**
 * \ingroup         GUI_LED
 * \name            GUI_LED_FLAGS
 * \anchor          GUI_LED_FLAGS
 * \{
 */

#define GUI_LED_FLAG_ON                     0x01    /*!< Indicates LED is on */

/**
 * \}
 */

/**
 * \ingroup         GUI_LED
 * \brief           LED object structure
 */
typedef struct {
    gui_handle C;                                   /*!< Global widget object */
    
    gui_led_type_t type;                            /*!< Led shape type */
    uint8_t flags;                                  /*!< flags management for LED */
} gui_led_t;

#define CFG_TOGGLE          0x01
#define CFG_SET             0x02
#define CFG_TYPE            0x03

static uint8_t gui_led_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_LIGHTBLUE,                            /*!< Default color when led is on */
    GUI_COLOR_DARKBLUE,                             /*!< Default color when led is off */
    GUI_COLOR_GRAY,                                 /*!< Default border color when led is on */
    GUI_COLOR_BLACK,                                /*!< Default border color when led is off */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("LED"),                             /*!< Widget name */ 
    .size = sizeof(gui_led_t),                      /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_led_callback,                   /*!< Control function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       evt: Event type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_led_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_led_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_SETPARAM: {                     /* Set parameter for widget */
            gui_widget_param* p = GUI_EVT_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_SET:
                    if (*(uint8_t *)p->data) {
                        o->flags |= GUI_LED_FLAG_ON;
                    } else {
                        o->flags &= ~GUI_LED_FLAG_ON;
                    }
                    break;
                case CFG_TOGGLE: 
                    o->flags ^= GUI_LED_FLAG_ON;    /* Toggle flag */
                    break;
                case CFG_TYPE: 
                    o->type = *(gui_led_type_t *)p->data;   /* Set type */
                    break;
                default: break;
            }
            GUI_EVT_RESULTTYPE_U8(result) = 1;      /* Save result */
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_color_t c1, c2;
            gui_dim_t x, y, width, height;
            
            x = gui_widget_getabsolutex(h);         /* Get absolute position on screen */
            y = gui_widget_getabsolutey(h);         /* Get absolute position on screen */
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            /* Get drawing colors */
            if (o->flags & GUI_LED_FLAG_ON) {       /* If LED is on */
                c1 = guii_widget_getcolor(h, GUI_LED_COLOR_ON);
                c2 = guii_widget_getcolor(h, GUI_LED_COLOR_ON_BORDER);
            } else {
                c1 = guii_widget_getcolor(h, GUI_LED_COLOR_OFF);
                c2 = guii_widget_getcolor(h, GUI_LED_COLOR_OFF_BORDER);
            }
            
            if (o->type == GUI_LED_TYPE_RECT) {     /* When led has rectangle shape */
                gui_draw_filledrectangle(disp, x + 1, y + 1, width - 2, height - 2, c1);
                gui_draw_rectangle(disp, x, y, width, height, c2);
            } else {
                gui_draw_filledcircle(disp, x + width / 2, y + height / 2, width / 2, c1);
                gui_draw_circle(disp, x + width / 2, y + height / 2, width / 2, c2);
            }
            return 1;                               /* */
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef l

/**
 * \brief           Create new led widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       evt_fn: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_led_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
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
gui_led_setcolor(gui_handle_p h, gui_led_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Set led type
 * \param[in]       h: Widget handle
 * \param[in]       type: New type for LED
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_led_settype(gui_handle_p h, gui_led_type_t type) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_TYPE, &type, 1, 1);
}

/**
 * \brief           Toggle LED state
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_led_toggle(gui_handle_p h) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_TOGGLE, NULL, 1, 0);
}

/**
 * \brief           Set LED value either on (turn on) or off (turn off)
 * \param[in]       h: Widget handle
 * \param[in]       state: LED state, either `1` or `0` according to selected state
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_led_set(gui_handle_p h, uint8_t state) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_SET, &state, 1, 0);
}

/**
 * \brief           Check if led is ON
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_led_ison(gui_handle_p h) {
    gui_led_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return (o->flags & GUI_LED_FLAG_ON) == GUI_LED_FLAG_ON;
}

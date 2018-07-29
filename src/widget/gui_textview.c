/**	
 * \file            gui_textview.c
 * \brief           Text view widget
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
#include "widget/gui_textview.h"

#define __GT(x)             ((gui_textview_t *)(x))

#define CFG_VALIGN          0x01
#define CFG_HALIGN          0x02

static uint8_t gui_textview_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT,                             /*!< Default text color */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("TEXTVIEW"),                        /*!< Widget name */
    .size = sizeof(gui_textview_t),                 /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_textview_callback,              /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Define number of colors */
};
#define o                   ((gui_textview_t *)(h))

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       ctr: Callback type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          1 if command processed, 0 otherwise
 */
static uint8_t
gui_textview_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            gui_widget_param* p = GUI_WIDGET_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_HALIGN: 
                    o->halign = *(gui_textalign_halign_t *)p->data;
                    break;
                case CFG_VALIGN: 
                    o->valign = *(gui_textalign_valign_t *)p->data;
                    break;
                default: break;
            }
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_dim_t x, y, wi, hi;
            gui_color_t bg;
            
            if (guii_widget_isfontandtextset(h)) {  /* Check if font is prepared for drawing */
                gui_draw_font_t f;
                
                x = guii_widget_getabsolutex(h);    /* Get absolute X coordinate */
                y = guii_widget_getabsolutey(h);    /* Get absolute Y coordinate */
                wi = gui_widget_getwidth(h, 0);     /* Get widget width */
                hi = gui_widget_getheight(h, 0);    /* Get widget height */
                
                /* Draw background if necessary */
                bg = guii_widget_getcolor(h, GUI_TEXTVIEW_COLOR_BG);
                if (bg != GUI_COLOR_TRANS) {
                    gui_draw_filledrectangle(disp, x, y, wi, hi, bg);
                }
                
                gui_draw_font_init(&f);             /* Init structure */
                
                f.x = x + 1;
                f.y = y + 1;
                f.width = wi - 2;
                f.height = hi - 2;
                f.align = (uint8_t)o->halign | (uint8_t)o->valign;
                f.flags |= GUI_FLAG_FONT_MULTILINE; /* Enable multiline */
                f.color1width = f.width;
                f.color1 = guii_widget_getcolor(h, GUI_TEXTVIEW_COLOR_TEXT);
                gui_draw_writetext(disp, gui_widget_getfont(h, 0), gui_widget_gettext(h, 0), &f);
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            guii_keyboard_data_t* kb = GUI_WIDGET_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (guii_widget_processtextkey(h, kb)) {
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_WC_Click: {
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new text view widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_textview_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect) {
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
gui_textview_setcolor(gui_handle_p h, gui_textview_color_t index, gui_color_t color, const uint8_t protect) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    ret = guii_widget_setcolor(h, (uint8_t)index, color, 1);/* Set color */
    if (ret && index == GUI_TEXTVIEW_COLOR_BG) {    /* Check success */
        guii_widget_setinvalidatewithparent(h, color == GUI_COLOR_TRANS);   /* When widget is invalidated, invalidate parent too */
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return ret;
}

/**
 * \brief           Set vertical align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_textview_setvalign(gui_handle_p h, gui_textalign_valign_t align, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_VALIGN, &align, 1, 1, protect);  /* Set parameter */
}

/**
 * \brief           Set horizontal align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Horizontal align
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_textview_sethalign(gui_handle_p h, gui_textalign_halign_t align, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_HALIGN, &align, 1, 1, protect);  /* Set parameter */
}

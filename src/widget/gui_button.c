/**	
 * \file            gui_button.c
 * \brief           Button widget
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
#include "widget/gui_button.h"

#define __GB(x)             ((gui_button_t *)(x))

#define CFG_BORDER_RADIUS   0x01

static uint8_t gui_button_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_GRAY,                                 /*!< Default background color index */
    GUI_COLOR_BLACK,                                /*!< Default foreground color index */
    GUI_COLOR_GRAY,                                 /*!< Default border color index in array */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("BUTTON"),                          /*!< Widget name */
    .size = sizeof(gui_button_t),                   /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_button_callback,                /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

#define b                   ((gui_button_t *)(h))

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       ctr: Callback type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          1 if command processed, 0 otherwise
 */
static uint8_t
gui_button_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            guii_widget_set3dstyle(h, 1);          /* By default set 3D */
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            gui_widget_param* p = GUI_WIDGET_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_BORDER_RADIUS: b->borderradius = *(gui_dim_t *)p->data; break;
                default: break;
            }
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_color_t c1, c2;
            gui_dim_t x, y, width, height;
            
            x = guii_widget_getabsolutex(h);       /* Get absolute X coordinate */
            y = guii_widget_getabsolutey(h);       /* Get absolute Y coordinate */
            width = guii_widget_getwidth(h);       /* Get widget width */
            height = guii_widget_getheight(h);     /* Get widget height */
            
            if (guii_widget_getflag(h, GUI_FLAG_3D)) {
                c1 = guii_widget_getcolor(h, GUI_BUTTON_COLOR_BG);
                c2 = guii_widget_getcolor(h, GUI_BUTTON_COLOR_FG);
            } else {
                if (guii_widget_getflag(h, GUI_FLAG_ACTIVE)) { /* Set colors for button */
                    c1 = guii_widget_getcolor(h, GUI_BUTTON_COLOR_FG);
                    c2 = guii_widget_getcolor(h, GUI_BUTTON_COLOR_BG);
                } else {
                    c2 = guii_widget_getcolor(h, GUI_BUTTON_COLOR_FG);
                    c1 = guii_widget_getcolor(h, GUI_BUTTON_COLOR_BG);
                }
            }
            
            /* Draw actual button structure */
            if (guii_widget_getflag(h, GUI_FLAG_3D)) {
                gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, c1);
                gui_draw_rectangle3d(disp, x, y, width, height, guii_widget_getflag(h, GUI_FLAG_ACTIVE) ? GUI_DRAW_3D_State_Lowered : GUI_DRAW_3D_State_Raised);
            } else {
                gui_draw_filledroundedrectangle(disp, x, y, width, height, b->borderradius, c1);
                gui_draw_roundedrectangle(disp, x, y, width, height, b->borderradius, c2);
            }
            
            /* Draw text if possible */
            if (guii_widget_isfontandtextset(h)) {
                gui_draw_font_t f;
                gui_draw_font_init(&f);             /* Init structure */
                
                f.x = x + 1;
                f.y = y + 1;
                f.width = width - 2;
                f.height = height - 2;
                f.align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                f.color1 = c2;
                gui_draw_writetext(disp, guii_widget_getfont(h), guii_widget_gettext(h), &f);
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            guii_keyboard_data_t* kb = GUI_WIDGET_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_CR || kb->kb.keys[0] == GUI_KEY_LF) {
                guii_widget_callback(h, GUI_WC_Click, NULL, NULL); /* Process click */
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_WC_ActiveIn:
        case GUI_WC_ActiveOut: {
            guii_widget_invalidate(h);              /* Invalidate widget */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef b

/**
 * \brief           Create new button widget
 * \param[in]       id: Widget unique ID to use for identity at callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          \ref widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_button_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_button_setcolor(gui_handle_p h, gui_button_color_t index, gui_color_t color) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color, 1);   /* Set color */
}

/**
 * \brief           Set border radius size
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Border radius size
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_button_setborderradius(gui_handle_p h, gui_dim_t size) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_BORDER_RADIUS, &size, 1, 1, 1);  /* Set parameter */
}

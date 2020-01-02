/**	
 * \file            gui_button.c
 * \brief           Button widget
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
#include "widget/gui_button.h"

/**
 * \ingroup         GUI_BUTTON
 * \brief           Button object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    gui_dim_t borderwidth;                          /*!< Border width */
    gui_dim_t borderradius;                         /*!< Border radius */
} gui_button_t;

#define CFG_BORDER_RADIUS   0x01

static uint8_t gui_button_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       evt: Event type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_button_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_button_t* b = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            guii_widget_setflag(h, GUI_FLAG_3D);    /* By default set 3D */
            return 1;
        }
        case GUI_EVT_SETPARAM: {                    /* Set parameter for widget */
            gui_widget_param* p = GUI_EVT_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_BORDER_RADIUS: b->borderradius = *(gui_dim_t *)p->data; break;
                default: break;
            }
            GUI_EVT_RESULTTYPE_U8(result) = 1;      /* Save result */
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_color_t c1, c2;
            gui_dim_t x, y, width, height;
            
            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
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
            if (gui_widget_isfontandtextset(h)) {
                gui_draw_text_t f;
                gui_draw_text_init(&f);             /* Init structure */
                
                f.x = x + 1;
                f.y = y + 1;
                f.width = width - 2;
                f.height = height - 2;
                f.align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                f.color1 = c2;
                gui_draw_writetext(disp, gui_widget_getfont(h), gui_widget_gettext(h), &f);
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_EVT_KEYPRESS: {
            guii_keyboard_data_t* kb = GUI_EVT_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_CR || kb->kb.keys[0] == GUI_KEY_LF) {
                guii_widget_callback(h, GUI_EVT_CLICK, NULL, NULL); /* Process click */
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_EVT_ACTIVEIN:
        case GUI_EVT_ACTIVEOUT: {
            gui_widget_invalidate(h);               /* Invalidate widget */
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
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       evt_fn: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_button_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
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
gui_button_setcolor(gui_handle_p h, gui_button_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Set border radius size
 * \param[in]       h: Widget handle
 * \param[in]       size: Border radius size
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_button_setborderradius(gui_handle_p h, gui_dim_t size) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_BORDER_RADIUS, &size, 1, 1); /* Set parameter */
}

/**
 * \brief           Set 3D mode on widget
 * \param[in]       h: Widget handle
 * \param[in]       enable: Set to `1` to enable 3D mode, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_button_set3dstyle(gui_handle_p h, uint8_t enable) {    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    
    if (enable && !guii_widget_getflag(h, GUI_FLAG_3D)) {  /* Enable style */
        guii_widget_setflag(h, GUI_FLAG_3D);        /* Enable 3D style */
        gui_widget_invalidate(h);                   /* Invalidate object */
    } else if (!enable && guii_widget_getflag(h, GUI_FLAG_3D)) {/* Disable style */
        guii_widget_clrflag(h, GUI_FLAG_3D);        /* Disable 3D style */
        gui_widget_invalidate(h);                   /* Invalidate object */
    }
    
    return 1;
}

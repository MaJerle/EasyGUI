/**	
 * \file            gui_edittext.c
 * \brief           Edittext widget
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
#include "widget/gui_edittext.h"
#include "widget/gui_widget.h"

#if GUI_CFG_USE_KEYBOARD
#include "gui/gui_keyboard.h"
#endif /* GUI_CFG_USE_KEYBOARD */

/**
 * \ingroup         GUI_EDITTEXT
 * \name            GUI_EDITTEXT_FLAGS
 * \anchor          GUI_EDITTEXT_FLAGS
 * \{
 */

#define GUI_EDITTEXT_FLAG_MULTILINE             0x01    /*!< Defines widget as multi-line edit */
    
/**
 * \}
 */

/**
 * \ingroup         GUI_EDITTEXT
 * \brief           Edittext object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    uint8_t flags;                                  /*!< List of widget flags */
    gui_edittext_valign_t valign;                   /*!< Vertical align setup */
    gui_edittext_halign_t halign;                   /*!< Horizontal align setup */
} gui_edittext_t;

#define CFG_MULTILINE       0x01
#define CFG_VALIGN          0x02
#define CFG_HALIGN          0x03
    
static uint8_t gui_edittext_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color index */
    GUI_COLOR_BLACK,                                /*!< Default foreground color index */
    GUI_COLOR_BLACK,                                /*!< Default border color index */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("EDITTEXT"),                        /*!< Widget name */
    .size = sizeof(gui_edittext_t),                 /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_edittext_callback,              /*!< Control function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

/* Check if edit text is multiline */
#define is_multiline(o)            (o->flags & GUI_EDITTEXT_FLAG_MULTILINE)

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       evt: Event type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_edittext_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_edittext_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            o->valign = GUI_EDITTEXT_VALIGN_CENTER;
            o->halign = GUI_EDITTEXT_HALIGN_LEFT;
            return 1;
        }
        case GUI_EVT_SETPARAM: {                    /* Set parameter for widget */
            gui_widget_param* p = GUI_EVT_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_MULTILINE:
                    if (*(uint8_t *)p->data && !is_multiline(h)) {
                        o->flags |= GUI_EDITTEXT_FLAG_MULTILINE;
                    } else if (!*(uint8_t *)p->data && is_multiline(h)) {
                        o->flags &= ~GUI_EDITTEXT_FLAG_MULTILINE;
                    }
                    break; /* Set max X value to widget */
                case CFG_HALIGN: 
                    o->halign = *(gui_edittext_halign_t *)p->data;
                    break;
                case CFG_VALIGN: 
                    o->valign = *(gui_edittext_valign_t *)p->data;
                    break;
                default: break;
            }
            GUI_EVT_RESULTTYPE_U8(result) = 1;
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_dim_t x, y, width, height;
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
    
            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, guii_widget_getcolor(h, GUI_EDITTEXT_COLOR_BG));
            
            if (guii_widget_isfocused(h)) {         /* Check if widget is in focus */
                gui_draw_rectangle(disp, x + 3, y + 3, width - 6, height - 6, guii_widget_getcolor(h, GUI_EDITTEXT_COLOR_BORDER));
            }
            
            if (gui_widget_isfontandtextset(h)) {   /* Ready to write string */
                gui_draw_text_t f;
                gui_draw_text_init(&f);             /* Init font drawing */
                
                f.x = x + 5;
                f.y = y + 5;
                f.width = width - 10;
                f.height = height - 10;
                f.align = (uint8_t)o->halign | (uint8_t)o->valign;
                f.color1width = f.width;
                f.color1 = guii_widget_getcolor(h, GUI_EDITTEXT_COLOR_FG);
                f.flags |= GUI_FLAG_TEXT_RIGHTALIGN | GUI_FLAG_TEXT_EDITMODE;
                
                if (is_multiline(h)) {
                    f.flags |= GUI_FLAG_TEXT_MULTILINE; /* Set multiline flag for widget */
                }
                
                gui_draw_writetext(disp, gui_widget_getfont(h), gui_widget_gettext(h), &f);
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_EVT_FOCUSIN:
            gui_keyboard_show(h);
            return 1;
        case GUI_EVT_FOCUSOUT:
            gui_keyboard_hide();
            return 1;
        case GUI_EVT_KEYPRESS: {
            guii_keyboard_data_t* kb = GUI_EVT_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (guii_widget_processtextkey(h, kb)) {
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

#undef e

/**
 * \brief           Create new edit text widget
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
gui_edittext_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
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
gui_edittext_setcolor(gui_handle_p h, gui_edittext_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Set edit text to multi line mode
 * \note            When multiline is enabled, vertical text alignment is always top positioned
 * \param[in]       h: Widget handle
 * \param[in]       multiline: Set to `1` to enable multiline or `0` to disable
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_edittext_setmultiline(gui_handle_p h, uint8_t multiline) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MULTILINE, &multiline, 1, 0);
}

/**
 * \brief           Set vertical align for text inside text box
 * \param[in]       h: Widget handle
 * \param[in]       align: Vertical align
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_edittext_setvalign(gui_handle_p h, gui_edittext_valign_t align) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_VALIGN, &align, 1, 1);
}

/**
 * \brief           Set horizontal align for text inside text box
 * \param[in]       h: Widget handle
 * \param[in]       align: Horizontal align
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_edittext_sethalign(gui_handle_p h, gui_edittext_halign_t align) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_HALIGN, &align, 1, 1);
}

/**	
 * \file            gui_edittext.c
 * \brief           Edittext widget
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
#include "gui_edittext.h"

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
#define __GE(x)             ((GUI_EDITTEXT_t *)(x))
    
#define CFG_MULTILINE       0x01
#define CFG_VALIGN          0x02
#define CFG_HALIGN          0x03
    
static uint8_t gui_edittext_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color index */
    GUI_COLOR_BLACK,                                /*!< Default foreground color index */
    GUI_COLOR_BLACK,                                /*!< Default border color index */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("EDITTEXT"),                        /*!< Widget name */
    .Size = sizeof(GUI_EDITTEXT_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_edittext_callback,              /*!< Control function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define e          ((GUI_EDITTEXT_t *)h)

/* Check if edit text is multiline */
#define is_multiline(h)            (__GE(h)->Flags & GUI_EDITTEXT_FLAG_MULTILINE)

/* Widget callback */
static uint8_t
gui_edittext_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GE(h)->VAlign = GUI_EDITTEXT_VALIGN_CENTER;
            __GE(h)->HAlign = GUI_EDITTEXT_HALIGN_LEFT;
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* p = GUI_WIDGET_PARAMTYPE_WIDGETPARAM(param);
            switch (p->Type) {
                case CFG_MULTILINE:                 /* Enable/Disable multiline */
                    if (*(uint8_t *)p->Data && !is_multiline(h)) {
                        __GE(h)->Flags |= GUI_EDITTEXT_FLAG_MULTILINE;
                    } else if (!*(uint8_t *)p->Data && is_multiline(h)) {
                        __GE(h)->Flags &= ~GUI_EDITTEXT_FLAG_MULTILINE;
                    }
                    break; /* Set max X value to widget */
                case CFG_HALIGN: 
                    e->HAlign = *(GUI_EDITTEXT_HALIGN_t *)p->Data;
                    break;
                case CFG_VALIGN: 
                    e->VAlign = *(GUI_EDITTEXT_VALIGN_t *)p->Data;
                    break;
                default: break;
            }
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Dim_t x, y, width, height;
            GUI_Display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
    
            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, gui_widget_getcolor__(h, GUI_EDITTEXT_COLOR_BG));
            
            if (gui_widget_isfocused__(h)) {        /* Check if widget is in focus */
                gui_draw_rectangle(disp, x + 3, y + 3, width - 6, height - 6, gui_widget_getcolor__(h, GUI_EDITTEXT_COLOR_BORDER));
            }
            
            if (gui_widget_isfontandtextset__(h)) { /* Ready to write string */
                GUI_DRAW_FONT_t f;
                gui_draw_font_init(&f);             /* Init font drawing */
                
                f.X = x + 5;
                f.Y = y + 5;
                f.Width = width - 10;
                f.Height = height - 10;
                f.Align = (uint8_t)__GE(h)->HAlign | (uint8_t)__GE(h)->VAlign;
                f.Color1Width = f.Width;
                f.Color1 = gui_widget_getcolor__(h, GUI_EDITTEXT_COLOR_FG);
                f.Flags |= GUI_FLAG_FONT_RIGHTALIGN | GUI_FLAG_FONT_EDITMODE;
                
                if (is_multiline(h)) {
                    f.Flags |= GUI_FLAG_FONT_MULTILINE; /* Set multiline flag for widget */
                }
                
                gui_draw_writetext(disp, gui_widget_getfont__(h), gui_widget_gettext__(h), &f);
            }
            return 1;
        }
        case GUI_WC_FocusIn:
#if GUI_USE_KEYBOARD
            gui_keyboard_show__(h);                 /* Show keyboard */
#endif /* GUI_USE_KEYBOARD */
            return 1;
        case GUI_WC_FocusOut:
#if GUI_USE_KEYBOARD
            gui_keyboard_hide__();                  /* Hide keyboard */
#endif /* GUI_USE_KEYBOARD */
            return 1;
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = GUI_WIDGET_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (gui_widget_processtextkey__(h, kb)) {
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

#undef e

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new edit text widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p
gui_edittext_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_EDITTEXT_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_edittext_setcolor(GUI_HANDLE_p h, GUI_EDITTEXT_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color);  /* Set color */
}

/**
 * \brief           Set edit text to multi line mode
 * \note            When multiline is enabled, vertical text alignment is always top positioned
 * \param[in,out]   h: Widget handle
 * \param[in]       multiline: Set to 1 to enable multiline or 0 to disable
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_edittext_setmultiline(GUI_HANDLE_p h, uint8_t multiline) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MULTILINE, &multiline, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set vertical align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align. This parameter can be a value of \ref GUI_EDITTEXT_VALIGN_t enumeration
 * \retval          1: Align was set ok
 * \retval          0: Align was not set
 */
uint8_t
gui_edittext_setvalign(GUI_HANDLE_p h, GUI_EDITTEXT_VALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_VALIGN, &align, 1, 1);  /* Set parameter */
}

/**
 * \brief           Set horizontal align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align. This parameter can be a value of \ref GUI_EDITTEXT_HALIGN_t enumeration
 * \retval          1: Align was set ok
 * \retval          0: Align was not set
 */
uint8_t
gui_edittext_sethalign(GUI_HANDLE_p h, GUI_EDITTEXT_HALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_HALIGN, &align, 1, 1);  /* Set parameter */
}

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
#include "gui_button.h"

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
#define __GB(x)             ((GUI_BUTTON_t *)(x))

#define CFG_BORDER_RADIUS   0x01

static uint8_t gui_button_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_GRAY,                                 /*!< Default background color index */
    GUI_COLOR_BLACK,                                /*!< Default foreground color index */
    GUI_COLOR_GRAY,                                 /*!< Default border color index in array */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("BUTTON"),                          /*!< Widget name */
    .Size = sizeof(GUI_BUTTON_t),                   /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_button_callback,                /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define b                   ((GUI_BUTTON_t *)(h))

static uint8_t
gui_button_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            gui_widget_set3dstyle__(h, 1);          /* By default set 3D */
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* p = (GUI_WIDGET_Param_t *)param;
            switch (p->Type) {
                case CFG_BORDER_RADIUS: b->BorderRadius = *(GUI_Dim_t *)p->Data; break; /* Set max X value to widget */
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Color_t c1, c2;
            GUI_Dim_t x, y, width, height;
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            if (gui_widget_getflag__(h, GUI_FLAG_3D)) {
                c1 = gui_widget_getcolor__(h, GUI_BUTTON_COLOR_BG);
                c2 = gui_widget_getcolor__(h, GUI_BUTTON_COLOR_FG);
            } else {
                if (gui_widget_getflag__(h, GUI_FLAG_ACTIVE)) { /* Set colors for button */
                    c1 = gui_widget_getcolor__(h, GUI_BUTTON_COLOR_FG);
                    c2 = gui_widget_getcolor__(h, GUI_BUTTON_COLOR_BG);
                } else {
                    c2 = gui_widget_getcolor__(h, GUI_BUTTON_COLOR_FG);
                    c1 = gui_widget_getcolor__(h, GUI_BUTTON_COLOR_BG);
                }
            }
            
            /* Draw actual button structure */
            if (gui_widget_getflag__(h, GUI_FLAG_3D)) {
                gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, c1);
                gui_draw_rectangle3d(disp, x, y, width, height, gui_widget_getflag__(h, GUI_FLAG_ACTIVE) ? GUI_DRAW_3D_State_Lowered : GUI_DRAW_3D_State_Raised);
            } else {
                gui_draw_filledroundedrectangle(disp, x, y, width, height, b->BorderRadius, c1);
                gui_draw_roundedrectangle(disp, x, y, width, height, b->BorderRadius, c2);
            }
            
            /* Draw text if possible */
            if (gui_widget_isfontandtextset__(h)) {
                GUI_DRAW_FONT_t f;
                gui_draw_font_init(&f);             /* Init structure */
                
                f.X = x + 1;
                f.Y = y + 1;
                f.Width = width - 2;
                f.Height = height - 2;
                f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                f.Color1 = c2;
                gui_draw_writetext(disp, gui_widget_getfont__(h), gui_widget_gettext__(h), &f);
            }
            return 1;
        }
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (kb->KB.Keys[0] == GUI_KEY_CR || kb->KB.Keys[0] == GUI_KEY_LF) {
                gui_widget_callback__(h, GUI_WC_Click, 0, 0);   /* Process click */
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED; /* Key handled */
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
        case GUI_WC_ActiveIn:
        case GUI_WC_ActiveOut: {
            gui_widget_invalidate__(h);             /* Invalidate widget */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef b

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new button widget
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
gui_button_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_BUTTON_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_button_setcolor(GUI_HANDLE_p h, GUI_BUTTON_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Set border radius size
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Border radius size
 * \retval          1: Border radius was set ok
 * \retval          0: Border radius was not set
 */
uint8_t
gui_button_setborderradius(GUI_HANDLE_p h, GUI_Dim_t size) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_BORDER_RADIUS, &size, 1, 1);    /* Set parameter */
}

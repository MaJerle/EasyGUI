/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_textview.h"

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
#define __GT(x)             ((GUI_TEXTVIEW_t *)(x))

#define CFG_VALIGN          0x01
#define CFG_HALIGN          0x02

static uint8_t gui_textview_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT,                             /*!< Default text color */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("TEXTVIEW"),                        /*!< Widget name */
    .Size = sizeof(GUI_TEXTVIEW_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_textview_callback,              /*!< Callback function */
    .Colors = Colors,
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o                   ((GUI_TEXTVIEW_t *)(h))

static uint8_t
gui_textview_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* p = (GUI_WIDGET_Param_t *)param;
            switch (p->Type) {
                case CFG_HALIGN: 
                    o->HAlign = *(GUI_TEXTVIEW_HALIGN_t *)p->Data;
                    break;
                case CFG_VALIGN: 
                    o->VAlign = *(GUI_TEXTVIEW_VALIGN_t *)p->Data;
                    break;
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, wi, hi;
            GUI_Color_t bg;
            
            if (gui_widget_isfontandtextset__(h)) { /* Check if font is prepared for drawing */
                GUI_DRAW_FONT_t f;
                
                x = gui_widget_getabsolutex__(h);   /* Get absolute X coordinate */
                y = gui_widget_getabsolutey__(h);   /* Get absolute Y coordinate */
                wi = gui_widget_getwidth__(h);      /* Get widget width */
                hi = gui_widget_getheight__(h);     /* Get widget height */
                
                /* Draw background if necessary */
                bg = gui_widget_getcolor__(h, GUI_TEXTVIEW_COLOR_BG);
                if (bg != GUI_COLOR_TRANS) {
                    gui_draw_filledrectangle(disp, x, y, wi, hi, bg);
                }
                
                gui_draw_font_init(&f);             /* Init structure */
                
                f.X = x + 1;
                f.Y = y + 1;
                f.Width = wi - 2;
                f.Height = hi - 2;
                f.Align = (uint8_t)o->HAlign | (uint8_t)o->VAlign;
                f.Flags |= GUI_FLAG_FONT_MULTILINE; /* Enable multiline */
                f.Color1Width = f.Width;
                f.Color1 = gui_widget_getcolor__(h, GUI_TEXTVIEW_COLOR_TEXT);
                gui_draw_writetext(disp, gui_widget_getfont__(h), gui_widget_gettext__(h), &f);
            }
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Touch has been handled */
            return 1;
        }
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (gui_widget_processtextkey__(h, kb)) {
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED; /* Key handled */
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
        case GUI_WC_Click: {
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o
/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new text view widget
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
gui_textview_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_TEXTVIEW_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_textview_setcolor(GUI_HANDLE_p h, GUI_TEXTVIEW_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    ret = gui_widget_setcolor__(h, (uint8_t)index, color);  /* Set color */
    
    if (ret) {                                      /* Check success */
        if (index == GUI_TEXTVIEW_COLOR_BG) {       /* If background is transparent */
            __GUI_ENTER();
            gui_widget_setinvalidatewithparent__(h, color == GUI_COLOR_TRANS);  /* When widget is invalidated, invalidate parent too */
            __GUI_LEAVE();
        }
    }
    return ret;
}

/**
 * \brief           Set vertical align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align. This parameter can be a value of \ref GUI_TEXTVIEW_VALIGN_t enumeration
 * \retval          1: Align was set ok
 * \retval          0: Align was not set
 */
uint8_t
gui_textview_setvalign(GUI_HANDLE_p h, GUI_TEXTVIEW_VALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_VALIGN, &align, 1, 1);  /* Set parameter */
}

/**
 * \brief           Set horizontal align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align. This parameter can be a value of \ref GUI_TEXTVIEW_HALIGN_t enumeration
 * \retval          1: Align was set ok
 * \retval          0: Align was not set
 */
uint8_t
gui_textview_sethalign(GUI_HANDLE_p h, GUI_TEXTVIEW_HALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_HALIGN, &align, 1, 1);  /* Set parameter */
}

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
    
static
uint8_t gui_edittext_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
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
void TimerCallback(GUI_TIMER_t* timer) {
    GUI_EDITTEXT_t* edit = __GE(gui_timer_getparams__(timer));  /* Get parameters from timer */
    
    gui_widget_invalidate__(__GH(edit));            /* Invalidate widget */
}

#define e          ((GUI_EDITTEXT_t *)h)
/* Check if edit text is multiline */
#define __IsMultiline(h)            (__GE(h)->Flags & GUI_EDITTEXT_FLAG_MULTILINE)

/* Widget callback */
static
uint8_t gui_edittext_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GE(h)->VAlign = GUI_EDITTEXT_VALIGN_CENTER;
            __GE(h)->HAlign = GUI_EDITTEXT_HALIGN_LEFT;
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* p = (GUI_WIDGET_Param_t *)param;
            switch (p->Type) {
                case CFG_MULTILINE:                 /* Enable/Disable multiline */
                    if (*(uint8_t *)p->Data && !__IsMultiline(h)) {
                        __GE(h)->Flags |= GUI_EDITTEXT_FLAG_MULTILINE;
                    } else if (!*(uint8_t *)p->Data && __IsMultiline(h)) {
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
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Dim_t x, y, width, height;
            GUI_Display_t* disp = (GUI_Display_t *)param;
    
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
                
                if (__IsMultiline(h)) {
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
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
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
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

#undef e

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p gui_edittext_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

uint8_t gui_edittext_setcolor(GUI_HANDLE_p h, GUI_EDITTEXT_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color);  /* Set color */
}

uint8_t gui_edittext_setmultiline(GUI_HANDLE_p h, uint8_t multiline) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MULTILINE, &multiline, 1, 0);   /* Set parameter */
}

uint8_t gui_edittext_setvalign(GUI_HANDLE_p h, GUI_EDITTEXT_VALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_VALIGN, &align, 1, 1);  /* Set parameter */
}

uint8_t gui_edittext_sethalign(GUI_HANDLE_p h, GUI_EDITTEXT_HALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_HALIGN, &align, 1, 1);  /* Set parameter */
}

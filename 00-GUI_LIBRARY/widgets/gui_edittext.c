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
    
static
uint8_t GUI_EDITTEXT_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_Color_t Colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color index */
    GUI_COLOR_BLACK,                                /*!< Default foreground color index */
    GUI_COLOR_BLACK,                                /*!< Default border color index */
};

const static GUI_WIDGET_t Widget = {
    .Name = _GT("EDITTEXT"),                        /*!< Widget name */
    .Size = sizeof(GUI_EDITTEXT_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_EDITTEXT_Callback,              /*!< Control function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
void TimerCallback(GUI_TIMER_t* timer) {
    GUI_EDITTEXT_t* edit = __GE(__GUI_TIMER_GetParams(timer));  /* Get parameters from timer */
    
    __GUI_WIDGET_Invalidate(__GH(edit));            /* Invalidate widget */
}

#define e          ((GUI_EDITTEXT_t *)h)
/* Check if edit text is multiline */
#define __IsMultiline(h)            (__GE(h)->Flags & GUI_EDITTEXT_FLAG_MULTILINE)

/* Widget callback */
static
uint8_t GUI_EDITTEXT_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GE(h)->VAlign = GUI_EDITTEXT_VALIGN_CENTER;
            __GE(h)->HAlign = GUI_EDITTEXT_HALIGN_LEFT;
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Dim_t x, y, width, height;
            GUI_Display_t* disp = (GUI_Display_t *)param;
    
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            GUI_DRAW_Rectangle3D(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, width - 4, height - 4, __GUI_WIDGET_GetColor(h, GUI_EDITTEXT_COLOR_BG));
            
            if (__GUI_WIDGET_IsFocused(h)) {        /* Check if widget is in focus */
                GUI_DRAW_Rectangle(disp, x + 3, y + 3, width - 6, height - 6, __GUI_WIDGET_GetColor(h, GUI_EDITTEXT_COLOR_BORDER));
            }
            
            if (__GUI_WIDGET_IsFontAndTextSet(h)) { /* Ready to write string */
                GUI_DRAW_FONT_t f;
                GUI_DRAW_FONT_Init(&f);             /* Init font drawing */
                
                f.X = x + 5;
                f.Y = y + 5;
                f.Width = width - 10;
                f.Height = height - 10;
                f.Align = (uint8_t)__GE(h)->HAlign | (uint8_t)__GE(h)->VAlign;
                f.Color1Width = f.Width;
                f.Color1 = __GUI_WIDGET_GetColor(h, GUI_EDITTEXT_COLOR_FG);
                f.Flags |= GUI_FLAG_FONT_RIGHTALIGN | GUI_FLAG_FONT_EDITMODE;
                
                if (__IsMultiline(h)) {
                    f.Flags |= GUI_FLAG_FONT_MULTILINE; /* Set multiline flag for widget */
                }
                
                GUI_DRAW_WriteText(disp, __GH(h)->Font, __GH(h)->Text, &f);
            }
            return 1;
        }
        case GUI_WC_FocusIn:
#if GUI_USE_KEYBOARD
            __GUI_KEYBOARD_Show(h);                 /* Show keyboard */
#endif /* GUI_USE_KEYBOARD */
            return 1;
        case GUI_WC_FocusOut:
#if GUI_USE_KEYBOARD
            __GUI_KEYBOARD_Hide();                  /* Hide keyboard */
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
            if (__GUI_WIDGET_ProcessTextKey(h, kb)) {
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
GUI_HANDLE_p GUI_EDITTEXT_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_EDITTEXT_t* ptr;
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_EDITTEXT_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);   /* Allocate memory for basic widget */

    __GUI_LEAVE();                                  /* Leave GUI */
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_EDITTEXT_SetColor(GUI_HANDLE_p h, GUI_EDITTEXT_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set color */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_EDITTEXT_SetMultiline(GUI_HANDLE_p h, uint8_t multiline) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (multiline && !__IsMultiline(h)) {           /* Check to set multiline */
        __GE(h)->Flags |= GUI_EDITTEXT_FLAG_MULTILINE;  /* Enable multiline */
    } else if (!multiline && __IsMultiline(h)) {    /* Check to clear multiline */
        __GE(h)->Flags &= ~GUI_EDITTEXT_FLAG_MULTILINE; /* Enable multiline */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_EDITTEXT_SetVAlign(GUI_HANDLE_p h, GUI_EDITTEXT_VALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GE(h)->VAlign != align) {
        __GE(h)->VAlign = align;                    /* Set new parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_EDITTEXT_SetHAlign(GUI_HANDLE_p h, GUI_EDITTEXT_HALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GE(h)->HAlign != align) {
        __GE(h)->HAlign = align;                    /* Set new parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

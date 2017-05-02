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

static
uint8_t GUI_TEXTVIEW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT,                             /*!< Default text color */
};

const static GUI_WIDGET_t Widget = {
    .Name = _T("Textview"),                         /*!< Widget name */
    .Size = sizeof(GUI_TEXTVIEW_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_TEXTVIEW_Callback,              /*!< Callback function */
    .Colors = Colors,
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o                   ((GUI_TEXTVIEW_t *)(h))

static
uint8_t GUI_TEXTVIEW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, wi, hi;
            
            if (__GUI_WIDGET_IsFontAndTextSet(h)) { /* Check if font is prepared for drawing */
                GUI_DRAW_FONT_t f;
                
                x = __GUI_WIDGET_GetAbsoluteX(h);   /* Get absolute X coordinate */
                y = __GUI_WIDGET_GetAbsoluteY(h);   /* Get absolute Y coordinate */
                wi = __GUI_WIDGET_GetWidth(h);      /* Get widget width */
                hi = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
                
                GUI_DRAW_FilledRectangle(disp, x, y, wi, hi, __GUI_WIDGET_GetColor(h, GUI_TEXTVIEW_COLOR_BG));
            
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.X = x + 1;
                f.Y = y + 1;
                f.Width = wi - 2;
                f.Height = hi - 2;
                f.Align = (uint8_t)o->HAlign | (uint8_t)o->VAlign;
                f.Flags |= GUI_FLAG_FONT_MULTILINE; /* Enable multiline */
                f.Color1Width = f.Width;
                f.Color1 = __GUI_WIDGET_GetColor(h, GUI_TEXTVIEW_COLOR_TEXT);
                GUI_DRAW_WriteText(disp, __GH(h)->Font, __GH(h)->Text, &f);
            }
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Touch has been handled */
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            return 1;
        }
        case GUI_WC_KeyPress: {
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o
/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_TEXTVIEW_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_TEXTVIEW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_TEXTVIEW_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);    /* Allocate memory for basic widget */
    if (ptr) {        

    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_TEXTVIEW_SetColor(GUI_HANDLE_p h, GUI_TEXTVIEW_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set color */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_TEXTVIEW_SetVAlign(GUI_HANDLE_p h, GUI_TEXTVIEW_VALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GT(h)->VAlign != align) {
        __GT(h)->VAlign = align;                    /* Set new parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_TEXTVIEW_SetHAlign(GUI_HANDLE_p h, GUI_TEXTVIEW_HALIGN_t align) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GT(h)->HAlign != align) {
        __GT(h)->HAlign = align;                    /* Set new parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

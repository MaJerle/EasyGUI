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
#include "gui_checkbox.h"

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
#define __GC(x)             ((GUI_CHECKBOX_t *)(x))

static
uint8_t GUI_CHECKBOX_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color */
    GUI_COLOR_DARKGRAY,                             /*!< Default freground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
    GUI_COLOR_GRAY,                                 /*!< Default background color when disabled */
    GUI_COLOR_BLACK,                                /*!< Default text color for widget */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("CHECKBOX"),                        /*!< Widget name */
    .Size = sizeof(GUI_CHECKBOX_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_CHECKBOX_Callback,              /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define c                   ((GUI_CHECKBOX_t *)(h))

/* Set checked status */
static
uint8_t __SetChecked(GUI_HANDLE_p h, uint8_t state) {
    if (state && !(c->Flags & GUI_FLAG_CHECKBOX_CHECKED)) {
        c->Flags |= GUI_FLAG_CHECKBOX_CHECKED;      /* Set flag */
        __GUI_WIDGET_Callback(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        __GUI_WIDGET_Invalidate(h);
        return 1;
    } else if (!state && (c->Flags & GUI_FLAG_CHECKBOX_CHECKED)) {
        c->Flags &= ~GUI_FLAG_CHECKBOX_CHECKED;     /* Clear flag */
        __GUI_WIDGET_Callback(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        __GUI_WIDGET_Invalidate(h);
        return 1;
    }
    return 0;
}

/* Set disabled status */
static
uint8_t __SetDisabled(GUI_HANDLE_p h, uint8_t state) {
    if (state && !(c->Flags & GUI_FLAG_CHECKBOX_DISABLED)) {
        c->Flags |= GUI_FLAG_CHECKBOX_DISABLED;     /* Set flag */
        __GUI_WIDGET_Invalidate(h);
        return 1;
    } else if (!state && (c->Flags & GUI_FLAG_CHECKBOX_DISABLED)) {
        c->Flags &= ~GUI_FLAG_CHECKBOX_DISABLED;    /* Clear flag */
        __GUI_WIDGET_Invalidate(h);
        return 1;
    }
    return 0;
}

static
uint8_t GUI_CHECKBOX_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Color_t c1;
            GUI_Dim_t x, y, width, height, size, sx, sy;
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            size = 20;
            
            sx = x;
            sy = y + (height - size) / 2;
            
            if (__GC(h)->Flags & GUI_FLAG_CHECKBOX_DISABLED) {
                c1 = __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_DISABLED_BG);
            } else {
                c1 = __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_BG);
            }
            
            GUI_DRAW_FilledRectangle(disp, sx + 1, sy + 1, size - 2, size - 2, c1);
            GUI_DRAW_Rectangle3D(disp, sx, sy, size, size, GUI_DRAW_3D_State_Lowered);
            
            if (__GUI_WIDGET_IsFocused(h)) {        /* When in focus */
                GUI_DRAW_Rectangle(disp, sx + 2, sy + 2, size - 4, size - 4, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
            }
            
            if (__GC(h)->Flags & GUI_FLAG_CHECKBOX_CHECKED) {
                GUI_DRAW_Line(disp, sx + 4, sy + 5, sx + size - 4 - 2, sy + size - 4 - 1, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
                GUI_DRAW_Line(disp, sx + 4, sy + 4, sx + size - 4 - 1, sy + size - 4 - 1, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
                GUI_DRAW_Line(disp, sx + 5, sy + 4, sx + size - 4 - 1, sy + size - 4 - 2, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
                
                GUI_DRAW_Line(disp, sx + 4, sy + size - 4 - 2, sx + size - 4 - 2, sy + 4, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
                GUI_DRAW_Line(disp, sx + 4, sy + size - 4 - 1, sx + size - 4 - 1, sy + 4, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
                GUI_DRAW_Line(disp, sx + 5, sy + size - 4 - 1, sx + size - 4 - 1, sy + 5, __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_FG));
            }
            
            /* Draw text if possible */
            if (__GUI_WIDGET_IsFontAndTextSet(h)) {
                GUI_DRAW_FONT_t f;
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.X = sx + size + 5;
                f.Y = y + 1;
                f.Width = width - size - 5;
                f.Height = height - 2;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                f.Color1 = __GUI_WIDGET_GetColor(h, GUI_CHECKBOX_COLOR_TEXT);
                GUI_DRAW_WriteText(disp, __GUI_WIDGET_GetFont(h), __GUI_WIDGET_GetText(h), &f);
            }
            
            return 1;
        }
#if GUI_USE_TOUCH 
        case GUI_WC_TouchStart: {
            if (c->Flags & GUI_FLAG_CHECKBOX_DISABLED) {    /* Check if widget is disabled */
                *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;   /* Handle touch without focus */
            } else {
                *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Touch is handled and in focus */
            }
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            __SetChecked(h, (c->Flags & GUI_FLAG_CHECKBOX_CHECKED) ? 0 : 1);    /* Toggle checked state */
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef c


/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_CHECKBOX_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

uint8_t GUI_CHECKBOX_SetColor(GUI_HANDLE_p h, GUI_CHECKBOX_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set color */
    return ret;
}

uint8_t GUI_CHECKBOX_SetChecked(GUI_HANDLE_p h, uint8_t checked) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = __SetChecked(h, checked);                 /* Set checked status */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_CHECKBOX_IsChecked(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = !!(__GC(h)->Flags & GUI_FLAG_CHECKBOX_CHECKED);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_CHECKBOX_SetDisabled(GUI_HANDLE_p h, uint8_t disabled) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = __SetDisabled(h, disabled);               /* Set checked status */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_CHECKBOX_IsDisabled(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = !!(__GC(h)->Flags & GUI_FLAG_CHECKBOX_DISABLED);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

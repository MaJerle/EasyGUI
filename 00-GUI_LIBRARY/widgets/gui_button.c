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

static
uint8_t GUI_BUTTON_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
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
    .Callback = GUI_BUTTON_Callback,                /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define b                   ((GUI_BUTTON_t *)(h))
static
uint8_t GUI_BUTTON_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GUI_WIDGET_Set3DStyle(h, 1);          /* By default set 3D */
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
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            if (__GUI_WIDGET_GetFlag(h, GUI_FLAG_3D)) {
                c1 = __GUI_WIDGET_GetColor(h, GUI_BUTTON_COLOR_BG);
                c2 = __GUI_WIDGET_GetColor(h, GUI_BUTTON_COLOR_FG);
            } else {
                if (__GUI_WIDGET_GetFlag(h, GUI_FLAG_ACTIVE)) { /* Set colors for button */
                    c1 = __GUI_WIDGET_GetColor(h, GUI_BUTTON_COLOR_FG);
                    c2 = __GUI_WIDGET_GetColor(h, GUI_BUTTON_COLOR_BG);
                } else {
                    c2 = __GUI_WIDGET_GetColor(h, GUI_BUTTON_COLOR_FG);
                    c1 = __GUI_WIDGET_GetColor(h, GUI_BUTTON_COLOR_BG);
                }
            }
            
            /* Draw actual button structure */
            if (__GUI_WIDGET_GetFlag(h, GUI_FLAG_3D)) {
                GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, width - 4, height - 4, c1);
                GUI_DRAW_Rectangle3D(disp, x, y, width, height, __GUI_WIDGET_GetFlag(h, GUI_FLAG_ACTIVE) ? GUI_DRAW_3D_State_Lowered : GUI_DRAW_3D_State_Raised);
            } else {
                GUI_DRAW_FilledRoundedRectangle(disp, x, y, width, height, b->BorderRadius, c1);
                GUI_DRAW_RoundedRectangle(disp, x, y, width, height, b->BorderRadius, c2);
            }
            
            /* Draw text if possible */
            if (__GUI_WIDGET_IsFontAndTextSet(h)) {
                GUI_DRAW_FONT_t f;
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.X = x + 1;
                f.Y = y + 1;
                f.Width = width - 2;
                f.Height = height - 2;
                f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                f.Color1 = c2;
                GUI_DRAW_WriteText(disp, __GUI_WIDGET_GetFont(h), __GUI_WIDGET_GetText(h), &f);
            }
            return 1;
        }
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (kb->KB.Keys[0] == GUI_KEY_CR || kb->KB.Keys[0] == GUI_KEY_LF) {
                __GUI_WIDGET_Callback(h, GUI_WC_Click, 0, 0);   /* Process click */
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED; /* Key handled */
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
        case GUI_WC_ActiveIn:
        case GUI_WC_ActiveOut: {
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef b


/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_BUTTON_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

uint8_t GUI_BUTTON_SetColor(GUI_HANDLE_p h, GUI_BUTTON_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetColor(h, (uint8_t)index, color); /* Set color */
}

uint8_t GUI_BUTTON_SetBorderRadius(GUI_HANDLE_p h, GUI_Dim_t size) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_BORDER_RADIUS, &size, 1, 1);    /* Set parameter */
}

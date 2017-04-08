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

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    .Name = _T("Button"),                           /*!< Widget name */
    .Size = sizeof(GUI_BUTTON_t),                   /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_BUTTON_Callback                 /*!< Callback function */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define b                   ((GUI_BUTTON_t *)(h))
uint8_t GUI_BUTTON_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Color_t c1, c2;
            GUI_Dim_t x, y, width, height;
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            if (h->Flags & GUI_FLAG_ACTIVE) {       /* Set colors for button */
                c1 = b->Color[GUI_BUTTON_COLOR_FG];
                c2 = b->Color[GUI_BUTTON_COLOR_BG];
            } else {
                c2 = b->Color[GUI_BUTTON_COLOR_FG];
                c1 = b->Color[GUI_BUTTON_COLOR_BG];
            }
            
            /* Draw actual button structure */
            if (h->Flags & GUI_FLAG_3D) {
                GUI_DRAW_FilledRectangle(disp, x, y, width, height, c1);
                GUI_DRAW_Rectangle3D(disp, x, y, width, height, h->Flags & GUI_FLAG_ACTIVE ? GUI_DRAW_3D_State_Lowered : GUI_DRAW_3D_State_Raised);
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
                GUI_DRAW_WriteText(disp, h->Font, h->Text, &f);
            }
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchEnd: {
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            __GUI_DEBUG("B: Click\r\n");
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
GUI_HANDLE_p GUI_BUTTON_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags) {
    GUI_BUTTON_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_BUTTON_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, flags); /* Allocate memory for basic widget */
    if (ptr) {        
        /* Color setup */
        ptr->Color[GUI_BUTTON_COLOR_BG] = GUI_COLOR_GRAY;   /* Set background color */
        ptr->Color[GUI_BUTTON_COLOR_FG] = GUI_COLOR_BLACK;  /* Set foreground color */
        
        ptr->BorderRadius = 8;                      /* Set border radius */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_BUTTON_SetColor(GUI_HANDLE_p h, GUI_BUTTON_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GB(h)->Color[index] != color) {           /* Any parameter changed */
        __GB(h)->Color[index] = color;              /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_BUTTON_SetBorderRadius(GUI_HANDLE_p h, GUI_Dim_t size) {
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GB(h)->BorderRadius != size) {            /* Any dimensions changed */
        __GB(h)->BorderRadius = size;               /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

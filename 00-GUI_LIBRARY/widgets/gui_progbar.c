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
#include "gui_progbar.h"

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
#define GUI_PROGBAR_FLAG_PERCENT    0x01            /*!< Flag indicating percentages are enabled on view */

#define __GP(x)             ((GUI_PROGBAR_t *)(x))

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    .Name = _T("PROGBAR"),                          /*!< Widget name */
    .Size = sizeof(GUI_PROGBAR_t),                  /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_PROGBAR_Callback,               /*!< Callback function */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define p           ((GUI_PROGBAR_t *)h)
uint8_t GUI_PROGBAR_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Dim_t x, y, w, width, height;
            GUI_Display_t* disp = (GUI_Display_t *)param;
    
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute position on screen */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute position on screen */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
           
            w = ((width - 2) * (p->Value - p->Min)) / (p->Max - p->Min);   /* Get width for active part */
            
            GUI_DRAW_FilledRectangle(disp, x + w + 1, y, width - w - 2, height, p->Color[GUI_PROGBAR_COLOR_BG]);
            GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, w, height - 2, p->Color[GUI_PROGBAR_COLOR_FG]);
            GUI_DRAW_Rectangle(disp, x, y, width, height, p->Color[GUI_PROGBAR_COLOR_BORDER]);
            
            /* Draw text if possible */
            if (h->Font) {
                const GUI_Char* text = NULL;
                GUI_Char buff[5];
                
                if (p->Flags & GUI_PROGBAR_FLAG_PERCENT) {
                    sprintf((char *)buff, "%d%%", ((p->Value - p->Min) * 100) / (p->Max - p->Min));
                    text = buff;
                } else if (h->Text && GUI_STRING_Length(h->Text)) {
                    text = h->Text;
                }
                
                if (text) {                         /* If text is valid, print it */
                    GUI_DRAW_FONT_t f;
                    GUI_DRAW_FONT_Init(&f);         /* Init structure */
                    
                    f.X = x + 1;
                    f.Y = y + 1;
                    f.Width = width;
                    f.Height = height - 2;
                    f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.Color1Width = w ? w - 1 : 0;
                    f.Color1 = p->Color[GUI_PROGBAR_COLOR_BG];
                    f.Color2 = p->Color[GUI_PROGBAR_COLOR_FG];
                    GUI_DRAW_WriteText(disp, h->Font, text, &f);
                }
            }
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_PROGBAR_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags) {
    GUI_PROGBAR_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_PROGBAR_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, flags);    /* Allocate memory for basic widget */
    if (ptr) {        
        /* Color setup */
        ptr->Color[GUI_PROGBAR_COLOR_BG] = GUI_COLOR_GRAY;
        ptr->Color[GUI_PROGBAR_COLOR_FG] = GUI_COLOR_DARKRED;
        ptr->Color[GUI_PROGBAR_COLOR_BORDER] = GUI_COLOR_BLACK;
        
        ptr->Min = 0;
        ptr->Max = 100;
        ptr->Value = 50;
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

GUI_HANDLE_p GUI_PROGBAR_SetColor(GUI_HANDLE_p h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Color[index] != color) {
        __GP(h)->Color[index] = color;
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_PROGBAR_SetValue(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Value != val && val >= __GP(h)->Min && val <= __GP(h)->Max) {  /* Value has changed */
        __GP(h)->Value = val;                       /* Set value */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_PROGBAR_SetMin(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Min != val && val < __GP(h)->Max) {
        __GP(h)->Min = val;                         /* Set new parameter */
        if (__GP(h)->Value < __GP(h)->Min) {        /* Check current value */
            __GP(h)->Value = __GP(h)->Min;
        }
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_PROGBAR_SetMax(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Max != val && val > __GP(h)->Min) {
        __GP(h)->Max = val;                         /* Set new parameter */
        if (__GP(h)->Value > __GP(h)->Max) {        /* Check current value */
            __GP(h)->Value = __GP(h)->Max;
        }
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_PROGBAR_EnablePercentages(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (!(__GP(h)->Flags & GUI_PROGBAR_FLAG_PERCENT)) {
        __GP(h)->Flags |= GUI_PROGBAR_FLAG_PERCENT; /* Set percentage flag */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_PROGBAR_DisablePercentages(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Flags & GUI_PROGBAR_FLAG_PERCENT) {
        __GP(h)->Flags &= ~GUI_PROGBAR_FLAG_PERCENT;/* Set percentage flag */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

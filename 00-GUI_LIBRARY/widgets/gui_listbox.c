/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
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
#include "gui_listbox.h"

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
#define __GL(x)             ((GUI_LISTBOX_t *)(x))

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    .Name = _T("LISTBOX"),                          /*!< Widget name */
    .Size = sizeof(GUI_LISTBOX_t),                  /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_LISTBOX_Callback                /*!< Callback function */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o                   ((GUI_LISTBOX_t *)(h))
uint8_t GUI_LISTBOX_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
    static GUI_iDim_t tY;
#endif /* GUI_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, width, height;
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, width - 2, height - 2, GUI_COLOR_WHITE);
            GUI_DRAW_Rectangle(disp, x, y, width, height, GUI_COLOR_GRAY);
            
            /* Draw text if possible */
            if (h->Font && __GL(h)->Pointers) {     /* Check if font is ready */
                GUI_DRAW_FONT_t f;
                uint16_t index = __GL(h)->StartIndex;   /* Start index */
                
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.X = x + 2;
                f.Y = y + 1;
                f.Width = width - 20;
                f.Height = height - 2;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_TOP;
                f.Color1Width = f.Width;
                
                while (height && index < __GL(h)->Count) {  /* Try to process all strings */
                    if (index == __GL(h)->Selected) {
                        GUI_DRAW_FilledRectangle(disp, x + 1, f.Y + 2, f.Width, __GUI_MIN(f.Height, h->Font->Size), GUI_WIDGET_IsFocused(h) ? GUI_COLOR_BLUE : GUI_COLOR_GRAY);
                        f.Color1 = GUI_COLOR_RED;
                    } else {
                        f.Color1 = GUI_COLOR_BLACK;
                    }
                    GUI_DRAW_WriteText(disp, h->Font, __GL(h)->Pointers[index], &f);
                    if (f.Height < h->Font->Size) {
                        break;
                    } else {
                        f.Height -= h->Font->Size;
                        f.Y += h->Font->Size;
                    }
                    index++;
                }
            }
            
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            tY = ts->RelY[0];
            
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            if (h->Font) {
                GUI_Dim_t height = h->Font->Size;    /* Get element height */
                GUI_iDim_t diff = tY - ts->RelY[0];
                
                if (__GUI_ABS(diff) > height) {
                    if (diff > 0) {                 /* Move slide up = increase top pointer */
                        if (o->StartIndex < (o->Count - 1)) {
                            o->StartIndex++;        /* Go up */
                            __GUI_WIDGET_Invalidate(h);
                        }
                    } else {
                        if (o->StartIndex > 0) {
                            o->StartIndex--;        /* Go down */
                            __GUI_WIDGET_Invalidate(h);
                        }
                    }
                    tY = ts->RelY[0];               /* Save pointer */
                }
            }
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            if (h->Font) {
                uint16_t height = h->Font->Size;    /* Get element height */
                uint16_t tmpSelected;
                
                tmpSelected = ts->RelY[0] / height; /* Get temporary selected index */
                if ((o->StartIndex + tmpSelected) <= o->Count) {
                    o->Selected = o->StartIndex + tmpSelected;
                    __GUI_WIDGET_Invalidate(h);     /* Choose new selection */
                } 
            }
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
GUI_HANDLE_p GUI_LISTBOX_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags) {
    GUI_LISTBOX_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_LISTBOX_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, flags);   /* Allocate memory for basic widget */
    if (ptr) {        

    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

GUI_HANDLE_p GUI_LISTBOX_SetPointers(GUI_HANDLE_p h, GUI_Char** ptrs, uint16_t count) {
    __GUI_ASSERTPARAMS(h);                          /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GL(h)->Pointers && __GL(h)->Flags & GUI_FLAG_LISTBOX_DYNAMIC) {   /* If memory for pointers is so far dynamically allocated */
        __GL(h)->Flags &= ~GUI_FLAG_LISTBOX_DYNAMIC;    /* Clear flag */
        __GUI_MEMFREE(__GL(h)->Pointers);           /* Free memory */
    }
    
    __GL(h)->Pointers = ptrs;                       /* Save pointers */
    __GL(h)->MaxCount = count;
    __GL(h)->Count = count;
    __GL(h)->StartIndex = 0;
    
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

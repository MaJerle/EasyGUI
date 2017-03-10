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

static void __Draw(GUI_Display_t* disp, void* ptr);
static __GUI_TouchStatus_t __TouchDown(void* widget, GUI_TouchData_t* ts, __GUI_TouchStatus_t status);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        "Progbar",                                  /*!< Widget name */
        sizeof(GUI_PROGBAR_t),                      /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    __Draw,                                         /*!< Widget draw function */
    {
        __TouchDown,                                /*!< Touch down callback function */
        0,                                          /*!< Touch up callback function */
        0                                           /*!< Touch move callback function */
    }
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
static void __Draw(GUI_Display_t* disp, void* ptr) {
    GUI_Dim_t x, y, w;
    
    x = __GUI_WIDGET_GetAbsoluteX(ptr);             /* Get absolute position on screen */
    y = __GUI_WIDGET_GetAbsoluteY(ptr);             /* Get absolute position on screen */
   
    w = ((__GH(ptr)->Width - 2) * (__GP(ptr)->Value - __GP(ptr)->Min)) / (__GP(ptr)->Max - __GP(ptr)->Min);   /* Get width for active part */
    
    GUI_DRAW_FilledRectangle(disp, x + w + 1, y, __GH(ptr)->Width - w - 2, __GH(ptr)->Height, __GP(ptr)->Color[GUI_PROGBAR_COLOR_BG]);
    GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, w, __GH(ptr)->Height - 2, __GP(ptr)->Color[GUI_PROGBAR_COLOR_FG]);
    GUI_DRAW_Rectangle(disp, x, y, __GH(ptr)->Width, __GH(ptr)->Height, __GP(ptr)->Color[GUI_PROGBAR_COLOR_BORDER]);
    
    /* Draw text if possible */
    if (__GH(ptr)->Font) {
        const char* text = NULL;
        char buff[5];
        
        if (__GP(ptr)->Flags & GUI_PROGBAR_FLAG_PERCENT) {
            sprintf(buff, "%d%%", ((__GP(ptr)->Value - __GP(ptr)->Min) * 100) / (__GP(ptr)->Max - __GP(ptr)->Min));
            text = buff;
        } else if (__GH(ptr)->Text && strlen(__GH(ptr)->Text)) {
            text = __GH(ptr)->Text;
        }
        
        if (text) {                                 /* If text is valid, print it */
            GUI_DRAW_FONT_t f;
            memset((void *)&f, 0x00, sizeof(f));    /* Reset structure */
            
            f.X = x + 1;
            f.Y = y + 1;
            f.Width = __GH(ptr)->Width;
            f.Height = __GH(ptr)->Height - 2;
            f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
            f.Color1Width = w ? w - 1 : 0;
            f.Color1 = __GP(ptr)->Color[GUI_PROGBAR_COLOR_BG];
            f.Color2 = __GP(ptr)->Color[GUI_PROGBAR_COLOR_FG];
            GUI_DRAW_WriteText(disp, __GH(ptr)->Font, text, &f);
        }
    }
}

static __GUI_TouchStatus_t __TouchDown(void* widget, GUI_TouchData_t* ts, __GUI_TouchStatus_t status) {
    return touchHANDLEDNOFOCUS;                     /* Handle widget touch but ignore focus */
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_t GUI_PROGBAR_Create(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_PROGBAR_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_PROGBAR_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height);   /* Allocate memory for basic widget */
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
    
    return (GUI_HANDLE_t)ptr;
}

void GUI_PROGBAR_Remove(GUI_HANDLE_t* h) {
    __GUI_ASSERTPARAMSVOID(h && *h);                /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

GUI_HANDLE_t GUI_PROGBAR_SetColor(GUI_HANDLE_t h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Color[index] != color) {
        __GP(h)->Color[index] = color;
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_PROGBAR_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_SetSize(h, width, height);         /* Set widget size */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_PROGBAR_SetValue(GUI_HANDLE_t h, int32_t val) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Value != val && val >= __GP(h)->Min && val <= __GP(h)->Max) {  /* Value has changed */
        __GP(h)->Value = val;                       /* Set value */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_PROGBAR_SetMin(GUI_HANDLE_t h, int32_t val) {
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

GUI_HANDLE_t GUI_PROGBAR_SetMax(GUI_HANDLE_t h, int32_t val) {
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

GUI_HANDLE_t GUI_PROGBAR_SetText(GUI_HANDLE_t h, GUI_Const char* text) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_SetText(h, text);                  /* Set text to widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_PROGBAR_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_SetFont(h, font);                  /* Set font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_PROGBAR_EnablePercentages(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (!(__GP(h)->Flags & GUI_PROGBAR_FLAG_PERCENT)) {
        __GP(h)->Flags |= GUI_PROGBAR_FLAG_PERCENT; /* Set percentage flag */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_PROGBAR_DisablePercentages(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    if (__GP(h)->Flags & GUI_PROGBAR_FLAG_PERCENT) {
        __GP(h)->Flags &= ~GUI_PROGBAR_FLAG_PERCENT;/* Set percentage flag */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

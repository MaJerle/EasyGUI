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

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp);
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result);

#if GUI_USE_TOUCH  
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts);
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
static __GUI_KeyboardStatus_t __KeyPress(GUI_HANDLE_t h, GUI_KeyboardData_t* kb);
#endif /* GUI_USE_KEYBOARD */
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        "EDITTEXT",                                 /*!< Widget name */
        sizeof(GUI_EDITTEXT_t),                     /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    __Control,                                      /*!< Control function */
    __Draw,                                         /*!< Widget draw function */
#if GUI_USE_TOUCH
    {
        __TouchDown,                                /*!< Touch down callback function */
        0,                                          /*!< Touch up callback function */
        0                                           /*!< Touch move callback function */
    },
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    {
        __KeyPress,                                 /*!< Keyboard key pressed callback function */
    }
#endif /* GUI_USE_KEYBOARD */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result) {
    __GUI_UNUSED3(h, param, result);
    switch (ctrl) {                                 /* Handle control function if required */
        default:                                    /* Handle default option */
            return 0;                               /* Command was not processed */
    }
}

#define e          ((GUI_EDITTEXT_t *)h)
static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp) {
    GUI_Color_t c1, c2;
    GUI_Dim_t x, y;
    
    x = __GUI_WIDGET_GetAbsoluteX(h);               /* Get absolute X coordinate */
    y = __GUI_WIDGET_GetAbsoluteY(h);               /* Get absolute Y coordinate */
    
    GUI_DRAW_FilledRectangle(disp, x, y, h->Width, h->Height, __GE(h)->Color[GUI_EDITTEXT_COLOR_BG]);
    GUI_DRAW_Rectangle(disp, x, y, h->Width, h->Height, __GE(h)->Color[GUI_EDITTEXT_COLOR_BORDER]);
    
    if (__GUI_WIDGET_IsFocused(h)) {
        GUI_DRAW_Rectangle(disp, x + 2, y + 2, h->Width - 4, h->Height - 4, __GE(h)->Color[GUI_EDITTEXT_COLOR_BORDER]);
    }
    
    if (__GUI_WIDGET_IsFontAndTextSet(h)) {         /* Ready to write string */
        GUI_DRAW_FONT_t f;
        memset((void *)&f, 0x00, sizeof(f));        /* Reset structure */
        
        f.X = x + 5;
        f.Y = y + 5;
        f.Width = h->Width - 10;
        f.Height = h->Height - 10;
        f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
        f.Color1Width = f.Width;
        f.Color1 = __GE(h)->Color[GUI_EDITTEXT_COLOR_TEXT];
        f.Flags |= GUI_FLAG_FONT_RIGHTALIGN;
        GUI_DRAW_WriteText(disp, h->Font, h->Text, &f);
    }
}

#if GUI_USE_TOUCH
/* Process touch check */
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    return touchHANDLED;
}
#endif /* GUI_USE_TOUCH */

#if GUI_USE_KEYBOARD
static __GUI_KeyboardStatus_t __KeyPress(GUI_HANDLE_t h, GUI_KeyboardData_t* kb) {
    __GUI_DEBUG("K: %c (%3d)", kb->Key, kb->Key);
    __GUI_WIDGET_ProcessTextKey(h, kb);
    
    return keyHANDLED;
}
#endif /* GUI_USE_KEYBOARD */

#undef e

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_t GUI_EDITTEXT_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_EDITTEXT_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_EDITTEXT_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height);    /* Allocate memory for basic widget */
    if (ptr) {        
        /* Color setup */
        ptr->Color[GUI_EDITTEXT_COLOR_BG] = GUI_COLOR_WHITE;    /* Set background color */
        ptr->Color[GUI_EDITTEXT_COLOR_BORDER] = GUI_COLOR_BLACK;    /* Set background color */
        ptr->Color[GUI_EDITTEXT_COLOR_TEXT] = GUI_COLOR_BLACK;  /* Set foreground color */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_t)ptr;
}

void GUI_EDITTEXT_Remove(GUI_HANDLE_t* h) {
    __GUI_ASSERTPARAMSVOID(h && *h);                /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

uint32_t GUI_EDITTEXT_AllocTextMemory(GUI_HANDLE_t h, uint16_t size) {
    __GUI_ASSERTPARAMS(h && size > 1);              /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_AllocateTextMemory(h, size);       /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return h->TextMemSize;                          /* Return number of bytes allocated */
}

GUI_HANDLE_t GUI_EDITTEXT_FreeTextMemory(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_FreeTextMemory(h);                 /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_EDITTEXT_SetText(GUI_HANDLE_t h, const char* text) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetText(h, text);                  /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_EDITTEXT_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                        /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetSize(h, width, height);         /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_EDITTEXT_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetXY(h, x, y);                    /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_EDITTEXT_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetFont(h, font);                  /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_EDITTEXT_SetColor(GUI_HANDLE_t h, GUI_EDITTEXT_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GE(h)->Color[index] != color) {         /* Any parameter changed */
        __GE(h)->Color[index] = color;            /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

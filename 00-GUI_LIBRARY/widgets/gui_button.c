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

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp);
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result);

#if GUI_USE_TOUCH  
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts);
static __GUI_TouchStatus_t __TouchUp(GUI_HANDLE_t h, GUI_TouchData_t* ts);
static __GUI_TouchStatus_t __TouchMove(GUI_HANDLE_t h, GUI_TouchData_t* ts);
#endif /* GUI_USE_TOUCH */

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        "Button",                                   /*!< Widget name */
        sizeof(GUI_BUTTON_t),                       /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    __Control,                                      /*!< Control function */
    __Draw,                                         /*!< Widget draw function */
#if GUI_USE_TOUCH
    {
        __TouchDown,                                /*!< Touch down callback function */
        __TouchUp,                                  /*!< Touch up callback function */
        __TouchMove                                 /*!< Touch move callback function */
    },
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    {
        0,                                          /*!< Keyboard key pressed callback function */
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

#define b          ((GUI_BUTTON_t *)h)
static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp) {
    GUI_Color_t c1, c2;
    GUI_Dim_t x, y;
    
    x = __GUI_WIDGET_GetAbsoluteX(h);               /* Get absolute X coordinate */
    y = __GUI_WIDGET_GetAbsoluteY(h);               /* Get absolute Y coordinate */
    
    if (h->Flags & GUI_FLAG_ACTIVE) {               /* Set colors for button */
        c1 = b->Color[GUI_BUTTON_COLOR_FG];
        c2 = b->Color[GUI_BUTTON_COLOR_BG];
    } else {
        c2 = b->Color[GUI_BUTTON_COLOR_FG];// = (b->Color[GUI_BUTTON_COLOR_FG] + 2) * (b->Color[GUI_BUTTON_COLOR_FG] + 3);
        c1 = b->Color[GUI_BUTTON_COLOR_BG];// = (b->Color[GUI_BUTTON_COLOR_BG] + 2) * (b->Color[GUI_BUTTON_COLOR_BG] + 3);
    }
    
    /* Draw actual button structure */
    if (h->Flags & GUI_FLAG_3D) {
        GUI_DRAW_FilledRectangle(disp, x, y, b->C.Width, b->C.Height, c1);
        GUI_DRAW_Rectangle3D(disp, x, y, b->C.Width, b->C.Height, h->Flags & GUI_FLAG_ACTIVE ? GUI_DRAW_3D_State_Lowered : GUI_DRAW_3D_State_Raised);
    } else {
        GUI_DRAW_FilledRoundedRectangle(disp, x, y, b->C.Width, b->C.Height, b->BorderRadius, c1);
        GUI_DRAW_RoundedRectangle(disp, x, y, b->C.Width, b->C.Height, b->BorderRadius, c2);
    }
    
    /* Draw text if possible */
    if (__GUI_WIDGET_IsFontAndTextSet(h)) {
        GUI_DRAW_FONT_t f;
        memset((void *)&f, 0x00, sizeof(f));        /* Reset structure */
        
        f.X = x + 1;
        f.Y = y + 1;
        f.Width = h->Width - 2;
        f.Height = h->Height - 2;
        f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
        f.Color1Width = f.Width;
        f.Color1 = c2;
        GUI_DRAW_WriteText(disp, h->Font, h->Text, &f);
    }
}

#if GUI_USE_TOUCH  
static GUI_iDim_t tX, tY;

/* Process touch check */
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    /* Get X and Y positions relative on widget */
    tX = ts->X - __GUI_WIDGET_GetAbsoluteX(h);
    tY = ts->Y - __GUI_WIDGET_GetAbsoluteY(h);
    
    return touchHANDLED;
}

/* Process touch check */
static __GUI_TouchStatus_t __TouchMove(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    __GUI_WIDGET_SetXY(h, ts->X - __GUI_WIDGET_GetAbsoluteX(h->Parent) - tX, ts->Y - __GUI_WIDGET_GetAbsoluteY(h->Parent) - tY);
    
    return touchHANDLED;
}

/* Process touch check */
static __GUI_TouchStatus_t __TouchUp(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    __GUI_UNUSED2(h, ts);
    return touchHANDLED;
}
#endif /* GUI_USE_TOUCH */

#undef b

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_t GUI_BUTTON_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_BUTTON_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_BUTTON_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height);    /* Allocate memory for basic widget */
    if (ptr) {        
        /* Color setup */
        ptr->Color[GUI_BUTTON_COLOR_BG] = GUI_COLOR_GRAY;   /* Set background color */
        ptr->Color[GUI_BUTTON_COLOR_FG] = GUI_COLOR_BLACK;  /* Set foreground color */
        
        ptr->BorderRadius = 8;                      /* Set border radius */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_t)ptr;
}

void GUI_BUTTON_Remove(GUI_HANDLE_t* h) {
    __GUI_ASSERTPARAMSVOID(h && *h);                /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

uint32_t GUI_BUTTON_AllocTextMemory(GUI_HANDLE_t h, uint8_t size) {
    __GUI_ASSERTPARAMS(h && size > 1);              /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_AllocateTextMemory(h, size);       /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return h->TextMemSize;                          /* Return number of bytes allocated */
}

GUI_HANDLE_t GUI_BUTTON_FreeTextMemory(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_FreeTextMemory(h);                 /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetText(GUI_HANDLE_t h, const char* text) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetText(h, text);                  /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                        /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetSize(h, width, height);         /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetXY(h, x, y);                    /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetFont(h, font);                  /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetColor(GUI_HANDLE_t h, GUI_BUTTON_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GB(h)->Color[index] != color) {         /* Any parameter changed */
        __GB(h)->Color[index] = color;            /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetBorderRadius(GUI_HANDLE_t h, GUI_Dim_t size) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GB(h)->BorderRadius != size) {            /* Any dimensions changed */
        __GB(h)->BorderRadius = size;               /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetBorderWidth(GUI_HANDLE_t h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GB(h)->BorderWidth != width) {            /* Any parameter changed */
        __GB(h)->BorderWidth = width;               /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_BUTTON_SetRadius(GUI_HANDLE_t h, GUI_Dim_t rad) {
    __GUI_ASSERTPARAMS(h);                        /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GB(h)->BorderRadius != rad) {             /* Any parameter changed */
        __GB(h)->BorderRadius = rad;                /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

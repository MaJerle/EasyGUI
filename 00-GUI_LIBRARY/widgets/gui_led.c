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
#include "gui_led.h"

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
#define __GL(x)             ((GUI_LED_t *)(x))

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp);
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result);

#if GUI_USE_TOUCH  
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts);
#endif /* GUI_USE_TOUCH */

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        _T("LED"),                                  /*!< Widget name */
        sizeof(GUI_LED_t),                          /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    __Control,                                      /*!< Control function */
    __Draw,                                         /*!< Widget draw function */
#if GUI_USE_TOUCH
    {
        __TouchDown,                                /*!< Touch down callback function */
        0, 0
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
#define l           ((GUI_LED_t *)h)
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result) {
    __GUI_UNUSED3(h, param, result);
    switch (ctrl) {                                 /* Handle control function if required */
        default:                                    /* Handle default option */
            return 0;                               /* Command was not processed */
    }
}

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp) {
    GUI_Color_t c1, c2;
    GUI_Dim_t x, y;
    
    x = __GUI_WIDGET_GetAbsoluteX(h);               /* Get absolute position on screen */
    y = __GUI_WIDGET_GetAbsoluteY(h);               /* Get absolute position on screen */
    
    /* Get drawing colors */
    if (l->Flags & GUI_LED_FLAG_ON) {               /* If LED is on */
        c1 = l->Color[GUI_LED_COLOR_ON];
        c2 = l->Color[GUI_LED_COLOR_ON_BORDER];
    } else {
        c1 = l->Color[GUI_LED_COLOR_OFF];
        c2 = l->Color[GUI_LED_COLOR_OFF_BORDER];
    }
    
    if (l->Type == GUI_LED_TYPE_RECT) {             /* When led has rectangle shape */
        GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, h->Width - 2, h->Height - 2, c1);
        GUI_DRAW_Rectangle(disp, x, y, h->Width, h->Height, c2);
    } else {
        GUI_DRAW_FilledCircle(disp, x + h->Width / 2, y + h->Height / 2, h->Width / 2, c1);
        GUI_DRAW_Circle(disp, x + h->Width / 2, y + h->Height / 2, h->Width / 2, c2);
    }
}

#if GUI_USE_TOUCH
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    __GUI_UNUSED2(h, ts);
    return touchHANDLEDNOFOCUS;                     /* Handle touch press on LED but don't do anything */
}
#endif /* GUI_USE_TOUCH */
#undef l

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_t GUI_LED_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_LED_t* h;
    
    __GUI_ASSERTACTIVEWIN();                        /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    h = (GUI_LED_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height); /* Allocate memory for basic widget */
    if (h) {        
        /* Color setup */
        h->Color[GUI_LED_COLOR_ON] = GUI_COLOR_LIGHTBLUE;       /* Set color when led is on */
        h->Color[GUI_LED_COLOR_OFF] = GUI_COLOR_DARKBLUE;       /* Set color when led is off */
        h->Color[GUI_LED_COLOR_ON_BORDER] = GUI_COLOR_GRAY;     /* Set border color when led is on */
        h->Color[GUI_LED_COLOR_OFF_BORDER] = GUI_COLOR_BLACK;   /* Set border color when led is off */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_t)h;
}

void GUI_LED_Remove(GUI_HANDLE_t* h) {
    __GUI_ASSERTPARAMSVOID(h && *h);                /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

GUI_HANDLE_t GUI_LED_SetColor(GUI_HANDLE_t h, GUI_LED_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GL(h)->Color[index] != color) {           /* Any parameter changed */
        __GL(h)->Color[index] = color;              /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_LED_SetType(GUI_HANDLE_t h, GUI_LED_TYPE_t type) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GL(h)->Type != type) {                    /* Any parameter changed */
        __GL(h)->Type = type;                       /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_LED_Off(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if ((__GL(h)->Flags & GUI_LED_FLAG_ON)) {       /* Any parameter changed */
        __GL(h)->Flags &= ~GUI_LED_FLAG_ON;         /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_LED_On(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!(__GL(h)->Flags & GUI_LED_FLAG_ON)) {      /* Any parameter changed */
        __GL(h)->Flags |= GUI_LED_FLAG_ON;          /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_LED_Toggle(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GL(h)->Flags ^= GUI_LED_FLAG_ON;              /* Toggle enable bit */
    __GUI_WIDGET_Invalidate(h);                     /* Redraw object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_LED_Set(GUI_HANDLE_t h, GUI_Byte state) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (state && !(__GL(h)->Flags & GUI_LED_FLAG_ON)) { /* If led should be enabled but is now closed */
        __GL(h)->Flags |= GUI_LED_FLAG_ON;          /* Toggle enable bit */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    } else if (!state && (__GL(h)->Flags & GUI_LED_FLAG_ON)) {  /* If led should be disabled but is not enabled */
        __GL(h)->Flags &= ~GUI_LED_FLAG_ON;         /* Toggle enable bit */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

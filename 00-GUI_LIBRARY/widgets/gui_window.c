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
#include "gui_window.h"

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
#define __GW(x)             ((GUI_WINDOW_t *)(x))

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
        _T("Window"),                               /*!< Widget name */
        sizeof(GUI_WINDOW_t),                       /*!< Size of widget for memory allocation */
        1,                                          /*!< Allow children objects on widget */
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
        0,                                          /*!< Keyboard key pressed callback function */
    }
#endif /* GUI_USE_KEYBOARD */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define w          ((GUI_WINDOW_t *)h)
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result) {
    __GUI_UNUSED3(h, param, result);
    switch (ctrl) {                                 /* Handle control function if required */
        default:                                    /* Handle default option */
            return 0;                               /* Command was not processed */
    }
}

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp) {
    GUI_DRAW_FilledRectangle(disp, __GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h), h->Width, h->Height, w->Color[GUI_WINDOW_COLOR_BG]);
}

#if GUI_USE_TOUCH  
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    __GUI_UNUSED2(h, ts);
    return touchHANDLEDNOFOCUS;                     /* Notify that touch has been pressed */
}
#endif /* GUI_USE_TOUCH */
#undef w

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_t GUI_WINDOW_Create(GUI_ID_t id) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, 0, 0, GUI.LCD.Width, GUI.LCD.Height);    /* Allocate memory for basic widget */
    if (ptr) {
        ptr->Color[GUI_WINDOW_COLOR_BG] = GUI_COLOR_LIGHTGRAY;  /* Set default color */
        
        __GUI_WINDOW_SETACTIVE(ptr);                /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

GUI_HANDLE_t GUI_WINDOW_CreateChild(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height);    /* Allocate memory for basic widget */
    if (ptr) {        
        ptr->Color[GUI_WINDOW_COLOR_BG] = GUI_COLOR_LIGHTGRAY;  /* Set default color */
        
        /* Control setup */
        __GH(ptr)->Flags |= GUI_FLAG_CHILD;         /* This window is child window */
        
        __GUI_WINDOW_SETACTIVE(ptr);                /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

GUI_HANDLE_t GUI_WINDOW_SetColor(GUI_HANDLE_t h, GUI_WINDOW_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GW(h)->Color[index] != color) {
        __GW(h)->Color[index] = color;              /* Set property */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

GUI_HANDLE_t GUI_WINDOW_SetBorderRadius(GUI_HANDLE_t h, GUI_Dim_t radius) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (GUI_WINDOW_IsChild(h) && __GW(h)->BorderRadius != radius) {
        __GW(h)->BorderRadius = radius;             /* Set property */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

GUI_HANDLE_t GUI_WINDOW_SetBorderWidth(GUI_HANDLE_t h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (GUI_WINDOW_IsChild(h) && __GW(h)->BorderWidth != width) {
        __GW(h)->BorderWidth = width;               /* Set property */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

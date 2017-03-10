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

static void __Draw(GUI_Display_t* disp, void* ptr);
static __GUI_TouchStatus_t __TouchDown(void* widget, GUI_TouchData_t* ts, __GUI_TouchStatus_t status);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        "Window",                                   /*!< Widget name */
        sizeof(GUI_WINDOW_t),                       /*!< Size of widget for memory allocation */
        1,                                          /*!< Allow children objects on widget */
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
    GUI_DRAW_FilledRectangle(disp, __GUI_WIDGET_GetAbsoluteX(ptr), __GUI_WIDGET_GetAbsoluteY(ptr), __GH(ptr)->Width, __GH(ptr)->Height, __GW(ptr)->Color[GUI_WINDOW_COLOR_BG]);
}

static __GUI_TouchStatus_t __TouchDown(void* widget, GUI_TouchData_t* ts, __GUI_TouchStatus_t status) {
    return touchHANDLEDNOFOCUS;                     /* Notify that touch has been pressed */
}

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

GUI_HANDLE_t GUI_WINDOW_SetXY(GUI_HANDLE_t h, GUI_Dim_t x, GUI_Dim_t y) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (GUI_WINDOW_IsChild(h)) {
        __GUI_WIDGET_SetXY(h, x, y);                /* Set coordinates */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

GUI_HANDLE_t GUI_WINDOW_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (GUI_WINDOW_IsChild(h)) {
        __GUI_WIDGET_SetSize(h, width, height);     /* Set property */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

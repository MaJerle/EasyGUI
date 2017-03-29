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

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        _T("Window"),                               /*!< Widget name */
        sizeof(GUI_WINDOW_t),                       /*!< Size of widget for memory allocation */
        GUI_FLAG_WIDGET_ALLOW_CHILDREN,             /*!< List of widget flags */
    },
    GUI_WINDOW_Callback,                            /*!< Control function */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define w          ((GUI_WINDOW_t *)h)
uint8_t GUI_WINDOW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_DRAW_FilledRectangle(disp, __GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h), __GUI_WIDGET_GetWidth(h), __GUI_WIDGET_GetHeight(h), w->Color[GUI_WINDOW_COLOR_BG]);
            return 1;
        }
#if GUI_USE_TOUCH  
        case GUI_WC_TouchStart:
            *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;   /* Set handled status */
            return 1;
#endif /* GUI_USE_TOUCH */
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef w

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_WINDOW_Create(GUI_ID_t id) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, 0, 0, GUI.LCD.Width, GUI.LCD.Height, GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP); /* Allocate memory for basic widget */
    if (ptr) {
        ptr->Color[GUI_WINDOW_COLOR_BG] = GUI_COLOR_LIGHTGRAY;  /* Set default color */
        
        GUI_WINDOW_SetActive(__GH(ptr));            /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

GUI_HANDLE_p GUI_WINDOW_CreateChild(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, 0); /* Allocate memory for basic widget */
    if (ptr) {        
        ptr->Color[GUI_WINDOW_COLOR_BG] = GUI_COLOR_LIGHTGRAY;  /* Set default color */
        
        /* Control setup */
        __GH(ptr)->Flags |= GUI_FLAG_CHILD;         /* This window is child window */
        
        __GUI_WIDGET_SetPaddingTop(ptr, 10);
        __GUI_WIDGET_SetPaddingRight(ptr, 10);
        __GUI_WIDGET_SetPaddingBottom(ptr, 10);
        __GUI_WIDGET_SetPaddingLeft(ptr, 10);
        
        GUI_WINDOW_SetActive(__GH(ptr));            /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

void GUI_WINDOW_SetActive(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMSVOID(h);                      /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    GUI.WindowActive = h;                           /* Set new active window */
    __GUI_WIDGET_MoveDownTree(h);                   /* Move widget down on tree */
    
    __GUI_FOCUS_CLEAR();                            /* Clear focus on widget */
    __GUI_ACTIVE_CLEAR();                           /* Clear active on widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

GUI_HANDLE_p GUI_WINDOW_SetColor(GUI_HANDLE_p h, GUI_WINDOW_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GW(h)->Color[index] != color) {
        __GW(h)->Color[index] = color;              /* Set property */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

GUI_HANDLE_p GUI_WINDOW_SetBorderRadius(GUI_HANDLE_p h, GUI_Dim_t radius) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (GUI_WINDOW_IsChild(h) && __GW(h)->BorderRadius != radius) {
        __GW(h)->BorderRadius = radius;             /* Set property */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

GUI_HANDLE_p GUI_WINDOW_SetBorderWidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (GUI_WINDOW_IsChild(h) && __GW(h)->BorderWidth != width) {
        __GW(h)->BorderWidth = width;               /* Set property */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;                                       /* Return widget pointer */
}

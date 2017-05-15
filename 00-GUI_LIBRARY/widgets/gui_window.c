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

static
uint8_t GUI_WINDOW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT_TITLE,                       /*!< Default text color */
    GUI_COLOR_WIN_BLUE,                             /*!< Default top background color when widget in focus */
    GUI_COLOR_WIN_BLUE,                             /*!< Default top background color when widget not in focus */
};

const static GUI_WIDGET_t Widget = {
    .Name = _T("Window"),                           /*!< Widget name */
    .Size = sizeof(GUI_WINDOW_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN,        /*!< List of widget flags */
    .Callback = GUI_WINDOW_Callback,                /*!< Control function */
    .Colors = Colors,                               /*!< Pointer to colors array */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define w          ((GUI_WINDOW_t *)h)
static
uint8_t GUI_WINDOW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
   static GUI_iDim_t tX, tY, Mode = 0;
#endif /* GUI_USE_TOUCH */
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_iDim_t x, y, wi, hi, pt, topH;
            uint8_t inFocus;
            
            pt = __GUI_WIDGET_GetPaddingTop(h);
            
            x = __GUI_WIDGET_GetAbsoluteX(h);
            y = __GUI_WIDGET_GetAbsoluteY(h);
            wi = __GUI_WIDGET_GetWidth(h);
            hi = __GUI_WIDGET_GetHeight(h);
            
            GUI_DRAW_FilledRectangle(disp, x, y, wi, hi, __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_BG));
            if (__GH(h)->Flags & GUI_FLAG_CHILD) {
                GUI_iDim_t tX, tY, tW;
                
                GUI_DRAW_Rectangle3D(disp, x, y, wi, hi, GUI_DRAW_3D_State_Lowered);
                
                x += 2;
                y += 2;
                wi -= 4;
                hi -= 4;
                topH = pt - 4;
                
                inFocus = __GUI_WIDGET_IsFocused(h);    /* Check if window is in focus or any children widget */
                
                GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, wi - 2, topH, inFocus ? __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_TOP_BG_FOC) : __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_TOP_BG_NOFOC));
                
                /* Draw "hide" button */
                GUI_DRAW_Rectangle3D(disp, x + wi - 3 * topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledRectangle(disp, x + wi - 3 * topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                GUI_DRAW_FilledRectangle(disp, x + wi - 3 * topH + 4, y + topH - 6, (topH - 6) / 2, 2, GUI_COLOR_BLACK); 
                
                /* Draw maximize button */
                GUI_DRAW_Rectangle3D(disp, x + wi - 2 * topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledRectangle(disp, x + wi - 2 * topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                if (__GUI_WIDGET_IsExpanded(h)) {
                    GUI_iDim_t tmpX, tmpY;
                    tmpX = x + wi - 2 * topH + 4;
                    tmpY = y + 7;
                    GUI_DRAW_Rectangle(disp, tmpX, tmpY + 4, topH - 14, topH - 14, GUI_COLOR_BLACK);
                    GUI_DRAW_HLine(disp, tmpX + 1, tmpY + 5, topH - 16, GUI_COLOR_BLACK); 
                    
                    GUI_DRAW_FilledRectangle(disp, tmpX + 4, tmpY, topH - 15, 2, GUI_COLOR_BLACK);
                    GUI_DRAW_VLine(disp, tmpX + 4, tmpY + 2, 2, GUI_COLOR_BLACK);
                    GUI_DRAW_VLine(disp, tmpX + 4 + topH - 15, tmpY, topH - 15, GUI_COLOR_BLACK);
                    GUI_DRAW_HLine(disp, tmpX + topH - 14, tmpY + topH - 15, 4, GUI_COLOR_BLACK);
                } else {
                    GUI_DRAW_Rectangle(disp, x + wi - 2 * topH + 4, y + 7, topH - 10, topH - 10, GUI_COLOR_BLACK); 
                    GUI_DRAW_HLine(disp, x + wi - 2 * topH + 4, y + 8, topH - 10, GUI_COLOR_BLACK); 
                }
                
                /* Draw "close" button */
                GUI_DRAW_Rectangle3D(disp, x + wi - topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledRectangle(disp, x + wi - topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_WIN_RED);
                
                tX = x + wi - topH + 6;
                tY = y + 8;
                tW = topH - 15;
                
                GUI_DRAW_Line(disp, tX,     tY,     tX + tW,     tY + tW,     GUI_COLOR_WHITE);
                GUI_DRAW_Line(disp, tX + 1, tY,     tX + tW,     tY + tW - 1, GUI_COLOR_WHITE);
                GUI_DRAW_Line(disp, tX,     tY + 1, tX + tW - 1, tY + tW,     GUI_COLOR_WHITE);
                
                GUI_DRAW_Line(disp, tX,     tY + tW,     tX + tW,     tY,     GUI_COLOR_WHITE);
                GUI_DRAW_Line(disp, tX + 1, tY + tW,     tX + tW,     tY + 1, GUI_COLOR_WHITE);
                GUI_DRAW_Line(disp, tX,     tY + tW - 1, tX + tW - 1, tY,     GUI_COLOR_WHITE);
                
                if (__GUI_WIDGET_IsFontAndTextSet(h)) {
                    GUI_DRAW_FONT_t f;
                    GUI_DRAW_FONT_Init(&f);         /* Init structure */
                    
                    f.X = x + 3;
                    f.Y = y + 3;
                    f.Width = wi - 3 * topH - 5;
                    f.Height = topH - 3;
                    f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.Color1Width = f.Width;
                    f.Color1 = __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_TEXT);
                    GUI_DRAW_WriteText(disp, __GH(h)->Font, __GH(h)->Text, &f);
                }
            }
            
            //__GUI_WIDGET_SetColor(h, GUI_WINDOW_COLOR_BG, (__GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_BG) + 5) * (__GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_BG) + 10));
            
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */

            if (ts->TS.Count == 1 && __GH(h)->Flags & GUI_FLAG_CHILD) { /* For children widgets only on single touch */
                GUI_iDim_t pt, wi;
                pt = __GUI_WIDGET_GetPaddingTop(h); /* Get top padding */
                wi = __GUI_WIDGET_GetWidth(h);      /* Get widget width */
                
                if (ts->RelY[0] < pt && ts->RelX[0] < (wi - pt)) {
                    Mode = 1;
                    tX = ts->RelX[0];
                    tY = ts->RelY[0];
                }
                *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Set handled status */
            } else {
                *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;   /* Set handled status */
            }
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */
            
            if (Mode == 1) {
                GUI_iDim_t pX, pY;
                pX = __GUI_WIDGET_GetParentAbsoluteX(__GH(h));
                pY = __GUI_WIDGET_GetParentAbsoluteY(__GH(h));
                __GUI_WIDGET_SetXY(h, ts->TS.X[0] - pX - tX, ts->TS.Y[0] - pY - tY);
                
                if (__GUI_WIDGET_IsExpanded(h)) {   /* If it is expanded */
                    __GUI_WIDGET_SetExpanded(h, 0); /* Clear expanded mode */
                }
            }
            
            return 1;
        }
        case GUI_WC_TouchEnd: {
            Mode = 0;
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            GUI_iDim_t pt, wi;
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */
            
            pt = __GUI_WIDGET_GetPaddingTop(h);     /* Get top padding */
            wi = __GUI_WIDGET_GetWidth(h);          /* Get widget width */
            
            if (ts->RelY[0] <= pt) {                /* Top widget part detected */
                pt = pt - 4;
                if (ts->RelX[0] > (wi - 3 * pt) && ts->RelX[0] < (wi - 2 * pt)) {
                    __GUI_WIDGET_Hide(h);           /* Hide widget */
                } else if (ts->RelX[0] > (wi - 2 * pt) && ts->RelX[0] < (wi - pt)) {
                    __GUI_WIDGET_ToggleExpanded(h); /* Hide widget */
                } else if (ts->RelX[0] >= (wi - pt)) {
                    __GUI_WIDGET_Remove(h);         /* Remove widget */
                }
            }
            return 1;
        }
        case GUI_WC_DblClick: {
            GUI_iDim_t pt;
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */
            
            pt = __GUI_WIDGET_GetPaddingTop(h);     /* Get top padding */
            
            if (ts->RelY[0] <= pt) {                /* Top widget part detected */
                __GUI_WIDGET_ToggleExpanded(h);     /* Toggle expanded mode */
            }
            return 1;
        }
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (kb->KB.Keys[0] == GUI_KEY_DOWN) {
                __GUI_WIDGET_SetXY(h, __GH(h)->X, __GH(h)->Y + 1);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_UP) {
                __GUI_WIDGET_SetXY(h, __GH(h)->X, __GH(h)->Y - 1);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_LEFT) {
                __GUI_WIDGET_SetXY(h, __GH(h)->X - 1, __GH(h)->Y);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_RIGHT) {
                __GUI_WIDGET_SetXY(h, __GH(h)->X + 1, __GH(h)->Y);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
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
GUI_HANDLE_p GUI_WINDOW_CreateDesktop(GUI_ID_t id, GUI_WIDGET_CALLBACK_t cb) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, 0, 0, GUI.LCD.Width, GUI.LCD.Height, 0, cb, GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP);  /* Allocate memory for basic widget */
    if (ptr) {
        GUI_WINDOW_SetActive(__GH(ptr));            /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

GUI_HANDLE_p GUI_WINDOW_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags); /* Allocate memory for basic widget */
    if (ptr) {
        /* Control setup */
        __GH(ptr)->Flags |= GUI_FLAG_CHILD;         /* This window is child window */
        
        __GUI_WIDGET_SetPaddingTop(ptr, 30);
        __GUI_WIDGET_SetPaddingRight(ptr, 2);
        __GUI_WIDGET_SetPaddingBottom(ptr, 2);
        __GUI_WIDGET_SetPaddingLeft(ptr, 2);
        
        GUI_WINDOW_SetActive(__GH(ptr));            /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

uint8_t GUI_WINDOW_SetActive(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    GUI.WindowActive = h;                           /* Set new active window */
    __GUI_WIDGET_MoveDownTree(h);                   /* Move widget down on tree */
    
    __GUI_WIDGET_FOCUS_CLEAR();                     /* Clear focus on widget */
    __GUI_WIDGET_ACTIVE_CLEAR();                    /* Clear active on widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_WINDOW_SetColor(GUI_HANDLE_p h, GUI_WINDOW_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set desired color */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

GUI_HANDLE_p GUI_WINDOW_GetDesktop(void) {
    return (GUI_HANDLE_p)__GUI_LINKEDLIST_GETNEXT_GEN(&GUI.Root, NULL); /* Return desktop window */
}

/**	
 * \file            gui_window.c
 * \brief           Window widget
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
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

static uint8_t gui_window_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT_TITLE,                       /*!< Default text color */
    GUI_COLOR_WIN_BLUE,                             /*!< Default top background color when widget in focus */
    GUI_COLOR_WIN_BLUE,                             /*!< Default top background color when widget not in focus */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("WINDOW"),                          /*!< Widget name */
    .Size = sizeof(GUI_WINDOW_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN,        /*!< List of widget flags */
    .Callback = gui_window_callback,                /*!< Control function */
    .Colors = Colors,                               /*!< Pointer to colors array */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define w          ((GUI_WINDOW_t *)h)
static uint8_t
gui_window_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
   static GUI_iDim_t tX, tY, Mode = 0;
#endif /* GUI_USE_TOUCH */
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {                      /* Called immediatelly after widget is created */
            gui_window_setactive(h);                /* Set active window */
            return 1;
        }
        case GUI_WC_Draw: {
            uint8_t inFocus;
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_iDim_t x, y, wi, hi, pt, topH;
            
            pt = gui_widget_getpaddingtop__(h);
            
            x = gui_widget_getabsolutex__(h);
            y = gui_widget_getabsolutey__(h);
            wi = gui_widget_getwidth__(h);
            hi = gui_widget_getheight__(h);
            
            gui_draw_filledrectangle(disp, x, y, wi, hi, gui_widget_getcolor__(h, GUI_WINDOW_COLOR_BG));
            if (gui_widget_getflag__(h, GUI_FLAG_CHILD)) {
                GUI_iDim_t tX, tY, tW;
                
                gui_draw_rectangle3d(disp, x, y, wi, hi, GUI_DRAW_3D_State_Lowered);
                
                x += 2;
                y += 2;
                wi -= 4;
                hi -= 4;
                topH = pt - 4;
                
                inFocus = gui_widget_isfocused__(h);    /* Check if window is in focus or any children widget */
                
                gui_draw_filledrectangle(disp, x + 1, y + 1, wi - 2, topH, inFocus ? gui_widget_getcolor__(h, GUI_WINDOW_COLOR_TOP_BG_FOC) : gui_widget_getcolor__(h, GUI_WINDOW_COLOR_TOP_BG_NOFOC));
                
                /* Draw "hide" button */
                gui_draw_rectangle3d(disp, x + wi - 3 * topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + wi - 3 * topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                gui_draw_filledrectangle(disp, x + wi - 3 * topH + 4, y + topH - 6, (topH - 6) / 2, 2, GUI_COLOR_BLACK); 
                
                /* Draw maximize button */
                gui_draw_rectangle3d(disp, x + wi - 2 * topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + wi - 2 * topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                if (gui_widget_isexpanded__(h)) {
                    GUI_iDim_t tmpX, tmpY;
                    tmpX = x + wi - 2 * topH + 4;
                    tmpY = y + 7;
                    gui_draw_rectangle(disp, tmpX, tmpY + 4, topH - 14, topH - 14, GUI_COLOR_BLACK);
                    gui_draw_hline(disp, tmpX + 1, tmpY + 5, topH - 16, GUI_COLOR_BLACK); 
                    
                    gui_draw_filledrectangle(disp, tmpX + 4, tmpY, topH - 15, 2, GUI_COLOR_BLACK);
                    gui_draw_vline(disp, tmpX + 4, tmpY + 2, 2, GUI_COLOR_BLACK);
                    gui_draw_vline(disp, tmpX + 4 + topH - 15, tmpY, topH - 15, GUI_COLOR_BLACK);
                    gui_draw_hline(disp, tmpX + topH - 14, tmpY + topH - 15, 4, GUI_COLOR_BLACK);
                } else {
                    gui_draw_rectangle(disp, x + wi - 2 * topH + 4, y + 7, topH - 10, topH - 10, GUI_COLOR_BLACK); 
                    gui_draw_hline(disp, x + wi - 2 * topH + 4, y + 8, topH - 10, GUI_COLOR_BLACK); 
                }
                
                /* Draw "close" button */
                gui_draw_rectangle3d(disp, x + wi - topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + wi - topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_WIN_RED);
                
                tX = x + wi - topH + 6;
                tY = y + 8;
                tW = topH - 15;
                
                gui_draw_line(disp, tX,     tY,     tX + tW,     tY + tW,     GUI_COLOR_WHITE);
                gui_draw_line(disp, tX + 1, tY,     tX + tW,     tY + tW - 1, GUI_COLOR_WHITE);
                gui_draw_line(disp, tX,     tY + 1, tX + tW - 1, tY + tW,     GUI_COLOR_WHITE);
                
                gui_draw_line(disp, tX,     tY + tW,     tX + tW,     tY,     GUI_COLOR_WHITE);
                gui_draw_line(disp, tX + 1, tY + tW,     tX + tW,     tY + 1, GUI_COLOR_WHITE);
                gui_draw_line(disp, tX,     tY + tW - 1, tX + tW - 1, tY,     GUI_COLOR_WHITE);
                
                if (gui_widget_isfontandtextset__(h)) {
                    GUI_DRAW_FONT_t f;
                    gui_draw_font_init(&f);         /* Init structure */
                    
                    f.X = x + 3;
                    f.Y = y + 3;
                    f.Width = wi - 3 * topH - 5;
                    f.Height = topH - 3;
                    f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.Color1Width = f.Width;
                    f.Color1 = gui_widget_getcolor__(h, GUI_WINDOW_COLOR_TEXT);
                    gui_draw_writetext(disp, gui_widget_getfont__(h), gui_widget_gettext__(h), &f);
                }
            }
            
            //gui_widget_setcolor__(h, GUI_WINDOW_COLOR_BG, (gui_widget_getcolor__(h, GUI_WINDOW_COLOR_BG) + 5) * (gui_widget_getcolor__(h, GUI_WINDOW_COLOR_BG) + 10));
            
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */

            if (ts->TS.Count == 1 && gui_widget_getflag__(h, GUI_FLAG_CHILD)) { /* For children widgets only on single touch */
                GUI_iDim_t pt, wi;
                pt = gui_widget_getpaddingtop__(h); /* Get top padding */
                wi = gui_widget_getwidth__(h);      /* Get widget width */
                
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
                pX = gui_widget_getparentabsolutex__(__GH(h));
                pY = gui_widget_getparentabsolutey__(__GH(h));
                gui_widget_setposition__(h, ts->TS.X[0] - pX - tX, ts->TS.Y[0] - pY - tY);
                
                if (gui_widget_isexpanded__(h)) {   /* If it is expanded */
                    gui_widget_setexpanded__(h, 0); /* Clear expanded mode */
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
            
            pt = gui_widget_getpaddingtop__(h);     /* Get top padding */
            wi = gui_widget_getwidth__(h);          /* Get widget width */
            
            if (ts->RelY[0] <= pt) {                /* Top widget part detected */
                pt = pt - 4;
                if (ts->RelX[0] > (wi - 3 * pt) && ts->RelX[0] < (wi - 2 * pt)) {
                    gui_widget_hide__(h);           /* Hide widget */
                } else if (ts->RelX[0] > (wi - 2 * pt) && ts->RelX[0] < (wi - pt)) {
                    gui_widget_toggleexpanded__(h); /* Hide widget */
                } else if (ts->RelX[0] >= (wi - pt)) {
                    gui_widget_remove__(h);         /* Remove widget */
                }
            }
            return 1;
        }
        case GUI_WC_DblClick: {
            GUI_iDim_t pt;
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */
            
            pt = gui_widget_getpaddingtop__(h);     /* Get top padding */
            
            if (ts->RelY[0] <= pt) {                /* Top widget part detected */
                gui_widget_toggleexpanded__(h);     /* Toggle expanded mode */
            }
            return 1;
        }
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (kb->KB.Keys[0] == GUI_KEY_DOWN) {
                gui_widget_setposition__(h, __GH(h)->X, __GH(h)->Y + 1);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_UP) {
                gui_widget_setposition__(h, __GH(h)->X, __GH(h)->Y - 1);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_LEFT) {
                gui_widget_setposition__(h, __GH(h)->X - 1, __GH(h)->Y);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_RIGHT) {
                gui_widget_setposition__(h, __GH(h)->X + 1, __GH(h)->Y);
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef w

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
  
/**
 * \brief           Create new base widget window with desktop window as a parent
 * \param           id: Widget unique ID to use for identity for callback processing
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p
gui_window_createdesktop(GUI_ID_t id, GUI_WIDGET_CALLBACK_t cb) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, 0, 0, GUI.LCD.Width, GUI.LCD.Height, 0, cb, GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP);   /* Allocate memory for basic widget */
}

/**
 * \brief           Create new window widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p
gui_window_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_WINDOW_t* ptr;

    ptr = gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags); /* Allocate memory for basic widget */
    if (ptr) {
        __GUI_ENTER();                              /* Enter GUI */
        
        gui_widget_setflag__(__GH(ptr), GUI_FLAG_CHILD);    /* This window is child window */
        gui_widget_setpaddingtop__(__GH(ptr), 30);
        gui_widget_setpaddingright__(__GH(ptr), 2);
        gui_widget_setpaddingbottom__(__GH(ptr), 2);
        gui_widget_setpaddingleft__(__GH(ptr), 2);
        
        __GUI_LEAVE();                              /* Leave GUI */
    }
    return (GUI_HANDLE_p)ptr;
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_WINDOW_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_window_setcolor(GUI_HANDLE_p h, GUI_WINDOW_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}
 
/**
 * \brief           Set active window for future widgets and for current top window
 * \param[in]       h: Widget handle to set as active window
 * \retval          1: Widget was set ok
 * \retval          0: Widget was not set
 */
uint8_t
gui_window_setactive(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    GUI.WindowActive = h;                           /* Set new active window */
    gui_widget_movedowntree(h);                     /* Move widget down on tree */
    
    gui_widget_focus_clear();                       /* Clear focus on widget */
    gui_widget_active_clear();                      /* Clear active on widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get desktop window
 * \retval          Widget handle of desktop window
 */
GUI_HANDLE_p
gui_window_getdesktop(void) {
    return (GUI_HANDLE_p)gui_linkedlist_getnext_gen(&GUI.Root, NULL);   /* Return desktop window */
}

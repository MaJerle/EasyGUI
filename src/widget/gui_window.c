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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui_private.h"
#include "widget/gui_window.h"

#define __GW(x)             ((gui_window_t *)(x))

static uint8_t gui_window_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT_TITLE,                       /*!< Default text color */
    GUI_COLOR_WIN_BLUE,                             /*!< Default top background color when widget in focus */
    GUI_COLOR_WIN_BLUE,                             /*!< Default top background color when widget not in focus */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("WINDOW"),                          /*!< Widget name */
    .size = sizeof(gui_window_t),                   /*!< Size of widget for memory allocation */
    .flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN,        /*!< List of widget flags */
    .callback = gui_window_callback,                /*!< Control function */
    .colors = colors,                               /*!< Pointer to colors array */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

#define w          ((gui_window_t *)h)

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       ctr: Callback type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_window_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
#if GUI_CFG_USE_TOUCH
    static gui_dim_t tx, ty, Mode = 0;
#endif /* GUI_CFG_USE_TOUCH */
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {                      /* Called immediatelly after widget is created */
            gui_window_setactive(h, 0);             /* Set active window */
            return 1;
        }
        case GUI_WC_Draw: {
            uint8_t inFocus;
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_dim_t x, y, wi, hi, pt, topH;
            
            pt = gui_widget_getpaddingtop(h, 0);
            
            x = guii_widget_getabsolutex(h);
            y = guii_widget_getabsolutey(h);
            wi = gui_widget_getwidth(h, 0);
            hi = gui_widget_getheight(h, 0);
            
            gui_draw_filledrectangle(disp, x, y, wi, hi, guii_widget_getcolor(h, GUI_WINDOW_COLOR_BG));
            if (guii_widget_getflag(h, GUI_FLAG_CHILD)) {
                gui_dim_t tx, ty, tW;
                
                gui_draw_rectangle3d(disp, x, y, wi, hi, GUI_DRAW_3D_State_Lowered);
                
                x += 2;
                y += 2;
                wi -= 4;
                hi -= 4;
                topH = pt - 4;
                
                inFocus = guii_widget_isfocused(h);    /* Check if window is in focus or any children widget */
                
                gui_draw_filledrectangle(disp, x + 1, y + 1, wi - 2, topH, inFocus ? guii_widget_getcolor(h, GUI_WINDOW_COLOR_TOP_BG_FOC) : guii_widget_getcolor(h, GUI_WINDOW_COLOR_TOP_BG_NOFOC));
                
                /* Draw "hide" button */
                gui_draw_rectangle3d(disp, x + wi - 3 * topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + wi - 3 * topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                gui_draw_filledrectangle(disp, x + wi - 3 * topH + 4, y + topH - 6, (topH - 6) / 2, 2, GUI_COLOR_BLACK); 
                
                /* Draw maximize button */
                gui_draw_rectangle3d(disp, x + wi - 2 * topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + wi - 2 * topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                if (gui_widget_isexpanded(h, 0)) {
                    gui_dim_t tmpx, tmpy;
                    tmpx = x + wi - 2 * topH + 4;
                    tmpy = y + 7;
                    gui_draw_rectangle(disp, tmpx, tmpy + 4, topH - 14, topH - 14, GUI_COLOR_BLACK);
                    gui_draw_hline(disp, tmpx + 1, tmpy + 5, topH - 16, GUI_COLOR_BLACK); 
                    
                    gui_draw_filledrectangle(disp, tmpx + 4, tmpy, topH - 15, 2, GUI_COLOR_BLACK);
                    gui_draw_vline(disp, tmpx + 4, tmpy + 2, 2, GUI_COLOR_BLACK);
                    gui_draw_vline(disp, tmpx + 4 + topH - 15, tmpy, topH - 15, GUI_COLOR_BLACK);
                    gui_draw_hline(disp, tmpx + topH - 14, tmpy + topH - 15, 4, GUI_COLOR_BLACK);
                } else {
                    gui_draw_rectangle(disp, x + wi - 2 * topH + 4, y + 7, topH - 10, topH - 10, GUI_COLOR_BLACK); 
                    gui_draw_hline(disp, x + wi - 2 * topH + 4, y + 8, topH - 10, GUI_COLOR_BLACK); 
                }
                
                /* Draw "close" button */
                gui_draw_rectangle3d(disp, x + wi - topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + wi - topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_WIN_RED);
                
                tx = x + wi - topH + 6;
                ty = y + 8;
                tW = topH - 15;
                
                gui_draw_line(disp, tx,     ty,     tx + tW,     ty + tW,     GUI_COLOR_WHITE);
                gui_draw_line(disp, tx + 1, ty,     tx + tW,     ty + tW - 1, GUI_COLOR_WHITE);
                gui_draw_line(disp, tx,     ty + 1, tx + tW - 1, ty + tW,     GUI_COLOR_WHITE);
                
                gui_draw_line(disp, tx,     ty + tW,     tx + tW,     ty,     GUI_COLOR_WHITE);
                gui_draw_line(disp, tx + 1, ty + tW,     tx + tW,     ty + 1, GUI_COLOR_WHITE);
                gui_draw_line(disp, tx,     ty + tW - 1, tx + tW - 1, ty,     GUI_COLOR_WHITE);
                
                if (guii_widget_isfontandtextset(h)) {
                    gui_draw_font_t f;
                    gui_draw_font_init(&f);         /* Init structure */
                    
                    f.x = x + 3;
                    f.y = y + 3;
                    f.width = wi - 3 * topH - 5;
                    f.height = topH - 3;
                    f.align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.color1width = f.width;
                    f.color1 = guii_widget_getcolor(h, GUI_WINDOW_COLOR_TEXT);
                    gui_draw_writetext(disp, gui_widget_getfont(h, 0), gui_widget_gettext(h, 0), &f);
                }
            }
            
            //guii_widget_setcolor(h, GUI_WINDOW_COLOR_BG, (guii_widget_getcolor(h, GUI_WINDOW_COLOR_BG) + 5) * (guii_widget_getcolor(h, GUI_WINDOW_COLOR_BG) + 10));
            
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_WC_TouchStart: {
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED;  /* Set handled status */
            if (!guii_widget_hasparent(h)) {    /* Ignore on base window */
                return 1;
            }
            if (ts->ts.count == 1 && guii_widget_getflag(h, GUI_FLAG_CHILD)) { /* For children widgets only on single touch */
                gui_dim_t pt, wi;
                pt = gui_widget_getpaddingtop(h, 0);/* Get top padding */
                wi = gui_widget_getwidth(h, 0); /* Get widget width */
                
                if (ts->y_rel[0] < pt && ts->x_rel[0] < (wi - pt)) {
                    Mode = 1;
                    tx = ts->x_rel[0];
                    ty = ts->y_rel[0];
                }
                GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED; /* Set handled status */
            } else {
                Mode = 0;
                GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLEDNOFOCUS;  /* Set handled status */
            }
            return 1;
        }
        case GUI_WC_TouchMove: {
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            
            if (Mode == 1 && guii_widget_getflag(h, GUI_FLAG_CHILD)) {
                gui_dim_t px, py;

                px = guii_widget_getparentabsolutex(h);
                py = guii_widget_getparentabsolutey(h);
                gui_widget_setposition(h, ts->ts.x[0] - px - tx, ts->ts.y[0] - py - ty, 0);
                
                if (gui_widget_isexpanded(h, 0)) {  /* If it is expanded */
                    gui_widget_setexpanded(h, 0, 0);/* Clear expanded mode */
                }
                GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED; /* Set handled status */
            }
            
            return 1;
        }
        case GUI_WC_TouchEnd: {
            Mode = 0;
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_WC_Click: {
            gui_dim_t pt, wi;
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */

            pt = gui_widget_getpaddingtop(h, 0);    /* Get top padding */
            wi = gui_widget_getwidth(h, 0);         /* Get widget width */
            
            if (ts->y_rel[0] <= pt) {               /* Top widget part detected */
                pt = pt - 4;
                if (ts->x_rel[0] > (wi - 3 * pt) && ts->x_rel[0] < (wi - 2 * pt)) {
                    gui_widget_hide(h, 0);          /* Hide widget */
                } else if (ts->x_rel[0] > (wi - 2 * pt) && ts->x_rel[0] < (wi - pt)) {
                    gui_widget_toggleexpanded(h, 0);/* Change expanded mode */
                } else if (ts->x_rel[0] >= (wi - pt)) {
                    gui_widget_remove(&h, 0);       /* Remove widget */
                }
            }
            return 1;
        }
        case GUI_WC_DblClick: {
            gui_dim_t pt;
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            
            pt = gui_widget_getpaddingtop(h, 0);    /* Get top padding */
            
            if (ts->y_rel[0] <= pt) {               /* Top widget part detected */
                gui_widget_toggleexpanded(h, 0);    /* Toggle expanded mode */
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            guii_keyboard_data_t* kb = GUI_WIDGET_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_DOWN) {
                gui_widget_setposition(h, GUI_DIM(h->x), GUI_DIM(h->y + 1), 0);
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->kb.keys[0] == GUI_KEY_UP) {
                gui_widget_setposition(h, GUI_DIM(h->x), GUI_DIM(h->y - 1), 0);
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->kb.keys[0] == GUI_KEY_LEFT) {
                gui_widget_setposition(h, GUI_DIM(h->x - 1), GUI_DIM(h->y), 0);
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->kb.keys[0] == GUI_KEY_RIGHT) {
                gui_widget_setposition(h, GUI_DIM(h->x + 1), GUI_DIM(h->y), 0);
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef w
  
/**
 * \brief           Create new base widget window with desktop window as a parent
 * \param           id: Widget unique ID to use for identity for callback processing
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \return          Widget handle on success, `NULL` otherwise
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_window_createdesktop(gui_id_t id, gui_widget_callback_t cb, const uint8_t protect) {
    return (gui_handle_p)gui_widget_create(&widget, id, 0, 0, gui_lcd_getwidth(), gui_lcd_getheight(), 0, cb, GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP, protect);  /* Allocate memory for basic widget */
}

/**
 * \brief           Create new window widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_window_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect) {
    gui_window_t* ptr;

    __GUI_ENTER(protect);                           /* Enter GUI */
    ptr = gui_widget_create(&widget, id, x, y, width, height, parent, cb, flags, 0);/* Allocate memory for basic widget */
    if (ptr != NULL) {
        guii_widget_setflag(__GH(ptr), GUI_FLAG_CHILD); /* This window is child window */
        gui_widget_setpaddingtop(__GH(ptr), 30, 0);
        gui_widget_setpaddingright(__GH(ptr), 2, 0);
        gui_widget_setpaddingbottom(__GH(ptr), 2, 0);
        gui_widget_setpaddingleft(__GH(ptr), 2, 0);
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */
    return (gui_handle_p)ptr;
}

/**
 * \brief           Set color to specific part of widget
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_window_setcolor(gui_handle_p h, gui_window_color_t index, gui_color_t color, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return gui_widget_setcolor(h, (uint8_t)index, color, protect);  /* Set color */
}
 
/**
 * \brief           Set active window for future widgets and for current top window
 * \param[in]       h: Widget handle to set as active window
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_window_setactive(gui_handle_p h, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    
    __GUI_ENTER(protect);                           /* Enter GUI */
    GUI.window_active = h;                          /* Set new active window */
    guii_widget_movedowntree(h);                    /* Move widget down on tree */
    
    guii_widget_focus_clear();                      /* Clear focus on widget */
    guii_widget_active_clear();                     /* Clear active on widget */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Get desktop window
 * \return          Widget handle of desktop window
 */
gui_handle_p
gui_window_getdesktop(void) {
    return (gui_handle_p)gui_linkedlist_getnext_gen(&GUI.root, NULL);   /* Return desktop window */
}

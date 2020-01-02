/**	
 * \file            gui_listview.h
 * \brief           Listview widget
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef GUI_HDR_LISTVIEW_H
#define GUI_HDR_LISTVIEW_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_LISTVIEW List view
 * \brief           List view object
 * \{
 */

/**
 * \brief           List of listview colors
 */
typedef enum {
    GUI_LISTVIEW_COLOR_BG = 0x00,           /*!< Background color */
    GUI_LISTVIEW_COLOR_TEXT,                /*!< Text color index */
    GUI_LISTVIEW_COLOR_SEL_FOC,             /*!< Text color of selected item when widget is in focus */
    GUI_LISTVIEW_COLOR_SEL_NOFOC,           /*!< Text color of selected item when widget is not in focus */
    GUI_LISTVIEW_COLOR_SEL_FOC_BG,          /*!< Background color of selected item when widget is in focus */
    GUI_LISTVIEW_COLOR_SEL_NOFOC_BG,        /*!< Background color of selected item when widget is not in focus */
    GUI_LISTVIEW_COLOR_BORDER,              /*!< Border color index for top line when not in 3D mode */
} gui_listview_color_t;

struct gui_listview_row;

/**
 * \brief           Typedef for row for public usage
 */
typedef struct gui_listview_row* gui_listview_row_p;
   
gui_handle_p    gui_listview_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_listview_setcolor(gui_handle_p h, gui_listview_color_t index, gui_color_t color);

uint8_t         gui_listview_addcolumn(gui_handle_p h, const gui_char* text, gui_dim_t width);
uint8_t         gui_listview_setcolumnwidth(gui_handle_p h, uint16_t index, gui_dim_t width);

gui_listview_row_p gui_listview_addrow(gui_handle_p h);
uint8_t         gui_listview_removerow(gui_handle_p h, int16_t index);
uint8_t         gui_listview_removerows(gui_handle_p h);

uint8_t         gui_listview_setitemstring(gui_handle_p h, gui_listview_row_p row, uint16_t col, const gui_char* text);
uint8_t         gui_listview_setselection(gui_handle_p h, int16_t selection);
int16_t         gui_listview_getselection(gui_handle_p h);
uint8_t         gui_listview_setsliderauto(gui_handle_p h, uint8_t autoMode);
uint8_t         gui_listview_setslidervisibility(gui_handle_p h, uint8_t visible);
uint8_t         gui_listview_scroll(gui_handle_p h, int16_t step);
uint8_t         gui_listview_getitemvalue(gui_handle_p h, int16_t rindex, uint16_t cindex, gui_char* dst, size_t length);

/**
 * \}
 */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_LISTVIEW_H */

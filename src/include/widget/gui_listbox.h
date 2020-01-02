/**	
 * \file            gui_listbox.h
 * \brief           List box widget
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
#ifndef GUI_HDR_LISTBOX_H
#define GUI_HDR_LISTBOX_H

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
 * \defgroup        GUI_LISTBOX Listbox
 * \brief           LISTBOX widget
 * \{
 */
    
/**
 * \brief           List of listbox colors
 */
typedef enum {
    GUI_LISTBOX_COLOR_BG = 0x00,            /*!< Background color */
    GUI_LISTBOX_COLOR_TEXT,                 /*!< Text color index */
    GUI_LISTBOX_COLOR_SEL_FOC,              /*!< Text color of selected item when widget is in focus */
    GUI_LISTBOX_COLOR_SEL_NOFOC,            /*!< Text color of selected item when widget is not in focus */
    GUI_LISTBOX_COLOR_SEL_FOC_BG,           /*!< Background color of selected item when widget is in focus */
    GUI_LISTBOX_COLOR_SEL_NOFOC_BG,         /*!< Background color of selected item when widget is not in focus */
} gui_listbox_color_t;

gui_handle_p    gui_listbox_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_listbox_setcolor(gui_handle_p h, gui_listbox_color_t index, gui_color_t color);
uint8_t         gui_listbox_addstring(gui_handle_p h, const gui_char* text);
uint8_t         gui_listbox_deletefirststring(gui_handle_p h);
uint8_t         gui_listbox_deletelaststring(gui_handle_p h);
uint8_t         gui_listbox_deletestring(gui_handle_p h, int16_t index);
uint8_t         gui_listbox_setstring(gui_handle_p h, int16_t index, const gui_char* text);
uint8_t         gui_listbox_setselection(gui_handle_p h, int16_t selection);
int16_t         gui_listbox_getselection(gui_handle_p h);
uint8_t         gui_listbox_scroll(gui_handle_p h, int16_t step);

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

#endif /* GUI_HDR_LISTBOX_H */

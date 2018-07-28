/**	
 * \file            gui_listbox.h
 * \brief           List box widget
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
#ifndef __GUI_LISTBOX_H
#define __GUI_LISTBOX_H

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

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
#define GUI_FLAG_LISTBOX_SLIDER_ON      0x01/*!< Slider is currently active */
#define GUI_FLAG_LISTBOX_SLIDER_AUTO    0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \brief           LISTBOX string item object
 */
typedef struct {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_char* text;                         /*!< Text entry */
} gui_listbox_item_t;
    
/**
 * \brief           LISTBOX object structure
 */
typedef struct {
    gui_handle C;                           /*!< GUI handle object, must always be first on list */
    
    int16_t count;                          /*!< Current number of strings attached to this widget */
    int16_t selected;                       /*!< selected text index */
    int16_t visiblestartindex;              /*!< Index in array of string on top of visible area of widget */
    
    gui_linkedlistroot_t root;              /*!< Root of linked list entries */
    
    gui_dim_t sliderwidth;                  /*!< Slider width in units of pixels */
    uint8_t flags;                          /*!< Widget flags */
} gui_listbox_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

gui_handle_p    gui_listbox_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect);
uint8_t         gui_listbox_setcolor(gui_handle_p h, gui_listbox_color_t index, gui_color_t color, const uint8_t protect);
uint8_t         gui_listbox_addstring(gui_handle_p h, const gui_char* text, const uint8_t protect);
uint8_t         gui_listbox_deletefirststring(gui_handle_p h, const uint8_t protect);
uint8_t         gui_listbox_deletelaststring(gui_handle_p h, const uint8_t protect);
uint8_t         gui_listbox_deletestring(gui_handle_p h, uint16_t index, const uint8_t protect);
uint8_t         gui_listbox_setstring(gui_handle_p h, uint16_t index, const gui_char* text, const uint8_t protect);
uint8_t         gui_listbox_setselection(gui_handle_p h, int16_t selection, const uint8_t protect);
int16_t         gui_listbox_getselection(gui_handle_p h, const uint8_t protect);
uint8_t         gui_listbox_setsliderauto(gui_handle_p h, uint8_t autoMode, const uint8_t protect);
uint8_t         gui_listbox_setslidervisibility(gui_handle_p h, uint8_t visible, const uint8_t protect);
uint8_t         gui_listbox_scroll(gui_handle_p h, int16_t step, const uint8_t protect);

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

#endif /* __GUI_LISTBOX_H */

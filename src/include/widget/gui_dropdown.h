/**	
 * \file            gui_dropdown.h
 * \brief           Dropdown widget
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
#ifndef __GUI_DROPDOWN_H
#define __GUI_DROPDOWN_H

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
 * \defgroup        GUI_DROPDOWN Dropdown
 * \brief           Dropdown widget
 * \{
 */

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
#define GUI_FLAG_DROPDOWN_OPENED        0x01/*!< Dropdown is opened */
#define GUI_FLAG_DROPDOWN_SLIDER_ON     0x02/*!< Slider is currently active */
#define GUI_FLAG_DROPDOWN_SLIDER_AUTO   0x04/*!< Show right slider automatically when required, otherwise, manual mode is used */
#define GUI_FLAG_DROPDOWN_OPEN_UP       0x08/*!< Open dropdown in top direction instead of bottom */
    
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */
    
/**
 * \brief           List of dropdown colors
 */
typedef enum {
    GUI_DROPDOWN_COLOR_BG = 0x00,           /*!< Background color index */
    GUI_DROPDOWN_COLOR_TEXT,                /*!< Text color index */
    GUI_DROPDOWN_COLOR_SEL_FOC,             /*!< Text color index of selected item when widget is in focus */
    GUI_DROPDOWN_COLOR_SEL_NOFOC,           /*!< Text color index of selected item when widget is not in focus */
    GUI_DROPDOWN_COLOR_SEL_FOC_BG,          /*!< Background color index of selected item when widget is in focus */
    GUI_DROPDOWN_COLOR_SEL_NOFOC_BG,        /*!< Background color index of selected item when widget is not in focus */
} gui_dropdown_color_t;

/**
 * \brief           Dropdown open direction
 */
typedef enum {
    GUI_DROPDOWN_OPENDIR_DOWN = 0x00,       /*!< Dropdown will open down according to current position */
    GUI_DROPDOWN_OPENDIR_UP = 0x01          /*!< Dropdown will open up according to current position */
} gui_dropdown_opendir_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__

/**
 * \brief           Dropdown string item object
 */
typedef struct {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_char* text;                         /*!< Text entry */
} gui_dropdown_item_t;
    
/**
 * \brief           Dropdown object structure
 */
typedef struct {
    gui_handle C;                           /*!< GUI handle object, must always be first on list */
    
    int16_t count;                          /*!< Current number of strings attached to this widget */
    int16_t selected;                       /*!< selected text index */
    int16_t visiblestartindex;              /*!< Index in array of string on top of visible area of widget */
    
    gui_linkedlistroot_t root;              /*!< Root of linked list entries */
    
    gui_dim_t sliderwidth;                  /*!< Slider width in units of pixels */
    uint8_t flags;                          /*!< Widget flags */
} gui_dropdown_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

gui_handle_p    gui_dropdown_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_dropdown_setcolor(gui_handle_p h, gui_dropdown_color_t index, gui_color_t color);
uint8_t         gui_dropdown_addstring(gui_handle_p h, const gui_char* text);
uint8_t         gui_dropdown_deletefirststring(gui_handle_p h);
uint8_t         gui_dropdown_deletelaststring(gui_handle_p h);
uint8_t         gui_dropdown_deletestring(gui_handle_p h, uint16_t index);
uint8_t         gui_dropdown_setstring(gui_handle_p h, uint16_t index, const gui_char* text);
uint8_t         gui_dropdown_setselection(gui_handle_p h, int16_t selection);
int16_t         gui_dropdown_getselection(gui_handle_p h);
uint8_t         gui_dropdown_setsliderauto(gui_handle_p h, uint8_t autoMode);
uint8_t         gui_dropdown_setslidervisibility(gui_handle_p h, uint8_t visible);
uint8_t         gui_dropdown_scroll(gui_handle_p h, int16_t step);
uint8_t         gui_dropdown_setopendirection(gui_handle_p h, gui_dropdown_opendir_t dir);
    
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

#endif /* __GUI_DROPDOWN_H */

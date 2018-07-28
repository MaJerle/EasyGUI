/**	
 * \file            gui_listview.h
 * \brief           Listview widget
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
#ifndef __GUI_LISTVIEW_H
#define __GUI_LISTVIEW_H

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

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
#define GUI_FLAG_LISTVIEW_SLIDER_ON     0x01/*!< Slider is currently active */
#define GUI_FLAG_LISTVIEW_SLIDER_AUTO   0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \brief           Listview main row item
 */
typedef struct {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_linkedlistroot_t root;              /*!< Linked list root entry for \ref gui_listview_item_t column data entries */
} gui_listview_row_t;

/**
 * \brief           Listview row object
 * \hideinitializer
 */
#define __GLR(x)            ((gui_listview_row_t *)(x))

/**
 * \brief           Listview column item
 */
typedef struct {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_dim_t width;                       /*!< Column width in units of pixels */
    gui_char* text;                         /*!< Header column text size */
} gui_listview_col_t;

/**
 * \brief           Listview string item object
 */
typedef struct {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_char* text;                         /*!< Text entry */
} gui_listview_item_t;
    
/**
 * \brief           Listview object structure
 */
typedef struct {
    gui_handle C;                           /*!< GUI handle object, must always be first on list */
    
    /*
     * Use colums as pointer to array for faster data access
     */
    gui_listview_col_t** cols;              /*!< Pointer to pointers of column elements */
    uint16_t col_count;                     /*!< Number of columns in listview package */
    
    /*
     * Use linked list for rows
     */
    gui_linkedlistroot_t root;              /*!< Linked list root entry for \ref gui_listview_row_t for rows */
    
    int16_t count;                          /*!< Current number of strings attached to this widget */
    int16_t selected;                       /*!< selected text index */
    int16_t visiblestartindex;              /*!< Index in array of string on top of visible area of widget */
    
    gui_dim_t sliderwidth;                  /*!< Slider width in units of pixels */
    uint8_t flags;                          /*!< Widget flags */
} gui_listview_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

/**
 * \brief           Typedef for row for public usage
 */
typedef void* gui_listview_row_p;
   
gui_handle_p    gui_listview_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect);
uint8_t         gui_listview_setcolor(gui_handle_p h, gui_listview_color_t index, gui_color_t color, const uint8_t protect);

uint8_t         gui_listview_addcolumn(gui_handle_p h, const gui_char* text, gui_dim_t width, const uint8_t protect);
uint8_t         gui_listview_setcolumnwidth(gui_handle_p h, uint16_t index, gui_dim_t width, const uint8_t protect);

gui_listview_row_p gui_listview_addrow(gui_handle_p h, const uint8_t protect);
uint8_t         gui_listview_removerow(gui_handle_p h, gui_listview_row_p row, const uint8_t protect);
uint8_t         gui_listview_removerows(gui_handle_p h, const uint8_t protect);

uint8_t         gui_listview_setitemstring(gui_handle_p h, gui_listview_row_p row, uint16_t col, const gui_char* text, const uint8_t protect);
uint8_t         gui_listview_setselection(gui_handle_p h, int16_t selection, const uint8_t protect);
int16_t         gui_listview_getselection(gui_handle_p h, const uint8_t protect);
uint8_t         gui_listview_setsliderauto(gui_handle_p h, uint8_t autoMode, const uint8_t protect);
uint8_t         gui_listview_setslidervisibility(gui_handle_p h, uint8_t visible, const uint8_t protect);
uint8_t         gui_listview_scroll(gui_handle_p h, int16_t step, const uint8_t protect);
uint8_t         gui_listview_getitemvalue(gui_handle_p h, uint16_t rindex, uint16_t cindex, gui_char* dst, size_t length, const uint8_t protect);

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

#endif /* __GUI_LISTVIEW_H */

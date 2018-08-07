/**	
 * \file            gui_widget_list.h
 * \brief           Helper functions for widgets implementing view with list (dropdown, listbox, listview, debugbox, etc)
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
#ifndef __GUI_WIDGET_LIST_H
#define __GUI_WIDGET_LIST_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_WIDGET_LIST List view helpers
 * \brief           Helper functions for widgets implementing view for lists
 * \{
 *
 * These helper functions share the same functionality between different widgets
 *
 * Example widgets using this helper: \ref GUI_LISTBOX, \ref GUI_LISTVIEW, \ref GUI_DEBUGBOX, \ref GUI_DROPDOWN
 *
 */

/**
 * \brief           List data structure
 */
typedef struct {
    gui_linkedlistroot_t root;                      /*!< Linked list of list item entries */
    
    int16_t max_visible_items;                      /*!< Maximal number of visible list items at a time */
    int16_t count;                                  /*!< Number of all entries in a list */
    int16_t visiblestartindex;                      /*!< Index in array of string on top of visible area of widget */
    
    uint8_t (*check_values_cb)(gui_handle_p h);     /*!< Check values callback */
    int16_t (*entries_per_page_cb)(gui_handle_p h); /*!< Entries per page callback */
    uint8_t (*remove_item_cb)(gui_handle_p h, void* item);  /* Remove item callback */
} gui_widget_listdata_t;

uint8_t     gui_widget_list_init(gui_handle_p h, gui_widget_listdata_t* ld);
uint8_t     gui_widget_list_slide(gui_handle_p h, gui_widget_listdata_t* ld, int16_t dir);
int16_t     gui_widget_list_get_count(gui_handle_p h, gui_widget_listdata_t* ld);
uint8_t     gui_widget_list_add_item(gui_handle_p h, gui_widget_listdata_t* ld, void* element);
void *      gui_widget_list_get_item_byindex(gui_handle_p h, gui_widget_listdata_t* ld, int16_t index);
uint8_t     gui_widget_list_remove_item_byindex(gui_handle_p h, gui_widget_listdata_t* ld, int16_t index);
uint8_t     gui_widget_list_remove_items(gui_handle_p h, gui_widget_listdata_t* ld);
uint8_t     gui_widget_list_check_values(gui_handle_p h, gui_widget_listdata_t* ld);

uint8_t     gui_widget_list_inc_selection(gui_handle_p h, gui_widget_listdata_t* ld, int16_t dir, int16_t* curr_selected);
uint8_t     gui_widget_list_set_selection(gui_handle_p h, gui_widget_listdata_t* ld, int16_t* curr_selected, int16_t new_selection);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __GUI_WIDGET_LIST_H */
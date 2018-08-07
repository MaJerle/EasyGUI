/**	
 * \file            gui_widget_list.c
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
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "widget/gui_widget_list.h"

#define ENTRIES_PER_PAGE(h, ld)         (((ld)->entries_per_page_cb != NULL) ? (ld)->entries_per_page_cb(h) : 0)

/**
 * \brief           Set active selection
 * \param[in]       h: Widget handle
 * \param[in,out]   selected: Current selection
 * \param[in]       new_selection: New selection to be set
 * \return          `1` on success, `0` otherwise
 */
static void
set_selection(gui_handle_p h, gui_widget_listdata_t* ld, int16_t* selected, int16_t new_selection) {
    if (*selected != new_selection && new_selection < ld->count && new_selection >= -1) {
        *selected = new_selection;
        guii_widget_callback(h, GUI_EVT_SELECTIONCHANGED, NULL, NULL);
        gui_widget_invalidate(h);
    }
}

/**
 * \brief           Check values internal function
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
check_values(gui_handle_p h, gui_widget_listdata_t* ld) {
    ld->max_visible_items = ENTRIES_PER_PAGE(h, ld);
   
    /* Check scrolling capabilities */
    if (ld->visiblestartindex < 0) {
        ld->visiblestartindex = 0;
    } else if (ld->visiblestartindex > 0) {
        if (ld->count > ld->max_visible_items) {
            if (ld->visiblestartindex + ld->max_visible_items >= ld->count) {
                ld->visiblestartindex = ld->count - ld->max_visible_items;
            }
        } else {
            ld->visiblestartindex = 0;
        }
    }
    
    /* Call widget callback */
    if (ld->check_values_cb != NULL) {
        ld->check_values_cb(h);
    }
    return 1;
}

/**
 * \brief           Initialize list helper module
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_init(gui_handle_p h, gui_widget_listdata_t* ld) {
    GUI_UNUSED(h);
    GUI_UNUSED(ld);
    
    return 1;
}

/**
 * \brief           Get number of elements on linked list
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in]       dir: 
 * \return          Number of items on list
 */
uint8_t
gui_widget_list_slide(gui_handle_p h, gui_widget_listdata_t* ld, int16_t dir) {
    ld->max_visible_items = ENTRIES_PER_PAGE(h, ld);
    if (dir < 0) {
        if ((ld->visiblestartindex + dir) < 0) {
            ld->visiblestartindex = 0;
        } else {
            ld->visiblestartindex += dir;
        }
        gui_widget_invalidate(h);
    } else if (dir > 0) {
        if ((ld->visiblestartindex + dir) > (ld->count - ld->max_visible_items - 1)) {
            ld->visiblestartindex = ld->count - ld->max_visible_items;
        } else {
            ld->visiblestartindex += dir;
        }
        gui_widget_invalidate(h);
    }
    return 1;
}

/**
 * \brief           Add new item to list
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in]       element: Item to add. Item must have \ref gui_linkedlist_t structure as first element
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_add_item(gui_handle_p h, gui_widget_listdata_t* ld, void* element) {
    gui_linkedlist_add_gen(&ld->root, element); /* Add to linked list */
    ld->count++;                                /* Increase number of items */
    
    check_values(h, ld);
    
    return 1;
}

/**
 * \brief           Get item by index number
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in]       index: Index to get element for
 * \return          Element at specific index or `NULL` in case of failure
 */
void *
gui_widget_list_get_item_byindex(gui_handle_p h, gui_widget_listdata_t* ld, int16_t index) {
    if (index == 0) {
        return (void *)gui_linkedlist_getnext_gen(&ld->root, NULL);
    } else if (index == ld->count - 1) {
        return (void *)gui_linkedlist_getnext_gen(&ld->root, NULL);
    }
    return (void *)gui_linkedlist_getnext_byindex_gen(&ld->root, index);
}

/**
 * \brief           Remove item from list by index value
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in]       index: Index to search for item and remove
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_remove_item_byindex(gui_handle_p h, gui_widget_listdata_t* ld, int16_t index) {
    void* item;
    
    item = gui_linkedlist_getnext_byindex_gen(&ld->root, index);
    if (item != NULL) {
        item = gui_linkedlist_remove_gen(&ld->root, item);
        ld->count--;
        if (ld->remove_item_cb != NULL) {
            ld->remove_item_cb(h, item);
        }
        return 1;
    }
    return 0;
}

/**
 * \brief           Remove all items from list
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_remove_items(gui_handle_p h, gui_widget_listdata_t* ld) {
    void* item;
    
    while (ld->count) {
        item = gui_linkedlist_getnext_byindex_gen(&ld->root, 0);
        if (item != NULL) {
            item = gui_linkedlist_remove_gen(&ld->root, item);
            if (ld->remove_item_cb != NULL) {
                ld->remove_item_cb(h, item);
            }
        } else {
            return 0;
        }
        ld->count--;
    }
    return 1;
}

/**
 * \brief           Check all the values if ok represented and correct problematic
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_check_values(gui_handle_p h, gui_widget_listdata_t* ld) {
    return check_values(h, ld);
}

/**
 * \brief           Set new active selected item from a list
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in]       dir: Direction for selection set
 * \param[in,out]   curr_selected: Pointer to current selection which will be updated only
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_inc_selection(gui_handle_p h, gui_widget_listdata_t* ld, int16_t dir, int16_t* curr_selected) {
    if (dir < 0) {
        /* Slide elements up */
        if ((*curr_selected + dir) < 0) {
            set_selection(h, ld, curr_selected, 0);
        } else {
            set_selection(h, ld, curr_selected, *curr_selected + dir);
        }
    } else if (dir > 0) {
        /* Slide elements down */
        if ((*curr_selected + dir) > (ld->count - 1)) { 
            set_selection(h, ld, curr_selected, ld->count - 1);
        } else {
            set_selection(h, ld, curr_selected, *curr_selected + dir);
        }
    }
    return 1;
}

/**
 * \brief           Set new active selected item from a list
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in,out]   curr_selected: Pointer to current selection which will be updated only
 * \param[in]       new_selection: New selection value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_set_selection(gui_handle_p h, gui_widget_listdata_t* ld, int16_t* curr_selected, int16_t new_selection) {
    set_selection(h, ld, curr_selected, new_selection);
    return 1;
}

/**
 * \brief           Set top visible start list index
 * \param[in]       h: Widget handle
 * \param[in]       ld: List data handle
 * \param[in]       start: New start visible index
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_list_set_visible_start_index(gui_handle_p h, gui_widget_listdata_t* ld, int16_t start) {
    if (start < 0) {
        start = 0;
    }
    ld->visiblestartindex = start;
    if ((ld->visiblestartindex + ENTRIES_PER_PAGE(h, ld)) >= ld->count) {
        ld->visiblestartindex = ld->count - ENTRIES_PER_PAGE(h, ld);
    }
    return 1;
}

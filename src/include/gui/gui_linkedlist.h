/**	
 * \file            gui_linkedlist.h
 * \brief           Linked list manager
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
#ifndef __GUI_LINKEDLIST_H
#define __GUI_LINKEDLIST_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_UTILS
 * \brief       
 * \{
 */
#include "gui_utils.h"

/**
 * \defgroup        GUI_LINKEDLIST Linked list
 * \brief           Linked list generic functions
 * \{
 */

#if defined(GUI_INTERNAL) || __DOXYGEN__

/**
 * \brief           Get data from multi linked list object
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       element: Linked list element of \ref GUI_LinkedListMulti_t structure
 * \retval          Element in linked list
 * \hideinitializer
 */
#define gui_linkedlist_multi_getdata(element)     ((element) ? (element)->Element : 0)
void gui_linkedlist_add_gen(gui_linkedlistroot_t* root, gui_linkedlist_t* element);
gui_linkedlist_t* gui_linkedlist_remove_gen(gui_linkedlistroot_t* root, gui_linkedlist_t* element);
gui_linkedlist_t* gui_linkedlist_getnext_gen(gui_linkedlistroot_t* root, gui_linkedlist_t* element);
gui_linkedlist_t* gui_linkedlist_getprev_gen(gui_linkedlistroot_t* root, gui_linkedlist_t* element);
gui_linkedlist_t* gui_linkedlist_getnext_byindex_gen(gui_linkedlistroot_t* root, uint16_t index);
uint8_t gui_linkedlist_movedown_gen(gui_linkedlistroot_t* root, gui_linkedlist_t* element);
uint8_t gui_linkedlist_moveup_gen(gui_linkedlistroot_t* root, gui_linkedlist_t* element);
GUI_LinkedListMulti_t* gui_linkedlist_multi_add_gen(gui_linkedlistroot_t* root, void* element);
uint8_t gui_linkedlist_multi_remove_gen(gui_linkedlistroot_t* root, GUI_LinkedListMulti_t* element);
GUI_LinkedListMulti_t* gui_linkedlist_multi_getnext_gen(gui_linkedlistroot_t* root, GUI_LinkedListMulti_t* element);
GUI_LinkedListMulti_t* gui_linkedlist_multi_getprev_gen(gui_linkedlistroot_t* root, GUI_LinkedListMulti_t* element);
uint8_t gui_linkedlist_multi_movedown_gen(gui_linkedlistroot_t* root, GUI_LinkedListMulti_t* element);
uint8_t gui_linkedlist_multi_moveup_gen(gui_linkedlistroot_t* root, GUI_LinkedListMulti_t* element);
uint8_t gui_linkedlist_multi_find_remove(gui_linkedlistroot_t* root, void* element);
    
/**
 * \brief           Checks if linked list has any entries
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \return          `1` on success, `0` otherwise No entries
 * \retval          > 0: Has entries
 * \hideinitializer
 */
#define gui_linkedlist_hasentries(root)       (!!((root)->First))

/**
 * \defgroup        GUI_LINKEDLIST_WIDGET Widget linked list
 * \brief           Linked list related to widgets
 *
 * Each widget can only be on one linked list at a time and it has static allocation of \ref gui_linkedlist_t structure,
 * which simplifies linked list API management without need to allocate additinal link structure
 *
 * \{
 */

/**
 * \brief           Check if widget is first child element in linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise Widget is first child
 * \return          `1` on success, `0` otherwise Widget is not first child
 * \sa              gui_linkedlist_iswidgetlast
 * \hideinitializer
 */
#define gui_linkedlist_iswidgetfirst(h)       (!(h) || !(__GH(h))->List.Prev)

/**
 * \brief           Check if widget is last child element in linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise Widget is last child
 * \return          `1` on success, `0` otherwise Widget is not last child
 * \sa              gui_linkedlist_iswidgetfirst
 * \hideinitializer
 */
#define gui_linkedlist_iswidgetlast(h)        (!(h) || !(__GH(h))->List.Next)
void gui_linkedlist_widgetadd(gui_handle_ROOT_t* parent, gui_handle_p h);
void gui_linkedlist_widgetremove(gui_handle_p h);
uint8_t gui_linkedlist_widgetmoveup(gui_handle_p h);
uint8_t gui_linkedlist_widgetmovedown(gui_handle_p h);
gui_handle_p gui_linkedlist_widgetgetnext(gui_handle_ROOT_t* parent, gui_handle_p h);
gui_handle_p gui_linkedlist_widgetgetprev(gui_handle_ROOT_t* parent, gui_handle_p h);
uint8_t gui_linkedlist_widgetmovetobottom(gui_handle_p h);
uint8_t gui_linkedlist_widgetmovetotop(gui_handle_p h);
 
#endif /* GUI_INTERNAL || __DOXYGEN__ */

/**
 * \}
 */

void gui_linkedlist_printlist(void);

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

#endif /* __GUI_LINKEDLIST_H */

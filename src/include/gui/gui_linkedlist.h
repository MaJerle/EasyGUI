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
 *
 * \par Doubly linked list
 *
 * Library operates with <b>doubly</b> linked list type since each element in list has next and previous element known. This allows loop from first to last and from last to first easily.
 *
 * Linked list link structure is statically allocated which does not add addditional overhead.
 *
 * \par Multi linked list
 *
 * Second support is <b>multi</b> linked list. This approach allows single element to be part of multiple linked lists.
 * 
 * In relation to first, this approach needs dynamic link memory allocation for each linked list it exists.
 *
 * \note            This functions can only be used inside GUI library
 * \{
 */

#if defined(GUI_INTERNAL) || defined(DOXYGEN)

/**
 * \brief           Get data from multi linked list object
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       element: Linked list element of \ref GUI_LinkedListMulti_t structure
 * \retval          Element in linked list
 * \hideinitializer
 */
#define gui_linkedlist_multi_getdata(element)     ((element) ? (element)->Element : 0)
void gui_linkedlist_add_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
GUI_LinkedList_t* gui_linkedlist_remove_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
GUI_LinkedList_t* gui_linkedlist_getnext_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
GUI_LinkedList_t* gui_linkedlist_getprev_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
GUI_LinkedList_t* gui_linkedlist_getnext_byindex_gen(GUI_LinkedListRoot_t* root, uint16_t index);
uint8_t gui_linkedlist_movedown_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
uint8_t gui_linkedlist_moveup_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
GUI_LinkedListMulti_t* gui_linkedlist_multi_add_gen(GUI_LinkedListRoot_t* root, void* element);
uint8_t gui_linkedlist_multi_remove_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
GUI_LinkedListMulti_t* gui_linkedlist_multi_getnext_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
GUI_LinkedListMulti_t* gui_linkedlist_multi_getprev_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
uint8_t gui_linkedlist_multi_movedown_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
uint8_t gui_linkedlist_multi_moveup_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
uint8_t gui_linkedlist_multi_find_remove(GUI_LinkedListRoot_t* root, void* element);
    
/**
 * \brief           Checks if linked list has any entries
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \retval          0: No entries
 * \retval          > 0: Has entries
 * \hideinitializer
 */
#define gui_linkedlist_hasentries(root)       (!!((root)->First))

/**
 * \defgroup        GUI_LINKEDLIST_WIDGET Widget linked list
 * \brief           Linked list related to widgets
 *
 * Each widget can only be on one linked list at a time and it has static allocation of \ref GUI_LinkedList_t structure,
 * which simplifies linked list API management without need to allocate additinal link structure
 *
 * \{
 */

/**
 * \brief           Check if widget is first child element in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          1: Widget is first child
 * \retval          0: Widget is not first child
 * \sa              gui_linkedlist_iswidgetlast
 * \hideinitializer
 */
#define gui_linkedlist_iswidgetfirst(h)       (!(h) || !(__GH(h))->List.Prev)

/**
 * \brief           Check if widget is last child element in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          1: Widget is last child
 * \retval          0: Widget is not last child
 * \sa              gui_linkedlist_iswidgetfirst
 * \hideinitializer
 */
#define gui_linkedlist_iswidgetlast(h)        (!(h) || !(__GH(h))->List.Next)
void gui_linkedlist_widgetadd(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);
void gui_linkedlist_widgetremove(GUI_HANDLE_p h);
uint8_t gui_linkedlist_widgetmoveup(GUI_HANDLE_p h);
uint8_t gui_linkedlist_widgetmovedown(GUI_HANDLE_p h);
GUI_HANDLE_p gui_linkedlist_widgetgetnext(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);
GUI_HANDLE_p gui_linkedlist_widgetgetprev(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);
uint8_t gui_linkedlist_widgetmovetobottom(GUI_HANDLE_p h);
uint8_t gui_linkedlist_widgetmovetotop(GUI_HANDLE_p h);
 
#endif /* GUI_INTERNAL || defined(DOXYGEN) */

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

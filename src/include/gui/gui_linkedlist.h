/**	
 * \file            gui_linkedlist.h
 * \brief           Linked list manager
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
#ifndef GUI_HDR_LINKEDLIST_H
#define GUI_HDR_LINKEDLIST_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui/gui_utils.h"

/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_LINKEDLIST Linked list
 * \brief           Linked list generic functions
 * \{
 */

#if defined(GUI_INTERNAL) || __DOXYGEN__

/**
 * \brief           Get data from multi linked list object
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       e: Linked list element of \ref gui_linkedlistmulti_t structure
 * \retval          Element in linked list
 * \hideinitializer
 */
#define gui_linkedlist_multi_getdata(e)     (((e) != NULL) ? (e)->element : NULL)

void                    gui_linkedlist_add_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element);
gui_linkedlist_t*       gui_linkedlist_remove_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element);
gui_linkedlist_t*       gui_linkedlist_getnext_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element);
gui_linkedlist_t*       gui_linkedlist_getprev_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element);
gui_linkedlist_t*       gui_linkedlist_getnext_byindex_gen(gui_linkedlistroot_t* const root, uint16_t index);
uint8_t                 gui_linkedlist_movedown_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element);
uint8_t                 gui_linkedlist_moveup_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element);
gui_linkedlistmulti_t*  gui_linkedlist_multi_add_gen(gui_linkedlistroot_t* const root, void* const element);
uint8_t                 gui_linkedlist_multi_remove_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* element);
gui_linkedlistmulti_t*  gui_linkedlist_multi_getnext_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element);
gui_linkedlistmulti_t*  gui_linkedlist_multi_getprev_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element);
uint8_t                 gui_linkedlist_multi_movedown_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element);
uint8_t                 gui_linkedlist_multi_moveup_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element);
uint8_t                 gui_linkedlist_multi_find_remove(gui_linkedlistroot_t* const root, void* const element);
    
/**
 * \brief           Checks if linked list has any entries
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define gui_linkedlist_hasentries(root)       ((root)->first != NULL)

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
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define gui_linkedlist_iswidgetfirst(h)       ((h) == NULL || (h)->list.prev == NULL)

/**
 * \brief           Check if widget is last child element in linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define gui_linkedlist_iswidgetlast(h)        ((h) == NULL || (h)->list.next == NULL)

void            gui_linkedlist_widgetadd(gui_handle_p parent, gui_handle_p h);
void            gui_linkedlist_widgetremove(gui_handle_p h);
uint8_t         gui_linkedlist_widgetmoveup(gui_handle_p h);
uint8_t         gui_linkedlist_widgetmovedown(gui_handle_p h);
gui_handle_p    gui_linkedlist_widgetgetnext(gui_handle_p parent, gui_handle_p h);
gui_handle_p    gui_linkedlist_widgetgetprev(gui_handle_p parent, gui_handle_p h);
uint8_t         gui_linkedlist_widgetmovetobottom(gui_handle_p h);
uint8_t         gui_linkedlist_widgetmovetotop(gui_handle_p h);

#define GUI_LINKEDLIST_WIDGETSLISTNEXT(parent, current)     for ((current) = gui_linkedlist_widgetgetnext((parent), NULL); (current) != NULL; (current) = gui_linkedlist_widgetgetnext(NULL, (current))) 
#define GUI_LINKEDLIST_WIDGETSLISTPREV(parent, current)     for ((current) = gui_linkedlist_widgetgetprev((parent), NULL); (current) != NULL; (current) = gui_linkedlist_widgetgetprev(NULL, (current))) 

/**
 * \}
 */

void gui_linkedlist_printlist(void);
 
#endif /* GUI_INTERNAL || __DOXYGEN__ */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_LINKEDLIST_H */

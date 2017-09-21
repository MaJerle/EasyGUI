/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI linked list functions
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2017 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_LINKEDLIST_H
#define GUI_LINKEDLIST_H

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

/**
 * \brief           Add element to doubly linked list
 * \note            Element added to linkedlist must have \ref GUI_LinkedList_t as top structure
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to add to base linked list
 * \retval          None
 * \sa              gui_linkedlist_remove_gen, gui_linkedlist_multi_add_gen, gui_linkedlist_multi_remove_gen
 */
void gui_linkedlist_add_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
    
/**
 * \brief           Remove element from doubly linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to remove from base linked list
 * \retval          Pointer to removed \ref GUI_LinkedList_t element
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_multi_add_gen, gui_linkedlist_multi_remove_gen
 */
GUI_LinkedList_t* gui_linkedlist_remove_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Get next element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_getnext_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Next element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>First element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Next element of <b>element</b> if exists, first element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to get next element of
 * \retval          > 0: Pointer to next element
 * \retval          0: No elements anymore in list
 * \sa              gui_linkedlist_getprev_gen, gui_linkedlist_multi_getnext_gen, gui_linkedlist_multi_getprev_gen
 */
GUI_LinkedList_t* gui_linkedlist_getnext_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Get previous element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_getnext_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Previous element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>Last element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Previous element of <b>element</b> if exists, last element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to get next element of
 * \retval          > 0: Pointer to previous element
 * \retval          0: No elements anymore in list
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_multi_getnext_gen, gui_linkedlist_multi_getprev_gen
 */
GUI_LinkedList_t* gui_linkedlist_getprev_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Get item from linked list by index
 * \note            Index value is the number of item in list order
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       index: Number in list to get item
 * \retval          0: Element not found
 * \retval          > 0: Pointer to \ref GUI_LinkedList_t structure
 */
GUI_LinkedList_t* gui_linkedlist_getnext_byindex_gen(GUI_LinkedListRoot_t* root, uint16_t index);

/**
 * \brief           Move widget down for one on doubly linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to move down on base linked list
 * \retval          1: Element moved down
 * \retval          0: Element was not moved down as it is already on bottom
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_multi_moveup_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t gui_linkedlist_movedown_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Move widget up for one on doubly linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to move up on base linked list
 * \retval          1: Element moved up
 * \retval          0: Element was not moved up as it is already on bottom
 * \sa              gui_linkedlist_movedown_gen, gui_linkedlist_multi_moveup_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t gui_linkedlist_moveup_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Add element to multi linked list
 * \note            Element can be any type since \ref GUI_LinkedListMulti_t structure is dynamicall allocated
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to add to base linked list
 * \retval          > 0: Pointer to allocated \ref GUI_LinkedListMulti_t structure with linked list entries
 * \retval          0: Failed to allocate memory for \ref GUI_LinkedListMulti_t structure
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_remove_gen, gui_linkedlist_multi_remove_gen
 */
GUI_LinkedListMulti_t* gui_linkedlist_multi_add_gen(GUI_LinkedListRoot_t* root, void* element);
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to remove from base linked list
 * \retval          1: Element removed and memory deallocated
 * \retval          0: Failed to remove element
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_remove_gen, gui_linkedlist_multi_add_gen
 */
uint8_t gui_linkedlist_multi_remove_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Get next element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_multi_getnext_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Next element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>First element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Next element of <b>element</b> if exists, first element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedListMulti_t element to get next element of
 * \retval          > 0: Pointer to next element
 * \retval          0: No elements anymore in list
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_getprev_gen, gui_linkedlist_multi_getprev_gen
 */
GUI_LinkedListMulti_t* gui_linkedlist_multi_getnext_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Get previous element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_multi_getprev_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Previous element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>Last element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Previous element of <b>element</b> if exists, last element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedListMulti_t element to get next element of
 * \retval          > 0: Pointer to previous element
 * \retval          0: No elements anymore in list
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_getprev_gen, gui_linkedlist_multi_getnext_gen
 */
GUI_LinkedListMulti_t* gui_linkedlist_multi_getprev_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Move widget down for one on multi linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedListMulti_t element to move down on base linked list
 * \retval          1: Element moved down
 * \retval          0: Element was not moved down as it is already on bottom
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_movedown_gen, gui_linkedlist_multi_moveup_gen
 */
uint8_t gui_linkedlist_multi_movedown_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Move widget up for one on multi linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedListMulti_t element to move up on base linked list
 * \retval          1: Element moved up
 * \retval          0: Element was not moved up as it is already on bottom
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_movedown_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t gui_linkedlist_multi_moveup_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            Delete by element pointer, not by list item. It searches in all entries for element pointers and deletes all links with that element pointer
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Memory address in data part of linked list entry
 * \retval          1: Element removed and memory deallocated
 * \retval          0: Failed to remove element
 * \sa              gui_linkedlist_remove_gen, gui_linkedlist_multi_remove_gen
 */
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

/**
 * \brief           Add new widget to linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *parent: Pointer to root handle object of parent object.
 *                      Set to NULL if there is no parent widget.
 * \param[in]       h: Widget handle to add to linked list
 * \retval          None
 * \sa              gui_linkedlist_widgetremove
 */
void gui_linkedlist_widgetadd(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);

/**
 * \brief           Remove widget from linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to remove from linked list
 * \retval          None
 * \sa              gui_linkedlist_widgetadd
 */
void gui_linkedlist_widgetremove(GUI_HANDLE_p h);

/**
 * \brief           Move widget up for one in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to move up
 * \retval          1: Move has been successful
 * \retval          0: Move was not successful. Widget is on top and cannot be moved
 * \sa              gui_linkedlist_widgetmovedown
 */
uint8_t gui_linkedlist_widgetmoveup(GUI_HANDLE_p h);

/**
 * \brief           Move widget down for one in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to move down
 * \retval          1: Move has been successful
 * \retval          0: Move was not successful. Widget is on bottom and cannot be moved
 * \sa              gui_linkedlist_widgetmoveup
 */
uint8_t gui_linkedlist_widgetmovedown(GUI_HANDLE_p h);

/**
 * \brief           Get next widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h   <th>gui_linkedlist_widgetgetnext(parent, h)
 *                      <tr><td>0       <td>0   <td>0
 *                      <tr><td>0       <td>> 0 <td>Next widget of <b>h</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0   <td>First widget in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0 <td>Next widget of <b>h</b> if exists, first widget of <b>parent</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \code{c}
//Get all widgets of parent widget from first to last
//Parent object must have GUI_HANDLE_ROOT_t data type
void LoopWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;         //Value holding widgets of parent

    for (h = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)parent, NULL); h; h = gui_linkedlist_widgetgetnext(NULL, h)) {
        //Loop through each widget of parent
    }
}
\endcode
 * \param[in]       *parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \retval          > 0: Next widget in linked list
 * \retval          0: No widget in linked list anymore
 * \sa              gui_linkedlist_widgetgetprev
 */
GUI_HANDLE_p gui_linkedlist_widgetgetnext(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);

/**
 * \brief           Get previous widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h   <th>gui_linkedlist_widgetgetprev(parent, h)
 *                      <tr><td>0       <td>0   <td>0
 *                      <tr><td>0       <td>> 0 <td>Previous widget of <b>h</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0   <td>Last widget in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0 <td>Previous widget of <b>h</b> if exists, last widget of <b>parent</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \code{c}
//Get all widgets of parent widget in reverse order from last to first
//Parent object must have GUI_HANDLE_ROOT_t data type
void LoopWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;         //Value holding widgets of parent
    
    for (h = gui_linkedlist_widgetgetprev((GUI_HANDLE_ROOT_t *)parent, 0); h; h = gui_linkedlist_widgetgetprev(NULL, h)) {
        //Loop through each widget of parent
    }
}
\endcode
 * \param[in]       *parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \retval          > 0: Next widget in linked list
 * \retval          0: No widget in linked list anymore
 * \sa              gui_linkedlist_widgetgetnext
 */
GUI_HANDLE_p gui_linkedlist_widgetgetprev(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);

/**
 * \brief           Move widget to bottom in linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget to move to bottom
 * \retval          1: Widget moved to bottom
 * \retval          0: Widget is already at bottom
 * \sa              gui_linkedlist_widgetmovetotop
 */
uint8_t gui_linkedlist_widgetmovetobottom(GUI_HANDLE_p h);

/**
 * \brief           Move widget to top in linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget to move to top
 * \retval          1: Widget moved to top
 * \retval          0: Widget is already on top
 * \sa              gui_linkedlist_widgetmovetobottom
 */
uint8_t gui_linkedlist_widgetmovetotop(GUI_HANDLE_p h);
 
#endif /* GUI_INTERNAL || defined(DOXYGEN) */

/**
 * \}
 */

/**
 * \brief           Prints widget structure from root tre
 * \param[in]       *root: Pointer to \ref GUI_HANDLE_ROOT_t as root object.
 *                     Set to NULL to use GUI root widget
 * \retval          None
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

#endif

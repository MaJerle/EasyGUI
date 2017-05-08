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
#define __GUI_LINKEDLIST_MULTI_GetData(element)     ((element) ? (element)->Element : 0)

/**
 * \brief           Add element to doubly linked list
 * \note            Element added to linkedlist must have \ref GUI_LinkedList_t as top structure
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to add to base linked list
 * \retval          None
 * \sa              __GUI_LINKEDLIST_REMOVE_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_ADD_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_REMOVE_GEN
 */
void __GUI_LINKEDLIST_ADD_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
    
/**
 * \brief           Remove element from doubly linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to remove from base linked list
 * \retval          Pointer to removed \ref GUI_LinkedList_t element
 * \sa              __GUI_LINKEDLIST_ADD_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_ADD_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_REMOVE_GEN
 */
GUI_LinkedList_t* __GUI_LINKEDLIST_REMOVE_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Get next element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>__GUI_LINKEDLIST_GETNEXT_GEN(root, element)
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
 * \sa              __GUI_LINKEDLIST_GETPREV_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_GETNEXT_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_GETPREV_GEN
 */
GUI_LinkedList_t* __GUI_LINKEDLIST_GETNEXT_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Get previous element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>__GUI_LINKEDLIST_GETNEXT_GEN(root, element)
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
 * \sa              __GUI_LINKEDLIST_GETNEXT_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_GETNEXT_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_GETPREV_GEN
 */
GUI_LinkedList_t* __GUI_LINKEDLIST_GETPREV_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Move widget down for one on doubly linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to move down on base linked list
 * \retval          1: Element moved down
 * \retval          0: Element was not moved down as it is already on bottom
 * \sa              __GUI_LINKEDLIST_MOVEUP_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_MOVEUP_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_MOVEDOWN_GEN
 */
uint8_t __GUI_LINKEDLIST_MOVEDOWN_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Move widget up for one on doubly linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to move up on base linked list
 * \retval          1: Element moved up
 * \retval          0: Element was not moved up as it is already on bottom
 * \sa              __GUI_LINKEDLIST_MOVEDOWN_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_MOVEUP_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_MOVEDOWN_GEN
 */
uint8_t __GUI_LINKEDLIST_MOVEUP_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/**
 * \brief           Add element to multi linked list
 * \note            Element can be any type since \ref GUI_LinkedListMulti_t structure is dynamicall allocated
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to add to base linked list
 * \retval          > 0: Pointer to allocated \ref GUI_LinkedListMulti_t structure with linked list entries
 * \retval          0: Failed to allocate memory for \ref GUI_LinkedListMulti_t structure
 * \sa              __GUI_LINKEDLIST_ADD_GEN
 * \sa              __GUI_LINKEDLIST_REMOVE_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_REMOVE_GEN
 */
GUI_LinkedListMulti_t* __GUI_LINKEDLIST_MULTI_ADD_GEN(GUI_LinkedListRoot_t* root, void* element);
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedList_t element to remove from base linked list
 * \retval          1: Element removed and memory deallocated
 * \retval          0: Failed to remove element
 * \sa              __GUI_LINKEDLIST_ADD_GEN
 * \sa              __GUI_LINKEDLIST_REMOVE_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_ADD_GEN
 */
uint8_t __GUI_LINKEDLIST_MULTI_REMOVE_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Get next element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>__GUI_LINKEDLIST_MULTI_GETNEXT_GEN(root, element)
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
 * \sa              __GUI_LINKEDLIST_GETNEXT_GEN
 * \sa              __GUI_LINKEDLIST_GETPREV_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_GETPREV_GEN
 */
GUI_LinkedListMulti_t* __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Get previous element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>__GUI_LINKEDLIST_MULTI_GETPREV_GEN(root, element)
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
 * \sa              __GUI_LINKEDLIST_GETNEXT_GEN
 * \sa              __GUI_LINKEDLIST_GETPREV_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_GETNEXT_GEN
 */
GUI_LinkedListMulti_t* __GUI_LINKEDLIST_MULTI_GETPREV_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Move widget down for one on multi linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedListMulti_t element to move down on base linked list
 * \retval          1: Element moved down
 * \retval          0: Element was not moved down as it is already on bottom
 * \sa              __GUI_LINKEDLIST_MOVEUP_GEN
 * \sa              __GUI_LINKEDLIST_MOVEDOWN_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_MOVEUP_GEN
 */
uint8_t __GUI_LINKEDLIST_MULTI_MOVEDOWN_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);

/**
 * \brief           Move widget up for one on multi linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Pointer to \ref GUI_LinkedListMulti_t element to move up on base linked list
 * \retval          1: Element moved up
 * \retval          0: Element was not moved up as it is already on bottom
 * \sa              __GUI_LINKEDLIST_MOVEUP_GEN
 * \sa              __GUI_LINKEDLIST_MOVEDOWN_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_MOVEDOWN_GEN
 */
uint8_t __GUI_LINKEDLIST_MULTI_MOVEUP_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element);
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            Delete by element pointer, not by list item. It searches in all entries for element pointers and deletes all links with that element pointer
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       *element: Memory address in data part of linked list entry
 * \retval          1: Element removed and memory deallocated
 * \retval          0: Failed to remove element
 * \sa              __GUI_LINKEDLIST_REMOVE_GEN
 * \sa              __GUI_LINKEDLIST_MULTI_REMOVE_GEN
 */
uint8_t __GUI_LINKEDLIST_MULTI_FIND_REMOVE(GUI_LinkedListRoot_t* root, void* element);
    
/**
 * \brief           Checks if linked list has any entries
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \retval          0: No entries
 * \retval          > 0: Has entries
 */
#define __GUI_LINKEDLIST_HasEntries(root)       ((root)->First)

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
 * \sa              __GUI_LINKEDLIST_IsWidgetLast
 * \hideinitializer
 */
#define __GUI_LINKEDLIST_IsWidgetFirst(h)       (!(h) || !(__GH(h))->List.Prev)

/**
 * \brief           Check if widget is last child element in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          1: Widget is last child
 * \retval          0: Widget is not last child
 * \sa              __GUI_LINKEDLIST_IsWidgetFirst
 * \hideinitializer
 */
#define __GUI_LINKEDLIST_IsWidgetLast(h)        (!(h) || !(__GH(h))->List.Next)

/**
 * \brief           Add new widget to linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *parent: Pointer to root handle object of parent object.
 *                      Set to NULL if there is no parent widget.
 * \param[in]       h: Widget handle to add to linked list
 * \retval          None
 * \sa              __GUI_LINKEDLIST_WidgetRemove
 */
void __GUI_LINKEDLIST_WidgetAdd(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);

/**
 * \brief           Remove widget from linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to remove from linked list
 * \retval          None
 * \sa              __GUI_LINKEDLIST_WidgetAdd
 */
void __GUI_LINKEDLIST_WidgetRemove(GUI_HANDLE_p h);

/**
 * \brief           Move widget up for one in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to move up
 * \retval          1: Move has been successful
 * \retval          0: Move was not successful. Widget is on top and cannot be moved
 * \sa              __GUI_LINKEDLIST_WidgetMoveDown
 */
GUI_Byte __GUI_LINKEDLIST_WidgetMoveUp(GUI_HANDLE_p h);

/**
 * \brief           Move widget down for one in linked list
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to move down
 * \retval          1: Move has been successful
 * \retval          0: Move was not successful. Widget is on bottom and cannot be moved
 * \sa              __GUI_LINKEDLIST_WidgetMoveUp
 */
GUI_Byte __GUI_LINKEDLIST_WidgetMoveDown(GUI_HANDLE_p h);

/**
 * \brief           Get next widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h   <th>__GUI_LINKEDLIST_WidgetGetNext(parent, h)
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

    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
        //Loop through each widget of parent
    }
}
\endcode
 * \param[in]       *parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \retval          > 0: Next widget in linked list
 * \retval          0: No widget in linked list anymore
 * \sa              __GUI_LINKEDLIST_WidgetGetPrev
 */
GUI_HANDLE_p __GUI_LINKEDLIST_WidgetGetNext(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);

/**
 * \brief           Get previous widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h   <th>__GUI_LINKEDLIST_WidgetGetPrev(parent, h)
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
    
    for (h = __GUI_LINKEDLIST_WidgetGetPrev((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetPrev(NULL, h)) {
        //Loop through each widget of parent
    }
}
\endcode
 * \param[in]       *parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \retval          > 0: Next widget in linked list
 * \retval          0: No widget in linked list anymore
 * \sa              __GUI_LINKEDLIST_WidgetGetNext
 */
GUI_HANDLE_p __GUI_LINKEDLIST_WidgetGetPrev(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h);

/**
 * \brief           Move widget to bottom in linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget to move to bottom
 * \retval          1: Widget moved to bottom
 * \retval          0: Widget is already at bottom
 * \sa              __GUI_LINKEDLIST_WidgetMoveToTop
 */
GUI_Byte __GUI_LINKEDLIST_WidgetMoveToBottom(GUI_HANDLE_p h);

/**
 * \brief           Move widget to top in linked list of parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget to move to top
 * \retval          1: Widget moved to top
 * \retval          0: Widget is already on top
 * \sa              __GUI_LINKEDLIST_WidgetMoveToBottom
 */
GUI_Byte __GUI_LINKEDLIST_WidgetMoveToTop(GUI_HANDLE_p h);
 
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
void __GUI_LINKEDLIST_PrintList(GUI_HANDLE_ROOT_t* root);

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

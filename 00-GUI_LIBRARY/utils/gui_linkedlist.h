/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \brief   GUI window object manager
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

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
 * \addtogroup  GUI
 * \brief       
 * \{
 */
#include "gui.h"

/**
 * @defgroup      GUI_LINKEDLIST_Macros
 * @brief         Library defines
 * @{
 */
    
/**
 * \}
 */
 
/**
 * \defgroup      GUI_LINKEDLIST_Typedefs
 * \brief         Library Typedefs
 * \{
 */

/**
 * @}
 */

/**
 * \defgroup      GUI_LINKEDLIST_Functions
 * \brief         Library Functions
 * \{
 */

/* Generic linked list elements */
void __GUI_LINKEDLIST_ADD_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
void __GUI_LINKEDLIST_REMOVE_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
uint8_t __GUI_LINKEDLIST_MOVEDOWN_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);
uint8_t __GUI_LINKEDLIST_MOVEUP_GEN(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element);

/* Linked list functions designed for work with handles */
void __GUI_LINKEDLIST_ADD(GUI_HANDLE_ROOT_t* parent, void* ptr);
void __GUI_LINKEDLIST_REMOVE(void* ptr);
GUI_Byte __GUI_LINKEDLIST_MOVEUP(GUI_HANDLE_t h);
GUI_Byte __GUI_LINKEDLIST_MOVEDOWN(GUI_HANDLE_t h);

GUI_HANDLE_t __GUI_LINKEDLIST_GetNextWidget(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_t h);
GUI_HANDLE_t __GUI_LINKEDLIST_GetPrevWidget(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_t h);

GUI_Byte __GUI_LINKEDLIST_MoveDown_Widget(GUI_HANDLE_t h);
GUI_Byte __GUI_LINKEDLIST_MoveUp_Widget(GUI_HANDLE_t h);

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

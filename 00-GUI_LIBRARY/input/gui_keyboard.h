/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI virtual keyboard
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
#ifndef GUI_KEYBOARD_H
#define GUI_KEYBOARD_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_INPUT
 * \{
 */
#include "gui_input.h"
#include "gui_widget.h"

/**
 * \defgroup        GUI_KEYBOARD Virtual keyboard
 * \brief           Virtual keyboard on screen
 * \{
 */
    
/**
 * \defgroup        GUI_KEYBOARD_Private Private functions
 * \brief           Private keyboard functions
 *
 * \note            Since they are not thread safe, they can only be used when inside GUI library.
 *                  It means they can be used in custom widget implementations where thread safety is guaranteed
 *
 * \{
 */
 
/**
 * \brief           Hide opened virtual keyboard
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \retval          1: Keyboard hiding process started
 * \retval          0: Keyboard hiding process not started
 * \sa              __GUI_KEYBOARD_Show
 */
uint8_t __GUI_KEYBOARD_Hide(void);

/**
 * \brief           Show hidden virtual keyboard
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \retval          1: Keyboard opening process started
 * \retval          0: Keyboard opening process not started
 * \sa              __GUI_KEYBOARD_Hide
 */
uint8_t __GUI_KEYBOARD_Show(void);
    
/**
 * \}
 */
    
/**
 * \brief           Creates virtual keyboard for user interaction.
 * \note            This function must be called by user after \ref GUI_Init is called to use virtual keyboard
 * \retval          Keyboard handle
 */
GUI_HANDLE_p GUI_KEYBOARD_Create(void);

/**
 * \brief           Hide opened virtual keyboard
 * \retval          1: Keyboard hiding process started
 * \retval          0: Keyboard hiding process not started
 * \sa              GUI_KEYBOARD_Show
 */
uint8_t GUI_KEYBOARD_Hide(void);

/**
 * \brief           Show hidden virtual keyboard
 * \retval          1: Keyboard opening process started
 * \retval          0: Keyboard opening process not started
 * \sa              GUI_KEYBOARD_Hide
 */
uint8_t GUI_KEYBOARD_Show(void);
    
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

/**
 * \author  Tilen Majerle
 * \brief   GUI INPUT
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
#ifndef GUI_INPUT_H
#define GUI_INPUT_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_INPUT
 * \brief       
 * \{
 */
#include "../gui.h"

/**
 * \brief           Adds new touch data to internal buffer for further processing
 * \param[in]       *Data: Pointer to \ref GUI_TouchData_t touch data
 * \retval          1: Success
 * \retval          0: Failure
 */
uint8_t GUI_INPUT_AddTouch(const GUI_TouchData_t* Data);

/**
 * \brief           Adds new key data to internal buffer for further processing
 * \param[in]       *Data: Pointer to \ref GUI_KeyboardData_t key data
 * \retval          1: Success
 * \retval          0: Failure
 * \sa              GUI_Keys
 */
uint8_t GUI_INPUT_AddKey(const GUI_KeyboardData_t* Data);

#if !defined(DOXYGEN)
/**
 * \brief           Initializes input functionality
 * \param           None
 * \retval          None
 */
void __GUI_INPUT_Init(void);

/**
 * \brief           Initializes input functionality
 * \param[out]      *Data: Pointer to \ref GUI_TouchData_t to save touch data
 * \retval          Number of bytes read from internal buffer
 */
GUI_Byte_t __GUI_INPUT_ReadTouch(GUI_TouchData_t* Data);

/**
 * \brief           Initializes input functionality
 * \param[out]      *Data: Pointer to \ref GUI_TouchData_t to save keyboard data
 * \retval          Number of bytes read from internal buffer
 */
GUI_Byte_t __GUI_INPUT_ReadKey(GUI_KeyboardData_t* Data);
#endif

/**
 * \} GUI_INPUT
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

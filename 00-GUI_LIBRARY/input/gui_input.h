/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 *
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
#ifndef GUI_INPUT_H
#define GUI_INPUT_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  GUI
 * \brief       
 * \{
 */
#include "..\gui.h"

/**
 * @defgroup      GUI_INPUT_Macros
 * @brief         Library defines
 * @{
 */

/**
 * \}
 */
 
/**
 * \defgroup      GUI_INPUT_Typedefs
 * \brief         Library Typedefs
 * \{
 */

/**
 * @}
 */

/**
 * \defgroup      GUI_INPUT_Functions
 * \brief         Library Functions
 * \{
 */

void GUI_INPUT_AddTouch(const GUI_TouchData_t* Data);

//Internal purpose only
void __GUI_INPUT_Init(void);
GUI_Byte_t __GUI_INPUT_ReadTouch(GUI_TouchData_t* Data);

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

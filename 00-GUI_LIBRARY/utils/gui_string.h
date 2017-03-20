/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \brief   GUI string functions
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
#ifndef GUI_STRING_H
#define GUI_STRING_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_UTILS
 * \brief       
 * \{
 */
#include "../gui.h"

/**
 * \defgroup        GUI_STRING
 * \{
 */

#if GUI_USE_UNICODE
size_t __GUI_STRING_Length(const GUI_Char* src);
GUI_Char* __GUI_STRING_Copy(GUI_Char* dst, const GUI_Char* src);
GUI_Char* __GUI_STRING_CopyN(GUI_Char* dst, const GUI_Char* src, size_t len);
GUI_Char* __GUI_STRING_Compare(const GUI_Char* s1, const GUI_Char* s2);
#else
#define __GUI_STRING_Length         strlen
#define __GUI_STRING_Copy           strcpy
#define __GUI_STRING_CopyN          strncpy
#define __GUI_STRING_Compare        strcmp
#endif /* GUI_USE_UNICODE */

/* Total string length of bytes in string, no matter of unicode */
#define __GUI_STRING_LengthTotal    strlen

uint8_t __GUI_STRING_GetCh(const GUI_Char** str, uint32_t* out, uint8_t* len);
    
/**
 * \} GUI_STRING
 */

/**
 * \} GUI_UTILS
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

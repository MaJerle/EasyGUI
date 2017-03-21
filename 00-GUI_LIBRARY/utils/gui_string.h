/**
 * \author  Tilen Majerle
 * \brief   GUI string functions
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
 * \brief           String functions with UNICODE support
 * \{
 */

/**
 * \defgroup        GUI_STRING_UNICODE
 * \brief           Unicode processing with UTF-8 character encoding
 *
 * Core functions capable of encoding and decoding UTF-8 UNICODE format.
 *
 * https://en.wikipedia.org/wiki/UTF-8
 * \{
 */
    
/**
 * \brief           UNICODE processing information structure
 */
typedef struct GUI_STRING_UNICODE_t {
    uint8_t t;                                      /*!< Total number of bytes in UTF-8 sequence */
    uint8_t r;                                      /*!< Remaining bytes in UTF-8 sequnce before we get valid data */
    uint32_t res;                                   /*!< Current result in UTF8 sequence */
} GUI_STRING_UNICODE_t;

/**
 * \brief           UNICODE processing result information
 */
typedef enum GUI_STRING_UNICODE_Result_t {
    UNICODE_OK = 0x00,                              /*!< Indicates function successfully decode unicode sequence of 1-4 bytes */
    UNICODE_ERROR,                                  /*!< Indicates function got an error with input data from 2nd to 4th byte in UTF-8 encoding */
    UNICODE_PROGRESS,                               /*!< Indicates function decoding is still in progress and it waits for new character */
} GUI_STRING_UNICODE_Result_t;

/**
 * \brief           Initialize unicode processing structure
 * \param[in]       *s: Pointer to \ref GUI_STRING_UNICODE_t to initialize to default values
 * \retval          None
 * \sa              GUI_STRING_UNICODE_Decode()
 * \sa              GUI_STRING_UNICODE_Encode()
 */
void GUI_STRING_UNICODE_Init(GUI_STRING_UNICODE_t* s);

/**
 * \brief           Decodes single input byte of unicode formatted text
 * \param[in,out]   *s: Pointer to working \ref GUI_STRING_UNICODE_t structure for processing
 * \param[in]       c: Character to be used for encoding
 * \retval          Member of \ref GUI_STRING_UNICODE_Result_t indicating decoding status
 * \sa              GUI_STRING_UNICODE_Encode()
 */
GUI_STRING_UNICODE_Result_t GUI_STRING_UNICODE_Decode(GUI_STRING_UNICODE_t* s, const GUI_Char c);

/**
 * \brief           Encodes input character to UNICODE sequence of 1-4 bytes
 * \param[in]       c: Character to encode to UNICODE sequence
 * \param[out]      *out: Pointer to 4-bytes long array to store UNICODE information to
 * \retval          Number of bytes required for character encoding
 * \sa              GUI_STRING_UNICODE_Decode()
 */
uint8_t GUI_STRING_UNICODE_Encode(const uint32_t c, GUI_Char* out);

/**
 * \} GUI_STRING_UNICODE
 */
    
size_t GUI_STRING_Length(const GUI_Char* src);
size_t GUI_STRING_LengthTotal(const GUI_Char* src);
GUI_Char* GUI_STRING_Copy(GUI_Char* dst, const GUI_Char* src);
GUI_Char* GUI_STRING_CopyN(GUI_Char* dst, const GUI_Char* src, size_t len);
int GUI_STRING_Compare(const GUI_Char* s1, const GUI_Char* s2);
uint8_t GUI_STRING_IsPrintable(uint32_t ch);

uint8_t GUI_STRING_GetCh(const GUI_Char** str, uint32_t* out, uint8_t* len);
uint8_t GUI_STRING_GetChReverse(const GUI_Char** str, uint32_t* out, uint8_t* len);
    
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

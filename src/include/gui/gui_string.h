/**	
 * \file            gui_string.h
 * \brief           String manager
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
#ifndef __GUI_STRING_H
#define __GUI_STRING_H

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
 * \defgroup        GUI_STRING String functions
 * \brief           String functions with UNICODE support
 * \{
 */
    
/**
 * \defgroup        GUI_STRING_UNICODE Unicode processing
 * \brief           Unicode processing functions with UTF-8 character encoding
 *
 * Core functions capable of encoding and decoding UTF-8 UNICODE format.
 *
 * Decoding of raw unicode bytes can take up to 4 bytes in a row and stores value to 32-bit variable
 *
 * \par UNICODE structure table
 *
 * <table>
 *  <tr><th>Number of bytes <th>Bits for code point <th>First code points   <th>Last code point <th>Byte 1      <th>Byte 2      <th>Byte 3      <th>Byte 4
 *  <tr><td>1               <td>7                   <td>U+0000              <td>U+007F          <td>0xxxxxxx    <td>-           <td>-           <td>-
 *  <tr><td>2               <td>11                  <td>U+0080              <td>U+07FF          <td>110xxxxx    <td>10xxxxxx    <td>-           <td>-
 *  <tr><td>3               <td>16                  <td>U+0800              <td>U+FFFF          <td>1110xxxx    <td>10xxxxxx    <td>10xxxxxx    <td>-
 *  <tr><td>4               <td>21                  <td>U+10000             <td>U+10FFFF        <td>11110xxx    <td>10xxxxxx    <td>10xxxxxx    <td>10xxxxxx
 * </table>
 *
 * https://en.wikipedia.org/wiki/UTF-8
 *
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

void gui_string_unicode_init(GUI_STRING_UNICODE_t* s);
GUI_STRING_UNICODE_Result_t gui_string_unicode_decode(GUI_STRING_UNICODE_t* s, const GUI_Char c);
uint8_t gui_string_unicode_encode(const uint32_t c, GUI_Char* out);

/**
 * \}
 */
 
/**
 * \brief           String structure for parsing characters
 */
typedef struct GUI_STRING_t {
    const GUI_Char* Str;                /*!< Pointer to source string */
#if GUI_CFG_USE_UNICODE || defined(DOXYGEN)
    GUI_STRING_UNICODE_t S;             /*!< Unicode processing structure */
#endif /* GUI_CFG_USE_UNICODE || defined(DOXYGEN) */
} GUI_STRING_t;

size_t gui_string_length(const GUI_Char* src);
size_t gui_string_lengthtotal(const GUI_Char* src);
GUI_Char* gui_string_copy(GUI_Char* dst, const GUI_Char* src);
GUI_Char* gui_string_copyn(GUI_Char* dst, const GUI_Char* src, size_t len);
int gui_string_compare(const GUI_Char* s1, const GUI_Char* s2);
uint8_t gui_string_isprintable(uint32_t ch);
uint8_t gui_string_prepare(GUI_STRING_t* s, const GUI_Char* str);
uint8_t gui_string_getch(GUI_STRING_t* str, uint32_t* out, uint8_t* len);
uint8_t gui_string_getchreverse(GUI_STRING_t* str, uint32_t* out, uint8_t* len);
uint8_t gui_string_gotoend(GUI_STRING_t* str);
    
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

#endif /* __GUI_STRING_H */

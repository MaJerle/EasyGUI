/**	
 * \file            gui_string.h
 * \brief           String manager
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
#ifndef GUI_HDR_STRING_H
#define GUI_HDR_STRING_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_utils.h"

/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_STRING String functions
 * \brief           String functions with UNICODE support
 * \{
 */
    
/**
 * \defgroup        GUI_STRING_UNICODE Unicode processing
 * \brief           Unicode processing functions with UTF-8 character encoding
 * \{
 */
    
/**
 * \brief           UNICODE processing information structure
 */
typedef struct {
    uint8_t t;                                      /*!< Total number of bytes in UTF-8 sequence */
    uint8_t r;                                      /*!< Remaining bytes in UTF-8 sequnce before we get valid data */
    uint32_t res;                                   /*!< Current result in UTF8 sequence */
} gui_string_unicode_t;

/**
 * \brief           UNICODE processing result information
 */
typedef enum {
    UNICODE_OK = 0x00,                              /*!< Indicates function successfully decode unicode sequence of 1-4 bytes */
    UNICODE_ERROR,                                  /*!< Indicates function got an error with input data from 2nd to 4th byte in UTF-8 encoding */
    UNICODE_PROGRESS,                               /*!< Indicates function decoding is still in progress and it waits for new character */
} GUI_STRING_UNICODE_Result_t;

void gui_string_unicode_init(gui_string_unicode_t* const s);
GUI_STRING_UNICODE_Result_t gui_string_unicode_decode(gui_string_unicode_t* const s, const gui_char c);
uint8_t gui_string_unicode_encode(const uint32_t c, gui_char* out);

/**
 * \}
 */
 
/**
 * \brief           String structure for parsing characters
 */
typedef struct {
    const gui_char* str;                /*!< Pointer to source string */
#if GUI_CFG_USE_UNICODE || __DOXYGEN__
    gui_string_unicode_t s;             /*!< Unicode processing structure */
#endif /* GUI_CFG_USE_UNICODE || __DOXYGEN__ */
} gui_string_t;

size_t gui_string_length(const gui_char* const src);
size_t gui_string_lengthtotal(const gui_char* const src);
gui_char* gui_string_copy(gui_char* const dst, const gui_char* const src);
gui_char* gui_string_copyn(gui_char* const dst, const gui_char* const src, size_t len);
int gui_string_compare(const gui_char* const s1, const gui_char* const s2);
uint8_t gui_string_isprintable(uint32_t ch);
uint8_t gui_string_prepare(gui_string_t* const s, const gui_char* const str);
uint8_t gui_string_getch(gui_string_t* const str, uint32_t* const out, uint8_t* const len);
uint8_t gui_string_getchreverse(gui_string_t* const str, uint32_t* const out, uint8_t* const len);
uint8_t gui_string_gotoend(gui_string_t* const str);
    
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_STRING_H */

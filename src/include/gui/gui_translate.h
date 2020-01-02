/**	
 * \file            gui_translate.h
 * \brief           Translation manager
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
#ifndef GUI_HDR_TRANSLATE_H
#define GUI_HDR_TRANSLATE_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_utils.h"

/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_TRANSLATE Translation engine
 * \brief           String translation engine for widgets
 * \{
 */
 
const gui_char* gui_translate_get(const gui_char* const src);
uint8_t         gui_translate_setactivelanguage(const gui_translate_language_t* const lang);
uint8_t         gui_translate_setsourcelanguage(const gui_translate_language_t* const lang); 
    
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_TRANSLATE_H */

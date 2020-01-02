/**	
 * \file            gui_text.h
 * \brief           Text manager
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
#ifndef GUI_HDR_TEXT_H
#define GUI_HDR_TEXT_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_utils.h"

/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_TEXT Text functions
 * \brief           Text functions
 * \{
 */

const gui_font_char_t *     gui_text_getchardesc(const gui_font_t* font, uint32_t ch);
void                        gui_text_getcharsize(const gui_font_t* font, uint32_t ch, gui_dim_t* width, gui_dim_t* height);
gui_font_charentry_t *      gui_text_getcharentry(const gui_font_t* font, const gui_font_char_t* c);
gui_font_charentry_t *      gui_text_createcharentry(const gui_font_t* font, const gui_font_char_t* c);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_TEXT_H */

/**	
 * \file            gui_translate.c
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
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui_translate.h"

#if GUI_CFG_USE_TRANSLATE || __DOXYGEN__

/**
 * \brief           Get translated entry from input string
 * \param[in]       src: String to translate
 * \return          Pointer to translated string or source string if translate not found
 */
const gui_char*
gui_translate_get(const gui_char* const src) {
    size_t i;
    
    /* Try to find source string in translate table */
    if (GUI.translate.source == NULL || GUI.translate.active == NULL) { /* Check if languages set correctly */
        return src;                                 /* Just return original string */
    }
    
    /* Scan all entries and return appropriate string */
    for (i = 0; i < GUI.translate.source->count; i++) {
        if (gui_string_compare(src, GUI.translate.source->entries[i]) == 0) {
            if (i < GUI.translate.active->count) {  /* Check if in valid range */
                return GUI.translate.active->entries[i];    /* Return translated string */
            }
            break;                                  /* Stop execution at this point */
        }
    }
    return src;                                     /* Return main source */
}

/**
 * \brief           Set currently active language for translated entries
 * \note            These entries are returned when index matches the source string from source language
 * \param[in]       lang: Language with translation entries
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_translate_setactivelanguage(const gui_translate_language_t* const lang) {
    GUI.translate.active = lang;                    /* Set currently active language */
    return 1;
}

/**
 * \brief           Set source language for translated entries
 * \note            These entries are compared with input string to get index for translated value
 * \param[in]       lang: Language with translation entries
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_translate_setsourcelanguage(const gui_translate_language_t* const lang) {
    GUI.translate.source = lang;                    /* Set source language */
    return 1;
}

#endif /* GUI_CFG_USE_TRANSLATE || __DOXYGEN__ */

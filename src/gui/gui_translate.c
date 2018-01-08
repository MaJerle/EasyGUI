/**	
 * \file            gui_translate.c
 * \brief           Translation manager
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
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui_translate.h"

#if GUI_CFG_USE_TRANSLATE || __DOXYGEN__

/**
 * \brief           Get translated entry from input string
 * \param[in]       src: Pointer to \ref GUI_Char string to translate
 * \return          Pointer to translated string or source string if translate not found
 */
const GUI_Char*
gui_translate_get(const GUI_Char* src) {
    size_t i;
    /* Try to find source string in translate table */
    if (!GUI.Translate.Source || !GUI.Translate.Active) {   /* Check if languages set correctly */
        return src;                                 /* Just return original string */
    }
    
    /**
     * Scan all entries and return appropriate string
     */
    for (i = 0; i < GUI.Translate.Source->Count; i++) {
        if (gui_string_compare(src, GUI.Translate.Source->Entries[i]) == 0) {
            if (i < GUI.Translate.Active->Count) {  /* Check if in valid range */
                return GUI.Translate.Active->Entries[i];    /* Return translated string */
            }
            break;                                  /* Stop execution at this point */
        }
    }
    return src;                                     /* Return main source */
}

/**
 * \brief           Set currently active language for translated entries
 * \note            These entries are returned when index matches the source string from source language
 * \param[in]       lang: Pointer to \ref GUI_TRANSLATE_Language_t structure with translation entries
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_translate_setactivelanguage(const GUI_TRANSLATE_Language_t* lang) {
    GUI.Translate.Active = lang;                    /* Set currently active language */
    return 1;
}

/**
 * \brief           Set source language for translated entries
 * \note            These entries are compared with input string to get index for translated value
 * \param[in]       lang: Pointer to \ref GUI_TRANSLATE_Language_t structure with translation entries
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_translate_setsourcelanguage(const GUI_TRANSLATE_Language_t* lang) {
    GUI.Translate.Source = lang;                    /* Set source language */
    return 1;
}

#endif /* GUI_CFG_USE_TRANSLATE || __DOXYGEN__ */

/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   Translate engine for widget text
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
#ifndef GUI_TRANSLATE_H
#define GUI_TRANSLATE_H

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
 * \defgroup        GUI_TRANSLATE Translation engine
 * \brief           String translation engine for widgets
 * \{
 *
 * Translation works between source and active languages
 *
 * Both languages must contain the same array for entries in different languages.
 * When translate is performed, index from source table (if found) is used to return string from active table.
 *
 * \note            When translation language is changed, all widget must be manually redrawn to get effect
 *
 * \code{c}
//Create table for English entries
const GUI_Char* languageEnglishEntries[] = {
    _GT("Button"),
    _GT("Dropdown"),
    _GT("Listview"),
    _GT("Your string here without translate."),
};

//Create table for German entries
const GUI_Char* languageGermanEntries[] = {
    _GT("Taste"),           //Translate for "Button"
    _GT("Dropdown-Liste"),  //Translate for "Dropdown"
    _GT("Listenansicht"),   //Translate for "Listview"
    //Missing translate for "Your string here without translate."
};

//Create English language table
GUI_TRANSLATE_Language_t languageEnglish = {
    .Entries = languageEnglishEntries,
    .Count = GUI_COUNT_OF(languageEnglishEntries)
};

//Create German language structure
GUI_TRANSLATE_Language_t languageGerman = {
    .Entries = languageGermanEntries,
    .Count = GUI_COUNT_OF(languageGermanEntries)
};

//Set source and active languages
gui_translate_setsourcelanguage(&languageEnglish); //Translate from this language
gui_translate_setactivelanguage(&languageGerman);  //Translate to this language

//Now try to translate
printf("Button: %s\n", gui_translate_get(_GT("Button"))); //Prints: "Button: Taste\n"
printf("Dropdown: %s\n", gui_translate_get(_GT("Dropdown"))); //Prints: "Button: Dropdown-Liste\n"
printf("Listview: %s\n", gui_translate_get(_GT("Listview"))); //Prints: "Button: Listenansicht\n"
printf("Your string here without translate.: %s\n", gui_translate_get(_GT("Your string here without translate."))); //Prints: "Your string here without translate.: Your string here without translate.\n"
\endcode
 */

/**
 * \brief           Get translated entry from input string
 * \param[in]       *src: Pointer to \ref GUI_Char string to translate
 * \retval          Pointer to translated string or source string if translate not found
 */
const GUI_Char* gui_translate_get(const GUI_Char* src);

/**
 * \brief           Set currently active language for translated entries
 * \note            These entries are returned when index matches the source string from source language
 * \param[in]       *lang: Pointer to \ref GUI_TRANSLATE_Language_t structure with translation entries
 * \retval          1: Entries set ok
 * \retval          0: Entries were not set
 */
uint8_t gui_translate_setactivelanguage(const GUI_TRANSLATE_Language_t* lang);

/**
 * \brief           Set source language for translated entries
 * \note            These entries are compared with input string to get index for translated value
 * \param[in]       *lang: Pointer to \ref GUI_TRANSLATE_Language_t structure with translation entries
 * \retval          1: Entries set ok
 * \retval          0: Entries were not set
 */
uint8_t gui_translate_setsourcelanguage(const GUI_TRANSLATE_Language_t* lang); 
    
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

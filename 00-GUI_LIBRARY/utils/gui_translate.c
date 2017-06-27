/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_translate.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
const GUI_Char* GUI_TRANSLATE_Get(const GUI_Char* src) {
#if GUI_USE_TRANSLATE
    size_t i;
    /* Try to find source string in translate table */
    if (!GUI.Translate.Source || !GUI.Translate.Active) {   /* Check if languages set correctly */
        return src;                                 /* Just return original string */
    }
    
    /**
     * Scan all entries and return appropriate string
     */
    for (i = 0; i < GUI.Translate.Source->Count; i++) {
        if (GUI_STRING_Compare(src, GUI.Translate.Source->Entries[i]) == 0) {
            if (i < GUI.Translate.Active->Count) {  /* Check if in valid range */
                return GUI.Translate.Active->Entries[i];    /* Return translated string */
            }
            break;                                  /* Stop execution at this point */
        }
    }
#endif /* GUI_USE_TRANSLATE */
    return src;                                     /* Return main source */
}

uint8_t GUI_TRANSLATE_SetActiveLanguage(const GUI_TRANSLATE_Language_t* lang) {
#if GUI_USE_TRANSLATE
    GUI.Translate.Active = lang;                    /* Set currently active language */
    return 1;
#else
    __GUI_UNUSED(lang);
    return 0;
#endif /* GUI_USE_TRANSLATE */
}

uint8_t GUI_TRANSLATE_SetSourceLanguage(const GUI_TRANSLATE_Language_t* lang) {
#if GUI_USE_TRANSLATE
    GUI.Translate.Source = lang;                    /* Set source language */
    return 1;
#else
    __GUI_UNUSED(lang);
    return 0;
#endif /* GUI_USE_TRANSLATE */
}

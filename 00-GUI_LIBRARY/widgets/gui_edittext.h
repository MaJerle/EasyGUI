/**
 * \author  Tilen Majerle
 * \brief   GUI edit text widget
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
#ifndef GUI_EDITTEXT_H
#define GUI_EDITTEXT_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \brief       
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_EDITTEXT
 * \{
 */

/**
 * \brief           Edit text color list enumeration
 */
typedef enum GUI_EDITTEXT_COLOR_t {
    GUI_EDITTEXT_COLOR_BG = 0x00,
    GUI_EDITTEXT_COLOR_BORDER = 0x01,
    GUI_EDITTEXT_COLOR_TEXT = 0x02
} GUI_EDITTEXT_COLOR_t;
    
/**
 * \brief           GUI EDIT TEXT widget structure
 */
typedef struct GUI_EDITTEXT_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    GUI_Color_t Color[3];                   /*!< List of colors */
} GUI_EDITTEXT_t;


GUI_HANDLE_t GUI_EDITTEXT_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height);
void GUI_EDITTEXT_Remove(GUI_HANDLE_t* h);
uint32_t GUI_EDITTEXT_AllocTextMemory(GUI_HANDLE_t h, uint16_t size);
GUI_HANDLE_t GUI_EDITTEXT_FreeTextMemory(GUI_HANDLE_t h);
GUI_HANDLE_t GUI_EDITTEXT_SetText(GUI_HANDLE_t h, const GUI_Char* text);
GUI_HANDLE_t GUI_EDITTEXT_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height);
GUI_HANDLE_t GUI_EDITTEXT_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y);
GUI_HANDLE_t GUI_EDITTEXT_SetColor(GUI_HANDLE_t h, GUI_EDITTEXT_COLOR_t index, GUI_Color_t color);
GUI_HANDLE_t GUI_EDITTEXT_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font);

/**
 * \} GUI_EDITTEXT
 */

/**
 * \} GUI_WIDGETS
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

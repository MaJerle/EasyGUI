/**
 * \author  Tilen Majerle
 * \brief   GUI progress bar widget
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
#ifndef GUI_PROGBAR_H
#define GUI_PROGBAR_H

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
 * \defgroup        GUI_PROGBAR
 * \{
 */

#define GUI_PROGBAR_GetMin(h)           (((GUI_PROGBAR_t *)h)->Min)
#define GUI_PROGBAR_GetMax(h)           (((GUI_PROGBAR_t *)h)->Max)
#define GUI_PROGBAR_GetValue(h)         (((GUI_PROGBAR_t *)h)->Value)

/**
 * \brief           Progress bar color list enumeration
 */
typedef enum GUI_PROGBAR_COLOR_t {
    GUI_PROGBAR_COLOR_BG = 0x00,            /*!< Background color index */
    GUI_PROGBAR_COLOR_FG = 0x01,            /*!< Foreground (active part) color index */
    GUI_PROGBAR_COLOR_BORDER = 0x02         /*!< Border color */
} GUI_PROGBAR_COLOR_t;

/**
 * \brief           Progress bar widget structure
 */
typedef struct GUI_PROGBAR_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    GUI_Color_t Color[3];                   /*!< List of colors for widget */
    
    int32_t Min;                            /*!< Low value for progress bar */
    int32_t Max;                            /*!< High value for progress bar */
    int32_t Value;                          /*!< Current value for progress bar */
    uint8_t Flags;                          /*!< Flags variable */
} GUI_PROGBAR_t;

GUI_HANDLE_t GUI_PROGBAR_Create(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height);
void GUI_PROGBAR_Remove(GUI_HANDLE_t* h);
GUI_HANDLE_t GUI_PROGBAR_SetColor(GUI_HANDLE_t h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color);
GUI_HANDLE_t GUI_PROGBAR_SetText(GUI_HANDLE_t h, GUI_Const GUI_Char* text);
GUI_HANDLE_t GUI_PROGBAR_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font);
GUI_HANDLE_t GUI_PROGBAR_SetMin(GUI_HANDLE_t h, int32_t val);
GUI_HANDLE_t GUI_PROGBAR_SetMax(GUI_HANDLE_t h, int32_t val);
GUI_HANDLE_t GUI_PROGBAR_SetValue(GUI_HANDLE_t h, int32_t val);
GUI_HANDLE_t GUI_PROGBAR_EnablePercentages(GUI_HANDLE_t h);
GUI_HANDLE_t GUI_PROGBAR_DisablePercentages(GUI_HANDLE_t h);
GUI_HANDLE_t GUI_PROGBAR_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \} GUI_PROGBAR
 */

/**
 * \} GUI_WIDGETS
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

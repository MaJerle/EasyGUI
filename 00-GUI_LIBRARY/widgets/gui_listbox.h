/**
 * \author  Tilen Majerle
 * \brief   GUI LISTBOX widget
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

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
#ifndef GUI_LISTBOX_H
#define GUI_LISTBOX_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_LISTBOX LISTBOX
 * \brief           LISTBOX widget
 * \{
 */
    
#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_LISTBOX_DYNAMIC        0x01/*!< Pointers array allocated dynamically */
#define GUI_FLAG_LISTBOX_SLIDER_ON      0x02/*!< Slider is currently active */
#define GUI_FLAG_LISTBOX_SLIDER_AUTO    0x04/*!< Show right slider automatically when required */
#define GUI_FLAG_LISTBOX_SLIDER_MANUAL  0x08/*!< Show right slider always */
    
/**
 * \brief           LISTBOX object structure
 */
typedef struct GUI_LISTBOX_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int16_t MaxCount;                       /*!< Maximal number of allowed strings in listbox widget */
    int16_t Count;                          /*!< Current number of strings attached to this widget */
    int16_t Selected;                       /*!< Selected text index */
    int16_t StartIndex;                     /*!< Index in array of string on top of visible area of widget */
    GUI_Char** Pointers;                    /*!< Pointer to list of pointers of strings */
    GUI_Dim_t SliderWidth;                  /*!< Slider with in units of pixels */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_LISTBOX_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new LISTBOX widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p GUI_LISTBOX_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags);
GUI_HANDLE_p GUI_LISTBOX_AllocatePointers(GUI_HANDLE_p h, uint16_t count);
GUI_HANDLE_p GUI_LISTBOX_FreePointers(GUI_HANDLE_p h);
GUI_HANDLE_p GUI_LISTBOX_SetPointers(GUI_HANDLE_p h, GUI_Char** ptrs, uint16_t count);
GUI_HANDLE_p GUI_LISTBOX_SetString(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text);
GUI_HANDLE_p GUI_LISTBOX_AddString(GUI_HANDLE_p h, const GUI_Char* text);

uint8_t GUI_LISTBOX_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

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

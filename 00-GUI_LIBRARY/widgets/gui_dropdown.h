/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI dropdown widget
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
#ifndef GUI_DROPDOWN_H
#define GUI_DROPDOWN_H

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
 * \defgroup        GUI_DROPDOWN Dropdown
 * \brief           Dropdown widget
 * \{
 */

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_DROPDOWN_OPENED        0x01/*!< Dropdown is opened */
#define GUI_FLAG_DROPDOWN_SLIDER_ON     0x02/*!< Slider is currently active */
#define GUI_FLAG_DROPDOWN_SLIDER_AUTO   0x04/*!< Show right slider automatically when required, otherwise, manual mode is used */
#define GUI_FLAG_DROPDOWN_OPEN_UP       0x08/*!< Open dropdown in top direction instead of bottom */
    
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */
    
/**
 * \brief           List of dropdown colors
 */
typedef enum GUI_DROPDOWN_COLOR_t {
    GUI_DROPDOWN_COLOR_BG = 0x00,           /*!< Background color index */
    GUI_DROPDOWN_COLOR_TEXT,                /*!< Text color index */
    GUI_DROPDOWN_COLOR_SEL_FOC,             /*!< Text color index of selected item when widget is in focus */
    GUI_DROPDOWN_COLOR_SEL_NOFOC,           /*!< Text color index of selected item when widget is not in focus */
    GUI_DROPDOWN_COLOR_SEL_FOC_BG,          /*!< Background color index of selected item when widget is in focus */
    GUI_DROPDOWN_COLOR_SEL_NOFOC_BG,        /*!< Background color index of selected item when widget is not in focus */
} GUI_DROPDOWN_COLOR_t;

/**
 * \brief           Dropdown open direction
 */
typedef enum GUI_DROPDOWN_OPENDIR_t {
    GUI_DROPDOWN_OPENDIR_DOWN = 0x00,       /*!< Dropdown will open down according to current position */
    GUI_DROPDOWN_OPENDIR_UP = 0x01          /*!< Dropdown will open up according to current position */
} GUI_DROPDOWN_OPENDIR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)

/**
 * \brief           Dropdown string item object
 */
typedef struct GUI_DROPDOWN_ITEM_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_Char* Text;                         /*!< Text entry */
} GUI_DROPDOWN_ITEM_t;
    
/**
 * \brief           Dropdown object structure
 */
typedef struct GUI_DROPDOWN_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    GUI_iDim_t OldY;                        /*!< Y coordinate register before widget opened */
    GUI_iDim_t OldHeight;                   /*!< Height value register before widget opened */
    
    int16_t Count;                          /*!< Current number of strings attached to this widget */
    int16_t Selected;                       /*!< Selected text index */
    int16_t VisibleStartIndex;              /*!< Index in array of string on top of visible area of widget */
    
    GUI_LinkedListRoot_t Root;              /*!< Root of linked list entries */
    
    GUI_Dim_t SliderWidth;                  /*!< Slider width in units of pixels */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_DROPDOWN_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new dropdown widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p GUI_DROPDOWN_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to DROPDOWN
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors. This parameter can be a value of \ref GUI_DROPDOWN_COLOR_t enumeration
 * \param[in]       color: Actual color code to set
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t GUI_DROPDOWN_SetColor(GUI_HANDLE_p h, GUI_DROPDOWN_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Add a new string to list box
 * \param[in,out]   h: Widget handle
 * \param[in]       *text: Pointer to text to add to list. Only pointer is saved to memory!
 * \retval          1: String added to the end
 * \retval          0: String not added
 */
uint8_t GUI_DROPDOWN_AddString(GUI_HANDLE_p h, const GUI_Char* text);

/**
 * \brief           Delete first string from list
 * \param[in,out]   h: Widget handle
 * \retval          1: String deleted
 * \retval          0: String not deleted
 * \sa              GUI_DROPDOWN_DeleteString
 * \sa              GUI_DROPDOWN_DeleteLastString
 */
uint8_t GUI_DROPDOWN_DeleteFirstString(GUI_HANDLE_p h);

/**
 * \brief           Delete last string from list
 * \param[in,out]   h: Widget handle
 * \retval          1: String deleted
 * \retval          0: String not deleted
 * \sa              GUI_DROPDOWN_DeleteString
 * \sa              GUI_DROPDOWN_DeleteFirstString
 */
uint8_t GUI_DROPDOWN_DeleteLastString(GUI_HANDLE_p h);

/**
 * \brief           Delete specific entry from list
 * \param[in,out]   h: Widget handle
 * \param[in]       index: List index (position) to delete
 * \retval          1: String deleted
 * \retval          0: String not deleted
 * \sa              GUI_DROPDOWN_DeleteFirstString
 * \sa              GUI_DROPDOWN_DeleteLastString
 */
uint8_t GUI_DROPDOWN_DeleteString(GUI_HANDLE_p h, uint16_t index);

/**
 * \brief           Set string value to already added string index
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index (position) on list to set/change text
 * \param[in]       *text: Pointer to text to add to list. Only pointer is saved to memory!
 * \retval          1: String changed
 * \retval          0: String not changed
 */
uint8_t GUI_DROPDOWN_SetString(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text);

/**
 * \brief           Set selected value
 * \param[in,out]   h: Widget handle
 * \param[in]       selection: Set to -1 to invalidate selection or 0 - count-1 for specific selection 
 * \retval          1: Selection changed
 * \retval          0: Selection not changed
 */
uint8_t GUI_DROPDOWN_SetSelection(GUI_HANDLE_p h, int16_t selection);

/**
 * \brief           Get selected value
 * \param[in,out]   h: Widget handle
 * \retval          Selection number or -1 if no selection
 */
int16_t GUI_DROPDOWN_GetSelection(GUI_HANDLE_p h);

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to 1 for auto mode or 0 for manual mode
 * \retval          1: Set OK
 * \retval          0: Set ERROR
 * \sa              GUI_DROPDOWN_SetSliderVisibility
 */
uint8_t GUI_DROPDOWN_SetSliderAuto(GUI_HANDLE_p h, uint8_t autoMode);

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in,out]   h: Widget handle
 * \param[in]       visible: Slider visible status, 1 or 0
 * \retval          1: Set to desired value
 * \retval          0: Not set to desired value
 * \sa              GUI_DROPDOWN_SetSliderAuto
 */
uint8_t GUI_DROPDOWN_SetSliderVisibility(GUI_HANDLE_p h, uint8_t visible);

/**
 * \brief           Scroll list if possible
 * \param[in,out]   h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \retval          1: Scroll successful
 * \retval          0: Scroll not successful
 */
uint8_t GUI_DROPDOWN_Scroll(GUI_HANDLE_p h, int16_t step);

/**
 * \brief           Set opening direction for dropdown list
 * \param[in,out]   h: Widget handle
 * \param[in]       dir: Opening direction. This parameter can be a value of \ref GUI_DROPDOWN_OPENDIR_t enumeration
 * \retval          1: Open direction has been changed 
 * \retval          0: Open direction has not been changed
 */
uint8_t GUI_DROPDOWN_SetOpenDirection(GUI_HANDLE_p h, GUI_DROPDOWN_OPENDIR_t dir);
    
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

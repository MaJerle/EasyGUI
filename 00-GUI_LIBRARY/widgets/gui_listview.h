/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI listview
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
#ifndef GUI_LISTVIEW_H
#define GUI_LISTVIEW_H

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
 * \defgroup        GUI_LISTVIEW List view
 * \brief           List view object
 * \{
 *
 * Listview widget can display multiple columns and multiple rows of data at the same time
 *
 * \image html image_widget_listview.png Listview object example
 *
 * Example code for image above
 * \code{c}
size_t len;
GUI_LISTVIEW_ROW_p row;

//Create widget
handle = GUI_LISTVIEW_Create(0, 10, 10, 100, 40, 0, 0, 0);

//Create columns
GUI_LISTVIEW_AddColumn(handle, _GT("Name"), 120);
GUI_LISTVIEW_AddColumn(handle, _GT("LastName"), 150);
GUI_LISTVIEW_AddColumn(handle, _GT("Email"), 220);
GUI_LISTVIEW_AddColumn(handle, _GT("Phone"), 160);

//Create rows and add values for columns
for (len = 0; len < 10; len++) {
    row = GUI_LISTVIEW_AddRow(handle);
    GUI_LISTVIEW_SetItemString(handle, row, 0, _GT("Name 1"));
    GUI_LISTVIEW_SetItemString(handle, row, 1, _GT("Last name 2"));
    GUI_LISTVIEW_SetItemString(handle, row, 2, _GT("info@example.com"));
    GUI_LISTVIEW_SetItemString(handle, row, 3, _GT("0123456789"));
}
\endcode
 */

/**
 * \brief           List of listview colors
 */
typedef enum GUI_LISTVIEW_COLOR_t {
    GUI_LISTVIEW_COLOR_BG = 0x00,           /*!< Background color */
    GUI_LISTVIEW_COLOR_TEXT,                /*!< Text color index */
    GUI_LISTVIEW_COLOR_SEL_FOC,             /*!< Text color of selected item when widget is in focus */
    GUI_LISTVIEW_COLOR_SEL_NOFOC,           /*!< Text color of selected item when widget is not in focus */
    GUI_LISTVIEW_COLOR_SEL_FOC_BG,          /*!< Background color of selected item when widget is in focus */
    GUI_LISTVIEW_COLOR_SEL_NOFOC_BG,        /*!< Background color of selected item when widget is not in focus */
} GUI_LISTVIEW_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_LISTVIEW_SLIDER_ON     0x01/*!< Slider is currently active */
#define GUI_FLAG_LISTVIEW_SLIDER_AUTO   0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \brief           Listview main row item
 */
typedef struct GUI_LISTVIEW_ROW_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_LinkedListRoot_t Root;              /*!< Linked list root entry for \ref GUI_LISTVIEW_ITEM_t column data entries */
} GUI_LISTVIEW_ROW_t;

/**
 * \brief           Listview row object
 * \hideinitializer
 */
#define __GLR(x)            ((GUI_LISTVIEW_ROW_t *)(x))

/**
 * \brief           Listview column item
 */
typedef struct GUI_LISTVIEW_COL_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_iDim_t Width;                       /*!< Column width in units of pixels */
    GUI_Char* Text;                         /*!< Header column text size */
} GUI_LISTVIEW_COL_t;

/**
 * \brief           Listview string item object
 */
typedef struct GUI_LISTVIEW_ITEM_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_Char* Text;                         /*!< Text entry */
} GUI_LISTVIEW_ITEM_t;
    
/**
 * \brief           Listview object structure
 */
typedef struct GUI_LISTVIEW_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    /**
     * Use colums as pointer to array for faster data access
     */
    GUI_LISTVIEW_COL_t** Cols;              /*!< Pointer to pointers of column elements */
    uint16_t ColsCount;                     /*!< Number of columns in listview package */
    
    /**
     * Use linked list for rows
     */
    GUI_LinkedListRoot_t Root;              /*!< Linked list root entry for \ref GUI_LISTVIEW_ROW_t for rows */
    
    int16_t Count;                          /*!< Current number of strings attached to this widget */
    int16_t Selected;                       /*!< Selected text index */
    int16_t VisibleStartIndex;              /*!< Index in array of string on top of visible area of widget */
    
    GUI_Dim_t SliderWidth;                  /*!< Slider width in units of pixels */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_LISTVIEW_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Typedef for row for public usage
 */
typedef void* GUI_LISTVIEW_ROW_p;
    
/**
 * \brief           Create new LISTVIEW widget
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
GUI_HANDLE_p GUI_LISTVIEW_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to LISTVIEW
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors. This parameter can be a value of \ref GUI_LISTVIEW_COLOR_t enumeration
 * \param[in]       color: Actual color code to set
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t GUI_LISTVIEW_SetColor(GUI_HANDLE_p h, GUI_LISTVIEW_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Add new column to listview
 * \param[in]       h: Widget handle
 * \param[in]       *text: Text to display on top
 * \param[in]       width: Width of column in units of pixels
 * \retval          1: Column was added
 * \retval          0: Column was not added
 */
uint8_t GUI_LISTVIEW_AddColumn(GUI_HANDLE_p h, const GUI_Char* text, GUI_Dim_t width);

/**
 * \brief           Set column width in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       index: Index for column number. First column has index 0
 * \param[in]       width: Width of column in units of pixels
 * \retval          1: Column was added
 * \retval          0: Column was not added
 */
uint8_t GUI_LISTVIEW_SetColumnWidth(GUI_HANDLE_p h, uint16_t index, GUI_Dim_t width);

/**
 * \brief           Add new empty row
 * \param[in]       h: Widget handle
 * \retval          Row object handle
 */
GUI_LISTVIEW_ROW_p GUI_LISTVIEW_AddRow(GUI_HANDLE_p h);

/**
 * \brief           Set item string for specific row and column
 * \param[in]       h: Widget handle
 * \param[in]       row: Row object handle, previously returned with \ref GUI_LISTVIEW_AddRow function
 * \param[in]       col: Column number to set. First column is on index = 0
 * \param[in]       *text: Text to use for item
 * \retval          1: Item string was set ok
 * \retval          0: Item string was not set
 */
uint8_t GUI_LISTVIEW_SetItemString(GUI_HANDLE_p h, GUI_LISTVIEW_ROW_p row, uint16_t col, const GUI_Char* text);

/**
 * \brief           Set selected row number
 * \param[in,out]   h: Widget handle
 * \param[in]       selection: Set to -1 to invalidate selection or 0 - count-1 for specific selection 
 * \retval          1: Selection changed
 * \retval          0: Selection not changed
 */
uint8_t GUI_LISTVIEW_SetSelection(GUI_HANDLE_p h, int16_t selection);

/**
 * \brief           Get selected row number
 * \param[in,out]   h: Widget handle
 * \retval          Selection number or -1 if no selection
 */
int16_t GUI_LISTVIEW_GetSelection(GUI_HANDLE_p h);

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to 1 for auto mode or 0 for manual mode
 * \retval          1: Set OK
 * \retval          0: Set ERROR
 * \sa              GUI_LISTVIEW_SetSliderVisibility
 */
uint8_t GUI_LISTVIEW_SetSliderAuto(GUI_HANDLE_p h, uint8_t autoMode);

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in,out]   h: Widget handle
 * \param[in]       visible: Slider visible status, 1 or 0
 * \retval          1: Set to desired value
 * \retval          0: Not set to desired value
 * \sa              GUI_LISTVIEW_SetSliderAuto
 */
uint8_t GUI_LISTVIEW_SetSliderVisibility(GUI_HANDLE_p h, uint8_t visible);

/**
 * \brief           Scroll list if possible
 * \param[in,out]   h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \retval          1: Scroll successful
 * \retval          0: Scroll not successful
 */
uint8_t GUI_LISTVIEW_Scroll(GUI_HANDLE_p h, int16_t step);

/**
 * \brief           Get item text value from row index and column index
 * \param[in,out]   h: Widget handle
 * \param[in]       rindex: Row index
 * \param[in]       cindex: Column index
 * \param[in]       *dst: Pointer to \ref GUI_Char variable to save text to it
 * \param[in]       length: Length of destination array
 * \retval          1: Text was found and copied
 * \retval          0: Text was not found
 */
uint8_t GUI_LISTVIEW_GetItemValue(GUI_HANDLE_p h, uint16_t rindex, uint16_t cindex, GUI_Char* dst, size_t length);

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

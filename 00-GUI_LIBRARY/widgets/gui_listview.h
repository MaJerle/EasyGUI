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
handle = gui_listview_create(0, 10, 10, 100, 40, 0, 0, 0);

//Create columns
gui_listview_addcolumn(handle, _GT("Name"), 120);
gui_listview_addcolumn(handle, _GT("LastName"), 150);
gui_listview_addcolumn(handle, _GT("Email"), 220);
gui_listview_addcolumn(handle, _GT("Phone"), 160);

//Create rows and add values for columns
for (len = 0; len < 10; len++) {
    row = gui_listview_addrow(handle);
    gui_listview_setitemstring(handle, row, 0, _GT("Name 1"));
    gui_listview_setitemstring(handle, row, 1, _GT("Last name 2"));
    gui_listview_setitemstring(handle, row, 2, _GT("info@example.com"));
    gui_listview_setitemstring(handle, row, 3, _GT("0123456789"));
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
    GUI_LISTVIEW_COLOR_BORDER,              /*!< Border color index for top line when not in 3D mode */
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
   
GUI_HANDLE_p gui_listview_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_listview_setcolor(GUI_HANDLE_p h, GUI_LISTVIEW_COLOR_t index, GUI_Color_t color);
uint8_t gui_listview_addcolumn(GUI_HANDLE_p h, const GUI_Char* text, GUI_Dim_t width);
uint8_t gui_listview_setcolumnwidth(GUI_HANDLE_p h, uint16_t index, GUI_Dim_t width);
GUI_LISTVIEW_ROW_p gui_listview_addrow(GUI_HANDLE_p h);
uint8_t gui_listview_setitemstring(GUI_HANDLE_p h, GUI_LISTVIEW_ROW_p row, uint16_t col, const GUI_Char* text);
uint8_t gui_listview_setselection(GUI_HANDLE_p h, int16_t selection);
int16_t gui_listview_getselection(GUI_HANDLE_p h);
uint8_t gui_listview_setsliderauto(GUI_HANDLE_p h, uint8_t autoMode);
uint8_t gui_listview_setslidervisibility(GUI_HANDLE_p h, uint8_t visible);
uint8_t gui_listview_scroll(GUI_HANDLE_p h, int16_t step);
uint8_t gui_listview_getitemvalue(GUI_HANDLE_p h, uint16_t rindex, uint16_t cindex, GUI_Char* dst, size_t length);

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

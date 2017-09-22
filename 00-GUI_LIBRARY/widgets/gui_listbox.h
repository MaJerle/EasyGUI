/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI LISTBOX widget
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
 * \defgroup        GUI_LISTBOX Listbox
 * \brief           LISTBOX widget
 * \{
 *
 * Listbox widget is simple widget to display single list of data. For detailed support of list displays, refer to \ref GUI_LISTVIEW widget
 *
 * \image html image_widget_listbox.png Listbox in passive (left) and active (focused, right image) modes
 *
 * Example code of image above
 * \code{c}
GUI_Char* listboxtexts[] = {
    _GT("Item 0"),
    _GT("Item 1"),
    _GT("Item 2"),
    _GT("Item 3"),
    _GT("Item 4"),
    _GT("Item 5"),
    _GT("Item 6"),
    _GT("Item 7"),
    _GT("Item 8"),
    _GT("Item 9"),
    _GT("Item 10"),
    _GT("Item 11"),
    _GT("Item 12"),
};

size_t i;
    
//Create left listbox
handle = gui_listbox_create(1, 10, 10, 190, 195, h, 0, 0);
for (i = 0; i < COUNT_OF(listboxtexts); i++) {
    gui_listbox_addstring(handle, listboxtexts[i]);
}
//Force visible slider
gui_listbox_setsliderauto(handle, 0);
gui_listbox_setslidervisibility(handle, 1);

//Create right list box
handle = gui_listbox_create(1, 210, 10, 200, 195, h, 0, 0);
for (i = 0; i < COUNT_OF(listboxtexts); i++) {
    gui_listbox_addstring(handle, listboxtexts[i]);
}
gui_listbox_setsliderauto(handle, 0);
gui_listbox_setslidervisibility(handle, 0);
\endcode
 */
    
/**
 * \brief           List of listbox colors
 */
typedef enum GUI_LISTBOX_COLOR_t {
    GUI_LISTBOX_COLOR_BG = 0x00,            /*!< Background color */
    GUI_LISTBOX_COLOR_TEXT,                 /*!< Text color index */
    GUI_LISTBOX_COLOR_SEL_FOC,              /*!< Text color of selected item when widget is in focus */
    GUI_LISTBOX_COLOR_SEL_NOFOC,            /*!< Text color of selected item when widget is not in focus */
    GUI_LISTBOX_COLOR_SEL_FOC_BG,           /*!< Background color of selected item when widget is in focus */
    GUI_LISTBOX_COLOR_SEL_NOFOC_BG,         /*!< Background color of selected item when widget is not in focus */
} GUI_LISTBOX_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_LISTBOX_SLIDER_ON      0x01/*!< Slider is currently active */
#define GUI_FLAG_LISTBOX_SLIDER_AUTO    0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \brief           LISTBOX string item object
 */
typedef struct GUI_LISTBOX_ITEM_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_Char* Text;                         /*!< Text entry */
} GUI_LISTBOX_ITEM_t;
    
/**
 * \brief           LISTBOX object structure
 */
typedef struct GUI_LISTBOX_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int16_t Count;                          /*!< Current number of strings attached to this widget */
    int16_t Selected;                       /*!< Selected text index */
    int16_t VisibleStartIndex;              /*!< Index in array of string on top of visible area of widget */
    
    GUI_LinkedListRoot_t Root;              /*!< Root of linked list entries */
    
    GUI_Dim_t SliderWidth;                  /*!< Slider width in units of pixels */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_LISTBOX_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

GUI_HANDLE_p gui_listbox_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_listbox_setcolor(GUI_HANDLE_p h, GUI_LISTBOX_COLOR_t index, GUI_Color_t color);
uint8_t gui_listbox_addstring(GUI_HANDLE_p h, const GUI_Char* text);
uint8_t gui_listbox_deletefirststring(GUI_HANDLE_p h);
uint8_t gui_listbox_deletelaststring(GUI_HANDLE_p h);
uint8_t gui_listbox_deletestring(GUI_HANDLE_p h, uint16_t index);
uint8_t gui_listbox_setstring(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text);
uint8_t gui_listbox_setselection(GUI_HANDLE_p h, int16_t selection);
int16_t gui_listbox_getselection(GUI_HANDLE_p h);
uint8_t gui_listbox_setsliderauto(GUI_HANDLE_p h, uint8_t autoMode);
uint8_t gui_listbox_setslidervisibility(GUI_HANDLE_p h, uint8_t visible);
uint8_t gui_listbox_scroll(GUI_HANDLE_p h, int16_t step);

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

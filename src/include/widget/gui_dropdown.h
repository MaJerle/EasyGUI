/**	
 * \file            gui_dropdown.h
 * \brief           Dropdown widget
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

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
#define GUI_FLAG_DROPDOWN_OPENED        0x01/*!< Dropdown is opened */
#define GUI_FLAG_DROPDOWN_SLIDER_ON     0x02/*!< Slider is currently active */
#define GUI_FLAG_DROPDOWN_SLIDER_AUTO   0x04/*!< Show right slider automatically when required, otherwise, manual mode is used */
#define GUI_FLAG_DROPDOWN_OPEN_UP       0x08/*!< Open dropdown in top direction instead of bottom */
    
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */
    
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

#if defined(GUI_INTERNAL) || __DOXYGEN__

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
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_dropdown_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_dropdown_setcolor(GUI_HANDLE_p h, GUI_DROPDOWN_COLOR_t index, GUI_Color_t color);
uint8_t gui_dropdown_addstring(GUI_HANDLE_p h, const GUI_Char* text);
uint8_t gui_dropdown_deletefirststring(GUI_HANDLE_p h);
uint8_t gui_dropdown_deletelaststring(GUI_HANDLE_p h);
uint8_t gui_dropdown_deletestring(GUI_HANDLE_p h, uint16_t index);
uint8_t gui_dropdown_setstring(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text);
uint8_t gui_dropdown_setselection(GUI_HANDLE_p h, int16_t selection);
int16_t gui_dropdown_getselection(GUI_HANDLE_p h);
uint8_t gui_dropdown_setsliderauto(GUI_HANDLE_p h, uint8_t autoMode);
uint8_t gui_dropdown_setslidervisibility(GUI_HANDLE_p h, uint8_t visible);
uint8_t gui_dropdown_scroll(GUI_HANDLE_p h, int16_t step);
uint8_t gui_dropdown_setopendirection(GUI_HANDLE_p h, GUI_DROPDOWN_OPENDIR_t dir);
    
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

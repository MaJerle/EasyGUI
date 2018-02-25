/**	
 * \file            gui_debugbox.h
 * \brief           List box widget
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
#ifndef __GUI_DEBUGBOX_H
#define __GUI_DEBUGBOX_H

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
 * \defgroup        GUI_DEBUGBOX Listbox
 * \brief           Debug box widget
 * \{
 */
    
/**
 * \brief           List of debugbox colors
 */
typedef enum GUI_DEBUGBOX_COLOR_t {
    GUI_DEBUGBOX_COLOR_BG = 0x00,            /*!< Background color */
    GUI_DEBUGBOX_COLOR_TEXT,                 /*!< Text color index */
} GUI_DEBUGBOX_COLOR_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
#define GUI_FLAG_DEBUGBOX_SLIDER_ON      0x01/*!< Slider is currently active */
#define GUI_FLAG_DEBUGBOX_SLIDER_AUTO    0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \brief           DEBUGBOX string item object
 */
typedef struct GUI_DEBUGBOX_ITEM_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on list */
    GUI_Char* Text;                         /*!< Text entry */
} GUI_DEBUGBOX_ITEM_t;
    
/**
 * \brief           DEBUGBOX object structure
 */
typedef struct GUI_DEBUGBOX_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int16_t Count;                          /*!< Current number of strings attached to this widget */
    int16_t MaxCount;                       /*!< Maximal number of lines in debug window */
    int16_t VisibleStartIndex;              /*!< Index in array of string on top of visible area of widget */
    
    GUI_LinkedListRoot_t Root;              /*!< Root of linked list entries */
    
    GUI_Dim_t SliderWidth;                  /*!< Slider width in units of pixels */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_DEBUGBOX_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_debugbox_create(GUI_ID_t id, float x, float y, float width, float height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_debugbox_setcolor(GUI_HANDLE_p h, GUI_DEBUGBOX_COLOR_t index, GUI_Color_t color);
uint8_t gui_debugbox_addstring(GUI_HANDLE_p h, const GUI_Char* text);

uint8_t gui_debugbox_setsliderauto(GUI_HANDLE_p h, uint8_t autoMode);
uint8_t gui_debugbox_setslidervisibility(GUI_HANDLE_p h, uint8_t visible);
uint8_t gui_debugbox_scroll(GUI_HANDLE_p h, int16_t step);

uint8_t gui_debugbox_setmaxitems(GUI_HANDLE_p h, int16_t max_items);

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

#endif /* __GUI_DEBUGBOX_H */

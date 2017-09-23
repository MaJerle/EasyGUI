/**	
 * \file            gui_checkbox.h
 * \brief           Check bar widget
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
#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

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
 * \defgroup        GUI_CHECKBOX Checkbox
 * \brief           Checkbox widget
 * \{
 *
 * Checkbox widget can hold state of checked or cleared value. On top of it, it can be disabled to make look and usage even more comparable to Windows applications
 *
 * \image html image_widget_checkbox.png Checkbox widgets in different states
 *
 * Example code of image above:
 * \code{c}
//First checkbox widget
handle = gui_checkbox_create(0, 10, 10, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box default"));

//Second checkbox widget
handle = gui_checkbox_create(0, 10, 50, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box checked"));
gui_checkbox_setchecked(handle, 1);

//Third checkbox widget
handle = gui_checkbox_create(0, 10, 90, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box disabled"));
gui_checkbox_setdisabled(handle, 1);

//Forth checkbox widget
handle = gui_checkbox_create(0, 10, 130, 400, 40, 0, 0, 0);
gui_widget_settext(handle, _GT("Check box checked disabled"));
gui_checkbox_setchecked(handle, 1);
gui_checkbox_setdisabled(handle, 1);
\endcode
 */
    
/**
 * \brief           List of checkbox colors
 */
typedef enum GUI_CHECKBOX_COLOR_t {
    GUI_CHECKBOX_COLOR_BG = 0x00,           /*!< Background color index */
    GUI_CHECKBOX_COLOR_FG,                  /*!< Foreground color index */
    GUI_CHECKBOX_COLOR_BORDER,              /*!< Border color index */
    GUI_CHECKBOX_COLOR_DISABLED_BG,         /*!< Background color index when disabled */
    GUI_CHECKBOX_COLOR_TEXT,                /*!< Text color index */
} GUI_CHECKBOX_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)

#define GUI_FLAG_CHECKBOX_CHECKED           0x01    /*!< Indicates checkbox is checked */
#define GUI_FLAG_CHECKBOX_DISABLED          0x02    /*!< Indicates checkbox is disabled */
    
/**
 * \brief           Checkbox object structure
 */
typedef struct GUI_CHECKBOX_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    uint8_t Flags;                          /*!< Widget flags */
} GUI_CHECKBOX_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

GUI_HANDLE_p gui_checkbox_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_checkbox_setcolor(GUI_HANDLE_p h, GUI_CHECKBOX_COLOR_t index, GUI_Color_t color);
uint8_t gui_checkbox_setchecked(GUI_HANDLE_p h, uint8_t checked);
uint8_t gui_checkbox_ischecked(GUI_HANDLE_p h);
uint8_t gui_checkbox_setdisabled(GUI_HANDLE_p h, uint8_t disabled);
uint8_t gui_checkbox_isdisabled(GUI_HANDLE_p h);

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

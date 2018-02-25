/**	
 * \file            gui_textview.h
 * \brief           Text view widget
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
#ifndef GUI_TEXTVIEW_H
#define GUI_TEXTVIEW_H

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
 * \defgroup        GUI_TEXTVIEW Text view
 * \brief           Text view widget
 * \{
 */
    
/**
 * \brief           List of text view colors
 */
typedef enum GUI_TEXTVIEW_COLOR_t {
    GUI_TEXTVIEW_COLOR_BG = 0x00,           /*!< Background color */
    GUI_TEXTVIEW_COLOR_TEXT,                /*!< Default text color */
} GUI_TEXTVIEW_COLOR_t;

/**
 * \brief           Vertical align for text view inside box
 */
typedef enum GUI_TEXTVIEW_VALIGN_t {        
    GUI_TEXTVIEW_VALIGN_TOP = GUI_VALIGN_TOP,       /*!< Vertical align top, default value */
    GUI_TEXTVIEW_VALIGN_CENTER = GUI_VALIGN_CENTER, /*!< Vertical align center */
    GUI_TEXTVIEW_VALIGN_BOTTOM = GUI_VALIGN_BOTTOM  /*!< Vertical align bottom */
} GUI_TEXTVIEW_VALIGN_t;

/**
 * \brief           Horizontal align for text view inside box
 */
typedef enum GUI_TEXTVIEW_HALIGN_t {        
    GUI_TEXTVIEW_HALIGN_LEFT = GUI_HALIGN_LEFT,     /*!< Horizontal align left, default value */
    GUI_TEXTVIEW_HALIGN_CENTER = GUI_HALIGN_CENTER, /*!< Horizontal align center */
    GUI_TEXTVIEW_HALIGN_RIGHT = GUI_HALIGN_RIGHT    /*!< Horizontal align right */
} GUI_TEXTVIEW_HALIGN_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
/**
 * \brief           Dropdown object structure
 */
typedef struct GUI_TEXTVIEW_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */

    GUI_TEXTVIEW_VALIGN_t VAlign;           /*!< Vertical text align */
    GUI_TEXTVIEW_HALIGN_t HAlign;           /*!< Horizontal text align */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_TEXTVIEW_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_textview_create(GUI_ID_t id, float x, float y, float width, float height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_textview_setcolor(GUI_HANDLE_p h, GUI_TEXTVIEW_COLOR_t index, GUI_Color_t color);
uint8_t gui_textview_setvalign(GUI_HANDLE_p h, GUI_TEXTVIEW_VALIGN_t align);
uint8_t gui_textview_sethalign(GUI_HANDLE_p h, GUI_TEXTVIEW_HALIGN_t align);
    
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

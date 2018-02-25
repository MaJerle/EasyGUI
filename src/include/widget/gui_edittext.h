/**	
 * \file            gui_edittext.h
 * \brief           Edittext widget
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
#ifndef __GUI_EDITTEXT_H
#define __GUI_EDITTEXT_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \{
 */

#include "gui/gui.h"

/**
 * \defgroup        GUI_EDITTEXT Edit text
 * \brief           Single-line edit text widget
 * \{
 */

/**
 * \brief           Edit text color list enumeration
 */
typedef enum GUI_EDITTEXT_COLOR_t {
    GUI_EDITTEXT_COLOR_BG = 0x00,           /*!< Background color index */
    GUI_EDITTEXT_COLOR_FG,                  /*!< Foreground color index */
    GUI_EDITTEXT_COLOR_BORDER,              /*!< Border color index */
} GUI_EDITTEXT_COLOR_t;

/**
 * \brief           Vertical align for edit text inside box
 */
typedef enum GUI_EDITTEXT_VALIGN_t {        
    GUI_EDITTEXT_VALIGN_TOP = GUI_VALIGN_TOP,       /*!< Vertical align top, default value */
    GUI_EDITTEXT_VALIGN_CENTER = GUI_VALIGN_CENTER, /*!< Vertical align center */
    GUI_EDITTEXT_VALIGN_BOTTOM = GUI_VALIGN_BOTTOM  /*!< Vertical align bottom */
} GUI_EDITTEXT_VALIGN_t;

/**
 * \brief           Horizontal align for edit text inside box
 */
typedef enum GUI_EDITTEXT_HALIGN_t {        
    GUI_EDITTEXT_HALIGN_LEFT = GUI_HALIGN_LEFT,     /*!< Horizontal align left, default value */
    GUI_EDITTEXT_HALIGN_CENTER = GUI_HALIGN_CENTER, /*!< Horizontal align center */
    GUI_EDITTEXT_HALIGN_RIGHT = GUI_HALIGN_RIGHT    /*!< Horizontal align right */
} GUI_EDITTEXT_HALIGN_t;
    
#if defined(GUI_INTERNAL) || __DOXYGEN__

#define GUI_EDITTEXT_FLAG_MULTILINE         ((uint8_t)0x01) /*!< Defines widget as multi-line edit */

/**
 * \brief           GUI EDIT TEXT widget structure
 */
typedef struct GUI_EDITTEXT_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    uint8_t Flags;                          /*!< List of widget flags */
    GUI_EDITTEXT_VALIGN_t VAlign;           /*!< Vertical align setup */
    GUI_EDITTEXT_HALIGN_t HAlign;           /*!< Horizontal align setup */
} GUI_EDITTEXT_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_edittext_create(GUI_ID_t id, float x, float y, float width, float height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_edittext_setcolor(GUI_HANDLE_p h, GUI_EDITTEXT_COLOR_t index, GUI_Color_t color);
uint8_t gui_edittext_setmultiline(GUI_HANDLE_p h, uint8_t multiline);
uint8_t gui_edittext_setvalign(GUI_HANDLE_p h, GUI_EDITTEXT_VALIGN_t align);
uint8_t gui_edittext_sethalign(GUI_HANDLE_p h, GUI_EDITTEXT_HALIGN_t align);

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

#endif /* __GUI_EDITTEXT_H */

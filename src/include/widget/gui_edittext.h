/**	
 * \file            gui_edittext.h
 * \brief           Edittext widget
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef GUI_HDR_EDITTEXT_H
#define GUI_HDR_EDITTEXT_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui/gui.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_EDITTEXT Edit text
 * \brief           Single-line edit text widget
 * \{
 */

/**
 * \brief           Edit text color list enumeration
 */
typedef enum {
    GUI_EDITTEXT_COLOR_BG = 0x00,           /*!< Background color index */
    GUI_EDITTEXT_COLOR_FG,                  /*!< Foreground color index */
    GUI_EDITTEXT_COLOR_BORDER,              /*!< Border color index */
} gui_edittext_color_t;

/**
 * \brief           Vertical align for edit text inside box
 */
typedef enum {        
    GUI_EDITTEXT_VALIGN_TOP = GUI_VALIGN_TOP,       /*!< Vertical align top, default value */
    GUI_EDITTEXT_VALIGN_CENTER = GUI_VALIGN_CENTER, /*!< Vertical align center */
    GUI_EDITTEXT_VALIGN_BOTTOM = GUI_VALIGN_BOTTOM  /*!< Vertical align bottom */
} gui_edittext_valign_t;

/**
 * \brief           Horizontal align for edit text inside box
 */
typedef enum {        
    GUI_EDITTEXT_HALIGN_LEFT = GUI_HALIGN_LEFT,     /*!< Horizontal align left, default value */
    GUI_EDITTEXT_HALIGN_CENTER = GUI_HALIGN_CENTER, /*!< Horizontal align center */
    GUI_EDITTEXT_HALIGN_RIGHT = GUI_HALIGN_RIGHT    /*!< Horizontal align right */
} gui_edittext_halign_t;

gui_handle_p    gui_edittext_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_edittext_setcolor(gui_handle_p h, gui_edittext_color_t index, gui_color_t color);
uint8_t         gui_edittext_setmultiline(gui_handle_p h, uint8_t multiline);
uint8_t         gui_edittext_setvalign(gui_handle_p h, gui_edittext_valign_t align);
uint8_t         gui_edittext_sethalign(gui_handle_p h, gui_edittext_halign_t align);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_EDITTEXT_H */

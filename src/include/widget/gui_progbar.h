/**	
 * \file            gui_progbar.h
 * \brief           Progress bar widget
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
#ifndef GUI_HDR_PROGBAR_H
#define GUI_HDR_PROGBAR_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_PROGBAR Progress bar
 * \brief           Progress bar for progress visualization
 * \{
 */

/**
 * \brief           Progress bar color list enumeration
 */
typedef enum {
    GUI_PROGBAR_COLOR_BG = 0x00,            /*!< Background color index */
    GUI_PROGBAR_COLOR_FG = 0x01,            /*!< Foreground (active part) color index */
    GUI_PROGBAR_COLOR_BORDER = 0x02         /*!< Border color index */
} gui_progbar_color_t;

gui_handle_p    gui_progbar_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_progbar_setcolor(gui_handle_p h, gui_progbar_color_t index, gui_color_t color);
uint8_t         gui_progbar_setmin(gui_handle_p h, int32_t val);
uint8_t         gui_progbar_setmax(gui_handle_p h, int32_t val);
uint8_t         gui_progbar_setvalue(gui_handle_p h, int32_t val);
uint8_t         gui_progbar_setanimation(gui_handle_p h, uint8_t anim);
int32_t         gui_progbar_getmin(gui_handle_p h);
int32_t         gui_progbar_getmax(gui_handle_p h);
int32_t         gui_progbar_getvalue(gui_handle_p h);
uint8_t         gui_progbar_setpercentmode(gui_handle_p h, uint8_t enable);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_PROGBAR_H */

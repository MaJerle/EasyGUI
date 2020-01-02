/**	
 * \file            gui_container.h
 * \brief           Container widget
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
#ifndef GUI_HDR_CONTAINER_H
#define GUI_HDR_CONTAINER_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_CONTAINER Container
 * \brief           Container widget holding widgets
 * \{
 *
 * Container is basic widget suitable for custom children widgets.
 *
 * It is similar to \ref GUI_WINDOW widget except it does not implement any controls like hide, maximize and minimize
 */

/**
 * \brief           Container colors
 */    
typedef enum {
    GUI_CONTAINER_COLOR_BG = 0x00,          /*!< Background color index */
} gui_container_color_t;

gui_handle_p    gui_container_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_container_setcolor(gui_handle_p h, gui_container_color_t index, gui_color_t color);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_CONTAINER_H */

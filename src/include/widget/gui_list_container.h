/**	
 * \file            gui_list_container.h
 * \brief           List container widget
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
#ifndef GUI_HDR_LISTCONTAINER_H
#define GUI_HDR_LISTCONTAINER_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_LISTCONTAINER List view container
 * \brief           List view container is basic widget for scrolling
 * \{
 *
 * This widget supports scrolling (similar to ListView in Android).
 * When subwidgets are put to it and if they don't support touch move event,
 * event will be forwarded to next widget level (List container) and in 
 * case there are widgets outside visible area, they will be scrolled.
 */

/**
 * \brief           List of available colors for list container
 */
typedef enum {
    GUI_LISTCONTAINER_COLOR_BG = 0x00,      /*!< Color index for LED on status */
} gui_listcontainer_color_t;

/**
 * \brief           List of available container modes
 */
typedef enum {
    GUI_LISTCONTAINER_MODE_VERTICAL = 0x0,  /*!< Vertical mode */
    GUI_LISTCONTAINER_MODE_HORIZONTAL,      /*!< Horizontal mode */
    GUI_LISTCONTAINER_MODE_VERTICAL_HORIZONTAL, /*!< Vertical & Horizontal mode */
} gui_listcontainer_mode_t;

gui_handle_p    gui_listcontainer_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_listcontainer_setcolor(gui_handle_p h, gui_listcontainer_color_t index, gui_color_t color);
uint8_t         gui_listcontainer_setmode(gui_handle_p h, gui_listcontainer_mode_t mode);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_LISTCONTAINER_H */

/**	
 * \file            gui_slider.h
 * \brief           Slider widget
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
#ifndef GUI_HDR_SLIDER_H
#define GUI_HDR_SLIDER_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_SLIDER Slider
 * \brief           Simple slider widget for user interaction
 * \{
 */

/**
 * \brief           List of slider colors
 */
typedef enum {
    GUI_SLIDER_COLOR_BG_NONACTIVE = 0x00,   /*!< Background color index on slider part of non-active area */
    GUI_SLIDER_COLOR_BG_ACTIVE,             /*!< Background color index on slider part of active area */
    GUI_SLIDER_COLOR_FG,                    /*!< Foreground color index for foreground (circle, ...) part */
    GUI_SLIDER_COLOR_BORDER,                /*!< Border color index for bottom part */
} gui_slider_color_t;

/**
 * \brief           List of slider modes
 */
typedef enum {
    GUI_SLIDER_MODE_LEFT_RIGHT = 0x00,      /*!< Slider is horizontal, min on the left, max on the right */
    GUI_SLIDER_MODE_RIGHT_LEFT,             /*!< Slider is horizontal, min on the right, max on the left */
    GUI_SLIDER_MODE_BOTTOM_TOP,             /*!< Slider is horizontal, min on the bottom, max on the top */
    GUI_SLIDER_MODE_TOP_BOTTOM,             /*!< Slider is horizontal, min on the top, max on the bottom */
} gui_slider_mode_t;

gui_handle_p    gui_slider_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_slider_setcolor(gui_handle_p h, gui_slider_color_t index, gui_color_t color);
uint8_t         gui_slider_setmode(gui_handle_p h, gui_slider_mode_t mode);
uint8_t         gui_slider_setmin(gui_handle_p h, int32_t val);
uint8_t         gui_slider_setmax(gui_handle_p h, int32_t val);
uint8_t         gui_slider_setvalue(gui_handle_p h, int32_t val);
int32_t         gui_slider_getmin(gui_handle_p h);
int32_t         gui_slider_getmax(gui_handle_p h);
int32_t         gui_slider_getvalue(gui_handle_p h);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_SLIDER_H */

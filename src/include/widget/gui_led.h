/**	
 * \file            gui_led.h
 * \brief           LED widget
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
#ifndef GUI_HDR_LED_H
#define GUI_HDR_LED_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_LED Led
 * \brief           Simple LED
 * \{
 */

/**
 * \brief           List of available colors for LED
 */
typedef enum {
    GUI_LED_COLOR_ON = 0x00,                /*!< Color index for LED on status */
    GUI_LED_COLOR_OFF,                      /*!< Color index for LED off status */
    GUI_LED_COLOR_ON_BORDER,                /*!< Color index for LED border on status */
    GUI_LED_COLOR_OFF_BORDER                /*!< Color index for LED border off status */
} gui_led_color_t;

/**
 * \brief           List of available LED types
 */
typedef enum {
    GUI_LED_TYPE_RECT = 0x00,               /*!< Led is rectangular */
    GUI_LED_TYPE_CIRCLE = 0x01              /*!< Led is circle */
} gui_led_type_t;

gui_handle_p    gui_led_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_led_setcolor(gui_handle_p h, gui_led_color_t index, gui_color_t color);
uint8_t         gui_led_settype(gui_handle_p h, gui_led_type_t type);
uint8_t         gui_led_toggle(gui_handle_p h);
uint8_t         gui_led_set(gui_handle_p h, uint8_t state);
uint8_t         gui_led_ison(gui_handle_p h);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_LED_H */

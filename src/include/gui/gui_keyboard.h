/**	
 * \file            gui_keyboard.h
 * \brief           Software keyboard widget
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
#ifndef __GUI_KEYBOARD_H
#define __GUI_KEYBOARD_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_INPUT
 * \{
 */

#include "gui/gui_input.h"
#include "widget/gui_widget.h"

/**
 * \defgroup        GUI_KEYBOARD Virtual keyboard
 * \brief           Virtual keyboard on screen
 * \{
 *
 * \image html image_widget_keyboard_normal.png Characters layout
 * \image html image_widget_keyboard_numbers.png Numbers layout
 * \image html image_widget_keyboard_special.png Special characters layout
 *
 * Virtual keyboard consists of 2 types of widgets:
 *  
 *  - \ref GUI_CONTAINER widget as base element and
 *  - \ref GUI_BUTTON widget for each button
 *
 * Currently, it uses a lot of RAM as each button is separated widget.
 *
 * According to widget used by keyboard, it will choose the same font in keyboard.
 */
    
/**
 * \defgroup        GUI_KEYBOARD_Private Private functions
 * \brief           Private keyboard functions
 *
 * \note            Since they are not thread safe, they can only be used when inside GUI library.
 *                  It means they can be used in custom widget implementations where thread safety is guaranteed
 *
 * \{
 */
 
uint8_t gui_keyboard_hide__(void);
uint8_t gui_keyboard_show__(GUI_HANDLE_p h);
    
/**
 * \}
 */
    
GUI_HANDLE_p gui_keyboard_create(void);
uint8_t gui_keyboard_hide(void);
uint8_t gui_keyboard_show(GUI_HANDLE_p h);
    
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

#endif /* __GUI_KEYBOARD_H */

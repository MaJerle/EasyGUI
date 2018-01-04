/**	
 * \file            gui_slider.h
 * \brief           Slider widget
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
#ifndef __GUI_SLIDER_H
#define __GUI_SLIDER_H

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
 * \defgroup        GUI_SLIDER Slider
 * \brief           Simple slider widget for user interaction
 * \{
 *
 * Simple slider widget to select range between 2 (min, max) values.
 *
 * \image html image_widget_slider.png Slider widget in different states. On top is passive state (touch not active) and bottom is when touch is active
 *
 * Example code of image above:
 * \code{c}
//Create basic slider
handle = gui_slider_create(0, 10, 10, 300, 50, 0, 0, 0);
\endcode
 */

/**
 * \brief           List of slider colors
 */
typedef enum GUI_SLIDER_COLOR_t {
    GUI_SLIDER_COLOR_BG_NONACTIVE = 0x00,   /*!< Background color index on slider part of non-active area */
    GUI_SLIDER_COLOR_BG_ACTIVE,             /*!< Background color index on slider part of active area */
    GUI_SLIDER_COLOR_FG,                    /*!< Foreground color index for foreground (circle, ...) part */
    GUI_SLIDER_COLOR_BORDER,                /*!< Border color index for bottom part */
} GUI_SLIDER_COLOR_t;

/**
 * \brief           List of slider modes
 */
typedef enum GUI_SLIDER_MODE_t {
    GUI_SLIDER_MODE_LEFT_RIGHT = 0x00,      /*!< Slider is horizontal, min on the left, max on the right */
    GUI_SLIDER_MODE_RIGHT_LEFT,             /*!< Slider is horizontal, min on the right, max on the left */
    GUI_SLIDER_MODE_BOTTOM_TOP,             /*!< Slider is horizontal, min on the bottom, max on the top */
    GUI_SLIDER_MODE_TOP_BOTTOM,             /*!< Slider is horizontal, min on the top, max on the bottom */
} GUI_SLIDER_MODE_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__
    
/**
 * \brief           SLIDER object structure
 */
typedef struct GUI_SLIDER_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int32_t Min;                            /*!< Low value for slider */
    int32_t Max;                            /*!< High value for slider */
    int32_t Value;                          /*!< Current value for slider */
    GUI_SLIDER_MODE_t Mode;                 /*!< Slider mode */
    
    uint8_t MaxSize;                        /*!< Max size for animation */
    uint8_t CurrentSize;                    /*!< Current size for animation */
    
    uint8_t Flags;                          /*!< Widget flags */
} GUI_SLIDER_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_slider_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_slider_setcolor(GUI_HANDLE_p h, GUI_SLIDER_COLOR_t index, GUI_Color_t color);
uint8_t gui_slider_setmode(GUI_HANDLE_p h, GUI_SLIDER_MODE_t mode);
uint8_t gui_slider_setmin(GUI_HANDLE_p h, int32_t val);
uint8_t gui_slider_setmax(GUI_HANDLE_p h, int32_t val);
uint8_t gui_slider_setvalue(GUI_HANDLE_p h, int32_t val);
int32_t gui_slider_getmin(GUI_HANDLE_p h);
int32_t gui_slider_getmax(GUI_HANDLE_p h);
int32_t gui_slider_getvalue(GUI_HANDLE_p h);

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

#endif /* __GUI_SLIDER_H */

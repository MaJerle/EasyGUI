/**	
 * \file            gui_progbar.h
 * \brief           Progress bar widget
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
#ifndef GUI_PROGBAR_H
#define GUI_PROGBAR_H

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
 * \defgroup        GUI_PROGBAR Progress bar
 * \brief           Progress bar for progress visualization
 * \{
 *
 * Progress bar is used to show single progress value according to minimal, maximal and current values set to widget.
 *
 * \image html image_widget_progbar.png Progress bars
 *
 * Example code of image above:
 * \code{c}
handle = gui_progbar_create(2, 10, 10, 400, 40, h, 0, 0);
gui_widget_settext(handle, _GT("Progress bar with custom text"));

handle = gui_progbar_create(2, 10, 60, 400, 40, h, 0, 0);
gui_progbar_enablepercentages(handle);
gui_progbar_setmin(handle, 20);
gui_progbar_setmax(handle, 30);
gui_progbar_setvalue(handle, 22);    
\endcode
 */

/**
 * \brief           Progress bar color list enumeration
 */
typedef enum GUI_PROGBAR_COLOR_t {
    GUI_PROGBAR_COLOR_BG = 0x00,            /*!< Background color index */
    GUI_PROGBAR_COLOR_FG = 0x01,            /*!< Foreground (active part) color index */
    GUI_PROGBAR_COLOR_BORDER = 0x02         /*!< Border color index */
} GUI_PROGBAR_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Progress bar widget structure
 */
typedef struct GUI_PROGBAR_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int32_t Min;                            /*!< Low value for progress bar */
    int32_t Max;                            /*!< High value for progress bar */
    int32_t CurrentValue;                   /*!< Current value for progress bar */
    int32_t DesiredValue;                   /*!< Desired value, set by used */
    uint8_t Flags;                          /*!< Flags variable */
} GUI_PROGBAR_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

GUI_HANDLE_p gui_progbar_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_progbar_setcolor(GUI_HANDLE_p h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color);
uint8_t gui_progbar_setmin(GUI_HANDLE_p h, int32_t val);
uint8_t gui_progbar_setmax(GUI_HANDLE_p h, int32_t val);
uint8_t gui_progbar_setvalue(GUI_HANDLE_p h, int32_t val);
uint8_t gui_progbar_setanimation(GUI_HANDLE_p h, uint8_t anim);
int32_t gui_progbar_getmin(GUI_HANDLE_p h);
int32_t gui_progbar_getmax(GUI_HANDLE_p h);
int32_t gui_progbar_getvalue(GUI_HANDLE_p h);
uint8_t gui_progbar_setpercentmode(GUI_HANDLE_p h, uint8_t enable);

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

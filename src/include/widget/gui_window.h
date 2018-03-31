/** 
 * \file            gui_textview.h
 * \brief           Window widget
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
#ifndef __GUI_WINDOW_H
#define __GUI_WINDOW_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_WINDOW Window
 * \brief           Basic widget and core parent to all other widgets
 * \{
 */
    
/**
 * \brief           Window color list enumeration
 */
typedef enum GUI_WINDOW_COLOR_t {
    GUI_WINDOW_COLOR_BG = 0x00,             /*!< Background color index */
    GUI_WINDOW_COLOR_TEXT,                  /*!< Text color index */
    GUI_WINDOW_COLOR_TOP_BG_FOC,            /*!< Top background color when widget in focus */
    GUI_WINDOW_COLOR_TOP_BG_NOFOC,          /*!< Top background color when widget not in focus */
} GUI_WINDOW_COLOR_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__
/**
 * \brief           Window object structure
 */
typedef struct GUI_WINDOW_t {
    gui_handle_root_t C;                    /*!< GUI handle object, must always be first on list */
    
    uint32_t Number;                        /*!< Window number */
    
    gui_dim_t BorderRadius;                 /*!< Radius in units of pixels for children windows */
    gui_dim_t BorderWidth;                  /*!< Border width */
} GUI_WINDOW_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */
  
gui_handle_p gui_window_getdesktop(void);
uint8_t gui_window_setactive(gui_handle_p h);
gui_handle_p gui_window_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags);
uint8_t gui_window_setcolor(gui_handle_p h, GUI_WINDOW_COLOR_t index, gui_color_t color);

#if defined(GUI_INTERNAL)
gui_handle_p gui_window_createdesktop(gui_id_t id, gui_widget_callback_t cb);
#endif /* defined(GUI_INTERNAL) */
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __GUI_WINDOW_H */

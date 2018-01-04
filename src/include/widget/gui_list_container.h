/**	
 * \file            gui_list_container.h
 * \brief           List container widget
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
#ifndef __GUI_LIST_CONTAINER_H
#define __GUI_LIST_CONTAINER_H

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
 * \defgroup        GUI_LIST_CONTAINER List view container
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
typedef enum GUI_LIST_CONTAINER_COLOR_t {
    GUI_LIST_CONTAINER_COLOR_BG = 0x00,     /*!< Color index for LED on status */
} GUI_LIST_CONTAINER_COLOR_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__

/**
 * \brief           LIST CONTAINER object structure
 */
typedef struct GUI_LIST_CONTAINER_t {
    GUI_HANDLE_ROOT_t C;                    /*!< Global widget object */
    
    GUI_iDim_t MaxScrollX;                  /*!< Maximal scroll on X axis */
    GUI_iDim_t MaxScrollY;                  /*!< Maximal scroll on Y axis */
} GUI_LIST_CONTAINER_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_listcontainer_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

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

#endif /* __GUI_LIST_CONTAINER_H */

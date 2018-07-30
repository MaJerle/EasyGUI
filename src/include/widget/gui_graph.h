/**	
 * \file            gui_graph.c
 * \brief           Graph widget
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#ifndef __GUI_GRAPH_H
#define __GUI_GRAPH_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"

/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_GRAPH Graph
 * \brief           Graph which can display plots
 * \{
 */
    
/**
 * \brief           Graph color list enumeration
 * \sa              gui_graph_setcolor
 */
typedef enum {
    GUI_GRAPH_COLOR_BG = 0x00,              /*!< Background color index */
    GUI_GRAPH_COLOR_FG = 0x01,              /*!< Foreground color index (background of plotting area) */
    GUI_GRAPH_COLOR_BORDER = 0x02,          /*!< Border color index */
    GUI_GRAPH_COLOR_GRID = 0x03             /*!< Grid color index */
} gui_graph_color_t;

/**
 * \brief           Graph border list enumeration
 */
typedef enum {
    GUI_GRAPH_BORDER_TOP = 0x00,            /*!< Border top index */
    GUI_GRAPH_BORDER_RIGHT = 0x01,          /*!< Border right index */
    GUI_GRAPH_BORDER_BOTTOM = 0x02,         /*!< Border bottom index */
    GUI_GRAPH_BORDER_LEFT = 0x03            /*!< Border left index */
} gui_graph_border_t;

/**
 * \brief           Graph type enumeration
 */
typedef enum {
    GUI_GRAPH_TYPE_YT = 0x00,               /*!< Data type is Y versus time */
    GUI_GRAPH_TYPE_XY = 0x01,               /*!< Data type is Y versus X [Y(x)] */
} gui_graph_type_t;

/**
 * \brief           Graph data widget structure
 */
typedef struct {
#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE || __DOXYGEN__
    gui_linkedlistroot_t root;              /*!< Root linked list object of graph widgets */
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    gui_id_t id;                            /*!< Data ID */
    
    int16_t* data;                          /*!< Pointer to actual data object */
    size_t length;                          /*!< Size of data array */
    size_t ptr;                             /*!< Read/Write start pointer */
    
    gui_color_t color;                      /*!< Curve color */
    gui_graph_type_t type;                  /*!< Plot data type */
} gui_graph_data_t;

typedef gui_graph_data_t * gui_graph_data_p;/*!< GUI Graph data pointer */

#if defined(GUI_INTERNAL) || __DOXYGEN__
/**
 * \brief           Graph widget structure
 */
typedef struct {
    gui_handle C;                           /*!< GUI handle object, must always be first on list */
    gui_linkedlistroot_t root;              /*!< Linked list root object for data objects. It stores where first in last data exists for this graph */
    
    gui_dim_t border[4];                    /*!< Borders for widgets */
    uint8_t rows;                           /*!< Number of rows in plot represented with lines */
    uint8_t columns;                        /*!< Number of columns in plot represented with lines */
    float min_x;                            /*!< Minimal X value for plot */
    float max_x;                            /*!< Maximal X value for plot */
    float min_y;                            /*!< Minimal Y value for plot */
    float max_y;                            /*!< Maximal Y value for plot */
    float visible_min_x;                    /*!< Visible minimal X value for plot */
    float visible_max_x;                    /*!< Visible maximal X value for plot */
    float visible_min_y;                    /*!< Visible minimal Y value for plot */
    float visible_max_y;                    /*!< Visible maximal Y value for plot */
} gui_graph_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

gui_handle_p    gui_graph_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags);
uint8_t         gui_graph_setcolor(gui_handle_p h, gui_graph_color_t index, gui_color_t color);
uint8_t         gui_graph_setminx(gui_handle_p h, float v);
uint8_t         gui_graph_setmaxx(gui_handle_p h, float v);
uint8_t         gui_graph_setminy(gui_handle_p h, float v);
uint8_t         gui_graph_setmaxy(gui_handle_p h, float v);
uint8_t         gui_graph_setaxes(gui_handle_p h, float min_x, float max_x, float min_y, float max_y);
uint8_t         gui_graph_zoomreset(gui_handle_p h);
uint8_t         gui_graph_zoom(gui_handle_p h, float zoom, float x, float y);
uint8_t         gui_graph_attachdata(gui_handle_p h, gui_graph_data_p data);
uint8_t         gui_graph_detachdata(gui_handle_p h, gui_graph_data_p data);

gui_graph_data_p    gui_graph_data_create(gui_id_t id, gui_graph_type_t type, size_t length);
uint8_t             gui_graph_data_addvalue(gui_graph_data_p data, int16_t x, int16_t y);
uint8_t             gui_graph_data_setcolor(gui_graph_data_p data, gui_color_t color);
gui_graph_data_p    gui_graph_data_get_by_id(gui_handle_p graph_h, gui_id_t id);

 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __GUI_GRAPH_H */

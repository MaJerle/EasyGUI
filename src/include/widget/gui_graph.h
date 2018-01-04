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
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#ifndef __GUI_GRAPH_H
#define __GUI_GRAPH_H

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
 * \defgroup        GUI_GRAPH Graph
 * \brief           Graph which can display plots
 * \{
 *
 * Graph widget to display plot lines of 2 different types, either XY or YT.
 *
 * \image html image_widget_graph.png Graph widget with 2 data plots. Red is XY and green is YT
 *
 * Example code for image above
 * \code{c}
handle = gui_graph_create(ID_GRAPH_MAIN, 10, 10, 400, 220, h, 0, 0);

//Create data objects
if (!graphdata1) {
    graphdata1 = gui_graph_data_create(GUI_GRAPH_TYPE_XY, len);
    graphdata1->Color = GUI_COLOR_RED;
}
if (!graphdata2) {
    graphdata2 = gui_graph_data_create(GUI_GRAPH_TYPE_YT, len / 2);
    graphdata2->Color = GUI_COLOR_GREEN;
}

//Fill data objects
for (i = 0; i <= 360; i += 360 / len) {
    x = cos((float)i * (PI / 180.0f));
    y = sin((float)i * (PI / 180.0f));
    gui_graph_data_addvalue(graphdata1, x * radius, y * radius);
    gui_graph_data_addvalue(graphdata2, x * radius / 3, y * radius / 4);
}
gui_graph_attachdata(handle, graphdata1);
gui_graph_attachdata(handle, graphdata2);
\endcode
 */
    
/**
 * \brief           Graph color list enumeration
 * \sa              gui_graph_setcolor
 */
typedef enum GUI_GRAPH_COLOR_t {
    GUI_GRAPH_COLOR_BG = 0x00,              /*!< Background color index */
    GUI_GRAPH_COLOR_FG = 0x01,              /*!< Foreground color index (background of plotting area) */
    GUI_GRAPH_COLOR_BORDER = 0x02,          /*!< Border color index */
    GUI_GRAPH_COLOR_GRID = 0x03             /*!< Grid color index */
} GUI_GRAPH_COLOR_t;

/**
 * \brief           Graph border list enumeration
 */
typedef enum GUI_GRAPH_BORDER_t {
    GUI_GRAPH_BORDER_TOP = 0x00,            /*!< Border top index */
    GUI_GRAPH_BORDER_RIGHT = 0x01,          /*!< Border right index */
    GUI_GRAPH_BORDER_BOTTOM = 0x02,         /*!< Border bottom index */
    GUI_GRAPH_BORDER_LEFT = 0x03            /*!< Border left index */
} GUI_GRAPH_BORDER_t;

/**
 * \brief           Graph type enumeration
 */
typedef enum GUI_GRAPH_TYPE_t {
    GUI_GRAPH_TYPE_YT = 0x00,               /*!< Data type is Y versus time */
    GUI_GRAPH_TYPE_XY = 0x01,               /*!< Data type is Y versus X [Y(x)] */
} GUI_GRAPH_TYPE_t;

/**
 * \brief           Graph data widget structure
 */
typedef struct GUI_GRAPH_DATA_p {
#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE || defined(DOXYGEN)
    GUI_LinkedListRoot_t Root;              /*!< Root linked list object of graph widgets */
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    int16_t* Data;                          /*!< Pointer to actual data object */
    size_t Length;                          /*!< Size of data array */
    size_t Ptr;                             /*!< Read/Write start pointer */
    
    GUI_Color_t Color;                      /*!< Curve color */
    GUI_GRAPH_TYPE_t Type;                  /*!< Plot data type */
} GUI_GRAPH_DATA_t;

typedef GUI_GRAPH_DATA_t *  GUI_GRAPH_DATA_p;   /*!< GUI Graph data pointer */

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Graph widget structure
 */
typedef struct GUI_GRAPH_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    GUI_LinkedListRoot_t Root;              /*!< Linked list root object for data objects. It stores where first in last data exists for this graph */
    
    GUI_Dim_t Border[4];                    /*!< Borders for widgets */
    uint8_t Rows;                           /*!< Number of rows in plot represented with lines */
    uint8_t Columns;                        /*!< Number of columns in plot represented with lines */
    float MinX;                             /*!< Minimal X value for plot */
    float MaxX;                             /*!< Maximal X value for plot */
    float MinY;                             /*!< Minimal Y value for plot */
    float MaxY;                             /*!< Maximal Y value for plot */
    float VisibleMinX;                      /*!< Visible minimal X value for plot */
    float VisibleMaxX;                      /*!< Visible maximal X value for plot */
    float VisibleMinY;                      /*!< Visible minimal Y value for plot */
    float VisibleMaxY;                      /*!< Visible maximal Y value for plot */
} GUI_GRAPH_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

GUI_HANDLE_p gui_graph_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_graph_setcolor(GUI_HANDLE_p h, GUI_GRAPH_COLOR_t index, GUI_Color_t color);
uint8_t gui_graph_setminx(GUI_HANDLE_p h, float v);
uint8_t gui_graph_setmaxx(GUI_HANDLE_p h, float v);
uint8_t gui_graph_setminy(GUI_HANDLE_p h, float v);
uint8_t gui_graph_setmaxy(GUI_HANDLE_p h, float v);
uint8_t gui_graph_zoomreset(GUI_HANDLE_p h);
uint8_t gui_graph_zoom(GUI_HANDLE_p h, float zoom, float x, float y);
uint8_t gui_graph_attachdata(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data);
uint8_t gui_graph_detachdata(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data);

GUI_GRAPH_DATA_p gui_graph_data_create(GUI_GRAPH_TYPE_t type, size_t length);
uint8_t gui_graph_data_addvalue(GUI_GRAPH_DATA_p data, int16_t x, int16_t y);
uint8_t gui_graph_data_setcolor(GUI_GRAPH_DATA_p data, GUI_Color_t color);
 
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

#endif /* __GUI_GRAPH_H */

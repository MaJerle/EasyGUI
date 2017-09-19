/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI graph widget
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2017 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_GRAPH_H
#define GUI_GRAPH_H

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

/**
 * \brief           Create new graph widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for create procedure
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p gui_graph_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_GRAPH_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t gui_graph_setcolor(GUI_HANDLE_p h, GUI_GRAPH_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set minimal X value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal X value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setmaxx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t gui_graph_setminx(GUI_HANDLE_p h, float v);

/**
 * \brief           Set maximal X value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New maximal X value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setminx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t gui_graph_setmaxx(GUI_HANDLE_p h, float v);

/**
 * \brief           Set minimal Y value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal Y value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setminx, gui_graph_setmaxx, gui_graph_setmaxy
 */
uint8_t gui_graph_setminy(GUI_HANDLE_p h, float v);

/**
 * \brief           Set maximal Y value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New maximal Y value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setminx, gui_graph_setmaxx, gui_graph_setminy
 */
uint8_t gui_graph_setmaxy(GUI_HANDLE_p h, float v);

/**
 * \brief           Reset zoom of widget
 * \param[in,out]   h: Widget handle
 * \retval          1: Zoom was reseted
 * \retval          0: Zoom was not reseted
 */
uint8_t gui_graph_zoomreset(GUI_HANDLE_p h);

/**
 * \brief           Zoom widget display data
 * \param[in,out]   h: Widget handle
 * \param[in]       zoom: Zoom coeficient. Use 2.0f to double zoom, use 0.5 to unzoom 2 times, etc.
 * \param[in]       x: X coordinate on plot where zoom focus will apply. Valid value between 0 and 1 relative to width area. Use 0.5 to zoom to center
 * \param[in]       y: Y coordinate on plot where zoom focus will apply. Valid value between 0 and 1 relative to height area. Use 0.5 to zoom to center
 * \retval          1: Zoom was reseted
 * \retval          0: Zoom was not reseted
 */
uint8_t gui_graph_zoom(GUI_HANDLE_p h, float zoom, float x, float y);

/**
 * \brief           Attach new data object to graph widget
 * \param[in,out]   h: Graph widget handle
 * \param[in]       data: Data object handle
 * \retval          1: Attaching was successful
 * \retval          0: Attaching failed
 * \sa              gui_graph_detachdata
 */
uint8_t gui_graph_attachdata(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data);

/**
 * \brief           Detach existing data object from graph widget
 * \param[in,out]   h: Graph widget handle
 * \param[in]       data: Data object handle
 * \retval          1: Detaching was successful
 * \retval          0: Detaching failed
 * \sa              gui_graph_attachdata
 */
uint8_t gui_graph_detachdata(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data);

/**
 * \brief           Creates data object according to specific type
 * \note            Data type used in graph widget is 2-byte (short int)
 * 
 * \note            When \arg GUI_GRAPH_TYPE_XY is used, 2 * length * sizeof(short int) of bytes is allocated for X and Y value
 * \param[in]       type: Type of data. According to selected type different allocation size will occur
 * \param[in]       length: Number of points on plot.
 * \retval          > 0: \ref GUI_GRAPH_DATA_p object of created widget
 * \retval          0: Data creation failed
 */
GUI_GRAPH_DATA_p gui_graph_data_create(GUI_GRAPH_TYPE_t type, size_t length);

/**
 * \brief           Add new value to the end of data object
 * \param[in]       data: Data object handle
 * \param[in]       x: X position for point. Used only in case data type is \ref GUI_GRAPH_TYPE_XY, otherwise it is ignored
 * \param[in]       y: Y position for point. Always used no matter of data type
 * \retval          1: Value was added to data object ok
 * \retval          0: Value was not added to data object
 */
uint8_t gui_graph_data_addvalue(GUI_GRAPH_DATA_p data, int16_t x, int16_t y);
 
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

/**
 * \author  Tilen Majerle
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
 * \brief       
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_GRAPH Graph
 * \brief           Graph which can display plots
 * \{
 */
    
/**
 * \brief           Graph color list enumeration
 * \sa              GUI_GRAPH_SetColor
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
    GUI_GRAPH_TYPE_YT = 0x00,               /*!< Data type is Y value version time */
} GUI_GRAPH_TYPE_t;

/**
 * \brief           Graph data widget structure
 */
typedef struct GUI_GRAPH_DATA_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    GUI_HANDLE_t Graph;                     /*!< Pointer to graph object */
    uint16_t* Data;                         /*!< Pointer to actual data object */
    uint16_t Length;                        /*!< Size of data array */
    GUI_Color_t Color;                      /*!< Curve color */
    GUI_GRAPH_TYPE_t Type;                  /*!< Plot data type */
} GUI_GRAPH_DATA_t;

/**
 * \brief           Graph widget structure
 */
typedef struct GUI_GRAPH_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    GUI_LinkedListRoot_t Root;              /*!< Linked list root object for data objects */
    
    GUI_Color_t Color[4];                   /*!< List of colors for widget */
    GUI_Dim_t Border[4];                    /*!< Borders for widgets */
    uint8_t Rows;                           /*!< Number of rows in plot represented with lines */
    uint8_t Columns;                        /*!< Number of columns in plot represented with lines */
    int16_t MinX;                           /*!< Minimal X value for plot */
    int16_t MaxX;                           /*!< Maximal X value for plot */
    int16_t MinY;                           /*!< Minimal Y value for plot */
    int16_t MaxY;                           /*!< Maximal Y value for plot */
} GUI_GRAPH_t;


/**
 * \brief           Create new graph widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \retval          > 0: \ref GUI_HANDLE_t object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_t GUI_GRAPH_Create(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_GRAPH_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          Widget handle
 */
GUI_HANDLE_t GUI_GRAPH_SetColor(GUI_HANDLE_t h, GUI_GRAPH_COLOR_t index, GUI_Color_t color);
GUI_HANDLE_t GUI_GRAPH_AttachData(GUI_HANDLE_t h, GUI_HANDLE_t hd);

GUI_HANDLE_t GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_t type, uint16_t Length);
GUI_HANDLE_t GUI_GRAPH_DATA_AddValue(GUI_HANDLE_t h, uint16_t val);
 
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

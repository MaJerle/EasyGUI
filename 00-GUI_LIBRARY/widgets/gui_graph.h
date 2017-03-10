/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \brief   GUI graph widget
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

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
 * \defgroup        GUI_GRAPH
 */

/**
 * \defgroup        GUI_GRAPH_Macros
 * \brief           Library defines
 * \{
 */
    


/**
 * \} GUI_GRAPH_Macros
 */
 
/**
 * \defgroup        GUI_GRAPH_Typedefs
 * \brief           Library Typedefs
 * \{
 */

typedef enum GUI_GRAPH_COLOR_t {
    GUI_GRAPH_COLOR_BG = 0x00,
    GUI_GRAPH_COLOR_FG = 0x01,
    GUI_GRAPH_COLOR_BORDER = 0x02,
    GUI_GRAPH_COLOR_GRID = 0x03
} GUI_GRAPH_COLOR_t;

typedef enum GUI_GRAPH_BORDER_t {
    GUI_GRAPH_BORDER_TOP = 0x00,
    GUI_GRAPH_BORDER_RIGHT = 0x01,
    GUI_GRAPH_BORDER_BOTTOM = 0x02,
    GUI_GRAPH_BORDER_LEFT = 0x03
} GUI_GRAPH_BORDER_t;

typedef enum GUI_GRAPH_TYPE_t {
    GUI_GRAPH_TYPE_YT = 0x00,               /*!< Data type is Y value version time */
} GUI_GRAPH_TYPE_t;

typedef struct GUI_GRAPH_DATA_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    uint16_t* Data;                         /*!< Pointer to actual data object */
    uint16_t Length;                        /*!< Size of data array */
    GUI_Color_t Color;                      /*!< Curve color */
    GUI_GRAPH_TYPE_t Type;                  /*!< Plot data type */
} GUI_GRAPH_DATA_t;

typedef struct GUI_GRAPH_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    GUI_Color_t Color[4];                   /*!< List of colors for widget */
    GUI_Dim_t Border[4];                    /*!< Borders for widgets */
    uint8_t Rows;                           /*!< Number of vertical lines for plot */
    uint8_t Columns;                        /*!< Number of vertical lines for plot */
} GUI_GRAPH_t;

/**
 * \} GUI_GRAPH_Typedefs
 */

/**
 * \defgroup        GUI_GRAPH_Functions
 * \brief           Library Functions
 * \{
 */

GUI_HANDLE_t GUI_GRAPH_Create(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height);
void GUI_GRAPH_Remove(GUI_HANDLE_t* h);

GUI_HANDLE_t GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_t type, uint16_t Length);
void GUI_GRAPH_DATA_Remove(GUI_HANDLE_t* h);

/**
 * \} GUI_GRAPH_Functions
 */
 
/**
 * \} GUI_GRAPH
 */

/**
 * \} GUI_WIDGETS
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

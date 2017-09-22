/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI container widget
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
#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

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
 * \defgroup        GUI_CONTAINER Container
 * \brief           Container widget holding widgets
 * \{
 *
 * Container is basic widget suitable for custom children widgets.
 *
 * It is similar to \ref GUI_WINDOW widget except it does not implement any controls like hide, maximize and minimize
 */

/**
 * \brief           Container colors
 */    
typedef enum {
    GUI_CONTAINER_COLOR_BG = 0x00,          /*!< Background color index */
} GUI_CONTAINER_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Container object structure
 */
typedef struct GUI_CONTAINER_t {
    GUI_HANDLE_ROOT_t C;                    /*!< GUI handle object, must always be first on list */
} GUI_CONTAINER_t;

#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

GUI_HANDLE_p gui_container_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_container_setcolor(GUI_HANDLE_p h, GUI_CONTAINER_COLOR_t index, GUI_Color_t color);

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

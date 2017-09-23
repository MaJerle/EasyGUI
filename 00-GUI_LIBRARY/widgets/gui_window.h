/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI window object manager
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
#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

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
 * \defgroup        GUI_WINDOW Window
 * \brief           Basic widget and core parent to all other widgets
 * \{
 *
 * Window is the Win98 styled widget which can support children widgets.
 *
 * \image html image_widget_window.png Default window widget
 *
 * Example code of image above:
 * \code{c}
handle = gui_window_create(0, 40, 20, 300, 200, gui_window_getdesktop(), 0, 0);
gui_widget_setexpanded(handle, 1);
gui_widget_settext(handle, _GT("Window"));
\endcode
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

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Window object structure
 */
typedef struct GUI_WINDOW_t {
    GUI_HANDLE_ROOT_t C;                    /*!< GUI handle object, must always be first on list */
    
    uint32_t Number;                        /*!< Window number */
    
    GUI_Dim_t BorderRadius;                 /*!< Radius in units of pixels for children windows */
    GUI_Dim_t BorderWidth;                  /*!< Border width */
} GUI_WINDOW_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */
  
GUI_HANDLE_p gui_window_getdesktop(void);
uint8_t gui_window_setactive(GUI_HANDLE_p h);
GUI_HANDLE_p gui_window_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_window_setcolor(GUI_HANDLE_p h, GUI_WINDOW_COLOR_t index, GUI_Color_t color);

#if defined(GUI_INTERNAL)
GUI_HANDLE_p gui_window_createdesktop(GUI_ID_t id, GUI_WIDGET_CALLBACK_t cb);
#endif /* defined(GUI_INTERNAL) */
 
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

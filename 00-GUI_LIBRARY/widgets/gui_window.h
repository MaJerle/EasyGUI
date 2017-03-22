/**
 * \author  Tilen Majerle
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
 */
    
/**
 * \brief           Get desktop window
 * \hideinitializer
 */
#define GUI_WINDOW_GetDesktop()     (GUI_HANDLE_t)GUI.Root.First
    
/**
 * \brief           Set active window for future widgets and for current top window
 * \hideinitializer
 */
#define GUI_WINDOW_SetActive(p)   do {      \
    GUI.WindowActive = __GH(p);             \
} while (0)

/**
 * \brief           Get parent window object
 * \hideinitializer
 */
#define GUI_WINDOW_GetParent(p)     ((GUI_HANDLE_t)(p))->Parent

/**
 * \brief           Check if window is child
 * \hideinitializer
 */
#define GUI_WINDOW_IsChild(p)       (((GUI_HANDLE_t)(p))->Flags & GUI_FLAG_CHILD)
    
/**
 * \brief           Window color list enumeration
 */
typedef enum GUI_WINDOW_COLOR_t {
    GUI_WINDOW_COLOR_BG = 0x00,             /*!< Background color index */
} GUI_WINDOW_COLOR_t;

/**
 * \brief           Window object structure
 */
typedef struct GUI_WINDOW_t {
    GUI_HANDLE_ROOT_t C;                    /*!< GUI handle object, must always be first on list */
    
    uint32_t Number;                        /*!< Window number */
    
    GUI_Color_t Color[1];                   /*!< List of colors */
    
    GUI_Dim_t BorderRadius;                 /*!< Radius in units of pixels for children windows */
    GUI_Dim_t BorderWidth;                  /*!< Border width */
} GUI_WINDOW_t;

/**
 * \brief           Create new window widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \retval          > 0: \ref GUI_HANDLE_t object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_t GUI_WINDOW_Create(GUI_ID_t id);

/**
 * \brief           Create new child window widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \retval          > 0: \ref GUI_HANDLE_t object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_t GUI_WINDOW_CreateChild(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_WINDOW_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          Widget handle
 */
GUI_HANDLE_t GUI_WINDOW_SetColor(GUI_HANDLE_t h, GUI_WINDOW_COLOR_t index, GUI_Color_t color);
GUI_HANDLE_t GUI_WINDOW_SetBorderWidth(GUI_HANDLE_t h, GUI_Dim_t color);
GUI_HANDLE_t GUI_WINDOW_SetBorderRadius(GUI_HANDLE_t h, GUI_Dim_t radius);
 
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

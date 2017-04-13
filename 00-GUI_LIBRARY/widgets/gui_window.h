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
    
    GUI_TIMER_t* Timer;                     /*!< Pointer to timer structure */
    
    uint32_t Number;                        /*!< Window number */
    
    GUI_Dim_t BorderRadius;                 /*!< Radius in units of pixels for children windows */
    GUI_Dim_t BorderWidth;                  /*!< Border width */
} GUI_WINDOW_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */
    
/**
 * \brief           Get desktop window
 * \retval          Widget handle of desktop window
 */
GUI_HANDLE_p GUI_WINDOW_GetDesktop(void);
    
/**
 * \brief           Set active window for future widgets and for current top window
 * \param[in]       h: Widget handle to set as active window
 * \retval          1: Widget was set ok
 * \retval          0: Widget was not set
 */
uint8_t GUI_WINDOW_SetActive(GUI_HANDLE_p h);

/**
 * \brief           Create new base widget window with desktop window as a parent
 * \param           id: Widget unique ID to use for identity for callback processing
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p GUI_WINDOW_Create(GUI_ID_t id);

/**
 * \brief           Create new child window widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p GUI_WINDOW_CreateChild(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_WINDOW_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t GUI_WINDOW_SetColor(GUI_HANDLE_p h, GUI_WINDOW_COLOR_t index, GUI_Color_t color);
 
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

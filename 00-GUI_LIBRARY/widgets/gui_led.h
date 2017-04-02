/**
 * \author  Tilen Majerle
 * \brief   GUI led widget
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
#ifndef GUI_LED_H
#define GUI_LED_H

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
 * \defgroup        GUI_LED LED
 * \brief           Simple LED to indicate active or disabled status
 * \{
 */

#define GUI_LED_FLAG_ON             0x01    /*!< Indicates LED is on */

/**
 * \brief           List of available colors for LED
 */
typedef enum GUI_LED_COLOR_t {
    GUI_LED_COLOR_ON = 0x00,                /*!< Color index for LED on status */
    GUI_LED_COLOR_OFF = 0x01,               /*!< Color index for LED off status */
    GUI_LED_COLOR_ON_BORDER = 0x02,         /*!< Color index for LED border on status */
    GUI_LED_COLOR_OFF_BORDER = 0x03         /*!< Color index for LED border off status */
} GUI_LED_COLOR_t;

/**
 * \brief           List of available LED types
 */
typedef enum GUI_LED_TYPE_t {
    GUI_LED_TYPE_RECT = 0x00,               /*!< Led is rectangular */
    GUI_LED_TYPE_CIRCLE = 0x01              /*!< Led is circle */
} GUI_LED_TYPE_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           LED object structure
 */
typedef struct GUI_LED_t {
    GUI_HANDLE C;                           /*!< Global widget object */
    GUI_Color_t Color[4];                   /*!< List of colors for LED */
    GUI_LED_TYPE_t Type;                    /*!< Led shape type */
    GUI_Byte Flags;                         /*!< Flags management for LED */
} GUI_LED_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new led widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p GUI_LED_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_LED_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          Widget handle
 */
GUI_HANDLE_p GUI_LED_SetColor(GUI_HANDLE_p h, GUI_LED_COLOR_t index, GUI_Color_t color);
GUI_HANDLE_p GUI_LED_SetType(GUI_HANDLE_p h, GUI_LED_TYPE_t index);

GUI_HANDLE_p GUI_LED_Off(GUI_HANDLE_p h);
GUI_HANDLE_p GUI_LED_On(GUI_HANDLE_p h);
GUI_HANDLE_p GUI_LED_Toggle(GUI_HANDLE_p h);
GUI_HANDLE_p GUI_LED_Set(GUI_HANDLE_p h, uint8_t state);

uint8_t GUI_LED_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

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

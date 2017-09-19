/**
 * \author  Tilen Majerle <tilen@majerle.eu>
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
 * \defgroup        GUI_LED Led
 * \brief           Simple LED
 * \{
 *
 * Led widget is very simple widget used for indication purposes.
 * It can only have <b>ON</b> or <b>OFF</b> status.
 *
 * Widget is supported in different shapes, defined by \ref GUI_LED_TYPE_t enumeration.
 *
 * \image html image_widget_led.png LED widgets from left: 1. circle ON, circle OFF, rectangle ON, rectangle OFF
 *
 * Example code of image above:
 * \code{c}
handle = gui_led_create(0, 10, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_CIRCLE);
gui_led_set(handle, 1);

handle = gui_led_create(0, 50, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_CIRCLE);

handle = gui_led_create(0, 90, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_RECT);
gui_led_set(handle, 1);

handle = gui_led_create(0, 130, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_RECT);
\endcode
 */

/**
 * \brief           List of available colors for LED
 */
typedef enum GUI_LED_COLOR_t {
    GUI_LED_COLOR_ON = 0x00,                /*!< Color index for LED on status */
    GUI_LED_COLOR_OFF,                      /*!< Color index for LED off status */
    GUI_LED_COLOR_ON_BORDER,                /*!< Color index for LED border on status */
    GUI_LED_COLOR_OFF_BORDER                /*!< Color index for LED border off status */
} GUI_LED_COLOR_t;

/**
 * \brief           List of available LED types
 */
typedef enum GUI_LED_TYPE_t {
    GUI_LED_TYPE_RECT = 0x00,               /*!< Led is rectangular */
    GUI_LED_TYPE_CIRCLE = 0x01              /*!< Led is circle */
} GUI_LED_TYPE_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)

#define GUI_LED_FLAG_ON             0x01    /*!< Indicates LED is on */

/**
 * \brief           LED object structure
 */
typedef struct GUI_LED_t {
    GUI_HANDLE C;                           /*!< Global widget object */
    
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
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p gui_led_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_LED_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t gui_led_setcolor(GUI_HANDLE_p h, GUI_LED_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set led type
 * \param[in,out]   h: Widget handle
 * \param[in]       type: New type for LED. This parameter can be a value of \ref GUI_LED_TYPE_t enumeration
 * \retval          1: Type was set ok
 * \retval          0: Type was not set
 */
uint8_t gui_led_settype(GUI_HANDLE_p h, GUI_LED_TYPE_t type);

/**
 * \brief           Toggle LED state
 * \param[in,out]   h: Widget handle
 * \retval          1: Led was toggled ok
 * \retval          0: Led was not toggled
 */
uint8_t gui_led_toggle(GUI_HANDLE_p h);

/**
 * \brief           Set LED value either on (turn on) or off (turn off)
 * \param[in,out]   h: Widget handle
 * \param[in]       state: LED state, either 1 or 0 according to selected state
 * \retval          1: Led state was set ok
 * \retval          0: Led state was not set
 */
uint8_t gui_led_set(GUI_HANDLE_p h, uint8_t state);

/**
 * \brief           Check if led is ON
 * \param[in,out]   h: Widget handle
 * \retval          1: Led is ON
 * \retval          0: Led is OFF
 */
uint8_t gui_led_ison(GUI_HANDLE_p h);

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

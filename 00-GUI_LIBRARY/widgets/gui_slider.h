/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI slider
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
#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

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
 * \defgroup        GUI_SLIDER Slider
 * \brief           Simple slider widget for user interaction
 * \{
 *
 * Simple slider widget to select range between 2 (min, max) values.
 *
 * \image html image_widget_slider.png Slider widget in different states. On top is passive state (touch not active) and bottom is when touch is active
 *
 * Example code for slider widget
 * \code{c}
//Create basic slider
handle = GUI_SLIDER_Create(0, 10, 10, 300, 50, 0, 0, 0);
\endcode
 */

/**
 * \brief           List of slider colors
 */
typedef enum GUI_SLIDER_COLOR_t {
    GUI_SLIDER_COLOR_BG_NONACTIVE = 0x00,   /*!< Background color index on slider part of non-active area */
    GUI_SLIDER_COLOR_BG_ACTIVE,             /*!< Background color index on slider part of active area */
    GUI_SLIDER_COLOR_FG,                    /*!< Foreground color index for foreground (circle, ...) part */
    GUI_SLIDER_COLOR_BORDER,                /*!< Border color index for bottom part */
} GUI_SLIDER_COLOR_t;

/**
 * \brief           List of slider modes
 */
typedef enum GUI_SLIDER_MODE_t {
    GUI_SLIDER_MODE_LEFT_RIGHT = 0x00,      /*!< Slider is horizontal, min on the left, max on the right */
    GUI_SLIDER_MODE_RIGHT_LEFT,             /*!< Slider is horizontal, min on the right, max on the left */
    GUI_SLIDER_MODE_BOTTOM_TOP,             /*!< Slider is horizontal, min on the bottom, max on the top */
    GUI_SLIDER_MODE_TOP_BOTTOM,             /*!< Slider is horizontal, min on the top, max on the bottom */
} GUI_SLIDER_MODE_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
/**
 * \brief           SLIDER object structure
 */
typedef struct GUI_SLIDER_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int32_t Min;                            /*!< Low value for slider */
    int32_t Max;                            /*!< High value for slider */
    int32_t Value;                          /*!< Current value for slider */
    GUI_SLIDER_MODE_t Mode;                 /*!< Slider mode */
    
    uint8_t MaxSize;                        /*!< Max size for animation */
    uint8_t CurrentSize;                    /*!< Current size for animation */
    
    uint8_t Flags;                          /*!< Widget flags */
} GUI_SLIDER_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new slider widget
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
GUI_HANDLE_p GUI_SLIDER_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_SLIDER_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t GUI_SLIDER_SetColor(GUI_HANDLE_p h, GUI_SLIDER_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set slider mode (orientation)
 * \param[in]       h: Widget handle
 * \param[in]       mode: Slider mode. This parameter can be a value of \ref GUI_SLIDER_MODE_t enumeration
 * \retval          1: Mode was set ok
 * \retval          0: Mode was not set
 */
uint8_t GUI_SLIDER_SetMode(GUI_HANDLE_p h, GUI_SLIDER_MODE_t mode);

/**
 * \brief           Set slider minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New minimal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              GUI_SLIDER_SetValue, GUI_SLIDER_SetMax, GUI_SLIDER_GetValue, GUI_SLIDER_GetMin, GUI_SLIDER_GetMax         
 */
uint8_t GUI_SLIDER_SetMin(GUI_HANDLE_p h, int32_t val);

/**
 * \brief           Set slider maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New maximal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              GUI_SLIDER_SetMin, GUI_SLIDER_SetValue, GUI_SLIDER_GetValue, GUI_SLIDER_GetMin, GUI_SLIDER_GetMax  
 */
uint8_t GUI_SLIDER_SetMax(GUI_HANDLE_p h, int32_t val);

/**
 * \brief           Set slider current value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New current value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              GUI_SLIDER_SetMin, GUI_SLIDER_SetMax, GUI_SLIDER_GetValue, GUI_SLIDER_GetMin, GUI_SLIDER_GetMax  
 */
uint8_t GUI_SLIDER_SetValue(GUI_HANDLE_p h, int32_t val);

/**
 * \brief           Get slider minimal value
 * \param[in,out]   h: Widget handle
 * \retval          Minimal value
 * \sa              GUI_SLIDER_SetMin, GUI_SLIDER_SetValue, GUI_SLIDER_SetMax, GUI_SLIDER_GetValue, GUI_SLIDER_GetMax  
 */
int32_t GUI_SLIDER_GetMin(GUI_HANDLE_p h);

/**
 * \brief           Get slider maximal value
 * \param[in,out]   h: Widget handle
 * \retval          Maximal value
 * \sa              GUI_SLIDER_SetMin, GUI_SLIDER_SetValue, GUI_SLIDER_SetMax, GUI_SLIDER_GetValue, GUI_SLIDER_GetMin  
 */
int32_t GUI_SLIDER_GetMax(GUI_HANDLE_p h);

/**
 * \brief           Get slider current value
 * \param[in,out]   h: Widget handle
 * \retval          Current value
 * \sa              GUI_SLIDER_SetMin, GUI_SLIDER_SetValue, GUI_SLIDER_SetMax, GUI_SLIDER_GetMin, GUI_SLIDER_GetMax  
 */
int32_t GUI_SLIDER_GetValue(GUI_HANDLE_p h);

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

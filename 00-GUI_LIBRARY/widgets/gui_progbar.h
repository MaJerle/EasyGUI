/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI progress bar widget
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
#ifndef GUI_PROGBAR_H
#define GUI_PROGBAR_H

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
 * \defgroup        GUI_PROGBAR Progress bar
 * \brief           Progress bar for progress visualization
 * \{
 *
 * Progress bar is used to show single progress value according to minimal, maximal and current values set to widget.
 *
 * \image html image_widget_progbar.png Progress bars
 *
 * Example code of image above:
 * \code{c}
handle = GUI_PROGBAR_Create(2, 10, 10, 400, 40, h, 0, 0);
GUI_WIDGET_SetText(handle, _GT("Progress bar with custom text"));

handle = GUI_PROGBAR_Create(2, 10, 60, 400, 40, h, 0, 0);
GUI_PROGBAR_EnablePercentages(handle);
GUI_PROGBAR_SetMin(handle, 20);
GUI_PROGBAR_SetMax(handle, 30);
GUI_PROGBAR_SetValue(handle, 22);    
\endcode
 */

/**
 * \brief           Progress bar color list enumeration
 */
typedef enum GUI_PROGBAR_COLOR_t {
    GUI_PROGBAR_COLOR_BG = 0x00,            /*!< Background color index */
    GUI_PROGBAR_COLOR_FG = 0x01,            /*!< Foreground (active part) color index */
    GUI_PROGBAR_COLOR_BORDER = 0x02         /*!< Border color index */
} GUI_PROGBAR_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Progress bar widget structure
 */
typedef struct GUI_PROGBAR_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    int32_t Min;                            /*!< Low value for progress bar */
    int32_t Max;                            /*!< High value for progress bar */
    int32_t CurrentValue;                   /*!< Current value for progress bar */
    int32_t DesiredValue;                   /*!< Desired value, set by used */
    uint8_t Flags;                          /*!< Flags variable */
} GUI_PROGBAR_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new progress bar widget
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
GUI_HANDLE_p GUI_PROGBAR_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_PROGBAR_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t GUI_PROGBAR_SetColor(GUI_HANDLE_p h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New minimal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              GUI_PROGBAR_SetValue, GUI_PROGBAR_SetMax, GUI_PROGBAR_GetValue, GUI_PROGBAR_GetMin, GUI_PROGBAR_GetMax         
 */
uint8_t GUI_PROGBAR_SetMin(GUI_HANDLE_p h, int32_t val);

/**
 * \brief           Set progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New maximal value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              GUI_PROGBAR_SetMin, GUI_PROGBAR_SetValue, GUI_PROGBAR_GetValue, GUI_PROGBAR_GetMin, GUI_PROGBAR_GetMax  
 */
uint8_t GUI_PROGBAR_SetMax(GUI_HANDLE_p h, int32_t val);

/**
 * \brief           Set progress bar current value
 * \param[in,out]   h: Widget handle
 * \param[in]       val: New current value
 * \retval          1: Value was set ok
 * \retval          0: Value vas not set
 * \sa              GUI_PROGBAR_SetMin, GUI_PROGBAR_SetMax, GUI_PROGBAR_GetValue, GUI_PROGBAR_GetMin, GUI_PROGBAR_GetMax  
 */
uint8_t GUI_PROGBAR_SetValue(GUI_HANDLE_p h, int32_t val);

/**
 * \brief           Set progress bar to animation mode
 * \param[in,out]   h: Widget handle
 * \param[in]       anim: New animation value either 1 (enable) or 0 (disable)
 * \retval          1: Animation was set ok
 * \retval          0: Animation vas not set
 */
uint8_t GUI_PROGBAR_SetAnimation(GUI_HANDLE_p h, uint8_t anim);

/**
 * \brief           Get progress bar minimal value
 * \param[in,out]   h: Widget handle
 * \retval          Minimal value
 * \sa              GUI_PROGBAR_SetMin, GUI_PROGBAR_SetValue, GUI_PROGBAR_SetMax, GUI_PROGBAR_GetValue, GUI_PROGBAR_GetMax  
 */
int32_t GUI_PROGBAR_GetMin(GUI_HANDLE_p h);

/**
 * \brief           Get progress bar maximal value
 * \param[in,out]   h: Widget handle
 * \retval          Maximal value
 * \sa              GUI_PROGBAR_SetMin, GUI_PROGBAR_SetValue, GUI_PROGBAR_SetMax, GUI_PROGBAR_GetValue, GUI_PROGBAR_GetMin  
 */
int32_t GUI_PROGBAR_GetMax(GUI_HANDLE_p h);

/**
 * \brief           Get progress bar current value
 * \param[in,out]   h: Widget handle
 * \retval          Current value
 * \sa              GUI_PROGBAR_SetMin, GUI_PROGBAR_SetValue, GUI_PROGBAR_SetMax, GUI_PROGBAR_GetMin, GUI_PROGBAR_GetMax  
 */
int32_t GUI_PROGBAR_GetValue(GUI_HANDLE_p h);

/**
 * \brief           Sets percent mode. When in this mode, widget text is in percent according to current value between minimum and maximum
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value either to enable or disable mode
 * \retval          1: Percentage mode was disabled ok
 * \retval          0: Percange mode was not disabled
 */
uint8_t GUI_PROGBAR_SetPercentMode(GUI_HANDLE_p h, uint8_t enable);

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

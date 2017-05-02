/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI RADIO widget
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
#ifndef GUI_RADIO_H
#define GUI_RADIO_H

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
 * \defgroup        GUI_RADIO Radio
 * \brief           Radio box widget
 * \{
 */
    
/**
 * \brief           List of radio colors
 */
typedef enum {
    GUI_RADIO_COLOR_BG = 0x00,              /*!< Background color index */
    GUI_RADIO_COLOR_FG,                     /*!< Foreground color index */
    GUI_RADIO_COLOR_BORDER,                 /*!< Border color index */
    GUI_RADIO_COLOR_DISABLED_BG,            /*!< Background color index when disabled */
} GUI_RADIO_COLOR_t;
    
#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_RADIO_CHECKED              0x01    /*!< Indicates radio is currently checked */
#define GUI_FLAG_RADIO_DISABLED             0x02    /*!< Indicates radio is currently disabled */

/**
 * \brief           Radio object structure
 */
typedef struct GUI_RADIO_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    uint8_t GroupId;                        /*!< Group ID for radio box */
    uint32_t Value;                         /*!< Single radio value when selected */
    uint32_t SelectedValue;                 /*!< Currently selected value in radio group. 
                                                    All Widgets in the same group share the same value in this field all the time */
    uint8_t Flags;                          /*!< Flags for checkbox */
} GUI_RADIO_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new radio widget
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
GUI_HANDLE_p GUI_RADIO_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_RADIO_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t GUI_RADIO_SetColor(GUI_HANDLE_p h, GUI_RADIO_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set radio group for widget
 * \note            Radio widgets with the same group must be on the same parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       groupId: Group ID for widget
 * \retval          1: Group set ok
 * \retval          0: Group was not set
 * \sa              GUI_RADIO_GetGroup
 */
uint8_t GUI_RADIO_SetGroup(GUI_HANDLE_p h, uint8_t groupId);

/**
 * \brief           Get radio group for widget
 * \param[in,out]   h: Widget handle
 * \retval          Widget group
 * \sa              GUI_RADIO_SetGroup
 */
uint8_t GUI_RADIO_GetGroup(GUI_HANDLE_p h);

/**
 * \brief           Set value for widget when pressed
 * \param[in,out]   h: Widget handle
 * \param[in]       value: Value of widget group when specific widget is selected
 * \retval          1: Value set ok
 * \retval          0: Value was not set
 * \sa              GUI_RADIO_GetValue
 */
uint8_t GUI_RADIO_SetValue(GUI_HANDLE_p h, uint32_t value);

/**
 * \brief           Get value for specific widget
 * \param[in,out]   h: Widget handle
 * \retval          Widget value
 * \sa              GUI_RADIO_SetValue
 */
uint32_t GUI_RADIO_GetValue(GUI_HANDLE_p h);

/**
 * \brief           Get value of selected widget from widget group
 * \note            If 3 widgets share the same group ID, no matter which widget is used in this function, result will be always the same
 * \param[in,out]   h: Widget handle
 * \retval          Widget selected value
 */
uint32_t GUI_RADIO_GetSelectedValue(GUI_HANDLE_p h);

/**
 * \brief           Set disabled status to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       disabled: Disabled status
 * \retval          1: Radio was set to desired disabled value
 * \retval          0: Radio was not set to desired disabled value
 */
uint8_t GUI_RADIO_SetDisabled(GUI_HANDLE_p h, uint8_t disabled);

/**
 * \brief           Check if Radio is disabled
 * \param[in,out]   h: Widget handle
 * \retval          1: Radio is disabled
 * \retval          0: Radio is not disabled
 */
uint8_t GUI_RADIO_IsDisabled(GUI_HANDLE_p h);
    
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

/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI Checkbox widget
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
#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

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
 * \defgroup        GUI_CHECKBOX Checkbox
 * \brief           Checkbox widget
 * \{
 */
    
/**
 * \brief           List of checkbox colors
 */
typedef enum {
    GUI_CHECKBOX_COLOR_BG = 0x00,           /*!< Background color index */
    GUI_CHECKBOX_COLOR_FG,                  /*!< Foreground color index */
    GUI_CHECKBOX_COLOR_BORDER,              /*!< Border color index */
    GUI_CHECKBOX_COLOR_DISABLED_BG,         /*!< Background color index when disabled */
} GUI_CHECKBOX_COLOR_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)

#define GUI_FLAG_CHECKBOX_CHECKED           0x01    /*!< Indicates checkbox is checked */
#define GUI_FLAG_CHECKBOX_DISABLED          0x02    /*!< Indicates checkbox is disabled */
    
/**
 * \brief           Checkbox object structure
 */
typedef struct GUI_CHECKBOX_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    uint8_t Flags;                          /*!< Widget flags */
} GUI_CHECKBOX_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new checkbox widget
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
GUI_HANDLE_p GUI_CHECKBOX_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_CHECKBOX_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t GUI_CHECKBOX_SetColor(GUI_HANDLE_p h, GUI_CHECKBOX_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set checked status to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       checked: Checked status
 * \retval          1: Checkbox was set to desired checked value
 * \retval          0: Checkbox was not set to desired checked value
 */
uint8_t GUI_CHECKBOX_SetChecked(GUI_HANDLE_p h, uint8_t checked);

/**
 * \brief           Check if checkbox is checked
 * \param[in,out]   h: Widget handle
 * \retval          1: Checkbox is checked
 * \retval          0: Checkbox is not checked
 */
uint8_t GUI_CHECKBOX_IsChecked(GUI_HANDLE_p h);

/**
 * \brief           Set disabled status to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       disabled: Disabled status
 * \retval          1: Checkbox was set to desired disabled value
 * \retval          0: Checkbox was not set to desired disabled value
 */
uint8_t GUI_CHECKBOX_SetDisabled(GUI_HANDLE_p h, uint8_t disabled);

/**
 * \brief           Check if checkbox is disabled
 * \param[in,out]   h: Widget handle
 * \retval          1: Checkbox is disabled
 * \retval          0: Checkbox is not disabled
 */
uint8_t GUI_CHECKBOX_IsDisabled(GUI_HANDLE_p h);

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

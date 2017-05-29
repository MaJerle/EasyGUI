/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI Widget handle manager
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
#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_WIDGETS Widgets
 * \brief           Group for visible widgets on screen
 * \{
 */
    
/**
 * \defgroup        GUI_WIDGETS_CORE Core widget functions
 * \brief           Core functions for all widgets
 *
 * Use can use all function which do not start with <b>__GUI</b> which indicate private functions.
 *
 * \{
 */
#include "gui.h"
#include "gui_draw.h"

/**
 * \defgroup        GUI_WIDGET_ID_Values Predefined ID values
 * \brief           Macros for fast ID setup
 * \{
 */  
    
#define GUI_ID_WINDOW_BASE          ((GUI_ID_t)0x0000)                          /*!< Window base ID */
#define GUI_ID_WINDOW_1             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0001))   /*!< Window object ID 1 */
#define GUI_ID_WINDOW_2             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0002))   /*!< Window object ID 2 */
#define GUI_ID_WINDOW_3             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0003))   /*!< Window object ID 3 */
#define GUI_ID_WINDOW_4             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0004))   /*!< Window object ID 4 */
#define GUI_ID_WINDOW_5             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0005))   /*!< Window object ID 5 */
#define GUI_ID_WINDOW_6             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0006))   /*!< Window object ID 6 */
#define GUI_ID_WINDOW_7             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0007))   /*!< Window object ID 7 */
#define GUI_ID_WINDOW_8             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0008))   /*!< Window object ID 8 */
#define GUI_ID_WINDOW_9             ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x0009))   /*!< Window object ID 9 */
#define GUI_ID_WINDOW_10            ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x000A))   /*!< Window object ID 10 */
    
#define GUI_ID_BUTTON_BASE          ((GUI_ID_t)0x0100)                          /*!< Button base ID */
#define GUI_ID_BUTTON_1             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0001))   /*!< Button object ID 1 */
#define GUI_ID_BUTTON_2             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0002))   /*!< Button object ID 2 */
#define GUI_ID_BUTTON_3             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0003))   /*!< Button object ID 3 */
#define GUI_ID_BUTTON_4             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0004))   /*!< Button object ID 4 */
#define GUI_ID_BUTTON_5             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0005))   /*!< Button object ID 5 */
#define GUI_ID_BUTTON_6             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0006))   /*!< Button object ID 6 */
#define GUI_ID_BUTTON_7             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0007))   /*!< Button object ID 7 */
#define GUI_ID_BUTTON_8             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0008))   /*!< Button object ID 8 */
#define GUI_ID_BUTTON_9             ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x0009))   /*!< Button object ID 9 */
#define GUI_ID_BUTTON_10            ((GUI_ID_t)(GUI_ID_BUTTON_BASE + 0x000A))   /*!< Button object ID 10 */

#define GUI_ID_USER                 ((GUI_ID_t)(0x10000))                       /*!< Start number of user based ID values for widgets */
    
/**
 * \}
 */
 
/**
 * \defgroup        WIDGET_CREATE_FLAGS Flags for widget create
 * \brief           A list of flags supported for widget creation
 *
 * List of flags used on \ref __GUI_WIDGET_Create function when creating new widget
 * 
 * \{
 */
#define GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP       0x01 /*!< Indicates parent object of widget should be desktop window */

/**
 * \}
 */

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \defgroup        WIDGET_Private Private functions
 * \brief           Private widget functions and enumerations.
 *
 * \note            Since they are not thread safe, they can only be used when inside GUI library.
 *                  It means they can be used in custom widget implementations where thread safety is guaranteed
 *
 * \{
 */
 
/**
 * \brief           Widget footprint value
 * \hideinitializer
 */
#define GUI_WIDGET_FOOTPRINT            0x00ACCE55
 
/**
 * \brief           Checks if inserted pointer is valid widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Input pointer to test for widget
 * \retval          1: Pointer is widget
 * \retval          0: Pointer is not widget
 * \hideinitializer
 */
#define __GUI_WIDGET_IsWidget(h)        ((h) && __GH(h)->Footprint == GUI_WIDGET_FOOTPRINT)

/**
 * \brief           Get absolute X position on LCD for specific widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          X position on LCD
 * \hideinitializer
 */
GUI_iDim_t __GUI_WIDGET_GetAbsoluteX(GUI_HANDLE_p h);

/**
 * \brief           Get absolute Y position on LCD for specific widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Y position on LCD
 * \hideinitializer
 */
GUI_iDim_t __GUI_WIDGET_GetAbsoluteY(GUI_HANDLE_p h);

/**
 * \brief           Get absolute inner X position of parent widget
 * \note            This function returns inner X position in absolute form.
 *                     Imagine parent absolute X is 10, and left padding is 2. Function returns 12.
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \retval          Parent absolute inner X position
 */
GUI_iDim_t __GUI_WIDGET_GetParentAbsoluteX(GUI_HANDLE_p h);

/**
 * \brief           Get absolute inner Y position of parent widget
 * \note            This function returns inner Y position in absolute form.
 *                     Imagine parent absolute Y is 10, and top padding is 2. Function returns 12.
 *
 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \retval          Parent absolute inner Y position
 */
GUI_iDim_t __GUI_WIDGET_GetParentAbsoluteY(GUI_HANDLE_p h);

/**
 * \brief           Get widget relative X position according to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Relative X according to parent widget
 * \hideinitializer
 */
#define __GUI_WIDGET_GetRelativeX(h)    (__GUI_WIDGET_IsExpanded(h) ? 0 : __GH(h)->X)

/**
 * \brief           Get widget relative Y position according to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Relative Y according to parent widget
 * \hideinitializer
 */
#define __GUI_WIDGET_GetRelativeY(h)    (__GUI_WIDGET_IsExpanded(h) ? 0 : __GH(h)->Y)

/**
 * \brief           Invalidate widget for redraw 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_Invalidate(GUI_HANDLE_p h);

/**
 * \brief           Invalidate widget and parent widget for redraw 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_InvalidateWithParent(GUI_HANDLE_p h);

/**
 * \brief           Set if parent widget should be invalidated when we invalidate primary widget
 * \note            Useful for widgets where there is no background: Transparent images, textview, slider, etc
 * \param[in]       h: Widget handle
 * \param[in]       value: Value either to enable or disable. 0 = disable, > 0 = enable
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 */
uint8_t __GUI_WIDGET_SetInvalidateWithParent(GUI_HANDLE_p h, uint8_t value);

/**
 * \brief           Set X and Y coordinates relative to parent object
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent
 * \param[in]       y: Y position relative to parent
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y);

/**
 * \brief           Set widget size
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi);

/**
 * \brief           Enable 3D mode on widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_Enable3DStyle(GUI_HANDLE_p h);

/**
 * \brief           Disable 3D mode on widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_Disable3DStyle(GUI_HANDLE_p h);

/**
 * \brief           Set font used for widget drawing
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       *font: Pointer to \ref GUI_FONT_t structure with font information
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_SetFont(GUI_HANDLE_p h, const GUI_FONT_t* font);

/**
 * \brief           Set text for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            When memory for text is dynamically allocated, text will be copied to allocated memory,
 *                     otherwise it will just set the pointer to new text.
 *                     Any changes on this text after function call will affect on later results
 *
 * \param[in,out]   h: Widget handle
 * \param[in]       *text: Pointer to text to set
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text);

/**
 * \brief           Allocate text memory for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \retval          1: Successful
 * \retval          0: Failed
 * \sa              __GUI_WIDGET_FreeTextMemory, GUI_WIDGET_AllocTextMemory, GUI_WIDGET_FreeTextMemory
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_AllocTextMemory(GUI_HANDLE_p h, uint32_t size);

/**
 * \brief           Free text memory for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \sa              __GUI_WIDGET_AllocTextMemory, GUI_WIDGET_AllocTextMemory, GUI_WIDGET_FreeTextMemory
 * \hideinitializer
 */
uint8_t __GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h);

/**
 * \brief           Create new widget and add it to linked list to parent object
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *widget: Pointer to \ref GUI_WIDGET_t structure with widget description
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Widget callback function. Set to NULL to use default widget specific callback
 * \param[in]       flags: Flags for create procedure
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 * \sa              __GUI_WIDGET_Remove
 */
GUI_HANDLE_p __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Remove widget and all of its children widgets
 *  
 *                  Function checks widget and all its children if they can be deleted. 
 *                  If so, flag for delete will be set and procedure will be executed later when all other processing is done
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param           *h: Widget handle to remove
 * \retval          0: Failed
 * \retval          1: Success
 * \sa              __GUI_WIDGET_Create
 */
uint8_t __GUI_WIDGET_Remove(GUI_HANDLE_p h);

/**
 * \brief           Show widget from visible area
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget shown
 * \retval          0: Widget not shown
 * \sa              __GUI_WIDGET_Hide
 */
uint8_t __GUI_WIDGET_Show(GUI_HANDLE_p h);

/**
 * \brief           Hide widget from visible area
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget hidden
 * \retval          0: Widget not hidden
 * \sa              __GUI_WIDGET_Show
 */
uint8_t __GUI_WIDGET_Hide(GUI_HANDLE_p h);

/**
 * \brief           Toggle expandend (maximized) mode of widget (mostly of windows)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget expandend status toggled ok
 * \retval          0: Widget expandend status was not toggled
 */
uint8_t __GUI_WIDGET_ToggleExpanded(GUI_HANDLE_p h);

/**
 * \brief           Set expandend mode on widget. When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \retval          1: Widget expandend status set ok
 * \retval          0: Widget expandend status was not set
 */
uint8_t __GUI_WIDGET_SetExpanded(GUI_HANDLE_p h, uint8_t state);

/**
 * \brief           Set width of widget in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent, GUI_WIDGET_SetHeightPercent
 */
uint8_t __GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width);

/**
 * \brief           Set height of widget in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in units of pixels
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetWidthPercent, GUI_WIDGET_SetHeightPercent
 */
uint8_t __GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height);

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetHeightPercent
 */
uint8_t __GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width);

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in percentage
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent
 */
uint8_t __GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height);

/**
 * \brief           Check if widget is children of parent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \retval          1: Widget handle is in tree of parent handle
 * \retval          0: Widget handle is not in tree of parent handle
 */
uint8_t __GUI_WIDGET_IsChildOf(GUI_HANDLE_p h, GUI_HANDLE_p parent);

/**
 * \brief           Check if widget has set font and text
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          1: Widget has font and text set
 * \retval          0: Widget does not have font or text set
 */
uint8_t __GUI_WIDGET_IsFontAndTextSet(GUI_HANDLE_p h);

/**
 * \brief           Process text key (add character, remove it, move cursor, etc)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       *key: Pointer to \ref __GUI_KeyboardData_t structure
 * \retval          1: Key processed
 * \retval          0: Key not processed
 */
uint8_t __GUI_WIDGET_ProcessTextKey(GUI_HANDLE_p h, __GUI_KeyboardData_t* key);

/**
 * \brief           Set color to widget specific index
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors
 * \param[in]       color: Actual color code to set
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t __GUI_WIDGET_SetColor(GUI_HANDLE_p h, uint8_t index, GUI_Color_t color);

/**
 * \brief           Get widget top padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define __GUI_WIDGET_GetPaddingTop(h)               (uint8_t)(((__GH(h)->Padding >> 24) & 0xFFUL))

/**
 * \brief           Get widget right padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define __GUI_WIDGET_GetPaddingRight(h)             (uint8_t)(((__GH(h)->Padding >> 16) & 0xFFUL))

/**
 * \brief           Get widget bottom padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define __GUI_WIDGET_GetPaddingBottom(h)            (uint8_t)(((__GH(h)->Padding >>  8) & 0xFFUL))

/**
 * \brief           Get widget left padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define __GUI_WIDGET_GetPaddingLeft(h)              (uint8_t)(((__GH(h)->Padding >>  0) & 0xFFUL))

#define __GUI_WIDGET_SetPaddingTop(h, x)            (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)(x)) << 24))
#define __GUI_WIDGET_SetPaddingRight(h, x)          (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)(x)) << 16))
#define __GUI_WIDGET_SetPaddingBottom(h, x)         (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)(x)) <<  8))
#define __GUI_WIDGET_SetPaddingLeft(h, x)           (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)(x)) <<  0))
#define __GUI_WIDGET_SetPaddingTopBottom(h, x)  do {    \
    __GUI_WIDGET_SetPaddingTop(h, x);                   \
    __GUI_WIDGET_SetPaddingBottom(h, x);                \
} while (0)
#define __GUI_WIDGET_SetPaddingLeftRight(h, x)  do {    \
    __GUI_WIDGET_SetPaddingLeft(h, x);                  \
    __GUI_WIDGET_SetPaddingRight(h, x);                 \
} while (0)
#define __GUI_WIDGET_SetPadding(h, x)  do {             \
    __GUI_WIDGET_SetPaddingTop(h, x);                   \
    __GUI_WIDGET_SetPaddingLeft(h, x);                  \
    __GUI_WIDGET_SetPaddingBottom(h, x);                \
    __GUI_WIDGET_SetPaddingRight(h, x);                 \
} while (0)

/**
 * \brief           Get widget ID
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Widget ID
 * \hideinitializer
 */
#define __GUI_WIDGET_GetId(h)               (__GH(h)->Id)

/**
 * \brief           Get widget flag
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag value to check
 * \retval          0: Flag is not set
 * \retval          > 0: Flag is set
 */
#define __GUI_WIDGET_GetFlag(h, flag)       (__GH(h)->Flags & (flag))

/**
 * \brief           Checks if widget is expanded to maximum relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is expandend
 * \retval          0: Widget is not expanded
 * \hideinitializer
 */
#define __GUI_WIDGET_IsExpanded(h)          (!!(__GH(h)->Flags & GUI_FLAG_EXPANDED))

/**
 * \brief           Get pointer to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to parent widget
 * \hideinitializer
 */
#define __GUI_WIDGET_GetParent(h)           (__GH(h)->Parent)

/**
 * \brief           Process widget callback with command, parameters and result pointers
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       cmd: Callback command. This parameter can be a value of \ref GUI_WC_t enumeration
 * \param[in]       param: Pointer to parameters if any for this command
 * \param[out]      result: Pointer to result pointer where calback can store result
 * \retval          1: Command processed by widget
 * \retval          0: Command was not processed by widget
 * \hideinitializer
 */
#define __GUI_WIDGET_Callback(h, cmd, param, result)    (__GH(h)->Callback ? __GH(h)->Callback(h, cmd, param, result) : __GH(h)->Widget->Callback(h, cmd, param, result))

/**
 * \brief           Get widget colors from list of colors
 *                  It takes colors from allocated memory if exists or from default widget setup for default
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index from color array for specific widget
 * \retval          Color index
 * \hideinitializer
 */
#define __GUI_WIDGET_GetColor(h, index)     (__GH(h)->Colors ? __GH(h)->Colors[(uint8_t)(index)] : (__GH(h)->Widget->Colors ? __GH(h)->Widget->Colors[(uint8_t)(index)] : GUI_COLOR_BLACK))

/**
 * \brief           Get total width of widget in units of pixels
 *                     Function returns width of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total width in units of pixels
 * \sa              __GUI_WIDGET_GetInnerWidth
 */
GUI_Dim_t __GUI_WIDGET_GetWidth(GUI_HANDLE_p h);

/**
 * \brief           Get total height of widget
 *                     Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total height in units of pixels
 * \sa              __GUI_WIDGET_GetInnerHeight
 * \hideinitializer
 */
GUI_Dim_t __GUI_WIDGET_GetHeight(GUI_HANDLE_p h);

/**
 * \brief           Get inner width (total width - padding left - padding right)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Height in units of pixels
 * \sa              __GUI_WIDGET_GetInnerHeight
 * \hideinitializer
 */
#define __GUI_WIDGET_GetInnerWidth(h)               (__GUI_WIDGET_GetWidth(h) - (__GUI_WIDGET_GetPaddingLeft(h) + __GUI_WIDGET_GetPaddingRight(h)))

/**
 * \brief           Get inner height (total height - padding top - padding bottom)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Inner height in units of pixels
 * \sa              __GUI_WIDGET_GetInnerWidth
 * \hideinitializer
 */
#define __GUI_WIDGET_GetInnerHeight(h)              (__GUI_WIDGET_GetHeight(h) - (__GUI_WIDGET_GetPaddingTop(h) + __GUI_WIDGET_GetPaddingBottom(h)))

/**
 * \brief           Returns width of parent element. If parent does not exists, it returns LCD width
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Parent width in units of pixels
 * \sa              __GUI_WIDGET_GetParentHeight
 * \hideinitializer
 */
#define __GUI_WIDGET_GetParentWidth(h)              (__GH(h)->Parent ? __GUI_WIDGET_GetWidth(__GH(h)->Parent) : GUI.LCD.Width)

/**
 * \brief           Returns height of parent element. If parent does not exists, it returns LCD height
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \sa              __GUI_WIDGET_GetParentWidth
 * \hideinitializer
 */
#define __GUI_WIDGET_GetParentHeight(h)             (__GH(h)->Parent ? __GUI_WIDGET_GetHeight(__GH(h)->Parent) : GUI.LCD.Height)

/**
 * \brief           Returns inner width of parent element. If parent does not exists, it returns LCD width
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            It returns inner width = total width - padding left - padding right
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Parent width in units of pixels
 * \sa              __GUI_WIDGET_GetParentInnerHeight
 * \hideinitializer
 */
#define __GUI_WIDGET_GetParentInnerWidth(h)         (__GH(h)->Parent ? __GUI_WIDGET_GetInnerWidth(__GH(h)->Parent) : GUI.LCD.Width)

/**
 * \brief           Returns inner height of parent element. If parent does not exists, it returns LCD height
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            It returns inner height = total height - padding top - padding bottom
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \sa              __GUI_WIDGET_GetParentInnerWidth
 * \hideinitializer
 */
#define __GUI_WIDGET_GetParentInnerHeight(h)        (__GH(h)->Parent ? __GUI_WIDGET_GetInnerHeight(__GH(h)->Parent) : GUI.LCD.Height)

/**
 * \brief           Check if widget is visible
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is visible
 * \retval          0: Widget is not visible
 * \sa              __GUI_WIDGET_IsHidden
 * \hideinitializer
 */
#define __GUI_WIDGET_IsVisible(h)                   (!(__GH(h)->Flags & GUI_FLAG_HIDDEN))

/**
 * \brief           Check if widget is hidden
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is hidden
 * \retval          0: Widget is not hidden
 * \sa              __GUI_WIDGET_IsVisible
 * \hideinitializer
 */
#define __GUI_WIDGET_IsHidden(h)                    (__GH(h)->Flags & GUI_FLAG_HIDDEN)

/**
 * \brief           Check if widget allows children widgets
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget allows children widgets
 * \retval          0: Widget does not allow children widgets
 * \hideinitializer
 */
#define __GUI_WIDGET_AllowChildren(h)               (__GH(h)->Widget->Flags & GUI_FLAG_WIDGET_ALLOW_CHILDREN)

/**
 * \brief           Check if widget is base for dialog
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is dialog base
 * \retval          0: Widget is not dialog
 * \hideinitializer
 */
#define __GUI_WIDGET_IsDialogBase(h)                (__GH(h)->Widget->Flags & GUI_FLAG_WIDGET_DIALOG_BASE)

/**
 * \brief           Checks if widget handle is currently in focus
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is in focus or not
 * \sa              __GUI_WIDGET_IsActive
 * \hideinitializer
 */
#define __GUI_WIDGET_IsFocused(h)                   (__GH(h)->Flags & GUI_FLAG_FOCUS)

/**
 * \brief           Checks if widget handle is currently active
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is active or not
 * \sa              __GUI_WIDGET_IsFocused
 * \hideinitializer
 */
#define __GUI_WIDGET_IsActive(h)                    (__GH(h)->Flags & GUI_FLAG_ACTIVE)

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \retval          1: New value is different than previous and modification has been done
 * \retval          0: New value was not set
 */
uint8_t __GUI_WIDGET_SetZIndex(GUI_HANDLE_p h, int32_t zindex);

/**
 * \brief           Get z-index value from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 * \hideinitializer
 */
#define __GUI_WIDGET_GetZIndex(h)                   (__GH(h)->ZIndex)

/**
 * \}
 */
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */


/**
 * \brief           Get widget ID
 * \param[in,out]   h: Widget handle
 * \retval          Widget ID
 */
GUI_ID_t GUI_WIDGET_GetId(GUI_HANDLE_p h);

/**
 * \brief           Get first widget handle by ID
 * \note            If multiple widgets have the same ID, first found will be used
 * \param[in,out]   id: Widget ID to search for
 * \retval          > 0: Widget handle when widget found
 * \retval          0: Widget not found
 */
GUI_HANDLE_p GUI_WIDGET_GetById(GUI_ID_t id);

/**
 * \brief           Remove widget from memory
 * \note            If widget has child widgets, they will be removed too
 * \param[in,out]   *h: Pointer to widget handle. If removed, pointer will be invalidated and set to NULL
 * \retval          1: Widget deleted
 * \retval          0: Delete failed
 */
uint8_t GUI_WIDGET_Remove(GUI_HANDLE_p* h);

/**
 * \brief           Allocate memory for text operations if text will be dynamic
 * \note            When unicode feature is enabled, memory should be 4x required characters because unicode can store up to 4 bytes for single character
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \retval          Number of bytes allocated
 * \sa              GUI_WIDGET_FreeTextMemory
 */
uint32_t GUI_WIDGET_AllocTextMemory(GUI_HANDLE_p h, uint32_t size);

/**
 * \brief           Frees memory previously allocated for text
 * \param[in,out]   h: Widget handle to free memory on
 * \retval          1: Free was ok
 * \retval          0: Free was not ok
 * \sa              GUI_WIDGET_AllocTextMemory
 */
uint8_t GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h);

/**
 * \brief           Set text to widget
 * \note            If dynamic memory allocation was used then content will be copied to allocated memory
 *                     otherwise only pointer to input text will be used 
 *                     and each further change of input pointer text will affect to output
 * \param[in,out]   h: Widget handle
 * \param[in]       *text: Pointer to text to set to widget
 * \retval          1: Text was set ok
 * \retval          0: Text was not set
 * \sa              GUI_WIDGET_AllocTextMemory, GUI_WIDGET_FreeTextMemory, GUI_WIDGET_SetFont, GUI_WIDGET_GetText, GUI_WIDGET_GetTextCopy
 */
uint8_t GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text);

/**
 * \brief           Get text from widget
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to text from widget
 * \sa              GUI_WIDGET_SetText, GUI_WIDGET_GetTextCopy
 */
const GUI_Char* GUI_WIDGET_GetText(GUI_HANDLE_p h);

/**
 * \brief           Get text from widget
 * \note            Text from widget is copied to input pointer
 * \param[in,out]   h: Widget handle
 * \param[out]      *dst: Destination pointer
 * \param[in]       len: Size of output buffer in units of \ref GUI_Char
 * \retval          Pointer to text from widget
 * \sa              GUI_WIDGET_SetText, GUI_WIDGET_GetText
 */
const GUI_Char* GUI_WIDGET_GetTextCopy(GUI_HANDLE_p h, GUI_Char* dst, uint32_t len);

/**
 * \brief           Set widget font for drawing operations
 * \param[in,out]   h: Widget handle
 * \param[in]       *font: Pointer to \ref GUI_FONT_t object for font
 * \retval          1: Font was set ok
 * \retval          0: Font was not set
 * \sa              GUI_WIDGET_SetText, GUI_WIDGET_GetText
 */
uint8_t GUI_WIDGET_SetFont(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font);

/**
 * \brief           Set widget size in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \param[in]       height: Height in units of pixels
 * \retval          1: Size was set ok
 * \retval          0: Size was not set
 * \sa              GUI_WIDGET_SetXY
 */
uint8_t GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \brief           Set width of widget in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \retval          1: Width was set ok
 * \retval          0: Width was not set
 * \sa              GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent, GUI_WIDGET_SetHeightPercent
 */
uint8_t GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width);

/**
 * \brief           Set height of widget in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in units of pixels
 * \retval          1: Height was set ok
 * \retval          0: Height was not set
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetWidthPercent, GUI_WIDGET_SetHeightPercent
 */
uint8_t GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height);

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \retval          1: Width was set ok
 * \retval          0: Width was not set
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetHeightPercent
 */
uint8_t GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width);

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in percentage
 * \retval          1: Height was set ok
 * \retval          0: Height was not set
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent
 */
uint8_t GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height);

/**
 * \brief           Get total width of widget effective on screen in units of pixels
 *                     Function returns width of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total width in units of pixels
 * \sa              GUI_WIDGET_GetHeight, GUI_WIDGET_SetWidth
 */
GUI_Dim_t GUI_WIDGET_GetWidth(GUI_HANDLE_p h);

/**
 * \brief           Get total height of widget effective on screen in units of pixels
 *                     Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total height in units of pixels
 * \sa              GUI_WIDGET_GetWidth, GUI_WIDGET_SetHeight
 * \hideinitializer
 */
GUI_Dim_t GUI_WIDGET_GetHeight(GUI_HANDLE_p h);

/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetSize
 */
uint8_t GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y);

/**
 * \brief           Show widget from visible area
 * \param[in,out]   h: Widget handle
 * \retval          1: Text was shown ok
 * \retval          0: Text was not shown
 * \sa              GUI_WIDGET_Hide
 */
uint8_t GUI_WIDGET_Show(GUI_HANDLE_p h);

/**
 * \brief           Hide widget from visible area
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was hidden ok
 * \retval          0: Widget was not hidden
 * \sa              GUI_WIDGET_Show, GUI_WIDGET_PutOnFront
 */
uint8_t GUI_WIDGET_Hide(GUI_HANDLE_p h);

/**
 * \brief           Put widget to front view and put it to focused state
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was put ok
 * \retval          0: Widget was not put
 * \sa              GUI_WIDGET_Hide
 */
uint8_t GUI_WIDGET_PutOnFront(GUI_HANDLE_p h);

/**
 * \brief           Invalidate widget object and prepare to new redraw
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was invalidated ok
 * \retval          0: Widget was not invalidated
 */
uint8_t GUI_WIDGET_Invalidate(GUI_HANDLE_p h);

/**
 * \brief           Set callback function to widget
 * \param[in,out]   h: Widget handle object
 * \param[in]       callback: Callback function for widget
 * \retval          1: Callback was set ok
 * \retval          1: Callback was not set
 * \sa              GUI_WIDGET_ProcessDefaultCallback, GUI_WIDGET_Callback
 */
uint8_t GUI_WIDGET_SetCallback(GUI_HANDLE_p h, GUI_WIDGET_CALLBACK_t callback);

/**
 * \brief           Set custom user data to widget
 * \note            Specially useful in callback processing if required
 * \param[in,out]   h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \retval          1: Data were set OK
 * \retval          0: Data were not set
 * \sa              GUI_WIDGET_GetUserData
 */
uint8_t GUI_WIDGET_SetUserData(GUI_HANDLE_p h, void* data);

/**
 * \brief           Get custom user data from widget previously set with \ref GUI_WIDGET_SetUserData
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to user data
 * \sa              GUI_WIDGET_SetUserData
 */
void* GUI_WIDGET_GetUserData(GUI_HANDLE_p h);

/**
 * \brief           Widget callback function for all events
 * \note            Called either from GUI stack or from widget itself to notify user
 *
 * \note            Call this function inside custom callback widget function for unhandled events
 *                     It will automatically call required function according to input widget
 * \param[in,out]   h: Widget handle where callback occurred
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref GUI_WC_t enumeration
 * \param[in]       *param: Pointer to optional input data for command. Check \ref GUI_WC_t enumeration for more informations
 * \param[out]      *result: Pointer to optional result value. Check \ref GUI_WC_t enumeration for more informations
 * \retval          1: Command has been processed
 * \retval          0: Command has not been processed
 */
uint8_t GUI_WIDGET_ProcessDefaultCallback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/**
 * \brief           Widget callback function for all events
 * \note            Called from user outside callback. For calling default callback
 *                      inside custom callback for widget, use \ref GUI_WIDGET_ProcessDefaultCallback instead.
 *                      If called from inside widget callback, it may result in recursive calls.
 *
 * \param[in,out]   h: Widget handle where callback occurred
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref GUI_WC_t enumeration
 * \param[in]       *param: Pointer to optional input data for command. Check \ref GUI_WC_t enumeration for more informations
 * \param[out]      *result: Pointer to optional result value. Check \ref GUI_WC_t enumeration for more informations
 * \retval          1: Command has been processed
 * \retval          0: Command has not been processed
 * \sa              GUI_WIDGET_SetCallback
 */
uint8_t GUI_WIDGET_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/**
 * \brief           Check if widget is children of parent
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \retval          1: Widget handle is in tree of parent handle
 * \retval          0: Widget handle is not in tree of parent handle
 */
uint8_t GUI_WIDGET_IsChildOf(GUI_HANDLE_p h, GUI_HANDLE_p parent);

/**
 * \brief           Increase selection for widget
 * \note            Widget must implement \ref GUI_WC_IncSelection command in callback function and process it
 * \param[in,out]   h: Widget handle
 * \param[in]       dir: Increase direction. Positive number means number of increases, negative is number of decreases
 * \retval          1: Increase/Decrease was ok
 * \retval          0: Increase/Decrease was not ok
 */
uint8_t GUI_WIDGET_IncSelection(GUI_HANDLE_p h, int16_t dir);

/**
 * \brief           Set widget scroll on X axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y possibility
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for X direction
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              GUI_WIDGET_SetScrollY
 */
uint8_t GUI_WIDGET_SetScrollX(GUI_HANDLE_p h, GUI_iDim_t scroll);

/**
 * \brief           Set widget scroll on Y axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y possibility
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for Y direction
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              GUI_WIDGET_SetScrollX
 */
uint8_t GUI_WIDGET_SetScrollY(GUI_HANDLE_p h, GUI_iDim_t scroll);

/**
 * \brief           Manually set widget in focus
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget set to focus
 * \retval          0: Widget was not set to focus
 */
uint8_t GUI_WIDGET_SetFocus(GUI_HANDLE_p h);

/**
 * \brief           Set default font for widgets used on widget creation
 * \param[in]       *font: Pointer to \ref GUI_FONT_t with font
 * \retval          1: Font was set ok
 * \retval          0: Font was not set
 */
uint8_t GUI_WIDGET_SetFontDefault(const GUI_FONT_t* font);

/**
 * \brief           Set expandend mode on widget. When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \retval          1: Widget expandend status set ok
 * \retval          0: Widget expandend status was not set
 * \sa              GUI_WIDGET_IsExpanded
 */
uint8_t GUI_WIDGET_SetExpanded(GUI_HANDLE_p h, uint8_t state);

/**
 * \brief           Get widget expanded status
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is expanded
 * \retval          0: Widget is not expanded
 * \sa              GUI_WIDGET_SetExpanded
 */
uint8_t GUI_WIDGET_IsExpanded(GUI_HANDLE_p h);

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \retval          1: New value is different than previous and modification has been done
 * \retval          0: New value was not set
 */
uint8_t GUI_WIDGET_SetZIndex(GUI_HANDLE_p h, int32_t zindex);

/**
 * \brief           Get z-index value from widget
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 */
uint32_t GUI_WIDGET_GetZIndex(GUI_HANDLE_p h);

#if defined(GUI_INTERNAL) && !defined(DOXYGEN)
//Strictly private functions by GUI
//Widget initialization purpose
void __GUI_WIDGET_Init(void);

//Clipping regions
void __GUI_WIDGET_SetClippingRegion(GUI_HANDLE_p h);
uint8_t __GUI_WIDGET_IsInsideClippingRegion(GUI_HANDLE_p h);
uint8_t __GUI_WIDGET_GetLCDAbsPosAndVisibleWidthHeight(GUI_HANDLE_p h, GUI_iDim_t* xOut, GUI_iDim_t* yOut, GUI_iDim_t* wOut, GUI_iDim_t* hOut);

//Move widget down and all its parents with it
void __GUI_WIDGET_MoveDownTree(GUI_HANDLE_p h);

void __GUI_WIDGET_FOCUS_CLEAR(void);
void __GUI_WIDGET_FOCUS_SET(GUI_HANDLE_p h);
void __GUI_WIDGET_ACTIVE_CLEAR(void);
void __GUI_WIDGET_ACTIVE_SET(GUI_HANDLE_p h);

//Execute actual widget remove process
uint8_t __GUI_WIDGET_ExecuteRemove(void);
#endif /* !defined(DOXYGEN) */

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

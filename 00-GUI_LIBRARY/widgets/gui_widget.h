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
 *
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
 *
 * \section sect_widgets Widget organization
 *
 * Widgets are organized in multi depth linked list of parent widgets where each widget has reference to:
 *  - Next widget
 *  - Previous widget
 *  - Parent widget
 *  - Children first widget
 *  - Children last widget
 *
 * \par Next widget link
 *
 * Next widget in linked list is used to get next widget for drawing operation
 * Widget which is at the end of linked list is later visible on top on LCD.
 *
 * Drawing operations starts with 
 *
 * \par Previous widget link
 *
 * Previous link is used to get widget which is "behind" current widget. 
 * This link is used for touch management where widget at the end of linked list is first checked for events (touch, mouse, etc)
 *
 * \par Parent widget
 * 
 * Link to parent widget is always required to calculate currect widget position on LCD and for other tasks (like percentage width relative to parent, etc)
 *
 * \par Children first widget
 *
 * Children links are supported on widgets which implement \ref GUI_HANDLE_ROOT_t structure to handle children widgets (such as windows).
 * It hold link to less important widget (drawed first, less visible in some cases)
 *
 * \par Children last widget
 *
 * Children last widget is used for events processing (touch, mouse, etc)
 *
 *
 * Any operation on widgets structure (change Z-index, put on front, etc) is only moving widgets on linked list.
 * Linked list is most important part of GUI library therefore it is designed in robust way.
 */
    
/**
 * \defgroup        GUI_WIDGETS_CORE Core widget functions
 * \brief           Core functions for all widgets
 * \{
 *
 * Use can use all function which do not start with <b>__GUI</b> which indicate private functions.
 * 
 * Core widget functions are common functions to all widgets. They can perform:
 *  - Set text to widget (Static text from non-volatile memory)
 *  - Preallocate memory for text on widget (dynamic text)
 *  - Set font type for drawing
 *  - Set dimentions and positions
 *  - Get dimensions and positions relative to parent or absolute on screen
 *  - Set or get Z index of widget
 *  - Set transparency
 *  - Set widget as visible or hidden
 *  - Delete widget
 *  - Manually widget invalidation
 *  - ..and more
 *
 * In general, each widget also uses custom based functions. These are provided in widget specific section.
 */
#include "gui.h"
#include "gui_draw.h"

/**
 * \defgroup        GUI_WIDGET_ID_Values Predefined ID values
 * \brief           Macros for fast ID setup
 * \{
 */  
    
#define GUI_ID_WINDOW_BASE          ((GUI_ID_t)0x0000)                          /*!< Window base ID */
#define GUI_ID_KEYBOARD_BASE        ((GUI_ID_t)(GUI_ID_WINDOW_BASE + 0x1000))   /*!< Keyboard based ID */

#define GUI_ID_USER                 ((GUI_ID_t)(0x10000))                       /*!< Start number of user based ID values for widgets */

#define GUI_WIDGET_ZINDEX_MAX       (int32_t)(0x7FFFFFFF)                       /*!< Maximal allowed z-index value */
#define GUI_WIDGET_ZINDEX_MIN       (int32_t)(0x80000000)                       /*!< Maximal allowed z-index value */
    
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
#define gui_widget_iswidget__(h)        ((h) && __GH(h)->Footprint == GUI_WIDGET_FOOTPRINT)

/**
 * \brief           Get absolute X position on LCD for specific widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          X position on LCD
 * \hideinitializer
 */
GUI_iDim_t gui_widget_getabsolutex__(GUI_HANDLE_p h);

/**
 * \brief           Get absolute Y position on LCD for specific widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Y position on LCD
 * \hideinitializer
 */
GUI_iDim_t gui_widget_getabsolutey__(GUI_HANDLE_p h);

/**
 * \brief           Get absolute inner X position of parent widget
 * \note            This function returns inner X position in absolute form.
 *                     Imagine parent absolute X is 10, and left padding is 2. Function returns 12.
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \retval          Parent absolute inner X position
 */
GUI_iDim_t gui_widget_getparentabsolutex__(GUI_HANDLE_p h);

/**
 * \brief           Get absolute inner Y position of parent widget
 * \note            This function returns inner Y position in absolute form.
 *                     Imagine parent absolute Y is 10, and top padding is 2. Function returns 12.
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \retval          Parent absolute inner Y position
 */
GUI_iDim_t gui_widget_getparentabsolutey__(GUI_HANDLE_p h);

/**
 * \brief           Get widget relative X position according to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Relative X according to parent widget
 * \hideinitializer
 */
#define gui_widget_getrelativex__(h)    (gui_widget_isexpanded__(h) ? 0 : \
                                            (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT) ? (GUI_iDim_t)((float)GUI_ROUND(__GH(h)->X * gui_widget_getparentinnerwidth__(h)) / 100.0f) : __GH(h)->X) \
                                        )

/**
 * \brief           Get widget relative Y position according to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Relative Y according to parent widget
 * \hideinitializer
 */
#define gui_widget_getrelativey__(h)    (gui_widget_isexpanded__(h) ? 0 : \
                                            (gui_widget_getflag__(h, GUI_FLAG_YPOS_PERCENT) ? (GUI_iDim_t)((float)GUI_ROUND(__GH(h)->Y * gui_widget_getparentinnerheight__(h)) / 100.0f) : __GH(h)->Y) \
                                        )

/**
 * \brief           Invalidate widget for redraw 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t gui_widget_invalidate__(GUI_HANDLE_p h);

/**
 * \brief           Invalidate widget and parent widget for redraw 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t gui_widget_invalidatewithparent__(GUI_HANDLE_p h);

/**
 * \brief           Set if parent widget should be invalidated when we invalidate primary widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \note            Useful for widgets where there is no background: Transparent images, textview, slider, etc
 * \param[in]       h: Widget handle
 * \param[in]       value: Value either to enable or disable. 0 = disable, > 0 = enable
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 */
uint8_t gui_widget_setinvalidatewithparent__(GUI_HANDLE_p h, uint8_t value);

/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetPositionPercent
 */
uint8_t gui_widget_setposition__(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y);
 
/**
 * \brief           Set widget position relative to parent object in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetPosition
 */
uint8_t gui_widget_setpositionpercent__(GUI_HANDLE_p h, float x, float y);
 
/**
 * \brief           Set widget X position relative to parent object in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetXPositionPercent
 */
uint8_t gui_widget_setxposition__(GUI_HANDLE_p h, GUI_iDim_t x);
 
/**
 * \brief           Set widget X position relative to parent object in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetXPosition
 */
uint8_t gui_widget_setxpositionpercent__(GUI_HANDLE_p h, float x);
 
/**
 * \brief           Set widget Y position relative to parent object in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetYPositionPercent
 */
uint8_t gui_widget_setyposition__(GUI_HANDLE_p h, GUI_iDim_t y);
 
/**
 * \brief           Set widget Y position relative to parent object in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetYPosition
 */
uint8_t gui_widget_setypositionpercent__(GUI_HANDLE_p h, float y);

/**
 * \brief           Set widget size in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t gui_widget_setsize__(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi);

/**
 * \brief           Set widget size in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t gui_widget_setsizepercent__(GUI_HANDLE_p h, float wi, float hi);

/**
 * \brief           Set width of widget in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent, GUI_WIDGET_SetHeightPercent
 */
uint8_t gui_widget_setwidth__(GUI_HANDLE_p h, GUI_Dim_t width);

/**
 * \brief           Set height of widget in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in units of pixels
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetWidthPercent, GUI_WIDGET_SetHeightPercent
 */
uint8_t gui_widget_setheight__(GUI_HANDLE_p h, GUI_Dim_t height);

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetHeightPercent
 */
uint8_t gui_widget_setwidthpercent__(GUI_HANDLE_p h, float width);

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in percentage
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent
 */
uint8_t gui_widget_setheightpercent__(GUI_HANDLE_p h, float height);

/**
 * \brief           Set 3D mode on widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value to enable, either 1 or 0
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t gui_widget_set3dstyle__(GUI_HANDLE_p h, uint8_t enable);

/**
 * \brief           Set font used for widget drawing
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       *font: Pointer to \ref GUI_FONT_t structure with font information
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t gui_widget_setfont__(GUI_HANDLE_p h, const GUI_FONT_t* font);

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
 */
uint8_t gui_widget_settext__(GUI_HANDLE_p h, const GUI_Char* text);

/**
 * \brief           Get text from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to text from widget
 */
const GUI_Char* gui_widget_gettext__(GUI_HANDLE_p h);

/**
 * \brief           Get font from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to font used for widget
 */
const GUI_FONT_t* gui_widget_getfont__(GUI_HANDLE_p h);

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
uint8_t gui_widget_alloctextmemory__(GUI_HANDLE_p h, uint32_t size);

/**
 * \brief           Free text memory for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \sa              __GUI_WIDGET_AllocTextMemory, GUI_WIDGET_AllocTextMemory, GUI_WIDGET_FreeTextMemory
 * \hideinitializer
 */
uint8_t gui_widget_freetextmemory__(GUI_HANDLE_p h);

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
 * \retval          > 0: Pointer to allocated memory for widget with default settings
 * \retval          0: Widget creation failed
 * \sa              __GUI_WIDGET_Remove
 */
void* gui_widget_create__(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

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
 * \sa              __GUI_WIDGET_Create, GUI_WIDGET_Remove
 */
uint8_t gui_widget_remove__(GUI_HANDLE_p h);

/**
 * \brief           Show widget from visible area
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget shown
 * \retval          0: Widget not shown
 * \sa              __GUI_WIDGET_Hide
 */
uint8_t gui_widget_show__(GUI_HANDLE_p h);

/**
 * \brief           Hide widget from visible area
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget hidden
 * \retval          0: Widget not hidden
 * \sa              __GUI_WIDGET_Show
 */
uint8_t gui_widget_hide__(GUI_HANDLE_p h);

/**
 * \brief           Toggle expandend (maximized) mode of widget (mostly of windows)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget expandend status toggled ok
 * \retval          0: Widget expandend status was not toggled
 */
uint8_t gui_widget_toggleexpanded__(GUI_HANDLE_p h);

/**
 * \brief           Set expandend mode on widget. When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \retval          1: Widget expandend status set ok
 * \retval          0: Widget expandend status was not set
 */
uint8_t gui_widget_setexpanded__(GUI_HANDLE_p h, uint8_t state);

/**
 * \brief           Check if widget is children of parent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \retval          1: Widget handle is in tree of parent handle
 * \retval          0: Widget handle is not in tree of parent handle
 */
uint8_t gui_widget_ischildof__(GUI_HANDLE_p h, GUI_HANDLE_p parent);

/**
 * \brief           Check if widget has set font and text
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          1: Widget has font and text set
 * \retval          0: Widget does not have font or text set
 */
uint8_t gui_widget_isfontandtextset__(GUI_HANDLE_p h);

/**
 * \brief           Process text key (add character, remove it, move cursor, etc)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       *key: Pointer to \ref __GUI_KeyboardData_t structure
 * \retval          1: Key processed
 * \retval          0: Key not processed
 */
uint8_t gui_widget_processtextkey__(GUI_HANDLE_p h, __GUI_KeyboardData_t* key);

/**
 * \brief           Set color to widget specific index
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors
 * \param[in]       color: Actual color code to set
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t gui_widget_setcolor__(GUI_HANDLE_p h, uint8_t index, GUI_Color_t color);

/**
 * \brief           Get widget top padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingtop__(h)               (uint8_t)(((__GH(h)->Padding >> 24) & 0xFFUL))

/**
 * \brief           Get widget right padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingright__(h)             (uint8_t)(((__GH(h)->Padding >> 16) & 0xFFUL))

/**
 * \brief           Get widget bottom padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingbottom__(h)            (uint8_t)(((__GH(h)->Padding >>  8) & 0xFFUL))

/**
 * \brief           Get widget left padding as 8-bit value
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingleft__(h)              (uint8_t)(((__GH(h)->Padding >>  0) & 0xFFUL))

/**
 * \brief           Set top padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpaddingtop__(h, x)            (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)(x)) << 24))

/**
 * \brief           Set right padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpaddingright__(h, x)          (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)(x)) << 16))

/**
 * \brief           Set bottom padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpaddingbottom__(h, x)         (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)(x)) <<  8))

/**
 * \brief           Set left padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpaddingleft__(h, x)           (__GH(h)->Padding = (uint32_t)((__GH(h)->Padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)(x)) <<  0))

/**
 * \brief           Set top and bottom paddings on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpaddingtopbottom__(h, x)      do {\
    gui_widget_setpaddingtop__(h, x);                   \
    gui_widget_setpaddingbottom__(h, x);                \
} while (0)

/**
 * \brief           Set left and right paddings on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpaddingleftright__(h, x)      do {\
    gui_widget_setpaddingleft__(h, x);                  \
    gui_widget_setpaddingright__(h, x);                 \
} while (0)

/**
 * \brief           Set all paddings on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \retval          None
 * \hideinitializer
 */
#define gui_widget_setpadding__(h, x)               do {\
    gui_widget_setpaddingtop__(h, x);                   \
    gui_widget_setpaddingleft__(h, x);                  \
    gui_widget_setpaddingbottom__(h, x);                \
    gui_widget_setpaddingright__(h, x);                 \
} while (0)

/**
 * \brief           Get widget ID
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Widget ID
 * \hideinitializer
 */
#define gui_widget_getid__(h)                       (__GH(h)->Id)

/**
 * \brief           Get first widget handle by ID
 * \note            If multiple widgets have the same ID, first found will be used
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   id: Widget ID to search for
 * \retval          > 0: Widget handle when widget found
 * \retval          0: Widget not found
 */
GUI_HANDLE_p gui_widget_getbyid__(GUI_ID_t id);

/**
 * \brief           Get widget flag(s)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \retval          0: None flag is set
 * \retval          > 0: At least one flag is set
 * \sa              __GUI_WIDGET_SetFlag, __GUI_WIDGET_ClrFlag
 * \hideinitializer
 */
#define gui_widget_getflag__(h, flag)               (__GH(h)->Flags & (flag))

/**
 * \brief           Get widget core flag(s)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \retval          0: None flag is set
 * \retval          > 0: At least one flag is set
 * \sa              __GUI_WIDGET_SetFlag, __GUI_WIDGET_ClrFlag
 * \hideinitializer
 */
#define gui_widget_getcoreflag__(h, flag)           (__GH(h)->Widget->Flags & (flag))

/**
 * \brief           Set widget flag(s)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to set
 * \sa              __GUI_WIDGET_GetFlag, __GUI_WIDGET_ClrFlag
 * \hideinitializer
 */
#define gui_widget_setflag__(h, flag)               (__GH(h)->Flags |= (flag))

/**
 * \brief           Clear widget flag(s)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to clear
 * \sa              __GUI_WIDGET_SetFlag, __GUI_WIDGET_GetFlag
 * \hideinitializer
 */
#define gui_widget_clrflag__(h, flag)               (__GH(h)->Flags &= ~(flag))

/**
 * \brief           Checks if widget is expanded to maximum relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is expandend
 * \retval          0: Widget is not expanded
 * \hideinitializer
 */
#define gui_widget_isexpanded__(h)                  (!!gui_widget_getflag__(h, GUI_FLAG_EXPANDED))

/**
 * \brief           Checks if widget has enabled 3D mode
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget has 3D mode
 * \retval          0: Widget doesn't have 3D mode
 * \hideinitializer
 */
#define gui_widget_is3d__(h)                        (!!gui_widget_getflag__(h, GUI_FLAG_3D))

/**
 * \brief           Get pointer to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to parent widget
 * \hideinitializer
 */
#define gui_widget_getparent__(h)                   (__GH(h)->Parent)

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
#define gui_widget_callback__(h, cmd, param, result)    (__GH(h)->Callback ? __GH(h)->Callback(h, cmd, param, result) : __GH(h)->Widget->Callback(h, cmd, param, result))

/**
 * \brief           Get widget colors from list of colors
 *                  It takes colors from allocated memory if exists or from default widget setup for default
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index from color array for specific widget
 * \retval          Color index
 * \hideinitializer
 */
#define gui_widget_getcolor__(h, index)             (__GH(h)->Colors ? __GH(h)->Colors[(uint8_t)(index)] : (__GH(h)->Widget->Colors ? __GH(h)->Widget->Colors[(uint8_t)(index)] : GUI_COLOR_BLACK))

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
GUI_Dim_t gui_widget_getwidth__(GUI_HANDLE_p h);

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
GUI_Dim_t gui_widget_getheight__(GUI_HANDLE_p h);

/**
 * \brief           Get inner width (total width - padding left - padding right)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Height in units of pixels
 * \sa              __GUI_WIDGET_GetInnerHeight
 * \hideinitializer
 */
#define gui_widget_getinnerwidth__(h)               (gui_widget_getwidth__(h) - (gui_widget_getpaddingleft__(h) + gui_widget_getpaddingright__(h)))

/**
 * \brief           Get inner height (total height - padding top - padding bottom)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Inner height in units of pixels
 * \sa              __GUI_WIDGET_GetInnerWidth
 * \hideinitializer
 */
#define gui_widget_getinnerheight__(h)              (gui_widget_getheight__(h) - (gui_widget_getpaddingtop__(h) + gui_widget_getpaddingbottom__(h)))

/**
 * \brief           Returns width of parent element. If parent does not exists, it returns LCD width
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Parent width in units of pixels
 * \sa              __GUI_WIDGET_GetParentHeight
 * \hideinitializer
 */
#define gui_widget_getparentwidth__(h)              (__GH(h)->Parent ? gui_widget_getwidth__(__GH(h)->Parent) : GUI.LCD.Width)

/**
 * \brief           Returns height of parent element. If parent does not exists, it returns LCD height
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \sa              __GUI_WIDGET_GetParentWidth
 * \hideinitializer
 */
#define gui_widget_getparentheight__(h)             (__GH(h)->Parent ? gui_widget_getheight__(__GH(h)->Parent) : GUI.LCD.Height)

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
#define gui_widget_getparentinnerwidth__(h)         (__GH(h)->Parent ? gui_widget_getinnerwidth__(__GH(h)->Parent) : GUI.LCD.Width)

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
#define gui_widget_getparentinnerheight__(h)        (__GH(h)->Parent ? gui_widget_getinnerheight__(__GH(h)->Parent) : GUI.LCD.Height)

/**
 * \brief           Check if widget is visible in any way, either with transparency or hidden flag
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is visible
 * \retval          0: Widget is not visible
 * \sa              __GUI_WIDGET_IsHidden
 * \hideinitializer
 */
#define gui_widget_isvisible__(h)                   (!gui_widget_getflag__(h, GUI_FLAG_HIDDEN) && __GH(h)->Transparency)

/**
 * \brief           Check if widget is hidden
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is hidden
 * \retval          0: Widget is not hidden
 * \sa              __GUI_WIDGET_IsVisible
 * \hideinitializer
 */
#define gui_widget_ishidden__(h)                    (!gui_widget_isvisible__(h))

/**
 * \brief           Check if widget allows children widgets
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget allows children widgets
 * \retval          0: Widget does not allow children widgets
 * \hideinitializer
 */
#define gui_widget_allowchildren__(h)               (!!gui_widget_getcoreflag__(h, GUI_FLAG_WIDGET_ALLOW_CHILDREN))

/**
 * \brief           Check if widget is base for dialog
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is dialog base
 * \retval          0: Widget is not dialog
 * \hideinitializer
 */
#define gui_widget_isdialogbase__(h)                (!!gui_widget_getcoreflag__(h, GUI_FLAG_WIDGET_DIALOG_BASE) || !!gui_widget_getflag__(h, GUI_FLAG_WIDGET_DIALOG_BASE))

/**
 * \brief           Checks if widget handle is currently in focus
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is in focus or not
 * \sa              __GUI_WIDGET_IsActive
 * \hideinitializer
 */
#define gui_widget_isfocused__(h)                   (!!gui_widget_getflag__(h, GUI_FLAG_FOCUS))

/**
 * \brief           Checks if widget handle is currently active
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is active or not
 * \sa              __GUI_WIDGET_IsFocused
 * \hideinitializer
 */
#define gui_widget_isactive__(h)                    (!!gui_widget_getflag__(h, GUI_FLAG_ACTIVE))

/**
 * \brief           Check is widget has transparency
 * \note            Check if widget is visible and transparency is not set to 1 (full view)
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is visible and has transparency
 * \retval          0: Widget is either hidden or with no transparency
 */
#define gui_widget_istransparent__(h)               (gui_widget_isvisible__(h) && gui_widget_gettransparency__(h) < 0xFF)

/**
 * \brief           Get widget transparency value
 * \note            Value between 0 and 0xFF is used:
 *                      - 0x00: Widget is hidden
 *                      - 0xFF: Widget is fully visible
 *                      - between: Widget has transparency value
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Trasparency value
 * \sa              __GUI_WIDGET_SetTransparency
 * \hideinitializer
 */
#define gui_widget_gettransparency__(h)             ((uint8_t)(__GH(h)->Transparency))

/**
 * \brief           Set transparency level to widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       trans: Transparency level, where 0x00 means hidden and 0xFF means totally visible widget
 * \retval          1: Transparency set ok
 * \retval          0: Transparency was not set
 * \sa              __GUI_WIDGET_SetTransparency
 */
uint8_t gui_widget_settransparency__(GUI_HANDLE_p h, uint8_t trans);

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
uint8_t gui_widget_setzindex__(GUI_HANDLE_p h, int32_t zindex);

/**
 * \brief           Get z-index value from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 * \hideinitializer
 */
#define gui_widget_getzindex__(h)                   ((int32_t)(__GH(h)->ZIndex))

/**
 * \brief           Set custom user data to widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            Specially useful in callback processing if required
 * \param[in,out]   h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \retval          1: Data were set OK
 * \retval          0: Data were not set
 * \sa              __GUI_WIDGET_GetUserData
 */
uint8_t gui_widget_setuserdata__(GUI_HANDLE_p h, void* data);

/**
 * \brief           Get custom user data from widget previously set with \ref GUI_WIDGET_SetUserData
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to user data
 * \sa              __GUI_WIDGET_SetUserData
 */
void* gui_widget_getuserdata__(GUI_HANDLE_p h);

uint8_t gui_widget_setparam__(GUI_HANDLE_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent);
uint8_t gui_widget_getparam__(GUI_HANDLE_p h, uint16_t cfg, void* data);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_Text Text management
 * \brief           Text and font management functions
 * \{
 */

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
uint8_t GUI_WIDGET_SetFont(GUI_HANDLE_p h, const GUI_FONT_t* font);

/**
 * \brief           Get font from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to font used for widget
 */
const GUI_FONT_t* GUI_WIDGET_GetFont(GUI_HANDLE_p h);

/**
 * \brief           Set default font for widgets used on widget creation
 * \param[in]       *font: Pointer to \ref GUI_FONT_t with font
 * \retval          1: Font was set ok
 * \retval          0: Font was not set
 */
uint8_t GUI_WIDGET_SetFontDefault(const GUI_FONT_t* font);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_Size Size management
 * \brief           Size management functions
 * \{
 */

/**
 * \brief           Set widget size in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width value
 * \param[in]       height: Height value
 * \retval          1: Size was set ok
 * \retval          0: Size was not set
 * \sa              GUI_WIDGET_SetSizePercent
 */
uint8_t GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \brief           Set widget size in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width value
 * \param[in]       height: Height value
 * \retval          1: Size was set ok
 * \retval          0: Size was not set
 * \sa              GUI_WIDGET_SetSize
 */
uint8_t GUI_WIDGET_SetSizePercent(GUI_HANDLE_p h, float width, float height);

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
uint8_t GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, float width);

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in percentage
 * \retval          1: Height was set ok
 * \retval          0: Height was not set
 * \sa              GUI_WIDGET_SetWidth, GUI_WIDGET_SetHeight, GUI_WIDGET_SetWidthPercent
 */
uint8_t GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, float height);

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
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_Position Position management
 * \brief           Position management functions
 * \{
 */
 
/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetPositionPercent
 */
uint8_t GUI_WIDGET_SetPosition(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y);
 
/**
 * \brief           Set widget position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetPosition
 */
uint8_t GUI_WIDGET_SetPositionPercent(GUI_HANDLE_p h, float x, float y);
 
/**
 * \brief           Set widget X position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetXPositionPercent
 */
uint8_t GUI_WIDGET_SetXPosition(GUI_HANDLE_p h, GUI_iDim_t x);
 
/**
 * \brief           Set widget X position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetXPosition
 */
uint8_t GUI_WIDGET_SetXPositionPercent(GUI_HANDLE_p h, float x);
 
/**
 * \brief           Set widget Y position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetYPositionPercent
 */
uint8_t GUI_WIDGET_SetYPosition(GUI_HANDLE_p h, GUI_iDim_t y);
 
/**
 * \brief           Set widget Y position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              GUI_WIDGET_SetYPosition
 */
uint8_t GUI_WIDGET_SetYPositionPercent(GUI_HANDLE_p h, float y);

/**
 * \brief           Set widget scroll on X axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for X direction
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              GUI_WIDGET_SetScrollY
 */
uint8_t GUI_WIDGET_SetScrollX(GUI_HANDLE_p h, GUI_iDim_t scroll);

/**
 * \brief           Set widget scroll on Y axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for Y direction
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              GUI_WIDGET_SetScrollX
 */
uint8_t GUI_WIDGET_SetScrollY(GUI_HANDLE_p h, GUI_iDim_t scroll);

/**
 * \}
 */
 
/**
 * \defgroup        GUI_WIDGET_Visibility Visibility management
 * \brief           Visibility management functions
 * \{
 *
 * \section sect_gui_widget_transparency Widget transparency
 *
 * Library support transparency setup on widget. For this, 
 * special blending function must be enabled (best if support on hardware).
 *
 * \image html image_transparent_buttons.png Transparent of 0x80 on button over opaque button
 *
 * When transparency on widget should be used, additional RAM is used:
 *
 * \f$\ size=WidgetVisibleWidth*WidgetVisibleHeight*BytesPerPixel\f$
 *
 * The additional RAM usage is used if widget has transparency and its children has also custom transparency.
 * In this case, RAM used is for each widget separatelly and depends on number of tree length of widgets of transparency values.
 *
 * \par Blending technique for transparency
 *
 * Imagine this setup:
 *   - Desktop
 *      - <b>Widget 1</b>, children of desktop
 *         - <b>Widget 2</b>, children of Widget 1
 *            - <b>Widget 3</b>, children of Widget 2
 *
 * At some time, Widget1 has transparency = 0x80 (0.5) and Widget 3 has transparency = 0x40 (0.25)
 *
 * Drawing process would go like this:
 *
 * 1. Draw desktop widget on default drawing frame buffer
 * 2. Allocate memory for visible widget part for widget 1 and set RAM as drawing buffer
 * 3. Draw widget 1 on drawing buffer (allocated RAM)
 * 4. Draw widget 2 on drawing buffer (still allocated RAM of previous widget)
 * 5. Allocate memory for visible widget part for widget 3 and set RAM as drawing buffer
 * 6. Draw widget 3 on drawing buffer (latest allocated RAM of widget 3)
 * 7. With blending technique, merge widget 3 frame buffer with widget 1 frame buffer
 * 8. Set frame buffer back to widget 1 allocated RAM
 * 9. With blending technique, merge widget 1 frame buffer with desktop frame buffer
 * 10. Set frame buffer back to default drawing frame buffer
 */

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
 * \brief           Get widget transparency value
 * \note            Value between 0 and 0xFF is used:
 *                      - 0x00: Widget is hidden
 *                      - 0xFF: Widget is fully visible
 *                      - between: Widget has transparency value
 * \param[in,out]   h: Widget handle
 * \retval          Trasparency value
 * \sa              GUI_WIDGET_SetTransparency
 */
uint8_t GUI_WIDGET_GetTransparency(GUI_HANDLE_p h);

/**
 * \brief           Set transparency level to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       trans: Transparency level, where 0x00 means hidden and 0xFF means totally visible widget
 * \retval          1: Transparency set ok
 * \retval          0: Transparency was not set
 * \sa              GUI_WIDGET_GetTransparency
 */
uint8_t GUI_WIDGET_SetTransparency(GUI_HANDLE_p h, uint8_t trans);
 
/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_Misc Miscellaneous
 * \brief           Miscellaneous functions
 * \{
 */

/**
 * \brief           Invalidate widget object and prepare to new redraw
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was invalidated ok
 * \retval          0: Widget was not invalidated
 */
uint8_t GUI_WIDGET_Invalidate(GUI_HANDLE_p h);

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
 * \brief           Manually set widget in focus
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget set to focus
 * \retval          0: Widget was not set to focus
 */
uint8_t GUI_WIDGET_SetFocus(GUI_HANDLE_p h);

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \retval          1: New value is different than previous and modification has been done
 * \retval          0: New value was not set
 * \sa              GUI_WIDGET_GetZIndex
 */
uint8_t GUI_WIDGET_SetZIndex(GUI_HANDLE_p h, int32_t zindex);

/**
 * \brief           Get z-index value from widget
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 * \sa              GUI_WIDGET_SetZIndex
 */
int32_t GUI_WIDGET_GetZIndex(GUI_HANDLE_p h);

/**
 * \brief           Set 3D mode on widget
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value to enable, either 1 or 0
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t GUI_WIDGET_Set3DStyle(GUI_HANDLE_p h, uint8_t enable);

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
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_Callback Callback processing
 * \brief           Callback based functions
 * \{
 */

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
 * \}
 */
 
#if defined(GUI_INTERNAL) && !defined(DOXYGEN)
//Strictly private functions by GUI
//Widget initialization purpose
void gui_widget_init__(void);

//Clipping regions
uint8_t gui_widget_isinsideclippingregion__(GUI_HANDLE_p h);

//Move widget down and all its parents with it
void gui_widget_movedowntree__(GUI_HANDLE_p h);

void __GUI_WIDGET_FOCUS_CLEAR(void);
void __GUI_WIDGET_FOCUS_SET(GUI_HANDLE_p h);
void __GUI_WIDGET_ACTIVE_CLEAR(void);
void __GUI_WIDGET_ACTIVE_SET(GUI_HANDLE_p h);

//Execute actual widget remove process
uint8_t gui_widget_executeremove__(void);
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

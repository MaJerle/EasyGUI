/**	
 * \file            gui_widget.h
 * \brief           Widget specific shared functions
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#ifndef __GUI_WIDGET_H
#define __GUI_WIDGET_H

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
 * \{
 */
#include "gui/gui.h"
#include "gui/gui_draw.h"

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
 * \defgroup        GUI_WIDGET_CREATE_FLAGS Flags for widget create
 * \brief           A list of flags supported for widget creation
 *
 * List of flags used on \ref gui_widget_create__ function when creating new widget
 * 
 * \{
 */
#define GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP       0x01 /*!< Indicates parent object of widget should be desktop window */

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_PRIVATE Private functions
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Input pointer to test for widget
 * \return          1 on success, 0 otherwise Pointer is widget
 * \return          1 on success, 0 otherwise Pointer is not widget
 * \hideinitializer
 */
#define gui_widget_iswidget__(h)        ((h != NULL) && __GH(h)->Footprint == GUI_WIDGET_FOOTPRINT)

/**
 * \brief           Get widget relative X position according to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Relative X according to parent widget
 * \hideinitializer
 */
#define gui_widget_getrelativex__(h)    (gui_widget_isexpanded__(h) ? 0 : \
                                            (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT) ? (GUI_iDim_t)((float)GUI_ROUND(__GH(h)->X * gui_widget_getparentinnerwidth__(h)) / 100.0f) : __GH(h)->X) \
                                        )

/**
 * \brief           Get widget relative Y position according to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Relative Y according to parent widget
 * \hideinitializer
 */
#define gui_widget_getrelativey__(h)    (gui_widget_isexpanded__(h) ? 0 : \
                                            (gui_widget_getflag__(h, GUI_FLAG_YPOS_PERCENT) ? (GUI_iDim_t)((float)GUI_ROUND(__GH(h)->Y * gui_widget_getparentinnerheight__(h)) / 100.0f) : __GH(h)->Y) \
                                        )

/**
 * \brief           Get widget top padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingtop__(h)               (uint8_t)(((__GH(h)->Padding >> 24) & 0xFFUL))

/**
 * \brief           Get widget right padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingright__(h)             (uint8_t)(((__GH(h)->Padding >> 16) & 0xFFUL))

/**
 * \brief           Get widget bottom padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define gui_widget_getpaddingbottom__(h)            (uint8_t)(((__GH(h)->Padding >>  8) & 0xFFUL))

/**
 * \brief           Get widget left padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Widget ID
 * \hideinitializer
 */
#define gui_widget_getid__(h)                       (__GH(h)->Id)

/**
 * \brief           Get widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \return          1 on success, 0 otherwise None flag is set
 * \retval          > 0: At least one flag is set
 * \sa              __gui_widget_setflag, __gui_widget_clrflag
 * \hideinitializer
 */
#define gui_widget_getflag__(h, flag)               (__GH(h)->Flags & (flag))

/**
 * \brief           Get widget core flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \return          1 on success, 0 otherwise None flag is set
 * \retval          > 0: At least one flag is set
 * \sa              __gui_widget_setflag, __gui_widget_clrflag
 * \hideinitializer
 */
#define gui_widget_getcoreflag__(h, flag)           (__GH(h)->Widget->Flags & (flag))

/**
 * \brief           Set widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to set
 * \sa              __gui_widget_getflag, __gui_widget_clrflag
 * \hideinitializer
 */
#define gui_widget_setflag__(h, flag)               (__GH(h)->Flags |= (flag))

/**
 * \brief           Clear widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to clear
 * \sa              __gui_widget_setflag, __gui_widget_getflag
 * \hideinitializer
 */
#define gui_widget_clrflag__(h, flag)               (__GH(h)->Flags &= ~(flag))

/**
 * \brief           Checks if widget is expanded to maximum relative to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget is expandend
 * \return          1 on success, 0 otherwise Widget is not expanded
 * \hideinitializer
 */
#define gui_widget_isexpanded__(h)                  (!!gui_widget_getflag__(h, GUI_FLAG_EXPANDED))

/**
 * \brief           Checks if widget has enabled 3D mode
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget has 3D mode
 * \return          1 on success, 0 otherwise Widget doesn't have 3D mode
 * \hideinitializer
 */
#define gui_widget_is3d__(h)                        (!!gui_widget_getflag__(h, GUI_FLAG_3D))

/**
 * \brief           Get pointer to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to parent widget
 * \hideinitializer
 */
#define gui_widget_getparent__(h)                   (__GH(h)->Parent)

/**
 * \brief           Process widget callback with command, parameters and result pointers
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       cmd: Callback command. This parameter can be a value of \ref GUI_WC_t enumeration
 * \param[in]       param: Pointer to parameters if any for this command
 * \param[out]      result: Pointer to result pointer where calback can store result
 * \return          1 on success, 0 otherwise Command processed by widget
 * \return          1 on success, 0 otherwise Command was not processed by widget
 * \hideinitializer
 */
#define gui_widget_callback__(h, cmd, param, result)    (__GH(h)->Callback ? __GH(h)->Callback(h, cmd, param, result) : __GH(h)->Widget->Callback(h, cmd, param, result))

/**
 * \brief           Get widget colors from list of colors
 *                  It takes colors from allocated memory if exists or from default widget setup for default
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index from color array for specific widget
 * \retval          Color index
 * \hideinitializer
 */
#define gui_widget_getcolor__(h, index)             (__GH(h)->Colors ? __GH(h)->Colors[(uint8_t)(index)] : (__GH(h)->Widget->Colors ? __GH(h)->Widget->Colors[(uint8_t)(index)] : GUI_COLOR_BLACK))

/**
 * \brief           Get inner width (total width - padding left - padding right)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Height in units of pixels
 * \sa              __gui_widget_getinnerheight
 * \hideinitializer
 */
#define gui_widget_getinnerwidth__(h)               (gui_widget_getwidth__(h) - (gui_widget_getpaddingleft__(h) + gui_widget_getpaddingright__(h)))

/**
 * \brief           Get inner height (total height - padding top - padding bottom)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Inner height in units of pixels
 * \sa              __gui_widget_getinnerwidth
 * \hideinitializer
 */
#define gui_widget_getinnerheight__(h)              (gui_widget_getheight__(h) - (gui_widget_getpaddingtop__(h) + gui_widget_getpaddingbottom__(h)))

/**
 * \brief           Returns width of parent element. If parent does not exists, it returns LCD width
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Parent width in units of pixels
 * \sa              __gui_widget_getparentheight
 * \hideinitializer
 */
#define gui_widget_getparentwidth__(h)              (__GH(h)->Parent ? gui_widget_getwidth__(__GH(h)->Parent) : GUI.LCD.Width)

/**
 * \brief           Returns height of parent element. If parent does not exists, it returns LCD height
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \sa              __gui_widget_getparentwidth
 * \hideinitializer
 */
#define gui_widget_getparentheight__(h)             (__GH(h)->Parent ? gui_widget_getheight__(__GH(h)->Parent) : GUI.LCD.Height)

/**
 * \brief           Returns inner width of parent element. If parent does not exists, it returns LCD width
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            It returns inner width = total width - padding left - padding right
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Parent width in units of pixels
 * \sa              __gui_widget_getparentinnerheight
 * \hideinitializer
 */
#define gui_widget_getparentinnerwidth__(h)         (__GH(h)->Parent ? gui_widget_getinnerwidth__(__GH(h)->Parent) : GUI.LCD.Width)

/**
 * \brief           Returns inner height of parent element. If parent does not exists, it returns LCD height
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            It returns inner height = total height - padding top - padding bottom
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \sa              __gui_widget_getparentinnerwidth
 * \hideinitializer
 */
#define gui_widget_getparentinnerheight__(h)        (__GH(h)->Parent ? gui_widget_getinnerheight__(__GH(h)->Parent) : GUI.LCD.Height)

/**
 * \brief           Check if widget is visible in any way, either with transparency or hidden flag
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget is visible
 * \return          1 on success, 0 otherwise Widget is not visible
 * \sa              __gui_widget_ishidden
 * \hideinitializer
 */
#if GUI_CFG_USE_TRANSPARENCY
#define gui_widget_isvisible__(h)                   (!gui_widget_getflag__(h, GUI_FLAG_HIDDEN) && __GH(h)->Transparency)
#else
#define gui_widget_isvisible__(h)                   (!gui_widget_getflag__(h, GUI_FLAG_HIDDEN))
#endif

/**
 * \brief           Check if widget is hidden
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget is hidden
 * \return          1 on success, 0 otherwise Widget is not hidden
 * \sa              __gui_widget_isvisible
 * \hideinitializer
 */
#define gui_widget_ishidden__(h)                    (!gui_widget_isvisible__(h))

/**
 * \brief           Check if widget allows children widgets
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget allows children widgets
 * \return          1 on success, 0 otherwise Widget does not allow children widgets
 * \hideinitializer
 */
#define gui_widget_allowchildren__(h)               (!!gui_widget_getcoreflag__(h, GUI_FLAG_WIDGET_ALLOW_CHILDREN))

/**
 * \brief           Check if widget is base for dialog
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget is dialog base
 * \return          1 on success, 0 otherwise Widget is not dialog
 * \hideinitializer
 */
#define gui_widget_isdialogbase__(h)                (!!gui_widget_getcoreflag__(h, GUI_FLAG_WIDGET_DIALOG_BASE) || !!gui_widget_getflag__(h, GUI_FLAG_WIDGET_DIALOG_BASE))

/**
 * \brief           Checks if widget handle is currently in focus
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is in focus or not
 * \sa              __gui_widget_isactive
 * \hideinitializer
 */
#define gui_widget_isfocused__(h)                   (!!gui_widget_getflag__(h, GUI_FLAG_FOCUS))

/**
 * \brief           Checks if widget handle is currently active
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is active or not
 * \sa              __gui_widget_isfocused
 * \hideinitializer
 */
#define gui_widget_isactive__(h)                    (!!gui_widget_getflag__(h, GUI_FLAG_ACTIVE))

#if GUI_CFG_USE_TRANSPARENCY || __DOXYGEN__
/**
 * \brief           Check is widget has transparency
 * \note            Check if widget is visible and transparency is not set to 1 (full view)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise Widget is visible and has transparency
 * \return          1 on success, 0 otherwise Widget is either hidden or with no transparency
 */
#define gui_widget_istransparent__(h)               (gui_widget_isvisible__(h) && gui_widget_gettransparency__(h) < 0xFF)

/**
 * \brief           Get widget transparency value
 * \note            Value between 0 and 0xFF is used:
 *                      - 0x00: Widget is hidden
 *                      - 0xFF: Widget is fully visible
 *                      - between: Widget has transparency value
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Trasparency value
 * \sa              __gui_widget_settransparency
 * \hideinitializer
 */
#define gui_widget_gettransparency__(h)             ((uint8_t)(__GH(h)->Transparency))
#endif /* GUI_CFG_USE_TRANSPARENCY || __DOXYGEN__ */

/**
 * \brief           Get z-index value from widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 * \hideinitializer
 */
#define gui_widget_getzindex__(h)                   ((int32_t)(__GH(h)->ZIndex))

GUI_iDim_t gui_widget_getabsolutex__(GUI_HANDLE_p h);
GUI_iDim_t gui_widget_getabsolutey__(GUI_HANDLE_p h);
GUI_iDim_t gui_widget_getparentabsolutex__(GUI_HANDLE_p h);
GUI_iDim_t gui_widget_getparentabsolutey__(GUI_HANDLE_p h);
uint8_t gui_widget_invalidate__(GUI_HANDLE_p h);
uint8_t gui_widget_invalidatewithparent__(GUI_HANDLE_p h);
uint8_t gui_widget_setinvalidatewithparent__(GUI_HANDLE_p h, uint8_t value);
uint8_t gui_widget_setposition__(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y);
uint8_t gui_widget_setpositionpercent__(GUI_HANDLE_p h, float x, float y);
uint8_t gui_widget_setxposition__(GUI_HANDLE_p h, GUI_iDim_t x);
uint8_t gui_widget_setxpositionpercent__(GUI_HANDLE_p h, float x);
uint8_t gui_widget_setyposition__(GUI_HANDLE_p h, GUI_iDim_t y);
uint8_t gui_widget_setypositionpercent__(GUI_HANDLE_p h, float y);
uint8_t gui_widget_setsize__(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi);
uint8_t gui_widget_setsizepercent__(GUI_HANDLE_p h, float wi, float hi);
uint8_t gui_widget_setwidth__(GUI_HANDLE_p h, GUI_Dim_t width);
uint8_t gui_widget_setheight__(GUI_HANDLE_p h, GUI_Dim_t height);
uint8_t gui_widget_setwidthpercent__(GUI_HANDLE_p h, float width);
uint8_t gui_widget_setheightpercent__(GUI_HANDLE_p h, float height);
uint8_t gui_widget_set3dstyle__(GUI_HANDLE_p h, uint8_t enable);
uint8_t gui_widget_setfont__(GUI_HANDLE_p h, const GUI_FONT_t* font);
uint8_t gui_widget_settext__(GUI_HANDLE_p h, const GUI_Char* text);
const GUI_Char* gui_widget_gettext__(GUI_HANDLE_p h);
const GUI_FONT_t* gui_widget_getfont__(GUI_HANDLE_p h);

uint8_t gui_widget_alloctextmemory__(GUI_HANDLE_p h, uint32_t size);
uint8_t gui_widget_freetextmemory__(GUI_HANDLE_p h);
void* gui_widget_create__(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_widget_remove__(GUI_HANDLE_p h);
uint8_t gui_widget_show__(GUI_HANDLE_p h);
uint8_t gui_widget_hide__(GUI_HANDLE_p h);
uint8_t gui_widget_hidechildren__(GUI_HANDLE_p h);
uint8_t gui_widget_toggleexpanded__(GUI_HANDLE_p h);
uint8_t gui_widget_setexpanded__(GUI_HANDLE_p h, uint8_t state);
uint8_t gui_widget_ischildof__(GUI_HANDLE_p h, GUI_HANDLE_p parent);
uint8_t gui_widget_isfontandtextset__(GUI_HANDLE_p h);

uint8_t gui_widget_processtextkey__(GUI_HANDLE_p h, __GUI_KeyboardData_t* key);
uint8_t gui_widget_setcolor__(GUI_HANDLE_p h, uint8_t index, GUI_Color_t color);
GUI_HANDLE_p gui_widget_getbyid__(GUI_ID_t id);

uint8_t gui_widget_settransparency__(GUI_HANDLE_p h, uint8_t trans);
uint8_t gui_widget_setzindex__(GUI_HANDLE_p h, int32_t zindex);

GUI_Dim_t gui_widget_getwidth__(GUI_HANDLE_p h);
GUI_Dim_t gui_widget_getheight__(GUI_HANDLE_p h);

uint8_t gui_widget_setuserdata__(GUI_HANDLE_p h, void* data);
void* gui_widget_getuserdata__(GUI_HANDLE_p h);
uint8_t gui_widget_setparam__(GUI_HANDLE_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent);
uint8_t gui_widget_getparam__(GUI_HANDLE_p h, uint16_t cfg, void* data);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_TEXT Text management
 * \brief           Text and font management functions
 * \{
 */

uint32_t gui_widget_alloctextmemory(GUI_HANDLE_p h, uint32_t size);
uint8_t gui_widget_freetextmemory(GUI_HANDLE_p h);
uint8_t gui_widget_settext(GUI_HANDLE_p h, const GUI_Char* text);
const GUI_Char* gui_widget_gettext(GUI_HANDLE_p h);
const GUI_Char* gui_widget_gettextcopy(GUI_HANDLE_p h, GUI_Char* dst, uint32_t len);
uint8_t gui_widget_setfont(GUI_HANDLE_p h, const GUI_FONT_t* font);
const GUI_FONT_t* gui_widget_getfont(GUI_HANDLE_p h);
uint8_t gui_widget_setfontdefault(const GUI_FONT_t* font);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_SIZE Size management
 * \brief           Size management functions
 * \{
 */

uint8_t gui_widget_setsize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height);
uint8_t gui_widget_setsizepercent(GUI_HANDLE_p h, float width, float height);
uint8_t gui_widget_setwidth(GUI_HANDLE_p h, GUI_Dim_t width);
uint8_t gui_widget_setheight(GUI_HANDLE_p h, GUI_Dim_t height);
uint8_t gui_widget_setwidthpercent(GUI_HANDLE_p h, float width);
uint8_t gui_widget_setheightpercent(GUI_HANDLE_p h, float height);
GUI_Dim_t gui_widget_getwidth(GUI_HANDLE_p h);
GUI_Dim_t gui_widget_getheight(GUI_HANDLE_p h);
uint8_t gui_widget_setexpanded(GUI_HANDLE_p h, uint8_t state);
uint8_t gui_widget_isexpanded(GUI_HANDLE_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_POSITION Position management
 * \brief           Position management functions
 * \{
 */
 
uint8_t gui_widget_setposition(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y);
uint8_t gui_widget_setpositionpercent(GUI_HANDLE_p h, float x, float y);
uint8_t gui_widget_setxposition(GUI_HANDLE_p h, GUI_iDim_t x);
uint8_t gui_widget_setxpositionpercent(GUI_HANDLE_p h, float x);
uint8_t gui_widget_setyposition(GUI_HANDLE_p h, GUI_iDim_t y);
uint8_t gui_widget_setypositionpercent(GUI_HANDLE_p h, float y);
uint8_t gui_widget_setscrollx(GUI_HANDLE_p h, GUI_iDim_t scroll);
uint8_t gui_widget_setscrolly(GUI_HANDLE_p h, GUI_iDim_t scroll);
uint8_t gui_widget_incscrollx(GUI_HANDLE_p h, GUI_iDim_t scroll);
uint8_t gui_widget_incscrolly(GUI_HANDLE_p h, GUI_iDim_t scroll);
GUI_iDim_t gui_widget_getscrollx(GUI_HANDLE_p h);
GUI_iDim_t gui_widget_getscrolly(GUI_HANDLE_p h);

/**
 * \}
 */
 
/**
 * \defgroup        GUI_WIDGET_VISIBILITY Visibility management
 * \brief           Visibility management functions
 * \{
 */

uint8_t gui_widget_show(GUI_HANDLE_p h);
uint8_t gui_widget_hide(GUI_HANDLE_p h);
uint8_t gui_widget_hidechildren(GUI_HANDLE_p h);
uint8_t gui_widget_putonfront(GUI_HANDLE_p h);
uint8_t gui_widget_gettransparency(GUI_HANDLE_p h);
uint8_t gui_widget_settransparency(GUI_HANDLE_p h, uint8_t trans);
 
/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_MISC Miscellaneous
 * \brief           Miscellaneous functions
 * \{
 */

uint8_t gui_widget_invalidate(GUI_HANDLE_p h);
uint8_t gui_widget_setuserdata(GUI_HANDLE_p h, void* data);
void* gui_widget_getuserdata(GUI_HANDLE_p h);
uint8_t gui_widget_ischildof(GUI_HANDLE_p h, GUI_HANDLE_p parent);
uint8_t gui_widget_incselection(GUI_HANDLE_p h, int16_t dir);
uint8_t gui_widget_setfocus(GUI_HANDLE_p h);
uint8_t gui_widget_setzindex(GUI_HANDLE_p h, int32_t zindex);
int32_t gui_widget_getzindex(GUI_HANDLE_p h);
uint8_t gui_widget_set3dstyle(GUI_HANDLE_p h, uint8_t enable);
GUI_ID_t gui_widget_getid(GUI_HANDLE_p h);
GUI_HANDLE_p gui_widget_getbyid(GUI_ID_t id);
uint8_t gui_widget_remove(GUI_HANDLE_p* h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_PADDING Padding
 * \brief           Padding related functions
 * \{
 */
 
uint8_t gui_widget_setpaddingtop(GUI_HANDLE_p h, GUI_Dim_t x);
uint8_t gui_widget_setpaddingright(GUI_HANDLE_p h, GUI_Dim_t x);
uint8_t gui_widget_setpaddingbottom(GUI_HANDLE_p h, GUI_Dim_t x);
uint8_t gui_widget_setpaddingleft(GUI_HANDLE_p h, GUI_Dim_t x);
uint8_t gui_widget_setpaddingtopbottom(GUI_HANDLE_p h, GUI_Dim_t x);      
uint8_t gui_widget_setpaddingleftright(GUI_HANDLE_p h, GUI_Dim_t x);
uint8_t gui_widget_setpadding(GUI_HANDLE_p h, GUI_Dim_t x);

GUI_Dim_t gui_widget_getpaddingtop(GUI_HANDLE_p h);
GUI_Dim_t gui_widget_getpaddingright(GUI_HANDLE_p h);
GUI_Dim_t gui_widget_getpaddingbottom(GUI_HANDLE_p h);
GUI_Dim_t gui_widget_getpaddingleft(GUI_HANDLE_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_CALLBACK Callback processing
 * \brief           Callback based functions
 * \{
 */

uint8_t gui_widget_processdefaultcallback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_widget_setcallback(GUI_HANDLE_p h, GUI_WIDGET_CALLBACK_t callback);
uint8_t gui_widget_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);

/**
 * \}
 */
 
#if defined(GUI_INTERNAL) && !__DOXYGEN__
//Strictly private functions by GUI
//Widget initialization purpose
void gui_widget_init(void);

//Clipping regions
uint8_t gui_widget_isinsideclippingregion(GUI_HANDLE_p h);

//Move widget down and all its parents with it
void gui_widget_movedowntree(GUI_HANDLE_p h);

void gui_widget_focus_clear(void);
void gui_widget_focus_set(GUI_HANDLE_p h);
void gui_widget_active_clear(void);
void gui_widget_active_set(GUI_HANDLE_p h);

//Execute actual widget remove process
uint8_t gui_widget_executeremove(void);
#endif /* !__DOXYGEN__ */

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

#endif /* __GUI_WIDGET_H */

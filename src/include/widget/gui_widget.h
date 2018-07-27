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
    
#define GUI_ID_WINDOW_BASE          ((gui_id_t)0x0000)                          /*!< Window base ID */
#define GUI_ID_KEYBOARD_BASE        ((gui_id_t)(GUI_ID_WINDOW_BASE + 0x1000))   /*!< Keyboard based ID */

#define GUI_ID_USER                 ((gui_id_t)(0x10000))                       /*!< Start number of user based ID values for widgets */

#define GUI_WIDGET_ZINDEX_MAX       (int32_t)(0x7FFFFFFF)                       /*!< Maximal allowed z-index value */
#define GUI_WIDGET_ZINDEX_MIN       (int32_t)(0x80000000)                       /*!< Maximal allowed z-index value */
    
/**
 * \}
 */
 
/**
 * \defgroup        GUI_WIDGET_CREATE_FLAGS flags for widget create
 * \brief           A list of flags supported for widget creation
 *
 * List of flags used on \ref guii_widget_create function when creating new widget
 * 
 * \{
 */
#define GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP       0x01 /*!< Indicates parent object of widget should be desktop window */
#define GUI_FLAG_WIDGET_CREATE_NO_PARENT            0x02 /*!< No parent widget */

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
#define GUI_WIDGET_FOOTPRINT                        0x00ACCE55
 
/**
 * \brief           Checks if inserted pointer is valid widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Input pointer to test for widget
 * \return          `1` on success, `0` otherwise Pointer is widget
 * \return          `1` on success, `0` otherwise Pointer is not widget
 * \hideinitializer
 */
#define guii_widget_iswidget(h)                     ((h) != NULL && (h)->footprint == GUI_WIDGET_FOOTPRINT)

/**
 * \brief           Get widget relative X position according to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Relative X according to parent widget
 * \hideinitializer
 */
#define guii_widget_getrelativex(h)                 GUI_DIM((guii_widget_isexpanded(h) ? 0 : \
                                                        (guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) ? (gui_dim_t)((float)GUI_ROUND((h)->x * guii_widget_getparentinnerwidth(h)) / 100.0f) : (h)->x) \
                                                    ))

/**
 * \brief           Get widget relative Y position according to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Relative Y according to parent widget
 * \hideinitializer
 */
#define guii_widget_getrelativey(h)                 GUI_DIM(guii_widget_isexpanded(h) ? 0 : \
                                                        (guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) ? (gui_dim_t)((float)GUI_ROUND((h)->y * guii_widget_getparentinnerheight(h)) / 100.0f) : (h)->y) \
                                                    )

/**
 * \brief           Get widget top padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define guii_widget_getpaddingtop(h)                GUI_DIM((uint8_t)(((h)->padding >> 24) & 0xFFUL))

/**
 * \brief           Get widget right padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define guii_widget_getpaddingright(h)              GUI_DIM((uint8_t)(((h)->padding >> 16) & 0xFFUL))

/**
 * \brief           Get widget bottom padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define guii_widget_getpaddingbottom(h)             GUI_DIM((uint8_t)(((h)->padding >>  8) & 0xFFUL))

/**
 * \brief           Get widget left padding as 8-bit value
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Padding in units of pixels
 * \hideinitializer
 */
#define guii_widget_getpaddingleft(h)               GUI_DIM((uint8_t)(((h)->padding >>  0) & 0xFFUL))

/**
 * \brief           Set top padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpaddingtop(h, x)              do {\
    (h)->padding = (uint32_t)(((h)->padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)(x)) << 24);  /* Padding top */       \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Set right padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpaddingright(h, x)            do {\
    (h)->padding = (uint32_t)(((h)->padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)(x)) << 16);  /* Padding right */     \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Set bottom padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpaddingbottom(h, x)           do {\
    (h)->padding = (uint32_t)(((h)->padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)(x)) <<  8);  /* Padding bottom */    \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Set left padding on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpaddingleft(h, x)             do {\
    (h)->padding = (uint32_t)(((h)->padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)(x)) <<  0);  /* Padding left */      \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Set top and bottom paddings on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpaddingtopbottom(h, x)       do {\
    (h)->padding = (uint32_t)(((h)->padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)(x)) <<  24); /* Padding top */       \
    (h)->padding = (uint32_t)(((h)->padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)(x)) <<  8);  /* Padding bottom */    \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Set left and right paddings on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpaddingleftright(h, x)      do {\
    (h)->padding = (uint32_t)(((h)->padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)(x)) <<  16); /* Padding right */     \
    (h)->padding = (uint32_t)(((h)->padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)(x)) <<  0);  /* Padding left */      \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Set all paddings on widget
 * \note            Padding is used mainly for internal purpose only to set value for children widgets
 *                  where X or Y position is inside parent widget (ex. Window has padding to set top line with text and buttons)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: Padding in byte format
 * \hideinitializer
 */
#define guii_widget_setpadding(h, x)                do {\
    (h)->padding = GUI_U8(x) << 24 | GUI_U8(x) << 16 | GUI_U8(x) << 8 | GUI_U8(x) << 0;         /* Padding */           \
    SET_WIDGET_ABS_VALUES(h);   \
} while (0)

/**
 * \brief           Get widget ID
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Widget ID
 * \hideinitializer
 */
#define guii_widget_getid(h)                        ((gui_id_t)((h)->id))

/**
 * \brief           Get widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \return          0 on failure, non-zero otherwise
 * \hideinitializer
 */
#define guii_widget_getflag(h, flag)                (__GH(h)->flags & (flag))

/**
 * \brief           Get widget core flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \return          0 on failure, non-zero otherwise (flags)
 * \hideinitializer
 */
#define guii_widget_getcoreflag(h, flag)            (__GH(h)->widget->flags & (flag))

/**
 * \brief           Set widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to set
 * \hideinitializer
 */
#define guii_widget_setflag(h, flag)                ((h)->flags |= (flag))

/**
 * \brief           Clear widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       flag: Flag(s) to clear
 * \hideinitializer
 */
#define guii_widget_clrflag(h, flag)                ((h)->flags &= ~(flag))

/**
 * \brief           Checks if widget is expanded to maximum relative to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_isexpanded(h)                   (!!guii_widget_getflag(h, GUI_FLAG_EXPANDED))

/**
 * \brief           Checks if widget has enabled 3D mode
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_is3d(h)                         (!!guii_widget_getflag(h, GUI_FLAG_3D))

/**
 * \brief           Get pointer to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to parent widget
 * \hideinitializer
 */
#define guii_widget_getparent(h)                    ((h) != NULL ? (h)->parent : NULL)

/**
 * \brief           Check if widget has parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_hasparent(h)                    ((h) != NULL && (h)->parent != NULL)

/**
 * \brief           Process widget callback with command, parameters and result pointers
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       cmd: Callback command. This parameter can be a value of \ref gui_wc_t enumeration
 * \param[in]       param: Pointer to parameters if any for this command
 * \param[out]      result: Pointer to result pointer where calback can store result
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_callback(h, cmd, param, result) ((h)->callback != NULL ? (h)->callback(h, cmd, param, result) : (h)->widget->callback(h, cmd, param, result))

/**
 * \brief           Get widget colors from list of colors
 *                  It takes colors from allocated memory if exists or from default widget setup for default
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index from color array for specific widget
 * \retval          Color index
 * \hideinitializer
 */
#define guii_widget_getcolor(h, index)              ((h)->colors != NULL ? (h)->colors[(uint8_t)(index)] : ((h)->widget->colors != NULL ? (h)->widget->colors[(uint8_t)(index)] : GUI_COLOR_BLACK))

/**
 * \brief           Get inner width (total width - padding left - padding right)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \retval          height in units of pixels
 * \hideinitializer
 */
#define guii_widget_getinnerwidth(h)                GUI_DIM((guii_widget_getwidth(h) - (guii_widget_getpaddingleft(h) + guii_widget_getpaddingright(h))))

/**
 * \brief           Get inner height (total height - padding top - padding bottom)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \retval          Inner height in units of pixels
 * \hideinitializer
 */
#define guii_widget_getinnerheight(h)               GUI_DIM((guii_widget_getheight(h) - (guii_widget_getpaddingtop(h) + guii_widget_getpaddingbottom(h))))

/**
 * \brief           Returns width of parent element. If parent does not exists, it returns LCD width
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \retval          Parent width in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentwidth(h)               GUI_DIM((guii_widget_hasparent(h) ? guii_widget_getwidth(guii_widget_getparent(h)) : GUI.lcd.width))

/**
 * \brief           Returns height of parent element. If parent does not exists, it returns LCD height
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentheight(h)              GUI_DIM((guii_widget_hasparent(h) ? guii_widget_getheight(guii_widget_getparent(h)) : GUI.lcd.height))

/**
 * \brief           Returns inner width of parent element. If parent does not exists, it returns LCD width
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            It returns inner width = total width - padding left - padding right
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \retval          Parent width in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentinnerwidth(h)          GUI_DIM((guii_widget_hasparent(h) ? guii_widget_getinnerwidth(guii_widget_getparent(h)) : GUI.lcd.width))

/**
 * \brief           Returns inner height of parent element. If parent does not exists, it returns LCD height
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            It returns inner height = total height - padding top - padding bottom
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentinnerheight(h)         GUI_DIM((guii_widget_hasparent(h) ? guii_widget_getinnerheight(guii_widget_getparent(h)) : GUI.lcd.height))

/**
 * \brief           Check if widget is visible in any way, either with transparency or hidden flag
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#if GUI_CFG_USE_ALPHA
#define guii_widget_isvisible(h)                    (!guii_widget_getflag(h, GUI_FLAG_HIDDEN) && (h)->alpha)
#else /* GUI_CFG_USE_ALPHA */
#define guii_widget_isvisible(h)                    (!guii_widget_getflag(h, GUI_FLAG_HIDDEN))
#endif /* !GUI_CFG_USE_ALPHA */

/**
 * \brief           Check if widget is hidden
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_ishidden(h)                     (!guii_widget_isvisible(h))

/**
 * \brief           Check if widget allows children widgets
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_allowchildren(h)                (!!guii_widget_getcoreflag(h, GUI_FLAG_WIDGET_ALLOW_CHILDREN))

/**
 * \brief           Check if widget is base for dialog
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_isdialogbase(h)                 (!!(guii_widget_getcoreflag(h, GUI_FLAG_WIDGET_DIALOG_BASE) || guii_widget_getflag(h, GUI_FLAG_WIDGET_DIALOG_BASE)))

/**
 * \brief           Checks if widget handle is currently in focus
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is in focus or not
 * \hideinitializer
 */
#define guii_widget_isfocused(h)                    (!!guii_widget_getflag(h, GUI_FLAG_FOCUS))

/**
 * \brief           Checks if widget handle is currently active
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Status whether widget is active or not
 * \hideinitializer
 */
#define guii_widget_isactive(h)                     (!!guii_widget_getflag(h, GUI_FLAG_ACTIVE))

/**
 * \brief           Check is widget has alpha less than maximum
 * \note            Check if widget is visible and alpha is not set to 1 (full view)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
#define guii_widget_hasalpha(h)                     (guii_widget_isvisible(h) && guii_widget_getalpha(h) < 0xFF)

/**
 * \brief           Get widget alpha value
 * \note            Value between 0 and 0xFF is used:
 *                      - 0x00: Widget is hidden
 *                      - 0xFF: Widget is fully visible
 *                      - between: Widget has alpha value
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          Trasparency value
 * \hideinitializer
 */
#if GUI_CFG_USE_ALPHA || __DOXYGEN__
#define guii_widget_getalpha(h)                     GUI_U8((h)->alpha)
#else /* GUI_CFG_USE_ALPHA || __DOXYGEN__ */
#define guii_widget_getalpha(h)                     GUI_U8(0xFF)
#endif /* GUI_CFG_USE_ALPHA || __DOXYGEN__ */

/**
 * \brief           Get z-index value from widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 * \hideinitializer
 */
#define guii_widget_getzindex(h)                   GUI_I32((h)->zindex)

gui_dim_t       guii_widget_getabsolutex(gui_handle_p h);
gui_dim_t       guii_widget_getabsolutey(gui_handle_p h);
gui_dim_t       guii_widget_getparentabsolutex(gui_handle_p h);
gui_dim_t       guii_widget_getparentabsolutey(gui_handle_p h);
uint8_t         guii_widget_invalidate(gui_handle_p h);
uint8_t         guii_widget_invalidatewithparent(gui_handle_p h);
uint8_t         guii_widget_setinvalidatewithparent(gui_handle_p h, uint8_t value);
uint8_t         guii_widget_setposition(gui_handle_p h, gui_dim_t x, gui_dim_t y);
uint8_t         guii_widget_setpositionpercent(gui_handle_p h, float x, float y);
uint8_t         guii_widget_setxposition(gui_handle_p h, gui_dim_t x);
uint8_t         guii_widget_setxpositionpercent(gui_handle_p h, float x);
uint8_t         guii_widget_setyposition(gui_handle_p h, gui_dim_t y);
uint8_t         guii_widget_setypositionpercent(gui_handle_p h, float y);
uint8_t         guii_widget_setsize(gui_handle_p h, gui_dim_t wi, gui_dim_t hi);
uint8_t         guii_widget_setsizepercent(gui_handle_p h, float wi, float hi);
uint8_t         guii_widget_setwidth(gui_handle_p h, gui_dim_t width);
uint8_t         guii_widget_setheight(gui_handle_p h, gui_dim_t height);
uint8_t         guii_widget_setwidthpercent(gui_handle_p h, float width);
uint8_t         guii_widget_setheightpercent(gui_handle_p h, float height);
uint8_t         guii_widget_set3dstyle(gui_handle_p h, uint8_t enable);
uint8_t         guii_widget_setfont(gui_handle_p h, const gui_font_t* font);
uint8_t         guii_widget_settext(gui_handle_p h, const gui_char* text);
const gui_char *    guii_widget_gettext(gui_handle_p h);
const gui_font_t *  guii_widget_getfont(gui_handle_p h);

uint8_t         guii_widget_alloctextmemory(gui_handle_p h, uint32_t size);
uint8_t         guii_widget_freetextmemory(gui_handle_p h);
void*           guii_widget_create(const gui_widget_t* widget, gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags);
uint8_t         guii_widget_remove(gui_handle_p h);
uint8_t         guii_widget_show(gui_handle_p h);
uint8_t         guii_widget_hide(gui_handle_p h);
uint8_t         guii_widget_hidechildren(gui_handle_p h);
uint8_t         guii_widget_toggleexpanded(gui_handle_p h);
uint8_t         guii_widget_setexpanded(gui_handle_p h, uint8_t state);
uint8_t         guii_widget_ischildof(gui_handle_p h, gui_handle_p parent);
uint8_t         guii_widget_isfontandtextset(gui_handle_p h);

uint8_t         guii_widget_processtextkey(gui_handle_p h, guii_keyboard_data_t* key);
uint8_t         guii_widget_setcolor(gui_handle_p h, uint8_t index, gui_color_t color);
gui_handle_p    guii_widget_getbyid(gui_id_t id);

uint8_t         guii_widget_setalpha(gui_handle_p h, uint8_t trans);
uint8_t         guii_widget_setzindex(gui_handle_p h, int32_t zindex);

gui_dim_t       guii_widget_getwidth(gui_handle_p h);
gui_dim_t       guii_widget_getheight(gui_handle_p h);

uint8_t         guii_widget_setuserdata(gui_handle_p h, void* data);
void *          guii_widget_getuserdata(gui_handle_p h);

uint8_t         guii_widget_setparam(gui_handle_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent);
uint8_t         guii_widget_getparam(gui_handle_p h, uint16_t cfg, void* data);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_TEXT Text management
 * \brief           Text and font management functions
 * \{
 */

uint32_t        gui_widget_alloctextmemory(gui_handle_p h, uint32_t size);
uint8_t         gui_widget_freetextmemory(gui_handle_p h);
uint8_t         gui_widget_settext(gui_handle_p h, const gui_char* text);
const gui_char *    gui_widget_gettext(gui_handle_p h);
const gui_char *    gui_widget_gettextcopy(gui_handle_p h, gui_char* dst, uint32_t len);
uint8_t         gui_widget_setfont(gui_handle_p h, const gui_font_t* font);
const gui_font_t *  gui_widget_getfont(gui_handle_p h);
uint8_t         gui_widget_setfontdefault(const gui_font_t* font);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_SIZE Size management
 * \brief           Size management functions
 * \{
 */

uint8_t         gui_widget_setsize(gui_handle_p h, gui_dim_t width, gui_dim_t height);
uint8_t         gui_widget_setsizepercent(gui_handle_p h, float width, float height);
uint8_t         gui_widget_setwidth(gui_handle_p h, gui_dim_t width);
uint8_t         gui_widget_setheight(gui_handle_p h, gui_dim_t height);
uint8_t         gui_widget_setwidthpercent(gui_handle_p h, float width);
uint8_t         gui_widget_setheightpercent(gui_handle_p h, float height);
gui_dim_t       gui_widget_getwidth(gui_handle_p h);
gui_dim_t       gui_widget_getheight(gui_handle_p h);
uint8_t         gui_widget_setexpanded(gui_handle_p h, uint8_t state);
uint8_t         gui_widget_isexpanded(gui_handle_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_POSITION Position management
 * \brief           Position management functions
 * \{
 */
 
uint8_t         gui_widget_setposition(gui_handle_p h, gui_dim_t x, gui_dim_t y);
uint8_t         gui_widget_setpositionpercent(gui_handle_p h, float x, float y);
uint8_t         gui_widget_setxposition(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setxpositionpercent(gui_handle_p h, float x);
uint8_t         gui_widget_setyposition(gui_handle_p h, gui_dim_t y);
uint8_t         gui_widget_setypositionpercent(gui_handle_p h, float y);
uint8_t         gui_widget_setscrollx(gui_handle_p h, gui_dim_t scroll);
uint8_t         gui_widget_setscrolly(gui_handle_p h, gui_dim_t scroll);
uint8_t         gui_widget_incscrollx(gui_handle_p h, gui_dim_t scroll);
uint8_t         gui_widget_incscrolly(gui_handle_p h, gui_dim_t scroll);
gui_dim_t       gui_widget_getscrollx(gui_handle_p h);
gui_dim_t       gui_widget_getscrolly(gui_handle_p h);

/**
 * \}
 */
 
/**
 * \defgroup        GUI_WIDGET_VISIBILITY Visibility management
 * \brief           Visibility management functions
 * \{
 */

uint8_t         gui_widget_show(gui_handle_p h);
uint8_t         gui_widget_hide(gui_handle_p h);
uint8_t         gui_widget_hidechildren(gui_handle_p h);
uint8_t         gui_widget_putonfront(gui_handle_p h, uint8_t focus);
uint8_t         gui_widget_getalpha(gui_handle_p h);
uint8_t         gui_widget_setalpha(gui_handle_p h, uint8_t alpha);
 
/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_MISC Miscellaneous
 * \brief           Miscellaneous functions
 * \{
 */

uint8_t         gui_widget_invalidate(gui_handle_p h);
uint8_t         gui_widget_setuserdata(gui_handle_p h, void* data);
void *          gui_widget_getuserdata(gui_handle_p h);
uint8_t         gui_widget_ischildof(gui_handle_p h, gui_handle_p parent);
uint8_t         gui_widget_incselection(gui_handle_p h, int16_t dir);
uint8_t         gui_widget_setfocus(gui_handle_p h);
uint8_t         gui_widget_setzindex(gui_handle_p h, int32_t zindex);
int32_t         gui_widget_getzindex(gui_handle_p h);
uint8_t         gui_widget_set3dstyle(gui_handle_p h, uint8_t enable);
gui_id_t        gui_widget_getid(gui_handle_p h);
gui_handle_p    gui_widget_getbyid(gui_id_t id);
uint8_t         gui_widget_remove(gui_handle_p* h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_PADDING Padding
 * \brief           Padding related functions
 * \{
 */
 
uint8_t         gui_widget_setpaddingtop(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingright(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingbottom(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingleft(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingtopbottom(gui_handle_p h, gui_dim_t x);      
uint8_t         gui_widget_setpaddingleftright(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpadding(gui_handle_p h, gui_dim_t x);

gui_dim_t       gui_widget_getpaddingtop(gui_handle_p h);
gui_dim_t       gui_widget_getpaddingright(gui_handle_p h);
gui_dim_t       gui_widget_getpaddingbottom(gui_handle_p h);
gui_dim_t       gui_widget_getpaddingleft(gui_handle_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_CALLBACK Callback processing
 * \brief           Callback based functions
 * \{
 */

uint8_t         gui_widget_processdefaultcallback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);
uint8_t         gui_widget_setcallback(gui_handle_p h, gui_widget_callback_t callback);
uint8_t         gui_widget_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \}
 */
 
#if defined(GUI_INTERNAL) && !__DOXYGEN__
//Strictly private functions by GUI
//Widget initialization purpose
void guii_widget_init(void);

//Clipping regions
uint8_t guii_widget_isinsideclippingregion(gui_handle_p h, uint8_t check_sib_cover);

//Move widget down and all its parents with it
void guii_widget_movedowntree(gui_handle_p h);

void guii_widget_focus_clear(void);
void guii_widget_focus_set(gui_handle_p h);
void guii_widget_active_clear(void);
void guii_widget_active_set(gui_handle_p h);

//Execute actual widget remove process
uint8_t guii_widget_executeremove(void);
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

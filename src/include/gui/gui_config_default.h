/** 
 * \file            gui_config_default.h
 * \brief           Library configuration
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef GUI_HDR_CONFIG_DEFAULT_H
#define GUI_HDR_CONFIG_DEFAULT_H

/**
 * \defgroup        GUI_CONF Configuration
 * \brief           Default configuration setup
 * \{
 */
 
/**
 * \brief           Enables (1) or disables (0) OS support for library
 */
#ifndef GUI_CFG_OS
#define GUI_CFG_OS                              1
#endif

/**
 * \brief           Enables (1) or disables (0) touch support
 */
#ifndef GUI_CFG_USE_TOUCH
#define GUI_CFG_USE_TOUCH                       1
#endif

/**
 * \brief           Enables (1) or disables (0) keyboard support
 */
#ifndef GUI_CFG_USE_KEYBOARD
#define GUI_CFG_USE_KEYBOARD                    0
#endif

/**
 * \brief           Enables (1) or disabled (0) unicode strings
 *
 * \note            UTF-8 encoding can be used for unicode characters
 */
#ifndef GUI_CFG_USE_UNICODE
#define GUI_CFG_USE_UNICODE                     0
#endif

/**
 * \brief           Maximal number of touch entries in buffer
 */
#ifndef GUI_CFG_TOUCH_BUFFER_SIZE
#define GUI_CFG_TOUCH_BUFFER_SIZE               10
#endif

/**
 * \brief           Number of touch presses available at a time
 *                  
 *                  Specifies how many fingers can be detected by touch
 */
#ifndef GUI_CFG_TOUCH_MAX_PRESSES
#define GUI_CFG_TOUCH_MAX_PRESSES               2
#endif

/**
 * \brief           Maximal number of keyboard entries in buffer
 */
#ifndef GUI_CFG_KEYBOARD_BUFFER_SIZE
#define GUI_CFG_KEYBOARD_BUFFER_SIZE            10
#endif 

/**
 * \brief           Enables (1) or disables (0) automatic invalidation of graph widgets
 *                    when graph dataset changes
 *
 * \note            It requires additional memory because each grpah data saves reference
 *                    to parent graph widget for invalidation
 */
#ifndef GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
#define GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE   1
#endif

/**
 * \brief           Enables (1) or disables (0) widget mode inside parent only
 *                  
 *                  When enabled, widget can only be inside parent widget's visible area,
 *                  otherwise widget with width,height = 100,100 may be at x,y = -10,-10 relative to parent
 *                  and only part of widget will be visible
 *
 * \note            This can be used for scrolling mode when necessary
 */
#ifndef GUI_CFG_WIDGET_INSIDE_PARENT
#define GUI_CFG_WIDGET_INSIDE_PARENT            0
#endif

/**
 * \brief           Enables (1) or disables (0) automatic translations on widget text
 *
 * \note            When enabled, source and active languages must be set in \ref GUI_TRANSLATE module
 */
#ifndef GUI_CFG_USE_TRANSLATE
#define GUI_CFG_USE_TRANSLATE                   1
#endif

/**
 * \brief           Enables (1) or disables (0) library custom allocation algorithm.
 *      
 *                  When set to 0, malloc and free functions must be provided by system and sufficient dynamic memory must be resolved by user.
 */
#ifndef GUI_CFG_USE_MEM
#define GUI_CFG_USE_MEM                         1
#endif

/**
 * \brief           Memory alignment setup, used for memory allocation in systems where unaligned memory access is not allowed
 * \note            Value must be power of 2, in most cases number 4 will be ok.
 *
 *                  1: No memory alignment
 *                  2: align memory to 2 bytes
 *                  4: align memory to 4 bytes
 *                  8: align memory to 8 bytes
 */
#ifndef GUI_CFG_MEM_ALIGNMENT
#define GUI_CFG_MEM_ALIGNMENT                   4
#endif

/**
 * \brief           Enables (1) or disables (0) alpha option for widgets
 *
 *                  When enabled, widgets can overlay each other and bottom widget can be visible below top one
 */
#ifndef GUI_CFG_USE_ALPHA
#define GUI_CFG_USE_ALPHA                      0
#endif

/**
 * \brief           Enables (1) or disables (0) widgets' position and size cache
 *
 *                  All position/size values are relative to parent which means
 *                  that when drawing is in process, first step is to calculate
 *                  absolute coordinates on screen. This may take time as
 *                  there might be many parent widgets in tree and in case of many
 *                  widgets on same level, we can enter into huge loop calculations.
 *
 *                  To prevent calculation each time and to save time,
 *                  cache is introduced. In this case, every widget change in position/size values
 *                  will immediately calculate absolute values and save into additional information fields.
 *
 * \note            When modifying widget position/size, all children widgets must be modified too
 *                  as absolute position/size may change on them aswell
 *
 * \note            Enabling this feature significantly reduces calculation time,
 *                  but requires more memory for `4` dimension values (usually `8` bytes) per widget
 */
#ifndef GUI_CFG_USE_POS_SIZE_CACHE
#define GUI_CFG_USE_POS_SIZE_CACHE              0
#endif

/**
 * \brief           Enables `1` or disables `0` widget invalidate ignore after create event
 *
 *                  When every widget is created, minimum position and size must be set thus,
 *                  some invalidation must apply. To make algorithm effective for drawing,
 *                  invalidation may take some time when applying different settings.
 *
 *                  By setting this value to `1`, widget will not be invalidated after creation
 *                  and every attempt to invalidate will have no effect.
 *
 *                  Disable invalidate ignore for a widget after it is created,
 *                  by calling \ref gui_widget_setignoreinvalidate
 *
 *                  When feature is applied, user must do the following
 * \code{c}
gui_handle_p h;

//Create widget with flag to prevent invalidation
h = gui_somewidget_create(....);

//If any of these functions try to invalidate widget, 
//it will have no effect thus code might work faster
gui_somewidget_setparam1(h, ...);
gui_somewidget_setparam2(h, ...);
gui_somewidget_setparam3(h, ...);

//Disable ignore invalidate and force invalidation
gui_widget_setignoreinvalidate(h, 0, 1, 0);
\endcode
 */
#ifndef GUI_CFG_WIDGET_CREATE_IGNORE_INVALIDATE
#define GUI_CFG_WIDGET_CREATE_IGNORE_INVALIDATE 0
#endif

/**
 * \brief           Long click timeout in units of milliseconds
 *
 *                  Value indicates number of milliseconds of pressed state before
 *                  long click event is detected
 */
#ifndef GUI_CFG_LONG_CLICK_TIMEOUT
#define GUI_CFG_LONG_CLICK_TIMEOUT              1500
#endif

#ifndef GUI_CFG_SYS_PORT
#define GUI_CFG_SYS_PORT                        GUI_SYS_PORT_CMSIS_OS
#endif

/**
 * \}
 */

#endif /* GUI_HDR_CONFIG_DEFAULT_H */

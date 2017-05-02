/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI configuration
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
#ifndef GUI_CONF_H
#define GUI_CONF_H

/**
 * \addtogroup      GUI
 * \{
 */

/**
 * \defgroup        GUI_CONF Configuration
 * \brief           GUI configuration setup
 * \{
 */

/**
 * \brief           Enables (1) or disables (0) touch support
 */
#define GUI_USE_TOUCH                   1

/**
 * \brief           Enables (1) or disables (0) keyboard support
 */
#define GUI_USE_KEYBOARD                1

/**
 * \brief           Enables (1) or disabled (0) unicode strings
 *
 * \note            UTF-8 encoding can be used for unicode characters
 */
#define GUI_USE_UNICODE                 1

/**
 * \brief           Maximal number of touch entries in buffer
 */
#define GUI_TOUCH_BUFFER_SIZE           10

/**
 * \brief           Number of touch presses available at a time
 *                  
 *                  Specifies how many fingers can be detected by touch
 */
#define GUI_TOUCH_MAX_PRESSES           2

/**
 * \brief           Maximal number of keyboard entries in buffer
 */
#define GUI_KEYBOARD_BUFFER_SIZE        10

/**
 * \brief           Enables (1) or disables (0) automatic invalidation of graph widgets
 *                    when graph dataset changes
 *
 * \note            It requires additional memory because each grpah data saves reference
 *                    to parent graph widget for invalidation
 */
#define GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE       1

/**
 * \brief           Enables (1) or disables (0) widget mode inside parent only
 *                  When mode is enabled and widget is outside parent, it won't be visible
 *
 * \note            This can be used for scrolling mode when necessary
 */
#define GUI_WIDGET_INSIDE_PARENT        0

/**
 * \}
 */
 
/**
 * \}
 */

#endif

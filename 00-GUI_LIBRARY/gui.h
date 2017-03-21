/**
 * \author  Tilen Majerle
 * \website http://majerle.eu
 * \license MIT
 * \version 0.1.0
 * \brief   GUI Library
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
#ifndef GUI_H
#define GUI_H 010

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI
 * \brief           Platform independent, written in ANSII C, GUI AT parser library for SIMcom modules   
 * \{
 */

/* Important GUI definitions */
#include "gui_defs.h"
    
/* Proto threads */
#include "pt/pt.h"
/* GUI configuration */
#include "gui_config.h"
    
/* Include utilities */
#include "utils/gui_buffer.h"
#include "utils/gui_linkedlist.h"
#include "utils/gui_string.h"

/* GUI Low-Level drivers */
#include "gui_ll.h"
#include "tm_stm32_general.h"

#if !defined(DOXYGEN)
/**
 * \defgroup        GUI_Internal   
 * \{
 */
#define __GUI_ENTER()
#define __GUI_LEAVE()

#define __GUI_DEBUG(fmt, ...)       printf(fmt, ##__VA_ARGS__)

/**
 * \brief           GUI Handle object from main object
 */ 
#define __GH(x)                     ((GUI_HANDLE_t)(x))

/**
 * \brief           GUI Handle root object from main object with children widgets
 */ 
#define __GHR(x)                    ((GUI_HANDLE_ROOT_t *)(x))

/**
 * \brief           Allocate memory with specific size
 */
#define __GUI_MEMALLOC(size)        malloc(size)

/**
 * \brief           Free memory from specific address
 */
#define __GUI_MEMFREE(p)            free(p)

/**
 * \brief           Free memory for widget which was just deleted
 */
#define __GUI_MEMWIDFREE(p)         do {            \
    free(p);                                        \
    p = 0;                                          \
} while (0)

/**
 * \brief           Check input parameters and return value on failure
 */
#define __GUI_ASSERTPARAMS(c)       do {            \
    if (!(c)) {                                     \
        __GUI_DEBUG("Assert param failed in file %s and line %d\r\n", __FILE__, __LINE__);  \
        return 0;                                   \
    }                                               \
} while (0)

/**
 * \brief           Check input parameters and return void on failure
 */
#define __GUI_ASSERTPARAMSVOID(c)   do {            \
    if (!(c)) {                                     \
        __GUI_DEBUG("Assert param failed in file %s and line %d\r\n", __FILE__, __LINE__);  \
        return;                                     \
    }                                               \
} while (0)

/**
 * \brief           Check if window is active for widget create
 */
#define __GUI_ASSERTACTIVEWIN()     do {            \
    if (!GUI.WindowActive) {                        \
        __GUI_DEBUG("There is no active window for widget in file %s on line %d\r\n", __FILE__, __LINE__);  \
        return NULL;                                \
    }                                               \
} while (0)

#define __GUI_MAX(x, y)             ((x) > (y) ? (x) : (y))
#define __GUI_MIN(x, y)             ((x) < (y) ? (x) : (y))
#define __GUI_ABS(x)                ((x) >= 0 ? (x) : -(x))

/**
 * \brief           Check if 2 rectangle objects covers each other in any way
 */
#define __GUI_RECT_MATCH(x1, y1, w1, h1, x2, y2, w2, h2)    \
    !(                                                      \
        (x1) > ((x2) + (w2)) ||                             \
        (x2) > ((x1) + (w1)) ||                             \
        (y1) > ((y2) + (h2)) ||                             \
        (y2) > ((y1) + (h1))                                \
    )

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 1 parameter
 */
#define __GUI_UNUSED(x)             (void)(x)

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 2 parameters
 */
#define __GUI_UNUSED2(x, y)         { __GUI_UNUSED(x); __GUI_UNUSED(y); }

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 3 parameters
 */
#define __GUI_UNUSED3(x, y, z)      { __GUI_UNUSED(x); __GUI_UNUSED(y); __GUI_UNUSED(z); }

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 4 parameters
 */
#define __GUI_UNUSED4(x, y, z, k)   { __GUI_UNUSED(x); __GUI_UNUSED(y); __GUI_UNUSED(z); __GUI_UNUSED(k); }

/**
 * \} GUI_Internal
 */
#endif /* !defined(DOXYGEN) */

/**
 * \brief           GUI main object structure
 */
typedef struct GUI_t {
    uint32_t Time;                          /*!< Current time in units of milliseconds */
    GUI_LCD_t LCD;                          /*!< LCD low-level settings */
    GUI_LL_t LL;                            /*!< Low-level drawing routines for LCD */
    GUI_Display_t Display;                  /*!< Clipping management if exists */
    
    GUI_HANDLE_t WindowActive;              /*!< Pointer to currently active window when creating new widgets */
    GUI_HANDLE_t FocusedWidget;             /*!< Pointer to focused widget for keyboard events if any */
    GUI_HANDLE_t ActiveWidget;              /*!< Pointer to widget currently active by mouse or touch press */
    
    GUI_LinkedListRoot_t Root;              /*!< Root linked list widget */
} GUI_t;
extern GUI_t GUI;

/* Include widget structure */
#include "widgets/gui_widget.h"
#include "input/gui_input.h"

/**
 * \brief           Initializes GUI stack.
 *                    In additions, it prepares memory for work with widgets on later usage and
 *                    calls low-layer functions to initialize LCD or custom driver for LCD
 * \retval          Member of \ref GUI_Result_t enumeration
 */
GUI_Result_t GUI_Init(void);

/**
 * \brief           Processes all drawing operations for GUI
 * \retval          Number of jobs done in current call
 */
int32_t GUI_Process(void);

//Notify GUI from low-level that layer is in use
void GUI_LCD_ConfirmActiveLayer(GUI_Byte layer_num);
 
/**
 * \} GUI
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

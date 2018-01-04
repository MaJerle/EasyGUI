/**	
 * \file            gui.h
 * \brief           Core header file
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
#ifndef GUI_H
#define GUI_H 010

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI EasyGUI
 * \brief           EasyGUI Graphical User Interface for embedded systems
 * \{
 */

/* GUI configuration */
#include "gui_config.h"
    
/* Important GUI definitions */
#include "gui/gui_defs.h"
    
/* Proto threads */
#include "pt/pt.h"
    
/* Include utilities */
#include "gui/gui_buffer.h"
#include "gui/gui_linkedlist.h"
#include "gui/gui_string.h"
#include "gui/gui_timer.h"
#include "gui/gui_math.h"
#include "gui/gui_mem.h"
#include "gui/gui_translate.h"

/* GUI Low-Level drivers */
#include "system/gui_ll.h"

//#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \defgroup        GUI_Internal Internal macros 
 * \brief           List of internal macros which can be used only inside GUI library and are not visible by user outside GUI library
 * \{
 */

#if GUI_OS

#define __GUI_SYS_PROTECT()     gui_sys_protect()
#define __GUI_SYS_UNPROTECT()   gui_sys_unprotect()
#define __GUI_ENTER()           __GUI_SYS_PROTECT()
#define __GUI_LEAVE()           __GUI_SYS_UNPROTECT()

#else

#define __GUI_ENTER()
#define __GUI_LEAVE()
#define __GUI_SYS_PROTECT()
#define __GUI_SYS_UNPROTECT()

#endif /* GUI_OS */

/**
 * \brief           Debugging macro for inside GUI
 * \param[in]       fmt: Pointer to string with modifiers
 * \param[in]       ...: List of parameters for all the modifiers in fmt pointer
 * \note            When RTOS is not used, this macro has no meaning
 * \hideinitializer
 */
#define GUI_DEBUG(fmt, ...)         printf(fmt, ##__VA_ARGS__)

/**
 * \brief           GUI Handle object from main object
 * \retval          Pointer to GUI handle
 * \hideinitializer
 */ 
#define __GH(x)                     ((struct GUI_HANDLE *)(x))

/**
 * \brief           GUI Handle root object from main object with children widgets
 * \retval          Pointer to GUI root handle
 * \hideinitializer
 */ 
#define __GHR(x)                    ((struct GUI_HANDLE_ROOT *)(x))

/**
 * \brief           Allocate memory with specific size in bytes
 * \note            This function must take care of reseting memory to zero
 * \hideinitializer
 */
#define GUI_MEMALLOC(size)        gui_mem_calloc(size, 1)

/**
 * \brief           Reallocate memory with specific size in bytes
 * \hideinitializer
 */
#define GUI_MEMREALLOC(ptr, size) gui_mem_realloc(ptr, size)

/**
 * \brief           Free memory from specific address previously allocated with \ref GUI_MEMALLOC or \ref GUI_MEMREALLOC
 * \hideinitializer
 */
#define GUI_MEMFREE(p)              do {            \
    gui_mem_free(p);                                \
    (p) = NULL;                                     \
} while (0);

/**
 * \brief           Check input parameters and return value on failure
 * \hideinitializer
 */
#define __GUI_ASSERTPARAMS(c)       do {            \
    if (!(c) || !(GUI.Initialized)) {                                     \
        GUI_DEBUG("Assert param failed in file %s and line %d\r\n", (const char *)__FILE__, (unsigned)__LINE__);  \
        return 0;                                   \
    }                                               \
} while (0)

/**
 * \brief           Check if window is active for widget create
 * \hideinitializer
 */
#define __GUI_ASSERTACTIVEWIN()     do {            \
    if (!GUI.WindowActive) {                        \
        __GUI_DEBUG("There is no active window for widget in file %s on line %d\r\n", __FILE__, __LINE__);  \
        return NULL;                                \
    }                                               \
} while (0)

/**
 * \brief           Get maximal value between 2 values
 * \param[in]       x: First value
 * \param[in]       y: Second value
 * \retval          Maximal value
 * \hideinitializer
 */
#define GUI_MAX(x, y)               ((x) > (y) ? (x) : (y))

/**
 * \brief           Get minimal value between 2 values
 * \param[in]       x: First value
 * \param[in]       y: Second value
 * \retval          Minimal value
 * \hideinitializer
 */
#define GUI_MIN(x, y)               ((x) < (y) ? (x) : (y))

/**
 * \brief           Get absolute value of input
 * \param[in]       x: Input value
 * \retval          Absolute value of input
 * \hideinitializer
 */
#define GUI_ABS(x)                  ((x) >= 0 ? (x) : -(x))

/**
 * \brief           Check if 2 rectangle objects covers each other in any way
 * \hideinitializer
 */
#define __GUI_RECT_MATCH(h1x1, h1y1, h1x2, h1y2, h2x1, h2y1, h2x2, h2y2)    \
    !(                                                      \
        (h1x1) > (h2x2) ||                                  \
        (h1y1) > (h2y2) ||                                  \
        (h2x1) > (h1x2) ||                                  \
        (h2y1) > (h1y2)                                  \
    )

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 1 parameter
 * \hideinitializer
 */
#define GUI_UNUSED(x)               (void)(x)

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 2 parameters
 * \hideinitializer
 */
#define GUI_UNUSED2(x, y)           { GUI_UNUSED(x); GUI_UNUSED(y); }

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 3 parameters
 * \hideinitializer
 */
#define GUI_UNUSED3(x, y, z)        { GUI_UNUSED2(x, y); GUI_UNUSED(z); }

/**
 * \brief           Macro for unused variables to prevent compiler warnings
 * \note            It uses 4 parameters
 * \hideinitializer
 */
#define GUI_UNUSED4(x, y, z, k)     { GUI_UNUSED3(x, y, z); GUI_UNUSED(k); }

/**
 * \brief           Round value to the nearest integer and return it as float (can be casted to int without problems)
 * \param[in]       x: Value to round
 * \retval          Rounded value
 */
#define GUI_ROUND(x)                ((float)(x) + 0.5f)

/**
 * \brief           Align x value to specific number of bits, provided from \ref GUI_MEM_ALIGNMENT configuration
 * \param[in]       x: Input value to align
 * \retval          Input value aligned to specific number of bytes
 */
#define GUI_MEM_ALIGN(x)            ((x + (GUI_MEM_ALIGNMENT - 1)) & ~(GUI_MEM_ALIGNMENT - 1))

/**
 * \}
 */
//#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

#if GUI_OS
#include "system/gui_system.h"

#define GUI_SYS_MBOX_TYPE_TOUCH             0x01
#define GUI_SYS_MBOX_TYPE_KEYBOARD          0x02
#define GUI_SYS_MBOX_TYPE_REMOVE            0x03
#define GUI_SYS_MBOX_TYPE_TIMER             0x04
#define GUI_SYS_MBOX_TYPE_WIDGET_CREATED    0x05
#define GUI_SYS_MBOX_TYPE_INVALIDATE        0x06

/**
 * \brief           Message box data type
 */
typedef struct gui_mbox_msg_t {
    uint32_t type;                          /*!< Message type */
} gui_mbox_msg_t;

/**
 * \brief           OS dependant variables
 */
typedef struct GUI_OS_t {
    gui_sys_thread_t thread_id;             /*!< GUI thread ID */
    gui_sys_mbox_t mbox;                    /*!< Operating system message box */
} GUI_OS_t;
#endif /* GUI_OS */

/**
 * \brief           GUI main object structure
 */
typedef struct GUI_t {
    GUI_LCD_t LCD;                          /*!< LCD low-level settings */
    GUI_LL_t LL;                            /*!< Low-level drawing routines for LCD */
    
    uint32_t Flags;                         /*!< Core GUI flags management */
    
    GUI_Display_t Display;                  /*!< Clipping management */
    GUI_Display_t DisplayTemp;              /*!< Clipping for widgets for drawing and touch */
    
    GUI_HANDLE_p WindowActive;              /*!< Pointer to currently active window when creating new widgets */
    GUI_HANDLE_p FocusedWidget;             /*!< Pointer to focused widget for keyboard events if any */
    GUI_HANDLE_p FocusedWidgetPrev;         /*!< Pointer to previously focused widget */
    
    GUI_LinkedListRoot_t Root;              /*!< Root linked list of widgets */
    GUI_TIMER_CORE_t Timers;                /*!< Software structure management */
    
    GUI_LinkedListRoot_t RootFonts;         /*!< Root linked list of font widgets */
    
    GUI_WIDGET_PARAM_t WidgetParam;
    GUI_WIDGET_RESULT_t WidgetResult;
    
#if GUI_USE_TOUCH || defined(DOXYGEN)
    __GUI_TouchData_t TouchOld;             /*!< Old touch data, used for event management */
    __GUI_TouchData_t Touch;                /*!< Current touch data and processing tool */
    GUI_HANDLE_p ActiveWidget;              /*!< Pointer to widget currently active by touch */
    GUI_HANDLE_p ActiveWidgetPrev;          /*!< Previously active widget */
#endif /* GUI_USE_TOUCH */

#if GUI_USE_TRANSLATE
    GUI_TRANSLATE_t Translate;              /*!< Translation management structure */
#endif /* GUI_USE_TRANSLATE */
    
#if GUI_OS
    GUI_OS_t OS;                            /*!< Operating system dependant structure */
#endif /* GUI_OS */

    GUI_EventCallback_t EventCb;            /*!< Pointer to global GUI event callback function */
    
    uint8_t Initialized;                    /*!< Status indicating GUI is initialized */
} GUI_t;
#if defined(GUI_INTERNAL)
extern GUI_t GUI;

#endif /* defined(GUI_INTERNAL) */

/* Include widget structure */
#include "widget/gui_widget.h"
#include "gui/gui_input.h"

guir_t gui_init(void);
int32_t gui_process(void);
uint8_t gui_seteventcallback(GUI_EventCallback_t cb);
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

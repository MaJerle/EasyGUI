/**	
 * \file            gui_private.h
 * \brief           Private functions and enumeration
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
#ifndef GUI_PRIVATE_H
#define GUI_PRIVATE_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_PRIVATE Private functions
 * \brief           Private functions and enumerations
 * \{
 */

#include "gui/gui.h"

#if defined(GUI_INTERNAL) || __DOXYGEN__

#if GUI_CFG_OS
#include "system/gui_sys.h"

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
typedef struct GUI_CFG_OS_t {
    gui_sys_thread_t thread_id;             /*!< GUI thread ID */
    gui_sys_mbox_t mbox;                    /*!< Operating system message box */
} GUI_CFG_OS_t;
#endif /* GUI_CFG_OS */

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
    
#if GUI_CFG_USE_TOUCH || __DOXYGEN__
    __GUI_TouchData_t TouchOld;             /*!< Old touch data, used for event management */
    __GUI_TouchData_t Touch;                /*!< Current touch data and processing tool */
    GUI_HANDLE_p ActiveWidget;              /*!< Pointer to widget currently active by touch */
    GUI_HANDLE_p ActiveWidgetPrev;          /*!< Previously active widget */
#endif /* GUI_CFG_USE_TOUCH */

#if GUI_CFG_USE_TRANSLATE
    GUI_TRANSLATE_t Translate;              /*!< Translation management structure */
#endif /* GUI_CFG_USE_TRANSLATE */
    
#if GUI_CFG_OS
    GUI_CFG_OS_t OS;                            /*!< Operating system dependant structure */
#endif /* GUI_CFG_OS */

    GUI_EventCallback_t EventCb;            /*!< Pointer to global GUI event callback function */
    
    uint8_t Initialized;                    /*!< Status indicating GUI is initialized */
} GUI_t;

extern GUI_t GUI;



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

#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_UTILS_H */

/**	
 * \file            gui_private.h
 * \brief           Private functions and enumeration
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
#ifndef GUI_HDR_PRIVATE_H
#define GUI_HDR_PRIVATE_H

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
#include "gui/gui_text.h"

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
typedef struct {
    uint32_t type;                          /*!< Message type */
} gui_mbox_msg_t;

/**
 * \brief           OS dependant variables
 */
typedef struct {
    gui_sys_thread_t thread_id;             /*!< GUI thread ID */
    gui_sys_mbox_t mbox;                    /*!< Operating system message box */
} GUI_OS_t;
#endif /* GUI_CFG_OS */

/**
 * \brief           GUI main object structure
 */
typedef struct {
    gui_lcd_t lcd;                          /*!< LCD low-level settings */
    gui_ll_t ll;                            /*!< Low-level drawing routines for LCD */
    
    uint32_t flags;                         /*!< Core GUI flags management */
    
    gui_display_t display;                  /*!< Clipping management */
    gui_display_t display_temp;             /*!< Clipping for widgets for drawing and touch, used for drawing area of current widget */
    
    gui_handle_p window_active;             /*!< Pointer to currently active window when creating new widgets */
    gui_handle_p focused_widget;            /*!< Pointer to focused widget for keyboard events if any */
    gui_handle_p focused_widget_prev;       /*!< Pointer to previously focused widget */
    
    gui_linkedlistroot_t root;              /*!< Root linked list of widgets */
    gui_timer_core_t timers;                /*!< Software structure management */
    
    gui_linkedlistroot_t root_fonts;        /*!< Root linked list of font widgets */
    
    gui_evt_param_t evt_param;
    gui_evt_result_t evt_result;
    
#if GUI_CFG_USE_TOUCH || __DOXYGEN__
    gui_touch_data_t touch_old;             /*!< Old touch data, used for event management */
    guii_touch_data_t touch;                /*!< Current touch data and processing tool */
    gui_handle_p active_widget;             /*!< Pointer to widget currently active by touch */
    gui_handle_p active_widget_prev;        /*!< Previously active widget */
#endif /* GUI_CFG_USE_TOUCH */

#if GUI_CFG_USE_TRANSLATE
    gui_translate_t translate;              /*!< Translation management structure */
#endif /* GUI_CFG_USE_TRANSLATE */
    
#if GUI_CFG_OS
    GUI_OS_t OS;                            /*!< Operating system dependant structure */
#endif /* GUI_CFG_OS */

    gui_eventcallback_t evt_cb;             /*!< Pointer to global GUI event callback function */
    
    uint8_t initialized;                    /*!< Status indicating GUI is initialized */
} gui_t;

extern gui_t GUI;

/**
 * \brief           Check if 2 rectangle objects covers each other in any way
 * \hideinitializer
 */
#define GUI_RECT_MATCH(h1x1, h1y1, h1x2, h1y2, h2x1, h2y1, h2x2, h2y2)    \
    !(                                                      \
        (h1x1) > (h2x2) ||                                  \
        (h1y1) > (h2y2) ||                                  \
        (h2x1) > (h1x2) ||                                  \
        (h2y1) > (h1y2)                                  \
    )

/**
 * \brief           Check if first rectangle is fully inside second rectangle
 * \hideinitializer
 */
#define GUI_RECT_IS_INSIDE(h1x1, h1y1, h1x2, h1y2, h2x1, h2y1, h2x2, h2y2)    (((h1x1) >= (h2x1)) && ((h1x2) <= (h2x2)) && ((h1y1) >= (h2y1)) && ((h1y2) <= (h2y2)))

/**
 * \brief           GUI Handle object from main object
 * \retval          Pointer to GUI handle
 * \hideinitializer
 */ 
#define __GH(x)                     ((struct gui_handle *)(x))

/**
 * \brief           Check input parameters and return value on failure
 * \hideinitializer
 */
#define GUI_ASSERTPARAMS(c)         do {            \
    if (!(c) || !(GUI.initialized)) {               \
        GUI_DEBUG("Assert param failed in file %s and line %d\r\n", (const char *)__FILE__, (int)__LINE__); \
        return 0;                                   \
    }                                               \
} while (0)

/**
 * \brief           Check if window is active for widget create
 * \hideinitializer
 */
#define GUI_ASSERTACTIVEWIN()       do {            \
    if (GUI.window_active == NULL) {                \
        GUI_DEBUG("There is no active window for widget in file %s on line %d\r\n", (const char *)__FILE__, (int)__LINE__); \
        return NULL;                                \
    }                                               \
} while (0)

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_PRIVATE_H */

/**	
 * \file            gui.h
 * \brief           Core header file
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
#ifndef GUI_HDR_H
#define GUI_HDR_H

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
#include "gui/pt/pt.h"
    
/* Include utilities */
#include "gui/gui_buff.h"
#include "gui/gui_linkedlist.h"
#include "gui/gui_string.h"
#include "gui/gui_timer.h"
#include "gui/gui_math.h"
#include "gui/gui_mem.h"
#include "gui/gui_translate.h"

/* GUI Low-Level drivers */
#include "system/gui_ll.h"
#include "system/gui_sys.h"

//#if defined(GUI_INTERNAL) || __DOXYGEN__
/**
 * \defgroup        GUI_Internal Internal macros 
 * \brief           List of internal macros which can be used only inside GUI library and are not visible by user outside GUI library
 * \{
 */

#if GUI_CFG_OS
#define GUI_CORE_PROTECT(p)         if (p) gui_sys_protect()
#define GUI_CORE_UNPROTECT(p)       if (p) gui_sys_unprotect()
#else /* GUI_CFG_OS */
#define GUI_CORE_PROTECT(p)         GUI_UNUSED(p)
#define GUI_CORE_UNPROTECT(p)       GUI_UNUSED(p)
#endif /* !GUI_CFG_OS */

/**
 * \brief           Debugging macro for inside GUI
 * \param[in]       fmt: Pointer to string with modifiers
 * \param[in]       ...: List of parameters for all the modifiers in fmt pointer
 * \note            When RTOS is not used, this macro has no meaning
 * \hideinitializer
 */
#define GUI_DEBUG(fmt, ...)         printf(fmt, ##__VA_ARGS__)

/**
 * \brief           Allocate memory with specific size in bytes
 * \note            This function must take care of reseting memory to zero
 * \hideinitializer
 */
#define GUI_MEMALLOC(size)         gui_mem_calloc(1, size)

/**
 * \brief           Reallocate memory with specific size in bytes
 * \hideinitializer
 */
#define GUI_MEMREALLOC(ptr, size)  gui_mem_realloc(ptr, size)

/**
 * \brief           Free memory from specific address previously allocated with \ref GUI_MEMALLOC or \ref GUI_MEMREALLOC
 * \hideinitializer
 */
#define GUI_MEMFREE(p)     do {                     \
    gui_mem_free(p);                                \
    (p) = NULL;                                     \
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
 * \brief           align x value to specific number of bits, provided from \ref GUI_CFG_MEM_ALIGNMENT configuration
 * \param[in]       x: Input value to align
 * \retval          Input value aligned to specific number of bytes
 */
#define GUI_MEM_ALIGN(x)            ((x + (GUI_CFG_MEM_ALIGNMENT - 1)) & ~(GUI_CFG_MEM_ALIGNMENT - 1))

/**
 * \}
 */
//#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

/* Include widget structure */
#include "widget/gui_widget.h"
#include "gui/gui_input.h"

guir_t      gui_init(void);
int32_t     gui_process(void);
uint8_t     gui_seteventcallback(gui_eventcallback_t cb);

#if GUI_CFG_OS || __DOXYGEN__
uint8_t     gui_protect(const uint8_t protect);
uint8_t     gui_unprotect(const uint8_t unprotect);
uint8_t     gui_delay(const uint32_t ms);
#else /* GUI_CFG_OS || __DOXYGEN__ */
/* Empty macros */
#define     gui_protect(protect)
#define     gui_unprotect(unprotect)
#endif /* !(GUI_CFG_OS || __DOXYGEN__) */
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_H */

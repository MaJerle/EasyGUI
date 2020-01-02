/**	
 * \file            gui_sys.h
 * \brief           CMSIS os implementation of system functions
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
 * This file is part of GUI-AT.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef GUI_HDR_SYSTEM_H
#define GUI_HDR_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdlib.h>

#include "gui_config.h"

/**
 * \ingroup         GUI_PORT Porting
 * \defgroup        GUI_SYS System functions
 * \brief           System based function for OS management, timings, etc
 * \{
 */

/**
 * \brief           Thread function prototype
 */
typedef void (*gui_sys_thread_fn)(void *);

/**
 * \anchor          GUI_SYS_PORTS
 * \name            System ports
 * \{
 *
 * List of already available system ports. 
 * Configure \ref GUI_CFG_SYS_PORT with one of these values to use preconfigured ports
 */
 
#define GUI_SYS_PORT_CMSIS_OS               1   /*!< CMSIS-OS based port for OS systems capable of ARM CMSIS standard */
#define GUI_SYS_PORT_WIN32                  2   /*!< WIN32 based port to use ESP library with Windows applications */

/* Decide which port to include */
#if GUI_CFG_SYS_PORT == GUI_SYS_PORT_CMSIS_OS
#include "system/gui_sys_cmsis_os.h"
#elif GUI_CFG_SYS_PORT == GUI_SYS_PORT_WIN32
#include "system/gui_sys_win32.h"
#endif

/**
 * \}
 */

uint8_t     gui_sys_init(void);
uint32_t    gui_sys_now(void);

#if GUI_CFG_OS || __DOXYGEN__

uint8_t     gui_sys_protect(void);
uint8_t     gui_sys_unprotect(void);

uint8_t     gui_sys_mutex_create(gui_sys_mutex_t* p);
uint8_t     gui_sys_mutex_delete(gui_sys_mutex_t* p);
uint8_t     gui_sys_mutex_lock(gui_sys_mutex_t* p);
uint8_t     gui_sys_mutex_unlock(gui_sys_mutex_t* p);
uint8_t     gui_sys_mutex_isvalid(gui_sys_mutex_t* p);
uint8_t     gui_sys_mutex_invalid(gui_sys_mutex_t* p);

uint8_t     gui_sys_sem_create(gui_sys_sem_t* p, uint8_t cnt);
uint8_t     gui_sys_sem_delete(gui_sys_sem_t* p);
uint32_t    gui_sys_sem_wait(gui_sys_sem_t* p, uint32_t timeout);
uint8_t     gui_sys_sem_release(gui_sys_sem_t* p);
uint8_t     gui_sys_sem_isvalid(gui_sys_sem_t* p);
uint8_t     gui_sys_sem_invalid(gui_sys_sem_t* p);

uint8_t     gui_sys_mbox_create(gui_sys_mbox_t* b, size_t size);
uint8_t     gui_sys_mbox_delete(gui_sys_mbox_t* b);
uint32_t    gui_sys_mbox_put(gui_sys_mbox_t* b, void* m);
uint32_t    gui_sys_mbox_get(gui_sys_mbox_t* b, void** m, uint32_t timeout);
uint8_t     gui_sys_mbox_putnow(gui_sys_mbox_t* b, void* m);
uint8_t     gui_sys_mbox_getnow(gui_sys_mbox_t* b, void** m);
uint8_t     gui_sys_mbox_isvalid(gui_sys_mbox_t* b);
uint8_t     gui_sys_mbox_invalid(gui_sys_mbox_t* b);

uint8_t     gui_sys_thread_create(gui_sys_thread_t* t, const char* name, gui_sys_thread_fn thread_fn, void* const arg, size_t stack_size, gui_sys_thread_prio_t prio);
uint8_t		gui_sys_thread_terminate(gui_sys_thread_t* t);
uint8_t 	gui_sys_thread_yield(void);

#endif /* GUI_CFG_OS || __DOXYGEN__ */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_HDR_SYSTEM_H */

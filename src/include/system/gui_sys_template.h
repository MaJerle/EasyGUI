/**	
 * \file            gui_sys_template.h
 * \brief           Template file for system functions
 */

/*
 * Copyright (c) 2018 Tilen Majerle
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
 */
#ifndef __GUI_SYSTEM_H
#define __GUI_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdint.h"
#include "stdlib.h"

#include "gui_config.h"

/**
 * \addtogroup      GUI_PORT
 * \{
 */
 
/**
 * \defgroup        GUI_SYS System functions
 * \brief           System based function for OS management, timings, etc
 * \{
 */

#if GUI_CFG_OS || __DOXYGEN__
#include "cmsis_os.h"

/**
 * \brief           GUI system mutex ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef osMutexId           gui_sys_mutex_t;

/**
 * \brief           GUI system semaphore ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef osSemaphoreId       gui_sys_sem_t;

/**
 * \brief           GUI system message queue ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef osMessageQId        gui_sys_mbox_t;

/**
 * \brief           GUI system thread ID type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef osThreadId          gui_sys_thread_t;

/**
 * \brief           GUI system thread priority type
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
typedef osPriority          gui_sys_thread_prio_t;

/**
 * \brief           Value indicating message queue is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GUI_SYS_MBOX_NULL           (osMessageQId)0

/**
 * \brief           Value indicating semaphore is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GUI_SYS_SEM_NULL            (osSemaphoreId)0

/**
 * \brief           Value indicating mutex is not valid
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GUI_SYS_MUTEX_NULL          (osMutexId)0

/**
 * \brief           Value indicating timeout for OS timings
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GUI_SYS_TIMEOUT             ((uint32_t)osWaitForever)

/**
 * \brief           GUI stack threads priority parameter
 * \note            Usually normal priority is ok. If many threads are in the system and high traffic is introduced
 *                  This value might need to be set to higher value
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GUI_SYS_THREAD_PRIO         (osPriorityNormal)

/**
 * \brief           Stack size of system threads
 * \note            Keep as is in case of CMSIS based OS, otherwise change for your OS
 */
#define GUI_SYS_THREAD_SS           (1024)
#endif /* GUI_OS || __DOXYGEN__ */

uint8_t     gui_sys_init(void);
uint32_t    gui_sys_now(void);

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

uint8_t     gui_sys_thread_create(gui_sys_thread_t* t, const char* name, void(*thread_func)(void *), void* const arg, size_t stack_size, gui_sys_thread_prio_t prio);
 
/**
 * \}
 */
 
/**
 * \}
 */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __GUI_SYSTEM_H */

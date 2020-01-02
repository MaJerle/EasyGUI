/**	
 * \file            gui_sys_cmsis_os.h
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef GUI_HDR_SYSTEM_CMSIS_OS_H
#define GUI_HDR_SYSTEM_CMSIS_OS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdlib.h>

#include "gui_config.h"

#if GUI_CFG_OS && !__DOXYGEN__
	
#include "cmsis_os.h"

typedef osMutexId                   gui_sys_mutex_t;
typedef osSemaphoreId               gui_sys_sem_t;
typedef osMessageQId                gui_sys_mbox_t;
typedef osThreadId                  gui_sys_thread_t;
typedef osPriority                  gui_sys_thread_prio_t;
#define GUI_SYS_MBOX_NULL           (osMessageQId)0
#define GUI_SYS_SEM_NULL            (osSemaphoreId)0
#define GUI_SYS_MUTEX_NULL          (osMutexId)0
#define GUI_SYS_TIMEOUT             ((uint32_t)osWaitForever)
#define GUI_SYS_THREAD_PRIO         (osPriorityNormal)
#define GUI_SYS_THREAD_SS           (1024)

#endif /* GUI_CFG_OS && !__DOXYGEN__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_HDR_SYSTEM_CMSIS_OS_H */

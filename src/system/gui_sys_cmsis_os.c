/**	
 * \file            gui_sys_cmsis_os.c
 * \brief           System dependant functions for CMSIS OS based RTOS
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
#define GUI_INTERNAL
#include "system/gui_sys.h"
#include "stm32f7xx_hal.h"

#if !__DOXYGEN__

#if GUI_CFG_OS
static osMutexId sys_mutex;
#endif /* GUI_CFG_OS */

uint8_t
gui_sys_init(void) {
#if GUI_CFG_OS
    gui_sys_mutex_create(&sys_mutex);
#endif /* GUI_CFG_OS */
    return 1;
}

uint32_t
gui_sys_now(void) {
#if GUI_CFG_OS
    return osKernelSysTick();
#else
    return HAL_GetTick();
#endif
}

#if GUI_CFG_OS

uint8_t
gui_sys_protect(void) {
    gui_sys_mutex_lock(&sys_mutex);
    return 1;
}

uint8_t
gui_sys_unprotect(void) {
    gui_sys_mutex_unlock(&sys_mutex);
    return 1;
}

uint8_t
gui_sys_mutex_create(gui_sys_mutex_t* p) {
    osMutexDef(MUT);
    *p = osRecursiveMutexCreate(osMutex(MUT));
    return *p != NULL;
}

uint8_t
gui_sys_mutex_delete(gui_sys_mutex_t* p) {
    return osMutexDelete(*p) == osOK;
}

uint8_t
gui_sys_mutex_lock(gui_sys_mutex_t* p) {
    return osRecursiveMutexWait(*p, osWaitForever) == osOK;
}

uint8_t
gui_sys_mutex_unlock(gui_sys_mutex_t* p) {
    return osRecursiveMutexRelease(*p) == osOK;
}

uint8_t
gui_sys_mutex_isvalid(gui_sys_mutex_t* p) {
    return *p != NULL;
}

uint8_t
gui_sys_mutex_invalid(gui_sys_mutex_t* p) {
    *p = GUI_SYS_MUTEX_NULL;
    return 1;
}

uint8_t
gui_sys_sem_create(gui_sys_sem_t* p, uint8_t cnt) {
    osSemaphoreDef(SEM);
    *p = osSemaphoreCreate(osSemaphore(SEM), 1);
    
    if (*p != NULL && !cnt) {
        osSemaphoreWait(*p, 0);
    }
    return *p != NULL;
}

uint8_t
gui_sys_sem_delete(gui_sys_sem_t* p) {
    return osSemaphoreDelete(*p) == osOK;
}

uint32_t
gui_sys_sem_wait(gui_sys_sem_t* p, uint32_t timeout) {
    uint32_t tick = osKernelSysTick();
    return (osSemaphoreWait(*p, !timeout ? osWaitForever : timeout) == osOK) ? (osKernelSysTick() - tick) : GUI_SYS_TIMEOUT;
}

uint8_t
gui_sys_sem_release(gui_sys_sem_t* p) {
    return osSemaphoreRelease(*p) == osOK;
}

uint8_t
gui_sys_sem_isvalid(gui_sys_sem_t* p) {
    return *p != NULL;
}

uint8_t
gui_sys_sem_invalid(gui_sys_sem_t* p) {
    *p = GUI_SYS_SEM_NULL;
    return 1;
}

uint8_t
gui_sys_mbox_create(gui_sys_mbox_t* b, size_t size) {
    osMessageQDef(MBOX, size, void *);
    *b = osMessageCreate(osMessageQ(MBOX), NULL);
    return *b != NULL;
}

uint8_t
gui_sys_mbox_delete(gui_sys_mbox_t* b) {
    if (osMessageWaiting(*b)) {
        return 0;
    }
    return osMessageDelete(*b) == osOK;
}

uint32_t
gui_sys_mbox_put(gui_sys_mbox_t* b, void* m) {
    uint32_t tick = osKernelSysTick();
    return osMessagePut(*b, (uint32_t)m, osWaitForever) == osOK ? (osKernelSysTick() - tick) : GUI_SYS_TIMEOUT;
}

uint32_t
gui_sys_mbox_get(gui_sys_mbox_t* b, void** m, uint32_t timeout) {
    osEvent evt;
    uint32_t time = osKernelSysTick();
    
    evt = osMessageGet(*b, !timeout ? osWaitForever : timeout);
    if (evt.status == osEventMessage) {
        *m = evt.value.p;
        return osKernelSysTick() - time;
    }
    return GUI_SYS_TIMEOUT;
}

uint8_t
gui_sys_mbox_putnow(gui_sys_mbox_t* b, void* m) {
    return osMessagePut(*b, (uint32_t)m, 0) == osOK;
}

uint8_t
gui_sys_mbox_getnow(gui_sys_mbox_t* b, void** m) {
    osEvent evt;
    
    evt = osMessageGet(*b, 0);
    if (evt.status == osEventMessage) {
        *m = evt.value.p;
        return 1;
    }
    return 0;
}

uint8_t
gui_sys_mbox_isvalid(gui_sys_mbox_t* b) {
    return *b != NULL;
}

uint8_t
gui_sys_mbox_invalid(gui_sys_mbox_t* b) {
    *b = GUI_SYS_MBOX_NULL;
    return 1;
}

uint8_t
gui_sys_thread_create(gui_sys_thread_t* t, const char* name, void (*thread_func)(void *), void* const arg, size_t stack_size, gui_sys_thread_prio_t prio) {
    gui_sys_thread_t tmp;
    const osThreadDef_t thread_def = {(char *)name, (os_pthread)thread_func, (osPriority)prio, 0, stack_size};
    tmp = osThreadCreate(&thread_def, arg);
    if (t != NULL) {
        *t = tmp;
    }
    return tmp != NULL;
}
#endif /* GUI_CFG_OS */
#endif /* !__DOXYGEN__ */

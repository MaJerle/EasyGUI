#ifndef __GUI_SYSTEM_H
#define __GUI_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdlib.h"
#include "cmsis_os.h"
    
#include "gui_config.h"

#if GUI_OS
typedef osMutexId           gui_sys_mutex_t;
typedef osSemaphoreId       gui_sys_sem_t;
typedef osMessageQId        gui_sys_mbox_t;
typedef osThreadId          gui_sys_thread_t;
typedef osPriority          gui_sys_thread_prio_t;

#define SYS_MBOX_NULL       (osMessageQId)0
#define SYS_SEM_NULL        (osSemaphoreId)0
#define SYS_TIMEOUT         ((uint32_t)osWaitForever)
#define SYS_THREAD_PRIO     (osPriorityNormal)
#define SYS_THREAD_SS       (4 * configMINIMAL_STACK_SIZE)  /*!< Thread stack size must be taken into consideration and depends on number of children widgets you will have as entire processing is done recursively. */
    
uint8_t gui_sys_init(void);
uint8_t gui_sys_protect(void);
uint8_t gui_sys_unprotect(void);
    
uint8_t gui_sys_mutex_create(gui_sys_mutex_t* p);
uint8_t gui_sys_mutex_delete(gui_sys_mutex_t* p);
uint8_t gui_sys_mutex_lock(gui_sys_mutex_t* p);
uint8_t gui_sys_mutex_unlock(gui_sys_mutex_t* p);
uint8_t gui_sys_mutex_isvalid(gui_sys_mutex_t* p);
    
uint8_t gui_sys_sem_create(gui_sys_sem_t* p, uint8_t cnt);
uint8_t gui_sys_sem_delete(gui_sys_sem_t* p);
uint32_t gui_sys_sem_wait(gui_sys_sem_t* p, uint32_t timeout);
uint8_t gui_sys_sem_release(gui_sys_sem_t* p);
uint8_t gui_sys_sem_isvalid(gui_sys_sem_t* p);
uint8_t gui_sys_sem_invalid(gui_sys_sem_t* p);
    
uint8_t gui_sys_mbox_create(gui_sys_mbox_t* b, size_t size);
uint8_t gui_sys_mbox_delete(gui_sys_mbox_t* b);
uint32_t gui_sys_mbox_put(gui_sys_mbox_t* b, void* m);
uint32_t gui_sys_mbox_get(gui_sys_mbox_t* b, void** m, uint32_t timeout);
uint8_t gui_sys_mbox_putnow(gui_sys_mbox_t* b, void* m);
uint8_t gui_sys_mbox_getnow(gui_sys_mbox_t* b, void** m);
uint8_t gui_sys_mbox_isvalid(gui_sys_mbox_t* b);
uint8_t gui_sys_mbox_invalid(gui_sys_mbox_t* b);
    
gui_sys_thread_t gui_sys_thread_create(const char* name, void (*thread_func)(void *), void* const arg, size_t stack_size, gui_sys_thread_prio_t prio);
#endif /* GUI_OS */

uint32_t gui_sys_now(void);
    
#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __GUI_SYSTEM_H */

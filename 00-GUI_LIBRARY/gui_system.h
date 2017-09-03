#ifndef __GUI_SYSTEM_PORT_H
#define __GUI_SYSTEM_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdlib.h"
#include "cmsis_os.h"

#ifdef GUI_INTERNAL
    
#include "gui_config.h"

#define GUI_PORT_PREFIX     gui_
#define GUI_PASTER(x, y)    x ## y
#define GUI_EXECUTOR(x, y)  GUI_PASTER(x, y)
#define PF(x)               GUI_EXECUTOR(GUI_PORT_PREFIX, x)

#if GUI_RTOS    
typedef osMutexId           PF(sys_mutex_t);
typedef osSemaphoreId       PF(sys_sem_t);
typedef osMessageQId        PF(sys_mbox_t);
typedef osThreadId          PF(sys_thread_t);
typedef osPriority          PF(sys_thread_prio_t);

#define SYS_MBOX_NULL       (osMessageQId)0
#define SYS_SEM_NULL        (osSemaphoreId)0
#define SYS_TIMEOUT         ((uint32_t)osWaitForever)
#define SYS_THREAD_PRIO     osPriorityNormal
#define SYS_THREAD_SS       (4 * configMINIMAL_STACK_SIZE)
    
uint8_t PF(sys_init(void));
uint8_t PF(sys_protect(void));
uint8_t PF(sys_unprotect(void));
    
uint8_t PF(sys_mutex_create(PF(sys_mutex_t)* p));
uint8_t PF(sys_mutex_delete(PF(sys_mutex_t)* p));
uint8_t PF(sys_mutex_lock(PF(sys_mutex_t)* p));
uint8_t PF(sys_mutex_unlock(PF(sys_mutex_t)* p));
uint8_t PF(sys_mutex_isvalid(PF(sys_mutex_t)* p));
    
uint8_t PF(sys_sem_create(PF(sys_sem_t)* p, uint8_t cnt));
uint8_t PF(sys_sem_delete(PF(sys_sem_t)* p));
uint32_t PF(sys_sem_wait(PF(sys_sem_t)* p, uint32_t timeout));
uint8_t PF(sys_sem_release(PF(sys_sem_t)* p));
uint8_t PF(sys_sem_isvalid(PF(sys_sem_t)* p));
uint8_t PF(sys_sem_invalid(PF(sys_sem_t)* p));
    
uint8_t PF(sys_mbox_create(PF(sys_mbox_t)* b, size_t size));
uint8_t PF(sys_mbox_delete(PF(sys_mbox_t)* b));
uint32_t PF(sys_mbox_put(PF(sys_mbox_t)* b, void* m));
uint32_t PF(sys_mbox_get(PF(sys_mbox_t)* b, void** m, uint32_t timeout));
uint8_t PF(sys_mbox_putnow(PF(sys_mbox_t)* b, void* m));
uint8_t PF(sys_mbox_getnow(PF(sys_mbox_t)* b, void** m));
uint8_t PF(sys_mbox_isvalid(PF(sys_mbox_t)* b));
uint8_t PF(sys_mbox_invalid(PF(sys_mbox_t)* b));
    
PF(sys_thread_t) PF(sys_thread_create(const char* name, void (*thread_func)(void *), void* const arg, size_t stack_size, PF(sys_thread_prio_t) prio));
#endif /* GUI_RTOS */

uint32_t PF(sys_now(void));

#endif /* GUI_INTERNAL */
    
#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __GUI_SYSTEM_PORT_H */

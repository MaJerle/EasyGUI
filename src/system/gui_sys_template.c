#define GUI_INTERNAL
#include "gui_sys.h"
#include "stm32f7xx_hal.h"

#define VAL_MUTEX_INVALID           (osMutexId)0
#define VAL_SEM_INVALID             (osSemaphoreId)0
#define VAL_MBOX_INVALID            (osMessageQId)0

#if GUI_OS
static osMutexDef(sys_mutex);                   /* Define a mutex structure */
static osMutexId sys_mutex;                     /* Mutex ID for main protection */
#endif /* GUI_OS */

/**
 * \defgroup        SYS Port functions
 * \brief           System dependant functions
 * \{
 */
 
uint32_t
gui_sys_now(void) {
    return HAL_GetTick();                       /* Get current tick in units of milliseconds */
}

#if GUI_OS || __DOXYGEN__
uint8_t
gui_sys_init(void) {
    sys_mutex = osMutexCreate(osMutex(sys_mutex));  /* Create main system mutex for protection */
    return 0;
}

uint8_t
gui_sys_protect(void) {
    return osMutexWait(sys_mutex, osWaitForever) != osOK;   /* Wait forever for mutex */
}

uint8_t
gui_sys_unprotect(void) {
    return osMutexRelease(sys_mutex) != osOK;   /* Release the mutex */
}

/**
 * \defgroup        SYS_MUTEX Mutexes
 * \brief           Port functions for mutexes
 * \{
 *
 * Mutexes must support recursive lock, which means that it can be locked again if it is already
 */

uint8_t
gui_sys_mutex_create(gui_sys_mutex_t* p) {
    osMutexDef(MUT);                            /* Define a mutex */
    return (*p = osMutexCreate(osMutex(MUT))) == 0; /* Create mutex from init */
}

uint8_t
gui_sys_mutex_delete(gui_sys_mutex_t* p) {
    return osMutexDelete(*p) != osOK;           /* Delete mutex */
}
  
uint8_t
gui_sys_mutex_lock(gui_sys_mutex_t* p) {
    return osMutexWait(*p, osWaitForever) != osOK;  /* Wait forever for mutex */
}
  
uint8_t
gui_sys_mutex_unlock(gui_sys_mutex_t* p) {
    return osMutexRelease(*p) != osOK;          /* Release mutex */
}

uint8_t
gui_sys_mutex_isvalid(gui_sys_mutex_t* p) {
    return !*p;                                 /* Check if mutex is valid */
}

uint8_t
gui_sys_mutex_invalid(gui_sys_mutex_t* p) {
    *p = VAL_MUTEX_INVALID;                     /* Set mutex as invalid */
    return 0;
}

/**
 * \}
 */
    
/**
 * \defgroup        SYS_SEM Semaphores
 * \brief           Port functions for semaphores
 * \{
 *
 * Semaphores must be binary (single token) only
 */

uint8_t
gui_sys_sem_create(gui_sys_sem_t* p, uint8_t cnt) {
    osSemaphoreDef(SEM);                        /* Define semaphore info */
    *p = osSemaphoreCreate(osSemaphore(SEM), 1);    /* Create semaphore with one token */
    
    if (*p && !cnt) {                           /* We have valid entry */
        osSemaphoreWait(*p, 0);                 /* Lock semaphore immediatelly */
    }
    return !*p;
}

uint8_t
gui_sys_sem_delete(gui_sys_sem_t* p) {
    return osSemaphoreDelete(*p) != osOK;       /* Delete semaphore */
}

uint32_t
gui_sys_sem_wait(gui_sys_sem_t* p, uint32_t timeout) {
    uint32_t tick = osKernelSysTick();          /* Get start tick time */
    
    if (osSemaphoreWait(*p, !timeout ? osWaitForever : timeout) == osOK) {  /* Wait for semaphore with specific time */
        return osKernelSysTick() - tick;        /* Return time required to get semaphore */
    }
    return SYS_TIMEOUT;                         /* We reached timeout */
}

uint8_t
gui_sys_sem_release(gui_sys_sem_t* p) {
    return osSemaphoreRelease(*p) == osOK;      /* Release semaphore */
}

uint8_t
gui_sys_sem_isvalid(gui_sys_sem_t* p) {
    return !*p;                                 /* Check if valid */
}

uint8_t
gui_sys_sem_invalid(gui_sys_sem_t* p) {
    *p = VAL_SEM_INVALID;                       /* Invaldiate semaphore */
    return 0;
}

/**
 * \}
 */

/**
 * \defgroup        SYS_MESSAGE Messages
 * \brief           Port functions for messages
 * \{
 *
 * Messages are used for communication between different threads
 */
    
uint8_t
gui_sys_mbox_create(gui_sys_mbox_t* b, size_t size) {
    osMessageQDef(MBOX, size, void *);          /* Define new macro */
    return (*b = osMessageCreate(osMessageQ(MBOx), NULL)) == 0; /* Create message box */
}

uint8_t
gui_sys_mbox_delete(gui_sys_mbox_t* b) {
    if (osMessageWaiting(*b)) {                 /* We still have messages in queue, should not delete queue */
        return 1;                               /* Return error */
    }
    return osMessageDelete(*b) != osOK;         /* Delete message queue */
}

uint32_t
gui_sys_mbox_put(gui_sys_mbox_t* b, void* m) {
    uint32_t tick = osKernelSysTick();          /* Get start time */
    return osMessagePut(*b, (uint32_t)m, osWaitForever) == osOK ? (osKernelSysTick() - tick) : SYS_TIMEOUT; /* Put new message with forever timeout */
}

uint32_t
gui_sys_mbox_get(gui_sys_mbox_t* b, void** m, uint32_t timeout) {
    osEvent evt;
    uint32_t time = osKernelSysTick();          /* Get current time */
    
    evt = osMessageGet(*b, !timeout ? osWaitForever : timeout); /* Get message event */
    if (evt.status == osEventMessage) {         /* Did we get a message? */
        *m = evt.value.p;                       /* Set value */
        return osKernelSysTick() - timeout;     /* Return time required for reading message */
    }
    return SYS_TIMEOUT;
}

uint8_t
gui_sys_mbox_putnow(gui_sys_mbox_t* b, void* m) {
    return osMessagePut(*b, (uint32_t)m, 0) != osOK;   /* Put new message without timeout */
}

uint8_t
gui_sys_mbox_getnow(gui_sys_mbox_t* b, void** m) {
    osEvent evt;
    
    evt = osMessageGet(*b, 0);                  /* Get message event */
    if (evt.status == osEventMessage) {         /* Did we get a message? */
        *m = evt.value.p;                       /* Set value */
        return 0;
    }
    return 1;
}

uint8_t
gui_sys_mbox_isvalid(gui_sys_mbox_t* b) {
    return !*b;                                 /* Return status if message box is valid */
}

uint8_t
gui_sys_mbox_invalid(gui_sys_mbox_t* b) {
    *b = VAL_MBOX_INVALID;                      /* Invalidate message box */
    return 0;
}

/**
 * \}
 */

/**
 * \defgroup        SYS_THREAD Threads
 * \brief           Port functions for threads
 * \{
 */
    
gui_sys_thread_t
gui_sys_thread_create(const char* name, void (*thread_func)(void *), void* const arg, size_t stack_size, gui_sys_thread_prio_t prio) {
    const osThreadDef_t thread_def = {(char *)name, (os_pthread)thread_func, (osPriority)prio, 0, stack_size};  /* Create thread description */
    return osThreadCreate(&thread_def, arg);    /* Create thread */
}

#endif /* GUI_OS || __DOXYGEN__ */

/**
 * \}
 */

/**
 * \}
 */

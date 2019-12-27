#define GUI_INTERNAL
#include "system/gui_sys.h"
#include "stm32f7xx_hal.h"

#define VAL_MUTEX_INVALID           (osMutexId)0
#define VAL_SEM_INVALID             (osSemaphoreId)0
#define VAL_MBOX_INVALID            (osMessageQId)0

#if GUI_CFG_OS
static osMutexDef(sys_mutex);                   /* Define a mutex structure */
static osMutexId sys_mutex;                     /* Mutex ID for main protection */
#endif /* GUI_CFG_OS */

/**
 * \brief           Init system dependant parameters
 * \note            Called from high-level application layer when required
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_init(void) {
#if GUI_CFG_OS
    sys_mutex = osMutexCreate(osMutex(sys_mutex));  /* Create main system mutex for protection */
#endif /* GUI_CFG_OS */
    return 1;
}

/**
 * \brief           Get current time in units of milliseconds
 * \return          Current time in units of milliseconds
 */
uint32_t
gui_sys_now(void) {
    return HAL_GetTick();                       /* Get current tick in units of milliseconds */
}

#if GUI_CFG_OS || __DOXYGEN__

/**
 * \brief           Unprotect stack core
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_protect(void) {
    return osMutexWait(sys_mutex, osWaitForever) == osOK;   /* Wait forever for mutex */
}

/**
 * \brief           Unprotect stack core
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_unprotect(void) {
    return osMutexRelease(sys_mutex) == osOK;   /* Release the mutex */
}

/**
 * \brief           Create a new mutex and pass it to input pointer
 * \note            Recursive mutex must be created as it may be locked multiple times before unlocked
 * \param[out]      p: Pointer to mutex structure to save result to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mutex_create(gui_sys_mutex_t* p) {
    osMutexDef(MUT);                            /* Define a mutex */
    return (*p = osMutexCreate(osMutex(MUT))) != NULL;  /* Create mutex from init */
}

/**
 * \brief           Delete mutex from OS
 * \param[in]       p: Pointer to mutex structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mutex_delete(gui_sys_mutex_t* p) {
    return osMutexDelete(*p) == osOK;           /* Delete mutex */
}
  
/**
 * \brief           Wait forever to lock the mutex
 * \param[in]       p: Pointer to mutex structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mutex_lock(gui_sys_mutex_t* p) {
    return osMutexWait(*p, osWaitForever) == osOK;  /* Wait forever for mutex */
}
  
/**
 * \brief           Unlock mutex
 * \param[in]       p: Pointer to mutex structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mutex_unlock(gui_sys_mutex_t* p) {
    return osMutexRelease(*p) == osOK;          /* Release mutex */
}

/**
 * \brief           Check if mutex structure is valid OS entry
 * \param[in]       p: Pointer to mutex structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mutex_isvalid(gui_sys_mutex_t* p) {
    return *p != NULL;                          /* Check if mutex is valid */
}

/**
 * \brief           Set mutex structure as invalid
 * \param[in]       p: Pointer to mutex structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mutex_invalid(gui_sys_mutex_t* p) {
    *p = VAL_MUTEX_INVALID;                     /* Set mutex as invalid */
    return 1;
}

/**
 * \brief           Create a new binary semaphore and set initial state
 * \note            Semaphore may only have 1 token available
 * \param[out]      p: Pointer to semaphore structure to fill with result
 * \param[in]       cnt: Count indicating default semaphore state:
 *                     `0`: Lock it immediteally
 *                     `1`: Leave it unlocked
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_sem_create(gui_sys_sem_t* p, uint8_t cnt) {
    osSemaphoreDef(SEM);                        /* Define semaphore info */
    *p = osSemaphoreCreate(osSemaphore(SEM), 1);    /* Create semaphore with one token */
    
    if (*p != NULL && !cnt) {                   /* We have valid entry */
        osSemaphoreWait(*p, 0);                 /* Lock semaphore immediatelly */
    }
    return *p != NULL;
}

/**
 * \brief           Delete binary semaphore
 * \param[in]       p: Pointer to semaphore structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_sem_delete(gui_sys_sem_t* p) {
    return osSemaphoreDelete(*p) == osOK;       /* Delete semaphore */
}

/**
 * \brief           Wait for semaphore to be available
 * \param[in]       p: Pointer to semaphore structure
 * \param[in]       timeout: Timeout to wait in milliseconds. When `0` is applied, wait forever
 * \return          Number of milliseconds waited for semaphore to become available
 */
uint32_t
gui_sys_sem_wait(gui_sys_sem_t* p, uint32_t timeout) {
    uint32_t tick = osKernelSysTick();          /* Get start tick time */
    
    if (osSemaphoreWait(*p, !timeout ? osWaitForever : timeout) == osOK) {  /* Wait for semaphore with specific time */
        return osKernelSysTick() - tick;        /* Return time required to get semaphore */
    }
    return SYS_TIMEOUT;                         /* We reached timeout */
}

/**
 * \brief           Release semaphore
 * \param[in]       p: Pointer to semaphore structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_sem_release(gui_sys_sem_t* p) {
    return osSemaphoreRelease(*p) == osOK;
}

/**
 * \brief           Check if semaphore is valid
 * \param[in]       p: Pointer to semaphore structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_sem_isvalid(gui_sys_sem_t* p) {
    return *p != NULL;
}

/**
 * \brief           Invalid semaphore
 * \param[in]       p: Pointer to semaphore structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_sem_invalid(gui_sys_sem_t* p) {
    *p = VAL_SEM_INVALID;
    return 1;
}
    
/**
 * \brief           Create a new message queue with entry type of `void *`
 * \param[out]      b: Pointer to message queue structure
 * \param[in]       size: Number of entries for message queue to hold
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mbox_create(gui_sys_mbox_t* b, size_t size) {
    osMessageQDef(MBOX, size, void *);          /* Define new macro */
    return (*b = osMessageCreate(osMessageQ(MBOx), NULL)) != NULL;  /* Create message box */
}

/**
 * \brief           Delete message queue
 * \param[in]       b: Pointer to message queue structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mbox_delete(gui_sys_mbox_t* b) {
    if (osMessageWaiting(*b)) {
        return 0;
    }
    return osMessageDelete(*b) == osOK;
}

/**
 * \brief           Put a new entry to message queue and wait until memory available
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to entry to insert to message queue
 * \return          Time in units of milliseconds needed to put a message to queue
 */
uint32_t
gui_sys_mbox_put(gui_sys_mbox_t* b, void* m) {
    uint32_t tick = osKernelSysTick();          /* Get start time */
    return osMessagePut(*b, (uint32_t)m, osWaitForever) == osOK ? (osKernelSysTick() - tick) : SYS_TIMEOUT; /* Put new message with forever timeout */
}

/**
 * \brief           Get a new entry from message queue with timeout
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to pointer to result to save value from message queue to
 * \param[in]       timeout: Maximal timeout to wait for new message. When `0` is applied, wait for unlimited time
 * \return          Time in units of milliseconds needed to put a message to queue
 */
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

/**
 * \brief           Put a new entry to message queue without timeout (now or fail)
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to message to save to queue
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mbox_putnow(gui_sys_mbox_t* b, void* m) {
    return osMessagePut(*b, (uint32_t)m, 0) == osOK;/* Put new message without timeout */
}

/**
 * \brief           Get an entry from message queue immediatelly
 * \param[in]       b: Pointer to message queue structure
 * \param[in]       m: Pointer to pointer to result to save value from message queue to
 * \return          `1` on success, `0` otherwise
 */
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

/**
 * \brief           Check if message queue is valid
 * \param[in]       b: Pointer to message queue structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mbox_isvalid(gui_sys_mbox_t* b) {
    return *b != NULL;
}

/**
 * \brief           Invalid message queue
 * \param[in]       b: Pointer to message queue structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_mbox_invalid(gui_sys_mbox_t* b) {
    *b = VAL_MBOX_INVALID;                      /* Invalidate message box */
    return 1;
}

/**
 * \brief           Create a new thread
 * \param[out]      t: Pointer to thread identifier if create was successful
 * \param[in]       name: Name of a new thread
 * \param[in]       thread_func: Thread function to use as thread body
 * \param[in]       arg: Thread function argument
 * \param[in]       stack_size: Size of thread stack in uints of bytes. If set to `0`, reserve default stack size
 * \param[in]       prio: Thread priority
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_sys_thread_create(gui_sys_thread_t* t, const char* name, void (*thread_func)(void *), void* const arg, size_t stack_size, gui_sys_thread_prio_t prio) {
    gui_sys_thread_t tmp;
    const osThreadDef_t thread_def = {(char *)name, (os_pthread)thread_func, (osPriority)prio, 0, stack_size};  /* Create thread description */
    tmp = osThreadCreate(&thread_def, arg);     /* Create thread */
    if (t != NULL) {
        *t = tmp;
    }
    return tmp != NULL;
}

#endif /* GUI_CFG_OS || __DOXYGEN__ */

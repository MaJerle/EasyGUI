/**	
 * \file            gui_sys_win32.c
 * \brief           System dependant functions for WIN32
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
#include "system/gui_sys.h"
#include <string.h>
#include <stdlib.h>
#include "windows.h"

#if !__DOXYGEN__

static LARGE_INTEGER freq, sys_start_time;

#if GUI_CFG_OS

/**
 * \brief           Custom message queue implementation for WIN32
 */
typedef struct {
    gui_sys_sem_t sem_not_empty;                /*!< Semaphore indicates not empty */
    gui_sys_sem_t sem_not_full;                 /*!< Semaphore indicates not full */
    gui_sys_sem_t sem;                          /*!< Semaphore to lock access */
    size_t in, out, size;
    void* entries[1];
} win32_mbox_t;

static gui_sys_mutex_t sys_mutex;				/* Mutex ID for main protection */

static uint8_t
mbox_is_full(win32_mbox_t* m) {
    size_t size = 0;
    if (m->in > m->out) {
        size = (m->in - m->out);
    } else if (m->out > m->in) {
        size = m->size - m->out + m->in;
    }
    return size == m->size - 1;
}

static uint8_t
mbox_is_empty(win32_mbox_t* m) {
    return m->in == m->out;
}

#endif /* GUI_CFG_OS */

static uint32_t
osKernelSysTick(void) {
	LONGLONG ret;
	LARGE_INTEGER now;

    QueryPerformanceFrequency(&freq);           /* Get frequency */
	QueryPerformanceCounter(&now);              /* Get current time */
	ret = now.QuadPart - sys_start_time.QuadPart;
	return (uint32_t)(((ret) * 1000) / freq.QuadPart);
}

uint8_t
gui_sys_init(void) {
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&sys_start_time);   /* Get start time */

    gui_sys_mutex_create(&sys_mutex);
    return 1;
}

uint32_t
gui_sys_now(void) {
    return osKernelSysTick();
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
	*p = CreateMutex(NULL, FALSE, NULL);
	return *p != NULL;
}

uint8_t
gui_sys_mutex_delete(gui_sys_mutex_t* p) {
	return CloseHandle(*p);
}

uint8_t
gui_sys_mutex_lock(gui_sys_mutex_t* p) {
	DWORD ret;
	ret = WaitForSingleObject(*p, INFINITE);
	if (ret != WAIT_OBJECT_0) {
        return 0;
	}
	return 1;
}

uint8_t
gui_sys_mutex_unlock(gui_sys_mutex_t* p) {
	return !!ReleaseMutex(*p);
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
	HANDLE h;
	h = CreateSemaphore(NULL, !!cnt, 1, NULL);
	*p = h;
	return *p != NULL;
}

uint8_t
gui_sys_sem_delete(gui_sys_sem_t* p) {
	return CloseHandle(*p);
}

uint32_t
gui_sys_sem_wait(gui_sys_sem_t* p, uint32_t timeout) {
	DWORD ret;
    uint32_t tick = osKernelSysTick();          /* Get start tick time */
	
	if (!timeout) {
		ret = WaitForSingleObject(*p, INFINITE);
		return 1;
	} else {
		ret = WaitForSingleObject(*p, timeout);
		if (ret == WAIT_OBJECT_0) {
			return 1;
		} else {
			return GUI_SYS_TIMEOUT;
		}
	}
}

uint8_t
gui_sys_sem_release(gui_sys_sem_t* p) {
	return ReleaseSemaphore(*p, 1, NULL);
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
    win32_mbox_t* mbox;
    
    *b = 0;
    
    mbox = malloc(sizeof(*mbox) + size * sizeof(void *));
    if (mbox != NULL) {
        memset(mbox, 0x00, sizeof(*mbox));
        mbox->size = size + 1;
        gui_sys_sem_create(&mbox->sem, 1);
        gui_sys_sem_create(&mbox->sem_not_empty, 0);
        gui_sys_sem_create(&mbox->sem_not_full, 0);
        *b = mbox;
    }
    return *b != NULL;
}

uint8_t
gui_sys_mbox_delete(gui_sys_mbox_t* b) {
    win32_mbox_t* mbox = *b;
    gui_sys_sem_delete(&mbox->sem);
    gui_sys_sem_delete(&mbox->sem_not_full);
    gui_sys_sem_delete(&mbox->sem_not_empty);
    free(mbox);
    return 1;
}

uint32_t
gui_sys_mbox_put(gui_sys_mbox_t* b, void* m) {
    win32_mbox_t* mbox = *b;
    uint32_t time = osKernelSysTick();
  
    gui_sys_sem_wait(&mbox->sem, 0);            /* Wait for access */
    
    /*
     * Since function is blocking until ready to write something to queue,
     * wait and release the semaphores to allow other threads
     * to process the queue before we can write new value.
     */
    while (mbox_is_full(mbox)) {
        gui_sys_sem_release(&mbox->sem);        /* Release semaphore */
        gui_sys_sem_wait(&mbox->sem_not_full, 0);   /* Wait for semaphore indicating not full */
        gui_sys_sem_wait(&mbox->sem, 0);        /* Wait availability again */
    }
    mbox->entries[mbox->in] = m;
    if (mbox->in == mbox->out) {                /* Was the previous state empty? */
        gui_sys_sem_release(&mbox->sem_not_empty);  /* Signal non-empty state */
    }
    if (++mbox->in >= mbox->size) {
        mbox->in = 0;
    }
    gui_sys_sem_release(&mbox->sem);            /* Release access for other threads */
    return osKernelSysTick() - time;
}

uint32_t
gui_sys_mbox_get(gui_sys_mbox_t* b, void** m, uint32_t timeout) {
    win32_mbox_t* mbox = *b;
    uint32_t time = osKernelSysTick();          /* Get current time */
    uint32_t spent_time;
    
    /* Get exclusive access to message queue */
    if ((spent_time = gui_sys_sem_wait(&mbox->sem, timeout)) == GUI_SYS_TIMEOUT) {
        return spent_time;
    }
    
    /* Make sure we have something to read from queue. */
    while (mbox_is_empty(mbox)) {
        gui_sys_sem_release(&mbox->sem);        /* Release semaphore and allow other threads to write something */
        /*
         * Timeout = 0 means unlimited time
         * Wait either unlimited time or for specific timeout
         */
        if (!timeout) {
            gui_sys_sem_wait(&mbox->sem_not_empty, 0);
        } else {
            spent_time = gui_sys_sem_wait(&mbox->sem_not_empty, timeout);
            if (spent_time == GUI_SYS_TIMEOUT) {
                return spent_time;
            }
        }
        spent_time = gui_sys_sem_wait(&mbox->sem, timeout); /* Wait again for exclusive access */
    }
    
    /*
     * At this point, semaphore is not empty and
     * we have exclusive access to content
     */
    *m = mbox->entries[mbox->out];
    if (++mbox->out >= mbox->size) {
        mbox->out = 0;
    }
    
    /* Release it only if waiting for it */
    gui_sys_sem_release(&mbox->sem_not_full);   /* Release semaphore as it is not full */
    gui_sys_sem_release(&mbox->sem);            /* Release exclusive access to mbox */
    
    return osKernelSysTick() - time;
}

uint8_t
gui_sys_mbox_putnow(gui_sys_mbox_t* b, void* m) {
    win32_mbox_t* mbox = *b;

    gui_sys_sem_wait(&mbox->sem, 0);
    if (mbox_is_full(mbox)) {
        gui_sys_sem_release(&mbox->sem);
        return 0;
    }
    mbox->entries[mbox->in] = m;
    if (mbox->in == mbox->out) {
        gui_sys_sem_release(&mbox->sem_not_empty);
    }
    mbox->in++;
    if (mbox->in >= mbox->size) {
        mbox->in = 0;
    }
    gui_sys_sem_release(&mbox->sem);
    return 1;
}

uint8_t
gui_sys_mbox_getnow(gui_sys_mbox_t* b, void** m) {
    win32_mbox_t* mbox = *b;
    
    gui_sys_sem_wait(&mbox->sem, 0);            /* Wait exclusive access */
    if (mbox->in == mbox->out) {
        gui_sys_sem_release(&mbox->sem);        /* Release access */
        return 0;
    }
    
    *m = mbox->entries[mbox->out];
    mbox->out++;
    if (mbox->out >= mbox->size) {
        mbox->out = 0;
    }
    gui_sys_sem_release(&mbox->sem_not_full);   /* Queue not full anymore */
    gui_sys_sem_release(&mbox->sem);            /* Release semaphore */
    return 1;
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
gui_sys_thread_create(gui_sys_thread_t* t, const char* name, gui_sys_thread_fn thread_fn, void* const arg, size_t stack_size, gui_sys_thread_prio_t prio) {
    HANDLE h;
    DWORD id;
    h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_fn, arg, 0, &id);
    if (t != NULL) {
        *t = h;
    }
    return h != NULL;
}

uint8_t
gui_sys_thread_terminate(gui_sys_thread_t* t) {
    HANDLE h = NULL;

    if (t == NULL) {                            /* Shall we terminate ourself? */
        h = GetCurrentThread();                 /* Get current thread handle */
    } else {                                    /* We have known thread, find handle by looking at ID */
        h = *t;
    }
    TerminateThread(h, 0);
	return 1;
}

uint8_t
gui_sys_thread_yield(void) {
    /* Not implemented */
	return 1;
}

#endif /* GUI_CFG_OS */
#endif /* !__DOXYGEN__ */

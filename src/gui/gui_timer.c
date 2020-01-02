/**	
 * \file            gui_timer.c
 * \brief           Software timer manager
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
#include "gui/gui_private.h"
#include "gui/gui_timer.h"
#include "system/gui_sys.h"

#define GUI_FLAG_TIMER_ACTIVE           ((uint16_t)(1 << 0UL))  /*!< Timer is active */
#define GUI_FLAG_TIMER_PERIODIC         ((uint16_t)(1 << 1UL))  /*!< Timer will start from beginning after reach end */ 
#define GUI_FLAG_TIMER_CALL             ((uint16_t)(1 << 2UL))  /*!< Call callback function on timer */

#define guii_timer_isperiodic(t)        ((t)->flags & GUI_FLAG_TIMER_PERIODIC)

/**
 * \brief           Create new software timer
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       period: Timer period in units of milliseconds
 * \param[in]       callback: Pointer to timer callback
 * \param[in]       params: Pointer to user parameters used in callback
 * \return          Timer handle on success, NULL otherwise
 */
gui_timer_t *
guii_timer_create(uint16_t period, void (*callback)(gui_timer_t *), void* const params) {
    gui_timer_t* ptr;
    
    ptr = GUI_MEMALLOC(sizeof(*ptr));               /* Allocate memory for timer */
    if (ptr != NULL) {
        memset(ptr, 0x00, sizeof(*ptr));            /* Reset memory */
        
        ptr->period = period;                       /* Set period value */
        ptr->counter = period;                      /* Set current counter value */
        ptr->callback = callback;                   /* Set callback */
        ptr->params = params;                       /* Timer custom parameters */
        ptr->flags = 0;                             /* Timer flags management */
        
        gui_linkedlist_add_gen(&GUI.timers.list, (gui_linkedlist_t *)ptr);  /* Add timer to linked list */

#if GUI_CFG_OS
        gui_sys_mbox_putnow(&GUI.OS.mbox, NULL);    /* Add new message to queue */
#endif /* GUI_CFG_OS */
    }
    return ptr;
}

/**
 * \brief           Remove timer
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       t: Pointer to pointer to \ref gui_timer_t structure.
 *                      After timer remove, pointer value where it points to will be changed
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_timer_remove(gui_timer_t** const t) {  
    GUI_ASSERTPARAMS(t != NULL && *t != NULL);  
    gui_linkedlist_remove_gen(&GUI.timers.list, (gui_linkedlist_t *)(*t));  /* Remove timer from linked list */
    GUI_MEMFREE(*t);                                /* Free memory for timer */
    *t = NULL;                                      /* Clear pointer */
    
    return 1;
}

/**
 * \brief           Start timer
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       t: Pointer to \ref gui_timer_t structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_timer_start(gui_timer_t* const t) {
    GUI_ASSERTPARAMS(t);
    t->counter = t->period;                         /* Reset counter to top value */
    t->flags &= ~GUI_FLAG_TIMER_PERIODIC;           /* Clear periodic flag */
    t->flags |= GUI_FLAG_TIMER_ACTIVE;              /* Set active flag */

#if GUI_CFG_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, NULL);        /* Add new message to queue */
#endif /* GUI_CFG_OS */
    
    return 1;
}

/**
 * \brief           Start timer periodically. It will start again when callback is called
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       t: Pointer to \ref gui_timer_t structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_timer_startperiodic(gui_timer_t* const t) {
    GUI_ASSERTPARAMS(t);
    t->counter = t->period;                         /* Reset counter to top value */
    t->flags |= GUI_FLAG_TIMER_ACTIVE | GUI_FLAG_TIMER_PERIODIC;    /* Set active flag */
    
    return 1;
}

/**
 * \brief           Stop timer
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       t: Pointer to \ref gui_timer_t structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_timer_stop(gui_timer_t* const t) {
    GUI_ASSERTPARAMS(t);
    t->counter = t->period;                         /* Reset counter to top value */
    t->flags &= ~GUI_FLAG_TIMER_ACTIVE;             /* Clear active flag */
    
    return 1;
}

/**
 * \brief           Reset timer to zero
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       t: Pointer to \ref gui_timer_t structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_timer_reset(gui_timer_t* const t) {
    GUI_ASSERTPARAMS(t);
    t->counter = t->period;                         /* Clear and reset timer */
    
    return 1;
}

/**
 * \brief           Internal processing called by GUI library
 * \note            This function is private and may be called only when OS protection is active
 * \note            Processes all timers in loop and calls callback function if needed 
 */
void
guii_timer_process(void) {
    gui_timer_t* t;
    volatile uint32_t time = gui_sys_now();         /* Get current time */
    volatile uint32_t lastTime = GUI.timers.time;
    uint32_t diff = time - lastTime;                /* Get difference in time */
    
    if (diff == 0) {                                /* No difference in time */
        return;
    }
    
    /* Process all software timers */
    for (t = (gui_timer_t *)GUI.timers.list.first; t != NULL;
        t = (gui_timer_t *)t->list.next) {
        if (t->flags & GUI_FLAG_TIMER_CALL) {       /* Counter is set to 0, process callback */
            t->flags &= ~GUI_FLAG_TIMER_CALL;       /* Clear timer flag */
            if (!guii_timer_isperiodic(t)) {        /* If timer is not periodic */
                guii_timer_stop(t);                 /* Stop timer */
            }
            if (t->callback != NULL) {              /* Process callback */
                t->callback(t);                     /* Call user function */
            }
        }
        
        if (!(t->flags & GUI_FLAG_TIMER_ACTIVE)) {  /* Timer is not active */
            continue;
        }
        if (t->counter >= diff) {                   /* Decrease counter value by zero */
            t->counter -= diff;                     /* Try to decrease to 0 */
            if (!t->counter) {
                t->counter = t->period;             /* Set new period value */
                t->flags |= GUI_FLAG_TIMER_CALL;    /* Set flag for timer callback next time */
            }
        } else {                                    /* Overflow mode */
            t->counter = 0;                         /* Set new counter value */
            t->flags |= GUI_FLAG_TIMER_CALL;        /* Set callback flag in next check */
        }
    }
    
    GUI.timers.time = time;                         /* Reset time */
}

/**
 * \brief           Get number of currently active timers
 * \return          Number of active timers
 */
uint32_t
guii_timer_getactivecount(void) {
    uint32_t cnt = 0;
    gui_timer_t* t;
    for (t = (gui_timer_t *)gui_linkedlist_getnext_gen(&GUI.timers.list, NULL); t != NULL;
        t = (gui_timer_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)t)) {
        if ((t->flags & GUI_FLAG_TIMER_ACTIVE)) {
            cnt++;
        }
    }
    return cnt;
}

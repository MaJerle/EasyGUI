/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_timer.h"
#include "gui_system.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define GUI_FLAG_TIMER_ACTIVE           ((uint16_t)(1 << 0UL))  /*!< Timer is active */
#define GUI_FLAG_TIMER_PERIODIC         ((uint16_t)(1 << 1UL))  /*!< Timer will start from beginning after reach end */ 
#define GUI_FLAG_TIMER_CALL             ((uint16_t)(1 << 2UL))  /*!< Call callback function on timer */

#define gui_timer_isperiodic__(t)       ((t)->Flags & GUI_FLAG_TIMER_PERIODIC)

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
#if GUI_OS
static gui_mbox_msg_t timer_msg = {GUI_SYS_MBOX_TYPE_TIMER};
#endif /* GUI_OS */

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new software timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       period: Timer period in units of milliseconds
 * \param[in]       *callback: Pointer to timer callback
 * \param[in]       *param: Pointer to user parameters used in callback
 * \retval          > 0: Timer created
 * \retval          0: Timer creation failed
 */
GUI_TIMER_t*
gui_timer_create__(uint16_t period, void (*callback)(GUI_TIMER_t *), void* params) {
    GUI_TIMER_t* ptr;
    
    ptr = GUI_MEMALLOC(sizeof(*ptr));               /* Allocate memory for timer */
    if (ptr) {
        memset(ptr, 0x00, sizeof(GUI_TIMER_t));     /* Reset memory */
        
        ptr->Period = period;                       /* Set period value */
        ptr->Counter = period;                      /* Set current counter value */
        ptr->Callback = callback;                   /* Set callback */
        ptr->Params = params;                       /* Timer custom parameters */
        ptr->Flags = 0;                             /* Timer flags management */
        
        gui_linkedlist_add_gen(&GUI.Timers.List, (GUI_LinkedList_t *)ptr);  /* Add timer to linked list */
#if GUI_OS
        gui_sys_mbox_putnow(&GUI.OS.mbox, &timer_msg);  /* Add new message to queue */
#endif /* GUI_OS */
    }
    return ptr;
}

/**
 * \brief           Remove timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       **t: Pointer to pointer to \ref GUI_TIMER_t structure.
 *                      After timer remove, pointer value where it points to will be changed
 * \retval          1: Timer was removed ok
 * \retval          0: Timer was not removed
 */
uint8_t
gui_timer_remove__(GUI_TIMER_t** t) {  
    __GUI_ASSERTPARAMS(t && *t);                    /* Check input parameters */  
    gui_linkedlist_remove_gen(&GUI.Timers.List, (GUI_LinkedList_t *)(*t));  /* Remove timer from linked list */
    GUI_MEMFREE(*t);                                /* Free memory for timer */
    *t = 0;                                         /* Restore pointer */
    
    return 1;
}

/**
 * \brief           Start timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          1: Timer was started ok
 * \retval          0: Timer was not started
 */
uint8_t
gui_timer_start__(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMS(t);                          /* Check input parameters */
    t->Counter = t->Period;                         /* Reset counter to top value */
    t->Flags &= ~GUI_FLAG_TIMER_PERIODIC;           /* Clear periodic flag */
    t->Flags |= GUI_FLAG_TIMER_ACTIVE;              /* Set active flag */
#if GUI_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, &timer_msg);  /* Add new message to queue */
#endif /* GUI_OS */
    
    return 1;
}

/**
 * \brief           Start timer periodically. It will start again when callback is called
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          1: Timer was started ok
 * \retval          0: Timer was not started
 */
uint8_t
gui_timer_startperiodic__(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMS(t);                          /* Check input parameters */
    t->Counter = t->Period;                         /* Reset counter to top value */
    t->Flags |= GUI_FLAG_TIMER_ACTIVE | GUI_FLAG_TIMER_PERIODIC;    /* Set active flag */
    
    return 1;
}

/**
 * \brief           Stop timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          1: Timer was stopped ok
 * \retval          0: Timer was not stopped
 */
uint8_t
gui_timer_stop__(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMS(t);                          /* Check input parameters */
    t->Counter = t->Period;                         /* Reset counter to top value */
    t->Flags &= ~GUI_FLAG_TIMER_ACTIVE;             /* Clear active flag */
    
    return 1;
}

/**
 * \brief           Reset timer to zero
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          1: Timer was reseted ok
 * \retval          0: Timer was not reseted
 */
uint8_t
gui_timer_reset__(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMS(t);                          /* Check input parameters */
    t->Counter = t->Period;                         /* Clear and reset timer */
    
    return 1;
}

/**
 * \brief           Internal processing called by GUI library
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \note            Processes all timers in loop and calls callback function if needed
 * \retval          None
 */
void
gui_timer_process(void) {
    GUI_TIMER_t* t;
    volatile uint32_t time = gui_sys_now();         /* Get current time */
    volatile uint32_t lastTime = GUI.Timers.Time;
    uint32_t diff = time - lastTime;                /* Get difference in time */
    
    if (diff == 0) {                                /* No difference in time */
        return;
    }
    
    /* Process all software timers */
    for (t = (GUI_TIMER_t *)GUI.Timers.List.First; t; t = (GUI_TIMER_t *)t->List.Next) {
        if (t->Flags & GUI_FLAG_TIMER_CALL) {       /* Counter is set to 0, process callback */
            t->Flags &= ~GUI_FLAG_TIMER_CALL;       /* Clear timer flag */
            if (!gui_timer_isperiodic__(t)) {       /* If timer is not periodic */
                gui_timer_stop__(t);                /* Stop timer */
            }
            if (t->Callback) {                      /* Process callback */
                t->Callback(t);                     /* Call user function */
            }
        }
        
        if (!(t->Flags & GUI_FLAG_TIMER_ACTIVE)) {  /* Timer is not active */
            continue;
        }
        if (t->Counter >= diff) {                   /* Decrease counter value by zero */
            t->Counter -= diff;                     /* Try to decrease to 0 */
            if (!t->Counter) {
                t->Counter = t->Period;             /* Set new period value */
                t->Flags |= GUI_FLAG_TIMER_CALL;    /* Set flag for timer callback next time */
            }
        } else {                                    /* Overflow mode */
            t->Counter = 0;                         /* Set new counter value */
            t->Flags |= GUI_FLAG_TIMER_CALL;        /* Set callback flag in next check */
        }
    }
    
    GUI.Timers.Time = time;                         /* Reset time */
}

uint32_t
gui_timer_getactivecount(void) {
    uint32_t cnt = 0;
    GUI_TIMER_t* t;
    for (t = (GUI_TIMER_t *)GUI.Timers.List.First; t; t = (GUI_TIMER_t *)t->List.Next) {
        if ((t->Flags & GUI_FLAG_TIMER_ACTIVE)) {
            cnt++;
        }
    }
    return cnt;
}

/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
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

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/

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
GUI_TIMER_t* __GUI_TIMER_Create(uint16_t period, void (*callback)(GUI_TIMER_t *), void* params, uint8_t flags) {
    GUI_TIMER_t* ptr;
    
    ptr = (GUI_TIMER_t *)__GUI_MEMALLOC(sizeof(GUI_TIMER_t));  /* Allocate memory for timer */
    if (ptr) {
        memset(ptr, 0x00, sizeof(GUI_TIMER_t));     /* Reset memory */
        
        ptr->Period = period;                       /* Set period value */
        ptr->Counter = period;                      /* Set current counter value */
        ptr->Callback = callback;                   /* Set callback */
        ptr->Params = params;                       /* Timer custom parameters */
        ptr->Flags = flags;                         /* Timer flags management */
        
        __GUI_LINKEDLIST_ADD_GEN(&GUI.Timers.List, &ptr->List); /* Add timer to linked list */
    }
    return ptr;
}

void __GUI_TIMER_Remove(GUI_TIMER_t** t) {  
    __GUI_ASSERTPARAMSVOID(t && *t);                /* Check input parameters */  
    __GUI_LINKEDLIST_REMOVE_GEN(&GUI.Timers.List, &(*t)->List); /* Remove timer from linked list */
    __GUI_MEMFREE(*t);                              /* Free memory for timer */
    *t = 0;                                         /* Restore pointer */
}

void __GUI_TIMER_Start(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMSVOID(t);                      /* Check input parameters */
    t->Counter = t->Period;                         /* Reset counter to top value */
    t->Flags |= GUI_FLAG_TIMER_ACTIVE;              /* Set active flag */
}

void __GUI_TIMER_Stop(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMSVOID(t);                      /* Check input parameters */
    t->Counter = t->Period;                         /* Reset counter to top value */
    t->Flags &= ~GUI_FLAG_TIMER_ACTIVE;             /* Clear active flag */
}

void __GUI_TIMER_Reset(GUI_TIMER_t* t) {
    __GUI_ASSERTPARAMSVOID(t);                      /* Check input parameters */
    t->Counter = t->Period;                         /* Clear and reset timer */
}

void __GUI_TIMER_Process(void) {
    GUI_TIMER_t* t;
    volatile uint32_t time = GUI.Time;
    volatile uint32_t lastTime = GUI.Timers.Time;
    uint32_t diff = time - lastTime;                /* Get difference in time */
    
    if (diff == 0) {                                /* No difference in time */
        return;
    }
    
    /* Process all software timers */
    for (t = (GUI_TIMER_t *)GUI.Timers.List.First; t; t = (GUI_TIMER_t *)t->List.Next) {
        if (t->Flags & GUI_FLAG_TIMER_CALL) {       /* Counter is set to 0, process callback */
            t->Flags &= ~GUI_FLAG_TIMER_CALL;       /* Clear timer flag */
            if (t->Callback) {
                t->Callback(t);                     /* Call user function */
            }
            if (!(t->Flags & GUI_FLAG_TIMER_PERIODIC)) {    /* If timer is not periodic */
                t->Flags &= ~GUI_FLAG_TIMER_ACTIVE; /* Stop timer */
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
            t->Counter = t->Period - diff;          /* Set new counter value */
            t->Flags |= GUI_FLAG_TIMER_CALL;        /* Set callback flag in next check */
        }
    }
    
    GUI.Timers.Time = time;                         /* Reset time */
}

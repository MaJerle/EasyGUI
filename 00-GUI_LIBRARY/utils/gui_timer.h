/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI software timer for widget
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_TIMER_H
#define GUI_TIMER_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_UTILS
 * \brief       
 * \{
 */
#include "gui_utils.h"

/**
 * \defgroup        GUI_TIMER Software timers
 * \brief           Generic software timers
 * \{
 */

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
#define GUI_FLAG_TIMER_ACTIVE           ((uint16_t)(1 << 0UL))  /*!< Timer is active */
#define GUI_FLAG_TIMER_PERIODIC         ((uint16_t)(1 << 1UL))  /*!< Timer will start from beginning after reach end */ 
#define GUI_FLAG_TIMER_CALL             ((uint16_t)(1 << 2UL))  /*!< Call callback function on timer */

/**
 * \brief           Create new software timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       period: Timer period in units of milliseconds
 * \param[in]       *callback: Pointer to timer callback
 * \param[in]       *param: Pointer to user parameters used in callback
 * \param[in]       flags: List of flags for timer
 * \retval          > 0: Timer created
 * \retval          0: Timer creation failed
 */
GUI_TIMER_t* __GUI_TIMER_Create(uint16_t period, void (*callback)(GUI_TIMER_t *), void* param, uint8_t flags);

/**
 * \brief           Remove timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       **t: Pointer to pointer to \ref GUI_TIMER_t structure.
 *                      After timer remove, pointer value where it points to will be changed
 * \retval          None
 */
void __GUI_TIMER_Remove(GUI_TIMER_t** t);

/**
 * \brief           Start timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          None
 */
void __GUI_TIMER_Start(GUI_TIMER_t* t);

/**
 * \brief           Stop timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          None
 */
void __GUI_TIMER_Stop(GUI_TIMER_t* t);

/**
 * \brief           Reset timer to zero
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *t: Pointer to \ref GUI_TIMER_t structure
 * \retval          None
 */
void __GUI_TIMER_Reset(GUI_TIMER_t* t);

/**
 * \brief           Internal processing called by GUI library
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \note            Processes all timers in loop and calls callback function if needed
 * \retval          None
 */
void __GUI_TIMER_Process(void);

/**
 * \brief           Get parameters from timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       t: Timer structure
 * \retval          Timer parameters
 * \hideinitializer
 */
#define __GUI_TIMER_GetParams(t)        (t)->Params

#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \}
 */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

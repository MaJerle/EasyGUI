/**	
 * \file            gui_timer.h
 * \brief           Software timer manager
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
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
 * Author:          Tilen Majerle <tilen@majerle.eu>
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
    

/**
 * \brief           Get parameters from timer
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       t: Timer structure
 * \retval          Timer parameters
 * \hideinitializer
 */
#define gui_timer_getparams__(t)        (t)->Params

GUI_TIMER_t* gui_timer_create__(uint16_t period, void (*callback)(GUI_TIMER_t *), void* param);
uint8_t gui_timer_remove__(GUI_TIMER_t** t);
uint8_t gui_timer_start__(GUI_TIMER_t* t);
uint8_t gui_timer_startperiodic__(GUI_TIMER_t* t);
uint8_t gui_timer_stop__(GUI_TIMER_t* t);
uint8_t gui_timer_reset__(GUI_TIMER_t* t);

uint32_t gui_timer_getactivecount(void);
void gui_timer_process(void);

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

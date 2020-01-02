/**	
 * \file            gui_timer.h
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
#ifndef GUI_HDR_TIMER_H
#define GUI_HDR_TIMER_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_utils.h"

/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_TIMER Software timers
 * \brief           Generic software timers
 * \{
 */
    
/**
 * \brief           Get parameters from timer
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       t: Timer structure
 * \retval          Timer parameters
 * \hideinitializer
 */
#define guii_timer_getparams(t)        ((t)->params)

gui_timer_t* guii_timer_create(uint16_t period, void (*callback)(gui_timer_t *), void* const param);
uint8_t guii_timer_remove(gui_timer_t** const t);
uint8_t guii_timer_start(gui_timer_t* const t);
uint8_t guii_timer_startperiodic(gui_timer_t* const t);
uint8_t guii_timer_stop(gui_timer_t* const t);
uint8_t guii_timer_reset(gui_timer_t* const t);

uint32_t guii_timer_getactivecount(void);
void guii_timer_process(void);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_TIMER_H */

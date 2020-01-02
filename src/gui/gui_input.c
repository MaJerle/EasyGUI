/**	
 * \file            gui_input.c
 * \brief           Input manager
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
#include "gui/gui_input.h"
#include "system/gui_sys.h"

/* Define buffers */
#if GUI_CFG_USE_TOUCH
static gui_buff_t buff_ts;
static uint8_t buff_ts_data[sizeof(gui_touch_data_t) * GUI_CFG_TOUCH_BUFFER_SIZE + 1];
#endif /* GUI_CFG_USE_TOUCH */

#if GUI_CFG_USE_KEYBOARD
static gui_buff_t buff_kb;
static uint8_t buff_kb_data[sizeof(gui_keyboard_data_t) * GUI_CFG_KEYBOARD_BUFFER_SIZE *  + 1];
#endif /* GUI_CFG_USE_KEYBOARD */

#if GUI_CFG_USE_TOUCH || __DOXYGEN__

/**
 * \brief           Add new touch data to internal buffer for further processing
 * \param[in]       ts: Pointer to \ref gui_touch_data_t touch data with valid input
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_input_touchadd(gui_touch_data_t* const ts) {
    uint8_t ret;
    GUI_ASSERTPARAMS(ts);
    
    ts->time = gui_sys_now();                       /* Set event time */
    ret = gui_buffer_write(&buff_ts, ts, sizeof(*ts)) ? 1 : 0;  /* Write data to buffer */
    
#if GUI_CFG_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, NULL);        /* Notify stack about new key added */
#endif /* GUI_CFG_OS */
    return ret;
}

/**
 * \brief           Reads new touch entry
 * \param[out]      ts: Pointer to \ref gui_touch_data_t structure to save touch into to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_input_touchread(gui_touch_data_t* const ts) {
    if (gui_buffer_getfull(&buff_ts) >= sizeof(*ts)) {
        gui_buffer_read(&buff_ts, ts, sizeof(*ts)); /* Read data from buffer */
        return 1;
    }
    return 0;
}

/**
 * \brief           Checks if anything available for touch inputs
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_input_touchavailable(void) {
    return gui_buffer_getfull(&buff_ts) > 0;       /* Check if any available touch */
}

#endif /* GUI_CFG_USE_TOUCH || __DOXYGEN__ */

#if GUI_CFG_USE_KEYBOARD || __DOXYGEN__

/**
 * \brief           Add new key data to internal buffer for further processing
 * \param[in]       kb: Pointer to \ref gui_keyboard_data_t key data
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_input_keyadd(gui_keyboard_data_t* const kb) {
    uint8_t ret;
    GUI_ASSERTPARAMS(kb);
    kb->time = gui_sys_now();                       /* Set event time */
    ret = gui_buffer_write(&buff_kb, kb, sizeof(*kb)) ? 1 : 0; /* Write data to buffer */
    
#if GUI_CFG_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, NULL);        /* Notify stack about new key added */
#endif /* GUI_CFG_OS */
    return ret;
}

/**
 * \brief           Read keyboard entry from buffer
 * \param[out]      kb: Pointer to \ref gui_keyboard_data_t to save entry to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_input_keyread(gui_keyboard_data_t* const kb) {
    if (gui_buffer_getfull(&buff_kb) >= sizeof(*kb)) {
        gui_buffer_read(&buff_kb, kb, sizeof(*kb)); /* Read data from buffer */
        return 1;
    }
    return 0;
}
#endif /* GUI_CFG_USE_KEYBOARD || __DOXYGEN__ */

/**
 * \brief           Initialize input manager for GUI
 */
void
guii_input_init(void) {
#if GUI_CFG_USE_TOUCH
    gui_buffer_init(&buff_ts, sizeof(buff_ts_data), buff_ts_data);
#endif /* GUI_CFG_USE_TOUCH */
#if GUI_CFG_USE_KEYBOARD
    gui_buffer_init(&buff_kb, sizeof(buff_kb_data), buff_kb_data);
#endif /* GUI_CFG_USE_KEYBOARD */
}

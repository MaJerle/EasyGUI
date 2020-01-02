/**	
 * \file            gui_buff.h
 * \brief           Ring buffer manager
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
#ifndef GUI_HDR_BUFF_H
#define GUI_HDR_BUFF_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_BUFFER Ring buffer
 * \brief           Generic cyclic buffer library
 * \{
 */
 
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define GUI_BUFFER_INITIALIZED      0x01    /*!< Buffer initialized flag */
#define GUI_BUFFER_MALLOC           0x02    /*!< Buffer uses malloc for memory */

/**
 * \brief  Buffer structure
 */
typedef struct gui_buff_t {
	uint32_t size;                  /*!< Size of buffer in units of bytes, DO NOT MOVE OFFSET, 0 */
	uint32_t in;                    /*!< Input pointer to save next value, DO NOT MOVE OFFSET, 1 */
	uint32_t out;                   /*!< Output pointer to read next value, DO NOT MOVE OFFSET, 2 */
	uint8_t* buff;                  /*!< Pointer to buffer data array, DO NOT MOVE OFFSET, 3 */
	uint8_t flags;                  /*!< flags for buffer, DO NOT MOVE OFFSET, 4 */
	void* arg;                      /*!< Pointer to user value if needed */
} gui_buff_t;

uint8_t gui_buffer_init(gui_buff_t* buff, uint32_t size, void* buff_ptr);
void gui_buffer_free(gui_buff_t* buff);
uint32_t gui_buffer_write(gui_buff_t* buff, const void* data, uint32_t count);
uint32_t gui_buffer_read(gui_buff_t* buff, void* data, uint32_t count);
uint32_t gui_buffer_getfree(gui_buff_t* buff);
uint32_t gui_buffer_getfull(gui_buff_t* buff);
void gui_buffer_reset(gui_buff_t* buff);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_BUFF_H */

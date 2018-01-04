/**	
 * \file            gui_buffer.h
 * \brief           Ring buffer manager
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
#ifndef __GUI_BUFFER_H
#define __GUI_BUFFER_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_UTILS
 * \{
 */
    
/**
 * \defgroup        GUI_BUFFER Ring buffer
 * \brief           Generic cyclic buffer library
 * \{
 */
 
#include "gui_utils.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"

#define GUI_BUFFER_INITIALIZED      0x01    /*!< Buffer initialized flag */
#define GUI_BUFFER_MALLOC           0x02    /*!< Buffer uses malloc for memory */

#ifndef GUI_BUFFER_FAST
#define GUI_BUFFER_FAST             1
#endif

/**
 * \brief  Buffer structure
 */
typedef struct _GUI_BUFFER_t {
	uint32_t Size;                  /*!< Size of buffer in units of bytes, DO NOT MOVE OFFSET, 0 */
	uint32_t In;                    /*!< Input pointer to save next value, DO NOT MOVE OFFSET, 1 */
	uint32_t Out;                   /*!< Output pointer to read next value, DO NOT MOVE OFFSET, 2 */
	uint8_t* Buffer;                /*!< Pointer to buffer data array, DO NOT MOVE OFFSET, 3 */
	uint8_t Flags;                  /*!< Flags for buffer, DO NOT MOVE OFFSET, 4 */
	void* UserParameters;           /*!< Pointer to user value if needed */
} GUI_BUFFER_t;

uint8_t gui_buffer_init(GUI_BUFFER_t* Buffer, uint32_t Size, void* BufferPtr);
void gui_buffer_free(GUI_BUFFER_t* Buffer);
uint32_t gui_buffer_write(GUI_BUFFER_t* Buffer, const void* Data, uint32_t count);
uint32_t gui_buffer_read(GUI_BUFFER_t* Buffer, void* Data, uint32_t count);
uint32_t gui_buffer_getfree(GUI_BUFFER_t* Buffer);
uint32_t gui_buffer_getfull(GUI_BUFFER_t* Buffer);
void gui_buffer_reset(GUI_BUFFER_t* Buffer);

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

#endif /* __GUI_BUFFER_H */

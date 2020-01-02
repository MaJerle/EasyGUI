/**	
 * \file            gui_mem.h
 * \brief           Memory manager
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
#ifndef GUI_HDR_MEM_H
#define GUI_HDR_MEM_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_utils.h"

/**
 * \ingroup         GUI_UTILS
 * \defgroup        GUI_MEM Memory management
 * \brief           Common memory management functions
 * \{
 */

/**
 * \brief           Single memory region descriptor
 */
typedef struct mem_region_t {
    void* start_address;                /*!< Start address of region */
    size_t size;                        /*!< Size in units of bytes of region */
} mem_region_t;

/**
 * \brief           Wrapper for memory region for GUI
 */
typedef mem_region_t gui_mem_region_t;

void* gui_mem_alloc(uint32_t size);
void* gui_mem_realloc(void* ptr, size_t size);
void* gui_mem_calloc(size_t num, size_t size);
void gui_mem_free(void* ptr);
size_t gui_mem_getfree(void);
size_t gui_mem_getfull(void);
size_t gui_mem_getminfree(void);

uint8_t gui_mem_assignmemory(const gui_mem_region_t* regions, size_t size);
    
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_MEM_H */

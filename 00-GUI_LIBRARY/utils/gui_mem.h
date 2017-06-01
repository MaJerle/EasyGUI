/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI memory management
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2017 Tilen Majerle

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
#ifndef GUI_MEM_H
#define GUI_MEM_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_UTILS
 * \{
 */
#include "gui_utils.h"

/**
 * \defgroup        GUI_MEM Memory management
 * \brief           Common memory management functions
 * \{
 */

/**
 * \brief           Single memory region descriptor
 */
typedef struct mem_region_t {
    void* StartAddress;                 /*!< Start address of region */
    size_t Size;                        /*!< Size in units of bytes of region */
} mem_region_t;

/**
 * \brief           Wrapper for memory region for GUI
 */
typedef mem_region_t GUI_MEM_Region_t;
    
/**
 * \defgroup        GUI_MEM_Int Internal functions
 * \note            Internal functions are not thread safe but since GUI API is thread safe,
 *                  they can be easily used inside API for fast processing without locking check
 * \brief           Internal functions API which is not thread safe
 * \{
 */

void* __GUI_MEM_Alloc(uint32_t size);
void* __GUI_MEM_Realloc(void* ptr, size_t size);
void* __GUI_MEM_Calloc(size_t num, size_t size);
void __GUI_MEM_Free(void* ptr);
size_t __GUI_MEM_GetFree(void);
size_t __GUI_MEM_GetFull(void);
size_t __GUI_MEM_GetMinFree(void);
  
/**
 * \}
 */
 
uint8_t GUI_MEM_AssignMemory(const GUI_MEM_Region_t* memory, size_t size);
void* GUI_MEM_Alloc(uint32_t size);
void* GUI_MEM_Realloc(void* ptr, size_t size);
void* GUI_MEM_Calloc(size_t num, size_t size);
void GUI_MEM_Free(void* ptr);
size_t GUI_MEM_GetFree(void);
size_t GUI_MEM_GetFull(void);
size_t GUI_MEM_GetMinFree(void);
    
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

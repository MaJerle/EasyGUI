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
 *
 * Since dynamic allocation can be hard job for some microcontrollers, 
 * library implements memory management parts with simple allocation process in different regions.
 *
 * Memory management mode allows allocation of different sizes and allows free memory. 
 * It also takes cover to free memory and to create one big block if multiple regions are freed in sequence
 *
 * Memory management can be used in multiple regions which may optimize RAM usage, for example:
 *  - Use TCM/CCM RAM for widget structures (low amount of bytes, up to ~150, but depends on widget)
 *  - Use of large amount of memory (ex. temporary frame buffers) on external RAM (SDRAM) module if supported by embedded device
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

/**
 * \brief           Allocate memory of specific size
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       size: Number of bytes to allocate
 * \retval          0: Allocation failed
 * \retval          >0: Pointer to allocated memory
 */
void* __GUI_MEM_Alloc(uint32_t size);

/**
 * \brief           Allocate memory of specific size
 * \note            After new memory is allocated, content of old one is copied to new allocated memory
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *ptr: Pointer to current allocated memory to resize, returned using \ref GUI_MEM_Alloc, \ref GUI_MEM_Calloc or \ref GUI_MEM_Realloc functions
 * \param[in]       size: Number of bytes to allocate on new memory
 * \retval          0: Allocation failed
 * \retval          >0: Pointer to allocated memory
 */
void* __GUI_MEM_Realloc(void* ptr, size_t size);

/**
 * \brief           Allocate memory of specific size and set memory to zero
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       num: Number of elements to allocate
 * \param[in]       size: Size of each element
 * \retval          0: Allocation failed
 * \retval          >0: Pointer to allocated memory
 */
void* __GUI_MEM_Calloc(size_t num, size_t size);

/**
 * \brief           Free memory
 * \param[in]       *ptr: Pointer to memory previously returned using \ref GUI_MEM_Alloc, \ref GUI_MEM_Calloc or \ref GUI_MEM_Realloc functions
 */
void __GUI_MEM_Free(void* ptr);

/**
 * \brief           Get total free size still available in memory to allocate
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \retval          Number of bytes available to allocate
 */
size_t __GUI_MEM_GetFree(void);

/**
 * \brief           Get total currently allocated memory in regions
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \retval          Number of bytes in use
 */
size_t __GUI_MEM_GetFull(void);

/**
 * \brief           Get minimal available number of bytes ever for allocation
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \retval          Number of minimal available number of bytes ever
 */
size_t __GUI_MEM_GetMinFree(void);
  
/**
 * \}
 */

/**
 * \brief           Assign memory region(s) for allocation functions
 * \note            You can allocate multiple regions by assigning start address and region size in units of bytes
 * \param[in]       *regions: Pointer to list of regions to use for allocations
 * \param[in]       size: Number of regions to use
 * \retval          1: Memory assigned ok
 * \retval          0: Memory was not assigned
 */
uint8_t GUI_MEM_AssignMemory(const GUI_MEM_Region_t* regions, size_t size);

/**
 * \brief           Allocate memory of specific size
 * \param[in]       size: Number of bytes to allocate
 * \retval          0: Allocation failed
 * \retval          >0: Pointer to allocated memory
 */
#define GUI_MEM_Alloc       __GUI_MEM_Alloc

/**
 * \brief           Allocate memory of specific size
 * \note            After new memory is allocated, content of old one is copied to new allocated memory
 * \param[in]       *ptr: Pointer to current allocated memory to resize, returned using \ref GUI_MEM_Alloc, \ref GUI_MEM_Calloc or \ref GUI_MEM_Realloc functions
 * \param[in]       size: Number of bytes to allocate on new memory
 * \retval          0: Allocation failed
 * \retval          >0: Pointer to allocated memory
 */
#define GUI_MEM_Realloc     __GUI_MEM_Realloc

/**
 * \brief           Allocate memory of specific size and set memory to zero
 * \param[in]       num: Number of elements to allocate
 * \param[in]       size: Size of each element
 * \retval          0: Allocation failed
 * \retval          >0: Pointer to allocated memory
 */
#define GUI_MEM_Calloc      __GUI_MEM_Calloc

/**
 * \brief           Free memory
 * \param[in]       *ptr: Pointer to memory previously returned using \ref GUI_MEM_Alloc, \ref GUI_MEM_Calloc or \ref GUI_MEM_Realloc functions
 */
#define GUI_MEM_Free        __GUI_MEM_Free

/**
 * \brief           Get total free size still available in memory to allocate
 * \retval          Number of bytes available to allocate
 */
#define GUI_MEM_GetFree     __GUI_MEM_GetFree

/**
 * \brief           Get total currently allocated memory in regions
 * \retval          Number of bytes in use
 */
#define GUI_MEM_GetFull     __GUI_MEM_GetFull

/**
 * \brief           Get minimal available number of bytes ever for allocation
 * \retval          Number of minimal available number of bytes ever
 */
#define GUI_MEM_GetMinFree  __GUI_MEM_GetMinFree
    
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

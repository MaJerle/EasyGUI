/**    
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
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
#include "gui_mem.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct MemBlock {
    struct MemBlock* NextFreeBlock;                 /*!< Pointer to next free block */
    size_t Size;                                    /*!< Size of block */
} MemBlock_t;

#define MEMBLOCK_METASIZE           sizeof(MemBlock_t)


#define GUI_USE_MEM     1

/**
 * \brief           Memory alignment bits
 */
#define MEM_ALIGN_BITS              ((size_t)0x03)

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
static MemBlock_t StartBlock;
static MemBlock_t* EndBlock = 0;
static size_t MemAvailableBytes = 0;
static size_t MemMinAvailableBytes = 0;
static size_t MemAllocBit = 0;

static size_t MemTotalSize = 0;                     /* Size of memory in units of bytes */
static uint8_t* HeapMem = 0;                        /* Array holding HEAP memory */

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
/* Initialize memory for first use */
static
void __mem_init(void) {
    void* MemStartAddr;
    MemBlock_t* FirstBlock;
    
    if (!HeapMem || !MemTotalSize) {                /* Check if memory assigned */
        return;
    }

    /**
     * TODO: Memory alignment bits check
     */
    
    MemStartAddr = (void *)HeapMem;                 /* Actual heap memory address*/

    /**
     * StartBlock is fixed variable for start list of free blocks
     *
     * Set free blocks linked list on initialized
     */
    StartBlock.NextFreeBlock = (MemBlock_t *)MemStartAddr;
    StartBlock.Size = 0;

    /**
     * Set pointer to end of free memory
     */
    EndBlock = (MemBlock_t *)((uint8_t *)MemStartAddr + MemTotalSize - MEMBLOCK_METASIZE);
    EndBlock->NextFreeBlock = 0;                    /* No more free blocks after end is reached */
    EndBlock->Size = 0;                             /* Empty block */

    /**
     * Initialize start of memory. On start, entire block is free
     */
    FirstBlock = (MemBlock_t *)MemStartAddr;
    FirstBlock->Size = MemTotalSize - MEMBLOCK_METASIZE;    /* Exclude end block in chain */
    FirstBlock->NextFreeBlock = EndBlock;           /* Last block is next free in chain */

    /**
     * Set number of free bytes available to allocate
     */
    MemAvailableBytes = FirstBlock->Size;
    MemMinAvailableBytes = MemAvailableBytes;
    
    /**
     * Set upper bit in memory allocation bit
     */
    MemAllocBit = 1 << (sizeof(uint8_t) * 8 - 1);
}

/* Insert block to list of free blocks */
static
void __mem_insertfreeblock(MemBlock_t* newBlock) {
    MemBlock_t* ptr;
    uint8_t* addr;

    /**
     * Find block position to insert new block between
     */
    for (ptr = &StartBlock; ptr && ptr->NextFreeBlock < newBlock; ptr = ptr->NextFreeBlock);

    /**
     * If the new inserted block and block before create a one big block (contiguous)
     * then try to merge them together
     */
    addr = (uint8_t *)ptr;
    if ((uint8_t *)(addr + ptr->Size) == (uint8_t *)newBlock) {
        ptr->Size += newBlock->Size;                /* Expand size of block before new inserted */
        newBlock = ptr;                             /* Set new block pointer to block before (expanded block) */
    }

    /**
    * Check if new block and its size is the same address as next free block newBlock points to
    */
    addr = (uint8_t *)newBlock;
    if ((uint8_t *)(addr + newBlock->Size) == (uint8_t *)ptr->NextFreeBlock) {
        if (ptr->NextFreeBlock == EndBlock) {       /* Does it points to the end? */
            newBlock->NextFreeBlock = EndBlock;     /* Set end block pointer */
        } else {
            newBlock->Size += ptr->NextFreeBlock->Size; /* Expand of current block for size of next free block which is right behind new block */
            newBlock->NextFreeBlock = ptr->NextFreeBlock->NextFreeBlock; /* Next free is pointed to the next one of previous next */
        }
    } else {
        newBlock->NextFreeBlock = ptr->NextFreeBlock;   /* Our next element is now from pointer next element */
    }

    /**
    * If merge with new block and block before was not made then there
    * is a gap between free memory before and new free memory.
    *
    * We have to set block before to point to next free which is new block
    */
    if (ptr != newBlock) {
        ptr->NextFreeBlock = newBlock;
    }
}

uint8_t mem_assignmem(const mem_region_t* regions, size_t len) {
    /**
     * TODO: Before memory change, check if entire memory is free
     *
     * TODO: Allow multiple regions
     */
    if (!HeapMem || !MemTotalSize) {
        HeapMem = regions->StartAddress;            /* Set pointer to memory */
        MemTotalSize = regions->Size;               /* Set total memory size */
        EndBlock = 0;                               /* Reset end block to unknown value */
        __mem_init();                               /* Reinit memory */
        return 1;
    }
    return 0;
}

void* mem_alloc(size_t size) {
    MemBlock_t *Prev, *Curr, *Next;
    void* retval = 0;

    if (!EndBlock) {                                /* If end block is not yet defined */
        __mem_init();                               /* Init library for first time */
        if (!EndBlock) {                            /* Check if still not initialized */
            return 0;
        }
    }
    
    /**
     * TODO: Check alignment maybe?
     */    
    if (!size || size >= MemAllocBit) {             /* Check input parameters */
        return 0;
    }

    size += MEMBLOCK_METASIZE;                      /* Increase allocation size for meta data */
    if ((size & MEM_ALIGN_BITS)) {
        size = (size + MEM_ALIGN_BITS) & ~MEM_ALIGN_BITS;
    }
    if (size > MemAvailableBytes) {                 /* Check if we have enough memory available */
        return 0;
    }

    /**
     * Try to find sufficient block for data
     * Go through free blocks until enough memory is found
     * or end block is reached (no next free block)
     */
    Prev = &StartBlock;                             /* Set first first block as previous */
    Curr = Prev->NextFreeBlock;                     /* Set next block as current */
    while ((Curr->Size < size) && (Curr->NextFreeBlock)) {
        Prev = Curr;
        Curr = Curr->NextFreeBlock;
        if ((uint32_t)Curr < 1000) {
            __nop();
        }
    }
    
    /**
     * Possible improvements
     * Try to find smallest available block for desired amount of memory
     * 
     * Feature may be very risky later because of fragmentation
     */
    
    if (Curr != EndBlock) {                         /* We found empty block of enough memory available */
        retval = (void *)((uint8_t *)Prev->NextFreeBlock + MEMBLOCK_METASIZE);    /* Set return value */
        Prev->NextFreeBlock = Curr->NextFreeBlock;  /* Since block is now allocated, remove it from free chain */

        /**
         * If found free block is much bigger than required, 
         * then split big block by 2 blocks (one used, second available)
         * There should be available memory for at least 2 metadata block size = 8 bytes of useful memory
         */
        if ((Curr->Size - size) > (2 * MEMBLOCK_METASIZE)) {    /* There is more available memory then required = split memory to one free block */
            Next = (MemBlock_t *)(((uint8_t *)Curr) + size);    /* Create next memory block which is still free */
            Next->Size = Curr->Size - size;         /* Set new block size for remaining of before and used */
            Curr->Size = size;                      /* Set block size for used block */

            /**
             * Add virtual block to list of free blocks.
             * It is placed directly after currently allocated memory
             */
            __mem_insertfreeblock(Next);            /* Insert free memory block to list of free memory blocks (linked list chain) */
        }
        Curr->Size |= MemAllocBit;                  /* Set allocated bit = memory is allocated */
        Curr->NextFreeBlock = 0;                    /* Clear next free block pointer as there is no one */

        MemAvailableBytes -= size;                  /* Decrease available memory */
        if (MemAvailableBytes < MemMinAvailableBytes) { /* Check if current available memory is less than ever before */
            MemMinAvailableBytes = MemAvailableBytes;   /* Update minimal available memory */
        }
    } else {
        /* Allocation failed, no free blocks of required size */
    }

    return retval;
}

void mem_free(void* ptr) {
    MemBlock_t* block;

    if (!ptr) {                                     /* To be in compliance with C free function */
        return;
    }

    block = (MemBlock_t *)(((uint8_t *)ptr) - MEMBLOCK_METASIZE);   /* Get block data pointer from input pointer */

    /**
     * Check if block is even allocated by upper bit on size
     * and next free block must be set to NULL in order to work properly
     */
    if ((block->Size & MemAllocBit) && !block->NextFreeBlock) {
        /**
         * Clear allocated bit before entering back to free list
         * List will automatically take care for fragmentation and mix segments back
         */
        block->Size &= ~MemAllocBit;                /* Clear allocated bit */
        MemAvailableBytes += block->Size;           /* Increase available bytes back */
        __mem_insertfreeblock(block);               /* Insert block to list of free blocks */
    }
}

/* Get size of user memory from input pointer */
size_t mem_getusersize(void* ptr) {
    MemBlock_t* block;
    
    if (!ptr) {
        return 0;
    }
    block = (MemBlock_t *)(((uint8_t *)ptr) - MEMBLOCK_METASIZE);   /* Get block meta data pointer */
    if (block->Size & MemAllocBit) {                /* Memory is actually allocated */
        return (block->Size & ~MemAllocBit) - MEMBLOCK_METASIZE;    /* return size of block */
    }
    return 0;
}

/* Allocate memory and set it to 0 */
void* mem_calloc(size_t num, size_t size) {
    void* ptr;
    size_t tot_len = num * size;
    
    if ((ptr = mem_alloc(tot_len)) != NULL) {       /* Try to allocate memory */
        memset(ptr, 0x00, tot_len);                 /* Reset entire memory */
    }
    return ptr;
}

/* Reallocate previously allocated memory */
void* mem_realloc(void* ptr, size_t size) {
    void* newPtr;
    size_t oldSize;
    
    if (!ptr) {                                     /* If pointer is not valid */
        return __GUI_MEM_Alloc(size);               /* Only allocate memory */
    }
    
    oldSize = mem_getusersize(ptr);                 /* Get size of old pointer */
    newPtr = mem_alloc(size);                       /* Try to allocate new memory block */
    if (newPtr) {                                   /* Check success */
        memcpy(newPtr, ptr, size > oldSize ? oldSize : size);   /* Copy old data to new array */
        mem_free(ptr);                              /* Free old pointer */
        return newPtr;                              /* Return new pointer */
    }
    return 0;
}

size_t mem_getfree(void) {
    return MemAvailableBytes;                       /* Return free bytes available for allocation */
}

size_t mem_getfull(void) {
    return MemTotalSize - MemAvailableBytes;        /* Return remaining bytes */
}

size_t mem_getminfree(void) {
    return MemMinAvailableBytes;                    /* Return minimal bytes ever available */
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
void* __GUI_MEM_Alloc(uint32_t size) {
#if GUI_USE_MEM
    return mem_alloc(size);                         /* Allocate memory and return pointer */ 
#else
    return malloc(size);
#endif    
}

void* __GUI_MEM_Realloc(void* ptr, size_t size) {
#if GUI_USE_MEM
    return mem_realloc(ptr, size);                  /* Reallocate and return pointer */
#else
    return realloc(ptr, size);
#endif    
}

void* __GUI_MEM_Calloc(size_t num, size_t size) {
#if GUI_USE_MEM
    return mem_calloc(num, size);                   /* Allocate memory and clear it to 0. Then return pointer */
#else
    return calloc(num, size);
#endif    
}

void __GUI_MEM_Free(void* ptr) {
#if GUI_USE_MEM
    mem_free(ptr);                                  /* Free already allocated memory */
#else
    free(ptr);
#endif    
}

size_t __GUI_MEM_GetFree(void) {
    return mem_getfree();                           /* Get free bytes available to allocate */
}

size_t __GUI_MEM_GetFull(void) {
    return mem_getfull();                           /* Get number of bytes allocated already */
}

size_t __GUI_MEM_GetMinFree(void) {
    return mem_getminfree();                        /* Get minimal number of bytes ever available for allocation */
}

/******************************************************************************/
/******************************************************************************/
/***                  Thread safe version of public API                      **/
/******************************************************************************/
/******************************************************************************/
uint8_t GUI_MEM_AssignMemory(const GUI_MEM_Region_t* regions, size_t len) {
    uint8_t ret;
    __GUI_ENTER();                                  /* Enter GUI */
    ret = mem_assignmem(regions, len);              /* Assign memory */
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;                                     
}

void* GUI_MEM_Alloc(uint32_t size) {
    void* ptr;
    __GUI_ENTER();                                  /* Enter GUI */
    ptr = __GUI_MEM_Alloc(size);                    /* Allocate memory for specific size */
    __GUI_LEAVE();                                  /* Leave GUI */
    return ptr;                                     
}

void* GUI_MEM_Realloc(void* ptr, size_t size) {
    __GUI_ENTER();                                  /* Enter GUI */
    ptr = __GUI_MEM_Realloc(ptr, size);             /* Realloc to new size */
    __GUI_LEAVE();                                  /* Leave GUI */
    return ptr;
}

void* GUI_MEM_Calloc(size_t num, size_t size) {
    void* ptr;
    __GUI_ENTER();                                  /* Enter GUI */
    ptr = __GUI_MEM_Calloc(num, size);              /* Calloc to new size */
    __GUI_LEAVE();                                  /* Leave GUI */
    return ptr;
}

void GUI_MEM_Free(void* ptr) {
    __GUI_ENTER();                                  /* Enter GUI */
    __GUI_MEM_Free(ptr);                            /* Free allocated memory */
    __GUI_LEAVE();                                  /* Leave GUI */
}

size_t GUI_MEM_GetFree(void) {
    size_t size;
    __GUI_ENTER();                                  /* Enter GUI */
    size = mem_getfree();                           /* Get free bytes available to allocate */
    __GUI_LEAVE();                                  /* Leave GUI */
    return size;
}

size_t GUI_MEM_GetFull(void) {
    size_t size;
    __GUI_ENTER();                                  /* Enter GUI */
    size = mem_getfull();                           /* Get number of bytes allocated already */
    __GUI_LEAVE();                                  /* Leave GUI */
    return size;
}

size_t GUI_MEM_GetMinFree(void) {
    size_t size;
    __GUI_ENTER();                                  /* Enter GUI */
    size = mem_getminfree();                        /* Get minimal number of bytes ever available for allocation */
    __GUI_LEAVE();                                  /* Leave GUI */
    return size;
}

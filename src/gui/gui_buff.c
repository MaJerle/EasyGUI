/**    
 * \file            gui_buff.c
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
#define GUI_INTERNAL
#include "gui/gui_buff.h"

/**
 * \brief          Initializes buffer structure for work
 * \param          buff: Pointer to \ref gui_buff_t structure to initialize
 * \param          size: Size of buffer in units of bytes
 * \param          buff_ptr: Pointer to array for buffer storage. Its length should be equal to \param Size parameter.
 *                    If NULL is passed as parameter, internal memory management will be used to allocate memory
 * \return         buff initialization status:
 *                    - 0: buff initialized OK
 *                    - > 0: buff initialization error. Malloc has failed with allocation
 */
uint8_t
gui_buffer_init(gui_buff_t* buff, uint32_t size, void* buff_ptr) {
    if (buff == NULL) {                         /* Check buffer structure */
        return 1;
    }
    memset(buff, 0, sizeof(*buff));             /* Set buffer values to all zeros */
    
    buff->size = size;                          /* Set default values */
    buff->buff = buff_ptr;
    
    if (buff->buff == NULL) {                   /* Check if malloc should be used */
        buff->buff = GUI_MEMALLOC(size * sizeof(uint8_t));  /* Try to allocate memory for buffer */
        if (buff->buff == NULL) {               /* Check if allocated */    
            buff->size = 0;                     /* Reset size */
            return 1;                           /* Return error */
        } else {
            buff->flags |= GUI_BUFFER_MALLOC;   /* Set flag for malloc */
        }
    }
    buff->flags |= GUI_BUFFER_INITIALIZED;      /* We are initialized */
    
    return 0;                                   /* Initialized OK */
}

/**
 * \brief  Free memory for buffer allocated using dynamic memory allocation
 * \note   This function has sense only if dynamic allocation was used for memory buffer
 * \param  buff: Pointer to \ref gui_buff_t structure
 */
void 
gui_buffer_free(gui_buff_t* buff) {
    if (buff == NULL) {                         /* Check buffer structure */
        return;
    }
    if (buff->flags & GUI_BUFFER_MALLOC) {      /* If malloc was used for allocation */
        GUI_MEMFREE(buff->buff);                /* Free memory */
    }
    buff->buff = NULL;
    buff->flags = 0;
    buff->size = 0;
}

/**
 * \brief  Writes data to buffer
 * \param  buff: Pointer to \ref gui_buff_t structure
 * \param  data: Pointer to data to be written
 * \param  count: Number of elements of type unsigned char to write
 * \return Number of elements written in buffer 
 */
uint32_t gui_buffer_write(gui_buff_t* buff, const void* data, uint32_t count) {
    uint32_t i = 0, free, tocopy;
    const uint8_t* d = (const uint8_t *)data;

    if (buff == NULL || count == 0) {           /* Check buffer structure */
        return 0;
    }
    if (buff->in >= buff->size) {               /* Check input pointer */
        buff->in = 0;
    }
    free = gui_buffer_getfree(buff);            /* Get free memory */
    if (free < count) {                         /* Check available memory */    
        if (free == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = free;                           /* Set values for write */
    }

    /* We have calculated memory for write */
    tocopy = buff->size - buff->in;             /* Calculate number of elements we can put at the end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    memcpy(&buff->buff[buff->in], d, tocopy);   /* Copy content to buffer */
    i += tocopy;                                /* Increase number of bytes we copied already */
    buff->in += tocopy;    
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to write */    
        memcpy(buff->buff, (void *)&d[i], count);   /* Copy content */
        buff->in = count;                       /* Set input pointer */
    }
    if (buff->in >= buff->size) {               /* Check input overflow */
        buff->in = 0;
    }
    return (i + count);                         /* Return number of elements stored in memory */
}

/**
 * \brief  Reads data from buffer
 * \param  buff: Pointer to \ref gui_buff_t structure
 * \param  data: Pointer to data where read values will be stored
 * \param  count: Number of elements of type unsigned char to read
 * \return Number of elements read from buffer 
 */
uint32_t
gui_buffer_read(gui_buff_t* buff, void* data, uint32_t count) {
    uint32_t i = 0, full, tocopy;
    uint8_t *d = (uint8_t *)data;
    
    if (buff == NULL || count == 0) {           /* Check buffer structure */
        return 0;
    }
    if (buff->out >= buff->size) {              /* Check output pointer */
        buff->out = 0;
    }
    full = gui_buffer_getfull(buff);            /* Get free memory */
    if (full < count) {                         /* Check available memory */
        if (full == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = full;                           /* Set values for write */
    }

    tocopy = buff->size - buff->out;            /* Calculate number of elements we can read from end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    memcpy(d, &buff->buff[buff->out], tocopy);  /* Copy content from buffer */
    i += tocopy;                                /* Increase number of bytes we copied already */
    buff->out += tocopy;
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to read */
        memcpy(&d[i], buff->buff, count);       /* Copy content */
        buff->out = count;                      /* Set input pointer */
    }
    if (buff->out >= buff->size) {              /* Check output overflow */
        buff->out = 0;
    }
    return (i + count);                         /* Return number of elements stored in memory */
}

/**
 * \brief  Gets number of free elements in buffer 
 * \param  buff: Pointer to \ref gui_buff_t structure
 * \return Number of free elements in buffer
 */
uint32_t
gui_buffer_getfree(gui_buff_t* buff) {
    uint32_t size = 0, in, out;
    
    if (buff == NULL) {                         /* Check buffer structure */
        return 0;
    }
    in = buff->in;                              /* Save values */
    out = buff->out;
    if (in == out) {                            /* Check if the same */
        size = buff->size;
    } else if (out > in) {                      /* Check normal mode */
        size = out - in;
    } else {                                    /* Check if overflow mode */
        size = buff->size - (in - out);
    }
    return size - 1;                            /* Return free memory */
}

/**
 * \brief  Gets number of elements in buffer 
 * \param  buff: Pointer to \ref gui_buff_t structure
 * \return Number of elements in buffer
 */
uint32_t
gui_buffer_getfull(gui_buff_t* buff) {
    uint32_t in, out, size;
    
    if (buff == NULL) {                         /* Check buffer structure */
        return 0;
    }
    in = buff->in;                              /* Save values */
    out = buff->out;
    if (in == out) {                            /* Pointer are same? */
        size = 0;
    } else if (in > out) {                      /* buff is not in overflow mode */
        size = in - out;
    } else {                                    /* buff is in overflow mode */
        size = buff->size - (out - in);
    }
    return size;                                /* Return number of elements in buffer */
}

/**
 * \brief  Resets (clears) buffer pointers
 * \param  buff: Pointer to \ref gui_buff_t structure
 */
void
gui_buffer_reset(gui_buff_t* buff) {
    if (buff == NULL) {                         /* Check buffer structure */
        return;
    }
    buff->in = 0;                               /* Reset values */
    buff->out = 0;
}

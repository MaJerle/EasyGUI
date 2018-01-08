/**	
 * \file            gui_buffer.c
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
#define GUI_INTERNAL
#include "gui/gui_buffer.h"

/**
 * \brief  Initializes buffer structure for work
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure to initialize
 * \param  Size: Size of buffer in units of bytes
 * \param  *BufferPtr: Pointer to array for buffer storage. Its length should be equal to \param Size parameter.
 *           If NULL is passed as parameter, internal memory management will be used to allocate memory
 * \return Buffer initialization status:
 *            - 0: Buffer initialized OK
 *            - > 0: Buffer initialization error. Malloc has failed with allocation
 */
uint8_t
gui_buffer_init(GUI_BUFFER_t* Buffer, uint32_t Size, void* BufferPtr) {
	if (Buffer == NULL) {									/* Check buffer structure */
		return 1;
	}
	memset(Buffer, 0, sizeof(*Buffer));        		        /* Set buffer values to all zeros */
    
	Buffer->Size = Size;                        			/* Set default values */
	Buffer->Buffer = BufferPtr;
	
	if (!Buffer->Buffer) {                      			/* Check if malloc should be used */
		Buffer->Buffer = GUI_MEMALLOC(Size * sizeof(uint8_t));  /* Try to allocate memory for buffer */
		if (Buffer->Buffer == NULL) {                  	    /* Check if allocated */    
			Buffer->Size = 0;                               /* Reset size */
			return 1;                           			/* Return error */
		} else {
			Buffer->Flags |= GUI_BUFFER_MALLOC;     		/* Set flag for malloc */
		}
	}
	Buffer->Flags |= GUI_BUFFER_INITIALIZED;				/* We are initialized */
	
	return 0;												/* Initialized OK */
}

/**
 * \brief  Free memory for buffer allocated using dynamic memory allocation
 * \note   This function has sense only if dynamic allocation was used for memory buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \return None
 */
void 
gui_buffer_free(GUI_BUFFER_t* Buffer) {
	if (Buffer == NULL) {									/* Check buffer structure */
		return;
	}
	if (Buffer->Flags & GUI_BUFFER_MALLOC) {			    /* If malloc was used for allocation */
		GUI_MEMFREE(Buffer->Buffer);                        /* Free memory */
	}
	Buffer->Flags = 0;
	Buffer->Size = 0;
}

/**
 * \brief  Writes data to buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *Data: Pointer to data to be written
 * \param  count: Number of elements of type unsigned char to write
 * \return Number of elements written in buffer 
 */
uint32_t gui_buffer_write(GUI_BUFFER_t* Buffer, const void* Data, uint32_t count) {
	uint32_t i = 0;
	uint32_t free;
    const uint8_t* d = (const uint8_t *)Data;
#if GUI_BUFFER_FAST
	uint32_t tocopy;
#endif

	if (Buffer == NULL || count == 0) {						/* Check buffer structure */
		return 0;
	}
	if (Buffer->In >= Buffer->Size) {						/* Check input pointer */
		Buffer->In = 0;
	}
	free = gui_buffer_getfree(Buffer);							/* Get free memory */
	if (free < count) {										/* Check available memory */	
		if (free == 0) {									/* If no memory, stop execution */
			return 0;
		}
		count = free;										/* Set values for write */
	}

	/* We have calculated memory for write */
#if GUI_BUFFER_FAST
	tocopy = Buffer->Size - Buffer->In;						/* Calculate number of elements we can put at the end of buffer */
	if (tocopy > count) {									/* Check for copy count */
		tocopy = count;
	}
	memcpy(&Buffer->Buffer[Buffer->In], d, tocopy);		    /* Copy content to buffer */
	i += tocopy;											/* Increase number of bytes we copied already */
	Buffer->In += tocopy;	
	count -= tocopy;
	if (count > 0) {										/* Check if anything to write */	
		memcpy(Buffer->Buffer, (void *)&d[i], count);		/* Copy content */
		Buffer->In = count;									/* Set input pointer */
	}
	if (Buffer->In >= Buffer->Size) {						/* Check input overflow */
		Buffer->In = 0;
	}
	return (i + count);										/* Return number of elements stored in memory */
#else
	while (count--) {										/* Go through all elements */
		Buffer->Buffer[Buffer->In++] = *d++;				/* Add to buffer */
		i++;												/* Increase number of written elements */
		if (Buffer->In >= Buffer->Size) {					/* Check input overflow */
			Buffer->In = 0;
		}
	}
	return i;												/* Return number of elements written */
#endif
}

/**
 * \brief  Reads data from buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *Data: Pointer to data where read values will be stored
 * \param  count: Number of elements of type unsigned char to read
 * \return Number of elements read from buffer 
 */
uint32_t
gui_buffer_read(GUI_BUFFER_t* Buffer, void* Data, uint32_t count) {
	uint32_t i = 0, full;
    uint8_t *d = (uint8_t *)Data;
#if GUI_BUFFER_FAST
	uint32_t tocopy;
#endif
	
	if (Buffer == NULL || count == 0) {						/* Check buffer structure */
		return 0;
	}
	if (Buffer->Out >= Buffer->Size) {						/* Check output pointer */
		Buffer->Out = 0;
	}
	full = gui_buffer_getfull(Buffer);							/* Get free memory */
	if (full < count) {										/* Check available memory */
		if (full == 0) {									/* If no memory, stop execution */
			return 0;
		}
		count = full;										/* Set values for write */
	}
#if GUI_BUFFER_FAST
	tocopy = Buffer->Size - Buffer->Out;					/* Calculate number of elements we can read from end of buffer */
	if (tocopy > count) {									/* Check for copy count */
		tocopy = count;
	}
	memcpy(d, &Buffer->Buffer[Buffer->Out], tocopy);		/* Copy content from buffer */
	i += tocopy;											/* Increase number of bytes we copied already */
	Buffer->Out += tocopy;
	count -= tocopy;
	if (count > 0) {										/* Check if anything to read */
		memcpy(&d[i], Buffer->Buffer, count);			    /* Copy content */
		Buffer->Out = count;								/* Set input pointer */
	}
	if (Buffer->Out >= Buffer->Size) {						/* Check output overflow */
		Buffer->Out = 0;
	}
	return (i + count);										/* Return number of elements stored in memory */
#else
	while (count--) {										/* Go through all elements */
		*d++ = Buffer->Buffer[Buffer->Out++];			    /* Read from buffer */
		i++;												/* Increase pointers */
		if (Buffer->Out >= Buffer->Size) {					/* Check output overflow */
			Buffer->Out = 0;
		}
	}
	return i;												/* Return number of elements stored in memory */
#endif
}

/**
 * \brief  Gets number of free elements in buffer 
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \return Number of free elements in buffer
 */
uint32_t
gui_buffer_getfree(GUI_BUFFER_t* Buffer) {
	uint32_t size = 0, in, out;
	
	if (Buffer == NULL) {									/* Check buffer structure */
		return 0;
	}
	in = Buffer->In;										/* Save values */
	out = Buffer->Out;
	if (in == out) {										/* Check if the same */
		size = Buffer->Size;
	} else if (out > in) {									/* Check normal mode */
		size = out - in;
	} else {												/* Check if overflow mode */
		size = Buffer->Size - (in - out);
	}
	return size - 1;										/* Return free memory */
}

/**
 * \brief  Gets number of elements in buffer 
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \return Number of elements in buffer
 */
uint32_t
gui_buffer_getfull(GUI_BUFFER_t* Buffer) {
	uint32_t in, out, size;
	
	if (Buffer == NULL) {									/* Check buffer structure */
		return 0;
	}
	in = Buffer->In;										/* Save values */
	out = Buffer->Out;
	if (in == out) {										/* Pointer are same? */
		size = 0;
	} else if (in > out) {									/* Buffer is not in overflow mode */
		size = in - out;
	} else {												/* Buffer is in overflow mode */
		size = Buffer->Size - (out - in);
	}
	return size;											/* Return number of elements in buffer */
}

/**
 * \brief  Resets (clears) buffer pointers
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \return None
 */
void
gui_buffer_reset(GUI_BUFFER_t* Buffer) {
	if (Buffer == NULL) {									/* Check buffer structure */
		return;
	}
	Buffer->In = 0;											/* Reset values */
	Buffer->Out = 0;
}

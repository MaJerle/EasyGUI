/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI generic cyclic buffer library
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
#ifndef GUI_BUFFER_H
#define GUI_BUFFER_H 110

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
 *
 * This buffer does not only feature basic read/write operations for cyclic buffers, it also has feature to read strings from buffer if needed.
 *
 * \par Read string procedure
 *
 * Each string in buffer has separator character, in most cases, Line Feed (0x0A) is used, and is also default value when buffer is initialized.
 * When reading as string from buffer, you have to know these things:
 *
\verbatim
- Buffer supports fast memory read/write operation using memory copy feature
- Buffer will first check if string delimiter character exists in buffer. 
    - If it exists, characters will be set to user buffer until delimiter is detected. 
	- Delimiter is included in string!
- If string delimiter is not in buffer, but cyclic buffer full, 
    then string will be also filled into user buffer, because we need to free
	some memory for future characters, including string delimiter character
- If user buffer size is less than number of characters in buffer before string delimiter is found, 
    string is also filled in user buffer
- In all other cases, if there is no string delimiter in buffer, buffer will not return anything and will check for it first.
\endverbatim
 *
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
	uint8_t StringDelimiter;        /*!< Character for string delimiter when reading from buffer as string, DO NOT MOVE OFFSET, 5 */
	void* UserParameters;           /*!< Pointer to user value if needed */
} GUI_BUFFER_t;

/**
 * \brief  Initializes buffer structure for work
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure to initialize
 * \param  Size: Size of buffer in units of bytes
 * \param  *BufferPtr: Pointer to array for buffer storage. Its length should be equal to \param Size parameter.
 *           If NULL is passed as parameter, internal memory management will be used to allocate memory
 * \retval Buffer initialization status:
 *            - 0: Buffer initialized OK
 *            - > 0: Buffer initialization error. Malloc has failed with allocation
 */
uint8_t gui_buffer_init(GUI_BUFFER_t* Buffer, uint32_t Size, void* BufferPtr);

/**
 * \brief  Free memory for buffer allocated using dynamic memory allocation
 * \note   This function has sense only if dynamic allocation was used for memory buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \retval None
 */
void gui_buffer_free(GUI_BUFFER_t* Buffer);

/**
 * \brief  Writes data to buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *Data: Pointer to data to be written
 * \param  count: Number of elements of type unsigned char to write
 * \retval Number of elements written in buffer 
 */
uint32_t gui_buffer_write(GUI_BUFFER_t* Buffer, const void* Data, uint32_t count);

/**
 * \brief  Writes data to buffer to top of buffer in reversed order
 * \note   This function is not thread safe so make sure you don't have read operations when you try to use this function.
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *Data: Pointer to data to be written
 * \param  count: Number of elements of type unsigned char to write
 * \retval Number of elements written in buffer on top in reverse order
 */
uint32_t gui_buffer_writetotop(GUI_BUFFER_t* Buffer, const void* Data, uint32_t count);

/**
 * \brief  Reads data from buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *Data: Pointer to data where read values will be stored
 * \param  count: Number of elements of type unsigned char to read
 * \retval Number of elements read from buffer 
 */
uint32_t gui_buffer_read(GUI_BUFFER_t* Buffer, void* Data, uint32_t count);

/**
 * \brief  Gets number of free elements in buffer 
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \retval Number of free elements in buffer
 */
uint32_t gui_buffer_getfree(GUI_BUFFER_t* Buffer);

/**
 * \brief  Gets number of elements in buffer 
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \retval Number of elements in buffer
 */
uint32_t gui_buffer_getfull(GUI_BUFFER_t* Buffer);

/**
 * \brief  Gets number of elements in buffer in fast way
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \retval Number of elements in buffer
 */
uint32_t gui_buffer_getfullfast(GUI_BUFFER_t* Buffer);

/**
 * \brief  Resets (clears) buffer pointers
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \retval None
 */
void gui_buffer_reset(GUI_BUFFER_t* Buffer);

/**
 * \brief  Checks if specific element value is stored in buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  Element: Element to check
 * \retval Status of element:
 *            -  < 0: Element was not found
 *            - >= 0: Element found, location in buffer is returned
 *                   Ex: If value 1 is returned, it means 1 read from buffer and your element will be returned
 */
int32_t gui_buffer_findelement(GUI_BUFFER_t* Buffer, uint8_t Element);

/**
 * \brief  Checks if specific data sequence are stored in buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *Data: Array with data sequence
 * \param  Size: Data size in units of bytes
 * \retval Status of sequence:
 *            -  < 0: Sequence was not found
 *            - >= 0: Sequence found, start sequence location in buffer is returned
 */
int32_t gui_buffer_find(GUI_BUFFER_t* Buffer, const void* Data, uint32_t Size);

/**
 * \brief  Sets string delimiter character when reading from buffer as string
 * \param  Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  StrDel: Character as string delimiter
 * \retval None
 */
#define gui_buffer_setstringdelimiter(Buffer, StrDel)  ((Buffer)->StringDelimiter = (StrDel))

/**
 * \brief  Writes string formatted data to buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *buff: Pointer to string to write 
 * \retval Number of characters written
 */
uint32_t gui_buffer_writestring(GUI_BUFFER_t* Buffer, const char* buff);

/**
 * \brief  Reads from buffer as string
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  *buff: Pointer to buffer where string will be stored
 * \param  buffsize: Buffer size in units of bytes
 * \retval Number of characters in string
 */
uint32_t gui_buffer_readstring(GUI_BUFFER_t* Buffer, char* buff, uint32_t buffsize);

/**
 * \brief  Checks if character exists in location in buffer
 * \param  *Buffer: Pointer to \ref GUI_BUFFER_t structure
 * \param  pos: Position in buffer, starting from 0
 * \param  *element: Pointer to save value at desired position to be stored into
 * \retval Check status:
 *            - 0: Buffer is not so long as position desired
 *            - > 0: Position to check was inside buffer data size
 */
int8_t gui_buffer_checkelement(GUI_BUFFER_t* Buffer, uint32_t pos, uint8_t* element);

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

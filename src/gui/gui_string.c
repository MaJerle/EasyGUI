/**	
 * \file            gui_string.c
 * \brief           String manager
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
#include "gui/gui_string.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

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

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Initialize unicode processing structure
 * \param[in]       *s: Pointer to \ref GUI_STRING_UNICODE_t to initialize to default values
 * \retval          None
 * \sa              gui_string_unicode_decode, gui_string_unicode_encode
 */
void
gui_string_unicode_init(GUI_STRING_UNICODE_t* s) {
    memset(s, 0x00, sizeof(*s));            /* Reset structure */
}

/**
 * \brief           Decodes single input byte of unicode formatted text
 * \param[in,out]   *s: Pointer to working \ref GUI_STRING_UNICODE_t structure for processing
 * \param[in]       c: Character to be used for decoding
 * \retval          Member of \ref GUI_STRING_UNICODE_Result_t indicating decoding status
 * \sa              gui_string_unicode_init, gui_string_unicode_encode
 */
GUI_STRING_UNICODE_Result_t
gui_string_unicode_decode(GUI_STRING_UNICODE_t* s, const GUI_Char c) {    
    if (!s->r) {                            /* First byte received */
        s->t = 0;
        if (c < 0x80) {                     /* One byte only in UTF-8 representation */
            s->res = c;                     /* Just return result */
            s->r = 0;                       /* Remaining bytes */
            s->t = 1;
            return UNICODE_OK;              /* Return OK */
        }
        if ((c & 0xE0) == 0xC0) {           /* 1 additional byte in a row = 110x xxxx */   
            s->res = c & 0x1F;              /* 5 lower bits from first byte as valid data */      
            s->r = 1;
        } else if ((c & 0xF0) == 0xE0) {    /* 2 additional bytes in a row = 1110 xxxx */
            s->res = c & 0x0F;              /* 4 lower bits from first byte as valid data*/
            s->r = 2;
        } else if ((c & 0xF8) == 0xF0) {    /* 3 additional bytes in a row = 1111 0xxx */
            s->res = c & 0x07;              /* 3 lower bits from first byte as valid data*/   
            s->r = 3;    
        } else {
            return UNICODE_ERROR;           /* Error parsing unicode byte */
        }
        s->t = s->r + 1;                    /* Number of bytes is 1 byte more than remaining in sequence */
        return UNICODE_PROGRESS;            /* We are in progress */
    } else {                                /* Remaining bytes */
        if ((c & 0xC0) == 0x80) {           /* Valid UTF-8 sequence */
            s->r--;                         /* Decrease remaining bytes of data */
            s->res = (s->res << 6) | (c & 0x3F);    /* Set new value for data */
            if (!s->r) {                    /* All bytes parsed? */
                return UNICODE_OK;          /* Return OK */
            } else {
                return UNICODE_PROGRESS;    /* Parsing is still in progress */
            }
        } else {                            /* Invalid byte sequence */
            s->res = 0;                     /* Reset remaining bytes */
            s->r = 0;                       /* Invalid character */
            //TODO: Consider using decode function again like: 
            //return gui_string_unicode_decode(s, c);
            return UNICODE_ERROR;           /* Return error */
        }
    }
}

/**
 * \brief           Encodes input character to UNICODE sequence of 1-4 bytes
 * \param[in]       c: Character to encode to UNICODE sequence
 * \param[out]      *out: Pointer to 4-bytes long array to store UNICODE information to
 * \retval          Number of bytes required for character encoding
 * \sa              gui_string_unicode_init, gui_string_unicode_decode
 */
uint8_t
gui_string_unicode_encode(const uint32_t c, GUI_Char* out) {
    if (c < 0x0080) {                       /* Normal ASCII character */
        *out++ = (uint8_t)c;                /* 1-byte sequence */
        return 1;
    } else { 
        if (c < 0x0800) {                   /* 2-bytes sequence */
            *out++ = (uint8_t)(0xC0 | ((c >> 6) & 0x3F));
            *out++ = (uint8_t)(0x80 | ((c >> 0) & 0x3F));
            return 2;
        } else {                              /* 3-bytes sequence */
            *out++ = (uint8_t)(0xE0 | ((c >> 12) & 0x3F));
            *out++ = (uint8_t)(0x80 | ((c >> 6) & 0x3F));
            *out++ = (uint8_t)(0x80 | ((c >> 0) & 0x3F));
            return 3;
        }
    }
}

/**
 * \brief           Return length of string
 *      
 *                  Example input string: EasyGUI\\xDF\\x8F
 *
 *                  1. When \ref GUI_CFG_USE_UNICODE is set to 1, function will try to parse unicode characters
 *                      and will return 8 on top input string
 *                  2. When \ref GUI_CFG_USE_UNICODE is set to 0, function will count all the bytes until string end is reached
 *                      and will return 9 on top input string
 *
 * \param[in]       *src: Pointer to source string to get length
 * \retval          Number of visible characters in string
 * \sa              gui_string_lengthtotal
 */
size_t
gui_string_length(const GUI_Char* src) {
#if GUI_CFG_USE_UNICODE
    size_t out = 0;
    const GUI_Char* tmp = src;
    GUI_STRING_UNICODE_t s;
    
    gui_string_unicode_init(&s);            /* Init unicode */
    while (*tmp) {                          /* Process string */
        if (gui_string_unicode_decode(&s, *tmp++) == UNICODE_OK) {  /* Process character */
            out++;                          /* Increase number of characters */
        }
    }
    return out;
#else
    return strlen((const char *)src);
#endif /* GUI_CFG_USE_UNICODE */
}

/**
 * \brief           Return total number of bytes required for string
 *      
 * \note            When \ref GUI_CFG_USE_UNICODE is set to 0, this function returns the same as \ref gui_string_length
 *
 * \param[in]       *src: Pointer to source string to get length
 * \retval          Number of visible characters in string
 * \sa              gui_string_length
 */
size_t
gui_string_lengthtotal(const GUI_Char* src) {
    return strlen((const char *)src);       /* Get string length */
}

/**
 * \brief           Copy string from source to destination no matter of \ref GUI_CFG_USE_UNICODE selection
 * \param[out]      *dst: Destination memory address
 * \param[in]       *src: Source memory address
 * \retval          Pointer to destination memory
 * \sa              gui_string_copyn
 */
GUI_Char*
gui_string_copy(GUI_Char* dst, const GUI_Char* src) {
    return (GUI_Char *)strcpy((char *)dst, (const char *)src);  /* Copy source string to destination */
}

/**
 * \brief           Copy string from source to destination with selectable number of bytes
 * \param[out]      *dst: Destination memory address
 * \param[in]       *src: Source memory address
 * \param[in]       len: Number of bytes to copy
 * \retval          Pointer to destination memory
 * \sa              gui_string_copy
 */
GUI_Char*
gui_string_copyn(GUI_Char* dst, const GUI_Char* src, size_t len) {
    return (GUI_Char *)strncpy((char *)dst, (const char *)src, len);    /* Copy source string to destination */
}

/**
 * \brief           Compare 2 strings
 * \param[in]       *s1: First string address
 * \param[in]       *s2: Second string address
 * \retval          0: Strings are the same
 * \retval          !=0: Strings are not the same
 */
int
gui_string_compare(const GUI_Char* s1, const GUI_Char* s2) {
    return strcmp((const char *)s1, (const char *)s2);
}

/**
 * \brief           Prepare string before it can be used with \ref gui_string_getch or \ref gui_string_getchreverse functions
 * \param[in,out]   *s: Pointer to \ref GUI_STRING_t as base string object
 * \param[in]       *str: Pointer to \ref GUI_Char with string used for manupulation
 * \retval          1: String prepared and ready to use
 * \retval          0: String was tno prepared
 */
uint8_t
gui_string_prepare(GUI_STRING_t* s, const GUI_Char* str) {
    s->Str = str;                           /* Save string pointer */
#if GUI_CFG_USE_UNICODE
    gui_string_unicode_init(&s->S);         /* Prepare unicode structure */
#endif /* GUI_CFG_USE_UNICODE */
    return 1;
}

/**
 * \brief           Get next decoded character from source string
 *
 * \code{c} 
GUI_Char myStr[] = "EasyGUI\xDF\x8F\xDF\x8F";   //Source string to check
GUI_STRING_t s;                                 //Create string variable
uint32_t ch;                                    //Output character
uint8_t i;                                      //Number of bytes required for character generation

//GUI_CFG_USE_UNICODE = 1: string length = 9;  Length total: 11
//GUI_CFG_USE_UNICODE = 0: string length = 11; Length total: 11
gui_string_prepare(&s, myStr);                  //Prepare string for reading
while (gui_string_getch(&s, &ch, &i)) {         //Go through entire string
    printf("I: %d, ch %c (%d)\r\n", (int)i, ch, (int)ch);   //Print character by character
}
\endcode
 * 
 * \note            When \ref GUI_CFG_USE_UNICODE is set to 1, multiple bytes may be used for single character
 * \param[in,out]   *s: Pointer to \ref GUI_STRING_t structure with input string. 
                        Function will internally change pointer of actual string where it points to to next character
 * \param[out]      *out: Pointer to output memory where output character will be saved
 * \param[out]      *len: Pointer to output memory where number of bytes for string will be saved
 * \retval          1: Character decoded OK
 * \retval          0: Error with character decode process or string has reach the end
 * \sa              gui_string_getchreverse
 */
uint8_t
gui_string_getch(GUI_STRING_t* s, uint32_t* out, uint8_t* len) {
#if GUI_CFG_USE_UNICODE
    GUI_STRING_UNICODE_Result_t r;
    
    if (!*s->Str) {                         /* End of string check */
        return 0;
    }
    
    while (*s->Str) {                       /* Check all characters */
        r = gui_string_unicode_decode(&s->S, *s->Str++);    /* Try to decode string */
        if (r == UNICODE_OK) {              /* Decode next character */
            break;
        }
    }
    *out = s->S.res;                        /* Save character for output */
    if (len) {                              /* Save number of bytes in this character */
        *len = s->S.t;                      /* Number of bytes according to UTF-8 encoding */
    }
    return 1;                               /* Return valid character sign */
#else
    *out = *s->Str;                         /* Save character for output */
    s->Str++;                               /* Increase input pointer where it points to */
    if (!*out) {                            /* End of string check */
        return 0;                           /* Invalid character */
    }
    if (len) {                              /* Save number of bytes in this character */
        *len = 1;                           /* 1-byte only */
    }
    return 1;                               /* Return valid character sign */
#endif /* GUI_CFG_USE_UNICODE */  
}

/**
 * \brief           Get character by character from end of string up
 * \note            Functionality is the same as \ref gui_string_getch except order is swapped
 * 
 * \note            String must be at the last character before function is first time called
 *
 * \code{c}
 //TODO: Update code!
GUI_Char myStr[] = "EasyGUI\xDF\x8F\xDF\x8F";   //Source string to check
GUI_STRING_t s;                                 //Create string variable
uint32_t ch;                                    //Output character
uint8_t i;                                      //Number of bytes required for character generation

//GUI_CFG_USE_UNICODE = 1: string length = 9;  Length total: 11
//GUI_CFG_USE_UNICODE = 0: string length = 11; Length total: 11
gui_string_prepare(&s, myStr);                  //Prepare string for reading
gui_string_gotoend(&ptr);                       //Go to last character of string
while (gui_string_getchreverse(&s, &ch, &i)) {  //Go through entire string
    printf("I: %d, ch %c (%d)\r\n", (int)i, ch, (int)ch);   //Print character by character
}
\endcode
 *
 * \note            When \ref GUI_CFG_USE_UNICODE is set to 1, multiple bytes may be used for single character
 * \param[in,out]   *str: Pointer to \ref GUI_STRING_t structure with input string. 
                        Function will internally change pointer of actual string where it points to to next character
 * \param[out]      *out: Pointer to output memory where output character will be saved
 * \param[out]      *len: Pointer to output memory where number of bytes for string will be saved
 * \retval          1: Character decoded OK
 * \retval          0: Error with character decode process or string has reach the start
 * \sa              gui_string_getch, gui_string_gotoend
 */
uint8_t
gui_string_getchreverse(GUI_STRING_t* str, uint32_t* out, uint8_t* len) {
#if GUI_CFG_USE_UNICODE
    const GUI_Char* ch = (str->Str) - 3;    /* Save character pointer, start 3 bytes before current active character */
    if (ch[3] < 0x80) {                     /* Normal ASCII character */
        *out = (uint32_t)ch[3];
        str->Str -= 1;
        if (len) {
            *len = 1;
        }
    } else {                                /* UTF-8 sequence */
        if ((ch[2] & 0xE0) == 0xC0 && (ch[3] & 0xC0) == 0x80) {
            *out = (uint32_t)(((ch[2] & 0x1F) << 6) | ((ch[3] & 0x3F) << 0));
            str->Str -= 2;
            if (len) {
                *len = 2;
            }
        } else if ((ch[1] & 0xF0) == 0xE0 && (ch[2] & 0xC0) == 0x80 && (ch[3] & 0xC0) == 0x80) {
            *out = (uint32_t)(((ch[1] & 0x0F) << 12) | ((ch[2] & 0x3F) << 6) | ((ch[3] & 0x3F) << 0));
            str->Str -= 3;
            if (len) {
                *len = 3;
            }
        } else if ((ch[0] & 0xF8) == 0xF0 && (ch[1] & 0xC0) == 0x80 && (ch[2] & 0xC0) == 0x80 && (ch[3] & 0xC0) == 0x80) {
            *out = (uint32_t)(((ch[0] & 0x07) << 18) | ((ch[1] & 0x3F) << 12) | ((ch[2] & 0x3F) << 6) | ((ch[3] & 0x3F) << 0));
            str->Str -= 4;
            if (len) {
                *len = 4;
            }
        } else {
            *out = 0;
            return 0;
        }
    }
    return 1;                               /* Return valid character sign */
#else
    const GUI_Char ch = *str->Str;          /* Save character pointer */
    str->Str--;                             /* Decrease input pointer where it points to */
    *out = (uint32_t)ch;                    /* Save character for output */
    if (!*out) {                            /* Check if valid character */
        return 0;
    }
    if (len) {                              /* Save number of bytes in this character */
        *len = 1;                           /* 1-byte only */
    }
    return 1;                               /* Return valid character sign */
#endif /* GUI_CFG_USE_UNICODE */  
}

/**
 *
 * \brief           Set character pointer to the last character in sequence
 * \param[in,out]   *str: Pointer to \ref GUI_STRING_t structure with string informations
 * \retval          1: Pointer set to last character
 * \retval          0: Pointer was not set to last character
 * \sa              gui_string_getchreverse
 */
uint8_t
gui_string_gotoend(GUI_STRING_t* str) {
    while (*str->Str) {                     /* Check characters */
        str->Str++;                         /* Go to next character */
    }
    str->Str--;                             /* Let's point to last character */
    return 1;
}

/**
 * \brief           Check if character is printable
 * \param[in]       ch: First memory address
 * \retval          1: Character is printable
 * \retval          0: Character is not printable
 */
uint8_t
gui_string_isprintable(uint32_t ch) {
    return (ch >= 32 && ch != 127) || (ch == '\r') || (ch == '\n');
}

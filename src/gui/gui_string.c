/**	
 * \file            gui_string.c
 * \brief           String manager
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
#include "gui/gui_string.h"

/**
 * \brief           Initialize unicode processing structure
 * \param[in]       s: Pointer to \ref gui_string_unicode_t to initialize to default values
 * \sa              gui_string_unicode_decode, gui_string_unicode_encode
 */
void
gui_string_unicode_init(gui_string_unicode_t* const s) {
    memset(s, 0x00, sizeof(*s));            /* Reset structure */
}

/**
 * \brief           Decodes single input byte of unicode formatted text
 * \param[in,out]   *s: Pointer to working \ref gui_string_unicode_t structure for processing
 * \param[in]       c: Character to be used for decoding
 * \return          Member of \ref GUI_STRING_UNICODE_Result_t indicating decoding status
 * \sa              gui_string_unicode_init, gui_string_unicode_encode
 */
GUI_STRING_UNICODE_Result_t
gui_string_unicode_decode(gui_string_unicode_t* const s, const gui_char c) {    
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
 * \param[out]      out: Pointer to 4-bytes long array to store UNICODE information to
 * \return          Number of bytes required for character encoding
 */
uint8_t
gui_string_unicode_encode(const uint32_t c, gui_char* out) {
    if (c < 0x0080) {                       /* Normal ASCII character */
        *out++ = (uint8_t)c;                /* 1-byte sequence */
        return 1;
    } else { 
        if (c < 0x0800) {                   /* 2-bytes sequence */
            *out++ = (uint8_t)(0xC0 | ((c >> 6) & 0x3F));
            *out++ = (uint8_t)(0x80 | ((c >> 0) & 0x3F));
            return 2;
        } else {                            /* 3-bytes sequence */
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
 *                  1. When \ref GUI_CFG_USE_UNICODE is set to `1`, function will try to parse unicode characters
 *                      and will return `8` on top input string
 *                  2. When \ref GUI_CFG_USE_UNICODE is set to `0`, function will count all the bytes until string termination is reached
 *                      and will return `9` on top input string
 *
 * \param[in]       src: Pointer to source string to get length
 * \return          Number of visible characters in string
 */
size_t
gui_string_length(const gui_char* const src) {
#if GUI_CFG_USE_UNICODE
    size_t out = 0;
    const gui_char* tmp = src;
    gui_string_unicode_t s;
    
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
 * \note            When \ref GUI_CFG_USE_UNICODE is set to `0`, this function returns the same as \ref gui_string_length
 *
 * \param[in]       src: Pointer to source string to get length
 * \return          Number of visible characters in string
 */
size_t
gui_string_lengthtotal(const gui_char* const src) {
    return strlen((const char *)src);       /* Get string length */
}

/**
 * \brief           Copy string from source to destination no matter of \ref GUI_CFG_USE_UNICODE selection
 * \param[out]      dst: Destination memory address
 * \param[in]       src: Source memory address
 * \return          Pointer to destination memory
 */
gui_char *
gui_string_copy(gui_char* const dst, const gui_char* const src) {
    return (gui_char *)strcpy((char *)dst, (const char *)src);  /* Copy source string to destination */
}

/**
 * \brief           Copy string from source to destination with selectable number of bytes
 * \param[out]      dst: Destination memory address
 * \param[in]       src: Source memory address
 * \param[in]       len: Number of bytes to copy
 * \return          Pointer to destination memory
 */
gui_char *
gui_string_copyn(gui_char* const dst, const gui_char* const src, size_t len) {
    return (gui_char *)strncpy((char *)dst, (const char *)src, len);    /* Copy source string to destination */
}

/**
 * \brief           Compare `2` strings
 * \param[in]       s1: First string address
 * \param[in]       s2: Second string address
 * \return          `0` if equal, non-zero otherwise
 */
int
gui_string_compare(const gui_char* const s1, const gui_char* const s2) {
    return strcmp((const char *)s1, (const char *)s2);
}

/**
 * \brief           Prepare string before it can be used with \ref gui_string_getch or \ref gui_string_getchreverse functions
 * \param[in,out]   s: Pointer to \ref gui_string_t as base string object
 * \param[in]       str: Pointer to \ref gui_char with string used for manipulation
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_string_prepare(gui_string_t* const s, const gui_char* const str) {
    s->str = str;                           /* Save string pointer */
#if GUI_CFG_USE_UNICODE
    gui_string_unicode_init(&s->s);         /* Prepare unicode structure */
#endif /* GUI_CFG_USE_UNICODE */
    return 1;
}

/**
 * \brief           Get next decoded character from source string
 *
 * \include         _example_string_getch.c
 * 
 * \note            When \ref GUI_CFG_USE_UNICODE is set to `1`, multiple bytes may be used for single character
 * \param[in,out]   s: Pointer to \ref gui_string_t structure with input string. 
                        Function will internally change pointer of actual string where it points to to next character
 * \param[out]      out: Pointer to output memory where output character will be saved
 * \param[out]      len: Pointer to output memory where number of bytes for string will be saved
 * \return          `1` on success, `0` otherwis
 * \sa              gui_string_getchreverse
 */
uint8_t
gui_string_getch(gui_string_t* const s, uint32_t* const out, uint8_t* const len) {
#if GUI_CFG_USE_UNICODE
    GUI_STRING_UNICODE_Result_t r;
    
    if (s == NULL || s->str == NULL || !*s->str) {  /* End of string check */
        return 0;
    }
    
    while (*s->str) {                       /* Check all characters */
        r = gui_string_unicode_decode(&s->s, *s->str++);    /* Try to decode string */
        if (r == UNICODE_OK) {              /* Decode next character */
            break;
        }
    }
    *out = s->s.res;                        /* Save character for output */
    if (len) {                              /* Save number of bytes in this character */
        *len = s->s.t;                      /* Number of bytes according to UTF-8 encoding */
    }
    return 1;                               /* Return valid character sign */
#else
    *out = *s->str;                         /* Save character for output */
    s->str++;                               /* Increase input pointer where it points to */
    if (!*out) {                            /* End of string check */
        return 0;                           /* Invalid character */
    }
    if (len != NULL) {                      /* Save number of bytes in this character */
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
 * \include         _example_string_getchreverse.c
 *
 * \note            When \ref GUI_CFG_USE_UNICODE is set to `1`, multiple bytes may be used for single character
 * \param[in,out]   *str: Pointer to \ref gui_string_t structure with input string. 
                        Function will internally change pointer of actual string where it points to to next character
 * \param[out]      out: Pointer to output memory where output character will be saved
 * \param[out]      len: Pointer to output memory where number of bytes for string will be saved
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_string_getchreverse(gui_string_t* const str, uint32_t* const out, uint8_t* const len) {
#if GUI_CFG_USE_UNICODE
    const gui_char* ch = (str->str) - 3;    /* Save character pointer, start 3 bytes before current active character */
    if (ch[3] < 0x80) {                     /* Normal ASCII character */
        *out = (uint32_t)ch[3];
        str->str -= 1;
        if (len != NULL) {
            *len = 1;
        }
    } else {                                /* UTF-8 sequence */
        if ((ch[2] & 0xE0) == 0xC0 && (ch[3] & 0xC0) == 0x80) {
            *out = (uint32_t)(((ch[2] & 0x1F) << 6) | ((ch[3] & 0x3F) << 0));
            str->str -= 2;
            if (len != NULL) {
                *len = 2;
            }
        } else if ((ch[1] & 0xF0) == 0xE0 && (ch[2] & 0xC0) == 0x80 && (ch[3] & 0xC0) == 0x80) {
            *out = (uint32_t)(((ch[1] & 0x0F) << 12) | ((ch[2] & 0x3F) << 6) | ((ch[3] & 0x3F) << 0));
            str->str -= 3;
            if (len != NULL) {
                *len = 3;
            }
        } else if ((ch[0] & 0xF8) == 0xF0 && (ch[1] & 0xC0) == 0x80 && (ch[2] & 0xC0) == 0x80 && (ch[3] & 0xC0) == 0x80) {
            *out = (uint32_t)(((ch[0] & 0x07) << 18) | ((ch[1] & 0x3F) << 12) | ((ch[2] & 0x3F) << 6) | ((ch[3] & 0x3F) << 0));
            str->str -= 4;
            if (len != NULL) {
                *len = 4;
            }
        } else {
            *out = 0;
            return 0;
        }
    }
    return 1;                               /* Return valid character sign */
#else
    const gui_char ch = *str->str;          /* Save character pointer */
    str->str--;                             /* Decrease input pointer where it points to */
    *out = (uint32_t)ch;                    /* Save character for output */
    if (!*out) {                            /* Check if valid character */
        return 0;
    }
    if (len != NULL) {                      /* Save number of bytes in this character */
        *len = 1;                           /* 1-byte only */
    }
    return 1;                               /* Return valid character sign */
#endif /* GUI_CFG_USE_UNICODE */  
}

/**
 *
 * \brief           Set character pointer to the last character in sequence
 * \param[in,out]   str: Pointer to \ref gui_string_t structure with string informations
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_string_gotoend(gui_string_t* const str) {
    while (*str->str) {                     /* Check characters */
        str->str++;                         /* Go to next character */
    }
    str->str--;                             /* Let's point to last character */
    return 1;
}

/**
 * \brief           Check if character is printable
 * \param[in]       ch: First memory address
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_string_isprintable(uint32_t ch) {
    return (ch >= 32 && ch != 127) || (ch == '\r') || (ch == '\n');
}

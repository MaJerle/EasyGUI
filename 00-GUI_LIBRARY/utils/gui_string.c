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
#include "gui_string.h"

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
void
gui_string_unicode_init(GUI_STRING_UNICODE_t* s) {
    memset(s, 0x00, sizeof(*s));            /* Reset structure */
}

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

size_t
gui_string_length(const GUI_Char* src) {
#if GUI_USE_UNICODE
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
#endif /* GUI_USE_UNICODE */
}

size_t
gui_string_lengthtotal(const GUI_Char* src) {
    return strlen((const char *)src);       /* Get string length */
}

GUI_Char*
gui_string_copy(GUI_Char* dst, const GUI_Char* src) {
    return (GUI_Char *)strcpy((char *)dst, (const char *)src);  /* Copy source string to destination */
}

GUI_Char*
gui_string_copyn(GUI_Char* dst, const GUI_Char* src, size_t len) {
    return (GUI_Char *)strncpy((char *)dst, (const char *)src, len);    /* Copy source string to destination */
}

int
gui_string_compare(const GUI_Char* s1, const GUI_Char* s2) {
    return strcmp((const char *)s1, (const char *)s2);
}

uint8_t
gui_string_prepare(GUI_STRING_t* s, const GUI_Char* str) {
    s->Str = str;                           /* Save string pointer */
#if GUI_USE_UNICODE
    gui_string_unicode_init(&s->S);         /* Prepare unicode structure */
#endif /* GUI_USE_UNICODE */
    return 1;
}

uint8_t
gui_string_getch(GUI_STRING_t* s, uint32_t* out, uint8_t* len) {
#if GUI_USE_UNICODE
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
#endif /* GUI_USE_UNICODE */  
}

uint8_t
gui_string_getchreverse(GUI_STRING_t* str, uint32_t* out, uint8_t* len) {
#if GUI_USE_UNICODE
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
#endif /* GUI_USE_UNICODE */  
}

uint8_t
gui_string_gotoend(GUI_STRING_t* str) {
    while (*str->Str) {                     /* Check characters */
        str->Str++;                         /* Go to next character */
    }
    str->Str--;                             /* Let's point to last character */
    return 1;
}

uint8_t
gui_string_isprintable(uint32_t ch) {
    return (ch >= 32 && ch != 127) || (ch == '\r') || (ch == '\n');
}

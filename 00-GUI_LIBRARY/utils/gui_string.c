/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
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
#include "gui_string.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct _utf8_t {
    uint8_t r, t;                           /*!< Remaining and total bytes for read procedure */
    uint32_t res;                           /*!< Current result in UTF8 sequence */
} _utf8_t;

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
#if GUI_USE_UNICODE
void utf8_init(_utf8_t* s) {
    memset(s, 0x00, sizeof(*s));            /* Reset structure */
}

uint8_t utf8_decode(_utf8_t* s, const uint8_t c) {    
    if (!s->r) {                            /* First byte received */
        s->t = 0;
        if (c < 0x80) {                     /* One byte only in UTF-8 representation */
            s->res = c;                     /* Just return result */
            s->r = 0;                       /* Remaining bytes */
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
        }
        s->t = s->r + 1;                    /* Number of bytes is 1 byte more than remaining in sequence */
    } else {                                /* Remaining bytes */
        if ((c & 0xC0) == 0x80) {           /* Valid UTF-8 sequence */
            s->r--;                         /* Decrease remaining bytes of data */
            s->res = (s->res << 6) | (c & 0x3F);    /* Set new value for data */
        } else {
            s->res = 0;                     /* Reset remaining bytes */
            s->r = 0;                       /* Invalid character */
            return 0;
        }
    }
    
    if (!s->r) {                            /* We reached the end of UTF-8 sequence */
        return 1;
    }
    return 0;
}

uint8_t utf8_encode(const uint16_t c, uint8_t* out) {
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
#endif /* GUI_USE_UNICODE */

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
#if GUI_USE_UNICODE
size_t __GUI_STRING_Length(const GUI_Char* src) {
    size_t out = 0;
    const GUI_Char* tmp = src;
    _utf8_t s;
    
    utf8_init(&s);                          /* Init UTF-8 */
    while (*tmp) {                          /* Process string */
        if (utf8_decode(&s, *tmp)) {        /* character processed */
            out++;                          /* Increase number of characters */
        }
        tmp++;                              /* Increase source data */
    }
    __GUI_DEBUG("len: %3d; %3d\r\n", tmp - src, out);
    return out;
}

GUI_Char* __GUI_STRING_Copy(GUI_Char* dst, const GUI_Char* src) {
    GUI_Char* ret = dst;
    while (*src) {
        *dst++ = *src++;
    }
    *dst = 0;
    return ret;
}

GUI_Char* __GUI_STRING_CopyN(GUI_Char* dst, const GUI_Char* src, size_t len) {
    GUI_Char* ret = dst;
    while (*src && len) {
        *dst++ = *src++;
        len--;
    }
    *dst = 0;
    return ret;
}

GUI_Char* __GUI_STRING_Compare(const GUI_Char* s1, const GUI_Char* s2) {
    while (*s1++ == *s2++ && *s1 && *s2);
    if (!*s1 && !*s2) {
        return 0;
    }
    return (GUI_Char *)s1;
}
#endif /* GUI_USE_UNICODE */

uint8_t __GUI_STRING_GetCh(const GUI_Char** str, uint32_t* out, uint8_t* len) {
    const GUI_Char* ch = *str;              /* Save character pointer */
#if GUI_USE_UNICODE
    _utf8_t s;
    
    utf8_init(&s);                          /* Init UTF-8 */
    while (*ch) {
        if (utf8_decode(&s, *ch++)) {       /* Decode next character */
            break;
        }
    }
    *str += s.t;                            /* Increase pointer by number of bytes in UTF-8 sequence */
    *out = s.res;                           /* Save character for output */
    if (len) {                              /* Save number of bytes in this character */
        *len = s.t;                         /* Number of bytes according to UTF-8 encoding */
    }
    return 1;                               /* Return valid character sign */
#else
    *str++;                                 /* Increase input pointer where it points to */
    *out = (uint32_t)*ch;                   /* Save character for output */
    if (len) {                              /* Save number of bytes in this character */
        *len = 1;                           /* 1-byte only */
    }
    return 1;                               /* Return valid character sign */
#endif /* GUI_USE_UNICODE */  
}

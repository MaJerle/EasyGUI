/**	
 * \file            gui_text.c
 * \brief           Text manager
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
#include "gui/gui_private.h"

#define CH_CR                       GUI_KEY_CR
#define CH_LF                       GUI_KEY_LF
#define CH_WS                       GUI_KEY_WS
#define get_char_from_value(ch)     (uint32_t)((CH_CR == (ch) || CH_LF == (ch)) ? CH_WS : (ch))

/**
 * \brief           Get character descriptor from specific character and font
 * \param[in]       font: Font to use for drawing
 * \param[in]       ch: Unicode decoded character
 * \return          Char info of specific font and character code
 */
const gui_font_char_t *
gui_text_getchardesc(const gui_font_t* font, uint32_t ch) {
    ch = get_char_from_value(ch);
    /* Try to get character from font */
    if (ch >= font->startchar && ch <= font->endchar) {
        return &font->data[(ch)-font->startchar];

    /* If it doesn't exist, try with question mark */
    } else if ('?' >= font->startchar && '?' <= font->endchar) {
        return &font->data[(uint32_t)'?' - font->startchar];
    }
    return NULL;
}

/**
 * \brief           Get width and height of specific character for specific font
 * \param[in]       font: Font used for character
 * \param[in]       ch: Unicode decoded character
 * \param[out]      width: Output width information in units of pixels
 * \param[out]      height: Output height information in units of pixels
 */
void
gui_text_getcharsize(const gui_font_t* font, uint32_t ch, gui_dim_t* width, gui_dim_t* height) {
    const gui_font_char_t* c = 0;
    
    c = gui_text_getchardesc(font, ch);
    if (c != NULL) {
        *width = c->x_size + c->x_margin;
        *height = c->y_size;
    } else {
        *width = 0;
        *height = 0;
    }
}

/**
 * \brief           Get character entry generated in memory for fast drawing
 * \param[in]       font: Font used for character
 * \param[in]       c: Character info handle
 * \return          Character entry on success, `NULL` otherwise
 */
gui_font_charentry_t *
gui_text_getcharentry(const gui_font_t* font, const gui_font_char_t* c) {
    gui_font_charentry_t* entry;

    /* Process all characters on linked list */
    for (entry = (gui_font_charentry_t *)gui_linkedlist_getnext_gen(&GUI.root_fonts, NULL); entry != NULL;
        entry = (gui_font_charentry_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)entry)) {
        if (entry->font == font && entry->ch == c) {
            return entry;
        }
    }
    return 0;
}

/**
 * \brief           Create new entry for character map and put it to linked list of known entries
 * \param[in]       font: Font for character
 * \param[in]       c: Character descriptor
 * \return          Character entry on success, `NULL` otherwise
 */
gui_font_charentry_t *
gui_text_createcharentry(const gui_font_t* font, const gui_font_char_t* c) {
    gui_font_charentry_t* entry = NULL;
    size_t columns, memDataSize, memsize;

    /* Calculate memory size for data */
    memsize = GUI_MEM_ALIGN(sizeof(*entry));
    memDataSize = (size_t)c->x_size * (size_t)c->y_size;
    memsize += GUI_MEM_ALIGN(memDataSize);          /* Align memory before increase */
    entry = GUI_MEMALLOC(memsize);                  /* Allocate memory for entry */
    if (entry != NULL) {                            /* Allocation was successful */
        uint16_t i, x;
        uint8_t b, k, t;
        uint8_t* ptr = (uint8_t *)entry;            /* Go to memory size */

        ptr += GUI_MEM_ALIGN(sizeof(*entry));       /* Go to start of data, at the end of aligned structure size */

        entry->ch = c;                              /* Set pointer to character */
        entry->font = font;                         /* Set pointer to font structure */

        if (font->flags & GUI_FLAG_FONT_AA) {       /* Anti-alliased font */
            columns = c->x_size >> 2;               /* Calculate number of bytes used for single character line */
            if (c->x_size % 4) {                    /* If only 1 column used */
                columns++;
            }
            x = 0;
            for (i = 0; i < c->y_size * columns; i++) { /* Inspect all vertical lines */
                b = c->data[i];                     /* Get byte of data */
                for (k = 0; k < 4; k++) {           /* Scan each bit in byte */
                    t = (b >> (6 - 2 * k)) & 0x03;  /* Get temporary bits on bottom */
                    switch (t) {
                        case 0:
                            *ptr |= 0x00;
                            break;
                        case 1:
                            *ptr |= 0x55;
                            break;
                        case 2:
                            *ptr |= 0xAA;
                            break;
                        default:
                            *ptr |= 0xFF;
                    }
                    ptr++;
                    x++;
                    if (x == c->x_size) {
                        x = 0;
                        break;
                    }
                }
            }
        } else {
            columns = c->x_size >> 3;               /* Calculate number of bytes used for single character line */
            if (c->x_size % 8) {                    /* If only 1 column used */
                columns++;
            }
            x = 0;
            for (i = 0; i < c->y_size * columns; i++) { /* Inspect all vertical lines */
                b = c->data[i];                     /* Get byte of data */
                for (k = 0; k < 8; k++) {           /* Scan each bit in byte */
                    if ((b >> (7 - k)) & 0x01) {
                        *ptr++ = 0xFF;
                    } else {
                        *ptr++ = 0x00;
                    }
                    x++;
                    if (x == c->x_size) {
                        x = 0;
                        break;
                    }
                }
            }
        }
        gui_linkedlist_add_gen(&GUI.root_fonts, (gui_linkedlist_t *)entry);  /* Add entry to linked list */
    }
    return entry;
}

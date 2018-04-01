/**	
 * \file            gui_draw.c
 * \brief           Drawing operations
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
#include "gui/gui_private.h"
#include "gui/gui_draw.h"

typedef struct GUI_StringRect_t {
    size_t Lines;                                   /*!< Number of lines processed */
    gui_dim_t Width;                                /*!< Rectangle width */
    gui_dim_t Height;                               /*!< Rectangle height */

    size_t IsEditMode;                              /*!< Status whether text is in edit mode */
    size_t ReadTotal;                               /*!< Total number of characters to read */
    size_t ReadDraw;                                /*!< Total number of characters to actually draw after read */

    GUI_DRAW_FONT_t* StringDraw;                    /*!< Pointer to object to draw string */
    const gui_font_t* Font;                         /*!< Pointer to used font */
} GUI_StringRect_t;

typedef struct GUI_StringRectVars_t {
    GUI_STRING_t s;                                 /*!< Pointer to input string */
    uint32_t ch, lastCh;                            /*!< Current and previous characters */
    gui_dim_t cW;                                   /*!< Current line width */
    size_t cnt;                                     /*!< Current count in line */

    size_t SpaceIndex;                              /*!< Index number of last space sequence start */
    size_t SpaceCount;                              /*!< Number of spaces in last sequence */
    gui_dim_t SpaceWidth;                           /*!< Width of last space sequence */
    const gui_char* SpacePtr;                       /*!< Pointer to space start sequence */
    size_t CharsIndex;                              /*!< Index number of non-space sequence start */
    size_t CharsCount;                              /*!< Number of non-space characters in sequence */
    gui_dim_t CharsWidth;                           /*!< Width of characters after last space detected */
    const gui_char* CharsPtr;                       /*!< Pointer to chars start sequence */
    uint8_t IsLineFeed;                             /*!< Status indicating character is line feed */
    uint8_t Final;                                  /*!< Status indicating we should do line check and finish */
    uint8_t IsBreak;                                /*!< Status indicating break occurred */
} GUI_StringRectVars_t;

#define CH_CR           GUI_KEY_CR
#define CH_LF           GUI_KEY_LF
#define CH_WS           GUI_KEY_WS
#define get_char_from_value(ch)      (uint32_t)((CH_CR == (ch) || CH_LF == (ch)) ? CH_WS : (ch))

static GUI_StringRectVars_t var;

/**
 * \brief           Get info structure from font for given character
 * \param[in]       font: Font to use for drawing
 * \param[in]       ch: Unicode decoded character for font
 * \return          Char info of specific font and character code
 */
static const gui_font_char_t *
string_get_char_ptr(const gui_font_t* font, uint32_t ch) {
    ch = get_char_from_value(ch);                   /* Get char from char value */
    if (ch >= font->startchar && ch <= font->endchar) { /* Character is in font structure */
        return &font->data[(ch) - font->startchar]; /* Return character pointer from font */
    } else if ('?' >= font->startchar && '?' <= font->endchar) {/* Try to return ? character */
        return &font->data[(uint32_t)'?' - font->startchar];/* Get pointer of ? character */
    }
    return 0;                                       /* No character in font */
}

/**
 * \brief           Get width and height of specific character
 * \param[in]       font: Font to use for drawing
 * \param[in]       ch: Unicode decoded character for font
 * \param[out]      width: Output width variable
 * \param[out]      width: Output height variable
 */
static void
__StringGetCharSize(const gui_font_t* font, uint32_t ch, gui_dim_t* width, gui_dim_t* height) {
    const gui_font_char_t* c = 0;
    
    c = string_get_char_ptr(font, ch);              /* Get character from font */
    if (c != NULL) {
        *width = c->x_size + c->x_margin;
        *height = c->y_size;
    } else {
        *width = 0;
        *height = 0;
    }
}

/* Get string rectangle width and height */
#define RECT_CONTINUE(incCnt)     if (1) {          \
    if (incCnt) var.cnt++;                          \
    var.lastCh = var.ch;                            \
    continue;                                       \
}

/**
 * \brief           Optimize rectangle string
 * \param[in]       var: String processing context
 * \param[in]       rect: Rectangle parameters
 * \param[in]       end: Flag set to 1 if this is last entry of string
 */
static void
process_string_rectangle_before_return(GUI_StringRectVars_t* var, GUI_StringRect_t* rect, uint8_t end) {
    /*
     * If spaces are last elements
     */
    rect->ReadDraw = rect->ReadTotal = var->cnt;    /* Set number of total and drawing characters */
    if (rect->IsEditMode) {                         /* Do not optimize strings if we are in edit mode */
        if (var->IsLineFeed) {                      /* Line feed forced new line */
            rect->ReadDraw--;                       /* Don't draw this character at all */
        }
        /*
         * If there was a word which was too long for single line, put it to next line
         */
        if (var->CharsIndex > var->SpaceIndex && !end) {    /* If characters are the last values */
            var->cW -= var->CharsWidth;             /* Decrease effective width for characters */
            var->cnt -= var->CharsCount;            /* Decrease number of total count */
            rect->ReadDraw -= var->CharsCount;      /* Decrease number of characters to read and draw */
            rect->ReadTotal -= var->CharsCount;     /* Decrease number of characters to read */
        }
        return;
    }
    if (end) {                                      /* We received final line (end of string) */
        if (var->SpaceIndex > var->CharsIndex) {    /* When spaces are last characters */
            /*
             * When last line received, flush only white spaces and ignore other check
             */
            var->cW -= var->SpaceWidth;             /* Decrease effective rectangle width */
            rect->ReadDraw -= var->SpaceCount;      /* Decrease number of drawing characters by removing spaces */
        }
    } else if (var->SpaceIndex > var->CharsIndex) { /* When spaces are last characters */
        /*
         * - Leave number of characters to read to flush white spaces from string 
         * - Decrease effective rectangle size by ignored white spaces
         */
        var->cW -= var->SpaceWidth;                 /* Decrease effective rectangle width */
        rect->ReadDraw -= var->SpaceCount;          /* Decrease number of drawing characters by removing spaces */
    } else if (var->CharsIndex > var->SpaceIndex) { /* When non-spaces are last characters */
        /*
         * - Decrease number of characters to read as we don't want to flush non-white space characters
         * - Decrease effective rectangle size by ignored non-white space characters
         *
         * - Leave number of characters to read to flush white spaces in front of non-white space characters
         * - Decrease effective rectangle size by ignored white space characters
         */
        var->cnt -= var->CharsCount;
        if (!end) {                                 /* If not yet end string */
            var->s.Str = var->CharsPtr;             /* Set string pointer to start of characters sequence */
        }
        var->cW -= var->CharsWidth;
        rect->ReadDraw -= var->CharsCount;          /* Decrease number of drawing characters by removing spaces */
        if (var->SpaceIndex + var->SpaceCount == var->CharsIndex) {
            var->cW -= var->SpaceWidth;             /* Decrease effective width of line */
            rect->ReadDraw -= var->SpaceCount;      /* Decrease number of characters we actually have to draw */
        }
    } else {
        /*
         * This can only happen if both indexes are zero which means,
         * that there is no white-spaces at all (or were stripped at beginning)
         * and word was too big for one line and was separated to multiple lines
         * or line did not reach end
         */
    }
    rect->ReadTotal = var->cnt;                     /* Total number of characters to read from string */
}

/**
 * \brief           Get a line of text for given rectangle
 * \param[in]       rect: Rectangle parameters
 * \param[in]       str: Current string pointer
 * \param[in]       onlyToNextLine: Scan line only until next line is detected
 * \return          Length of string in current line
 */
static size_t
string_rectangle(GUI_StringRect_t* rect, GUI_STRING_t* str, uint8_t onlyToNextLine) {
    gui_dim_t w, h, mW = 0, tH = 0;                /* Maximal width and total height */
    uint8_t i;
    const gui_char* lastS;
    GUI_STRING_t tmpStr;

    memset(&var, 0x00, sizeof(var));                /* Reset structure */
    memcpy(&var.s, str, sizeof(*str));              /* Copy memory */
    memcpy(&tmpStr, str, sizeof(*str));             /* Copy memory */

    var.IsBreak = onlyToNextLine;                   /* Set for break */

    tH = 0;
    if (rect->StringDraw->Flags & GUI_FLAG_FONT_MULTILINE) {    /* We want to know exact rectangle for drawing multi line texts including new lines and carriage return */
        while (1) {                                 /* Unlimited execution */
            lastS = var.s.Str;                      /* Save current string pointer */
            if (gui_string_getch(&var.s, &var.ch, &i)) {/* Get next character from string */
                /* Check for CR character */
                if (CH_CR == var.ch) {              /* Check character */
                    var.ch = CH_WS;                 /* Set it as space and don't care for more */
                }

                /* Check for LF character */
                var.IsLineFeed = 0;
                if (CH_LF == var.ch) {              /* LF is for new line */
                    var.IsLineFeed = 1;             /* Character is line feed */
                    var.ch = CH_WS;                 /* Set it as space */
                }

                /* Check for white space character */
                if (CH_WS == var.ch) {              /* We have white space character */
                    if (CH_WS != var.lastCh) {      /* Check if previous char was also white space */
                        var.SpaceIndex = var.cnt;   /* Save index at which space sequence start appear */
                        var.SpaceCount = 0;         /* Reset number of white spaces */
                        var.SpaceWidth = 0;         /* Reset white spaces width */
                        var.SpacePtr = lastS;       /* Save pointer to last string entry of spaces */
                    }

                    if (var.SpaceIndex == 0) {      /* If we are on beginning of line */
                        if (!rect->IsEditMode) {    /* Do not ignore front spaces when in edit mode = show plain text as is */
                            if (!var.IsLineFeed) {  /* Do not increase pointer if line feed was detected = force new line*/
                                if (onlyToNextLine) {   /* Increase input pointer only in single line mode */
                                    str->Str += 1;  /* Increase input pointer where it points to */
                                }                   /* Otherwise just ignore character and don't touch input pointer */
                                RECT_CONTINUE(0);   /* Continue to next character and don't increase cnt variable */
                            }
                        }
                    }
                } else {                            /* Non-space character */
                    if (CH_WS == var.lastCh) {      /* If character before was white space */
                        var.CharsIndex = var.cnt;   /* Save index at which character sequence appear */
                        var.CharsCount = 0;         /* Reset number of characters */
                        var.CharsWidth = 0;         /* Reset characters width */
                        var.CharsPtr = lastS;       /* Save pointer to last string entry */
                    }
                }

                /* Check if line feed or normal character */
                if (var.IsLineFeed) {               /* Check for line feed */
                    var.Final = 1;                  /* Stop execution at this point */
                    var.cnt++;                      /* Increase number of elements manually */
                    var.SpaceCount++;               /* Increase number of spaces on last element */
                } else {                            /* Try to get character size */
                    /* Try to fit character in current line */
                    __StringGetCharSize(rect->Font, var.ch, &w, &h);    /* Get character dimensions */
                    if ((var.cW + w) < rect->StringDraw->Width) {   /* Do we have enough memory available */
                        var.cW += w;                /* Increase total line width */
                        if (CH_WS == var.ch) {      /* Check if character is white space */
                            var.SpaceCount++;       /* Increase number of spaces in a row */
                            var.SpaceWidth += w;    /* Increase width of spaces in a row */
                        } else {
                            var.CharsCount++;       /* Increase number of characters in a row */
                            var.CharsWidth += w;    /* Increase width of characters in a row */
                        }
                    } else {
                        var.Final = 1;              /* No more characters available in line, stop execution */
                    }
                }

                /* Check if line should be "closed" */
                if (var.Final) {                    /* Is this final for this line? */
                    process_string_rectangle_before_return(&var, rect, 0);

                    if (mW < var.cW) {              /* Check for width value */
                        mW = var.cW;                /* Set as new maximal width */
                    }

                    tH += rect->StringDraw->LineHeight; /* Increase line height for next line processing */
                    if (onlyToNextLine) {           /* Read only single line? */
                        break;                      /* Stop execution at this point */
                    }

                    /* Prepare for new line */
                    memcpy(&tmpStr, &var.s, sizeof(tmpStr));    /* Copy memory */
                    memset(&var, 0x00, sizeof(var));/* Reset everything now */
                    memcpy(&var.s, &tmpStr, sizeof(var.s)); /* Copy memory */
                    continue;
                } else {
                    RECT_CONTINUE(1);
                }
            } else {                                /* Everything has been read and line may still be free */
                process_string_rectangle_before_return(&var, rect, 1);  /* Process size before return */
                tH += rect->StringDraw->LineHeight; /* Increase line height for next line processing */
                if (mW < var.cW) {                  /* Current width check */
                    mW = var.cW;                    /* Save as max width currently */
                }
                break;                              /* Stop while loop execution */
            }
        }
        rect->Width = mW;                           /* Save rectangle width */
    } else {
        var.cW = 0;
        while (gui_string_getch(&var.s, &var.ch, &i)) { /* Get next character from string */
            __StringGetCharSize(rect->Font, var.ch, &w, &h);/* Get character width and height */
            if (!(rect->StringDraw->Flags & GUI_FLAG_FONT_RIGHTALIGN) && (var.cW + w) > rect->StringDraw->Width) {  /* Check if end now */
                break;
            }
            if (CH_CR != var.ch && CH_LF != var.ch) {
                var.cW += w;                        /* Increase width */
            }
            var.cnt++;                              /* Increase number of characters to read */
        }
        rect->ReadTotal = rect->ReadDraw = var.cnt; /* Set values for drawing and reading */
        rect->Width = var.cW;                       /* Save width value */
        tH += rect->StringDraw->LineHeight;         /* Set line height */
    }
    rect->Height = tH;                              /* Save rectangle height value */
    return var.cnt;                                 /* Return number of characters to read in current line */
}

/**
 * \brief           Get character entry generated in memory for fast drawing
 * \param[in]       font: Font used for character
 * \param[in]       c: Character info handle
 * \return          Character entry or NULL on failure
 */
static GUI_FONT_CharEntry_t *
get_char_entry_from_font(const gui_font_t* font, const gui_font_char_t* c) {
    GUI_FONT_CharEntry_t* entry;
    
    for (entry = (GUI_FONT_CharEntry_t *)gui_linkedlist_getnext_gen(&GUI.RootFonts, NULL); entry != NULL;
        entry = (GUI_FONT_CharEntry_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)entry)) {
        if (entry->Font == font && entry->Ch == c) {
            return entry;
        }
    }
    return 0;
}

/* Create char and put it to RAM for fast drawing with memory to memory copy */
static GUI_FONT_CharEntry_t *
create_char_entry_from_font(const gui_font_t* font, const gui_font_char_t* c) {
    GUI_FONT_CharEntry_t* entry = NULL;
    uint16_t columns;
    uint16_t memsize = GUI_MEM_ALIGN(sizeof(*entry));   /* Get size of entry */
    uint16_t memDataSize;
    
    /* Calculate memory size for data */
    memDataSize = c->x_size * c->y_size;
    
    memsize += GUI_MEM_ALIGN(memDataSize);          /* Align memory before increase */
    entry = GUI_MEMALLOC(memsize);                  /* Allocate memory for entry */
    if (entry != NULL) {                            /* Allocation was successful */
        uint16_t i, x;
        uint8_t b, k, t;
        uint8_t* ptr = (uint8_t *)entry;            /* Go to memory size */
        ptr += GUI_MEM_ALIGN(sizeof(*entry));       /* Go to start of data, at the end of aligned structure size */
        
        entry->Ch = c;                              /* Set pointer to character */
        entry->Font = font;                         /* Set pointer to font structure */
        
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
            for (i = 0; i < c->y_size * columns; i++) {  /* Inspect all vertical lines */
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
        
        gui_linkedlist_add_gen(&GUI.RootFonts, (gui_linkedlist_t *)entry);  /* Add entry to linked list */
    }
    return entry;                                   /* Return new created entry */
}

/* Draw character to screen */
/* X and Y coordinates are TOP LEFT coordinates for character */
static void
draw_char(const gui_display_t* disp, const gui_font_t* font, const GUI_DRAW_FONT_t* draw, gui_dim_t x, gui_dim_t y, const gui_font_char_t* c) {
    uint8_t i, b, k, columns;
    gui_dim_t x1;
    
    while (!GUI.LL.IsReady(&GUI.LCD));              /* Wait till ready */
    
    y += c->y_pos;                                  /* Set Y position */
    
    if (!__GUI_RECT_MATCH(
        disp->X1, disp->Y1, disp->X2, disp->Y2,
        x, y, x + c->x_size, y + c->y_size
    )) {
        return;
    }
    
    if (GUI.LL.CopyChar != NULL) {                  /* If copying character function exists in low-level part */
        GUI_FONT_CharEntry_t* entry = NULL;
        
        entry = get_char_entry_from_font(font, c);  /* Get char entry from font and character for fast alpha drawing operations */
        if (entry == NULL) {
            entry = create_char_entry_from_font(font, c);   /* Create new entry */
        }
        if (entry != NULL) {                        /* We have valid data */
            gui_dim_t width, height, offlineSrc, offlineDst, tmpX;
            uint8_t* dst = 0;
            uint8_t* ptr = (uint8_t *)entry;        /* Get pointer */
            
            tmpX = x;                               /* Start X */
            
            ptr += sizeof(*entry);                  /* Go to start of data array */
            dst = (uint8_t *)(GUI.LCD.DrawingLayer->StartAddress + ((y - GUI.LCD.DrawingLayer->OffsetY) * GUI.LCD.DrawingLayer->Width + (x - GUI.LCD.DrawingLayer->OffsetX)) * GUI.LCD.PixelSize);
            
            width = c->x_size;                      /* Get X size */
            height = c->y_size;                     /* Get Y size */
            
            if (y < disp->Y1) {                     /* Start Y position if outside visible area */
                ptr += (disp->Y1 - y) * c->x_size;  /* Set offset for number of lines */
                dst += (disp->Y1 - y) * GUI.LCD.DrawingLayer->Width * GUI.LCD.PixelSize;  /* Set offset for number of LCD lines */
                height -= disp->Y1 - y;             /* Decrease effective height */
            }
            if ((y + c->y_size) > disp->Y2) {
                height -= y + c->y_size - disp->Y2; /* Decrease effective height */
            }
            if (x < disp->X1) {                     /* Set offset start address if required */
                ptr += (disp->X1 - x);              /* Set offset of start address in X direction */
                dst += (disp->X1 - x) * GUI.LCD.PixelSize;  /* Set offset of start address in X direction */
                width -= disp->X1 - x;              /* Increase source offline */
                tmpX += disp->X1 - x;               /* Increase effective start X position */
            }
            if ((x + c->x_size) > disp->X2) {
                width -= x + c->x_size - disp->X2;  /* Decrease effective width */
            }
            
            offlineSrc = c->x_size - width;         /* Set offline source */
            offlineDst = GUI.LCD.DrawingLayer->Width - width;   /* Set offline destination */
            
            /**
             * Check if character must be drawn with 2 colors, on the middle of color switch
             */
            if (tmpX < (draw->X + draw->Color1Width) && (tmpX + width) > (draw->X + draw->Color1Width)) {
                gui_dim_t firstWidth = (draw->X + draw->Color1Width) - tmpX;
                
                /* First part draw */
                GUI.LL.CopyChar(&GUI.LCD, GUI.LCD.DrawingLayer, ptr, dst, 
                    firstWidth, height,
                    offlineSrc + width - firstWidth, offlineDst + width - firstWidth, draw->Color1);
                
                /* Second part draw */
                GUI.LL.CopyChar(&GUI.LCD, GUI.LCD.DrawingLayer, ptr + firstWidth, dst + firstWidth * GUI.LCD.PixelSize, 
                    width - firstWidth, height,
                    offlineSrc + firstWidth, offlineDst + firstWidth, draw->Color2);
            } else {
                /* Draw entire character with single color */
                GUI.LL.CopyChar(&GUI.LCD, GUI.LCD.DrawingLayer, ptr, dst, 
                    width, height,
                    offlineSrc, offlineDst, (draw->X + draw->Color1Width) > x ? draw->Color1 : draw->Color2);
            }
            return;
        }
    }
    
    if (font->flags & GUI_FLAG_FONT_AA) {           /* Font has anti alliasing enabled */
        gui_color_t color;                          /* Temporary color for AA */
        uint8_t tmp, r1, g1, b1;
        
        columns = c->x_size / 4;                    /* Calculate number of bytes used for single character line */
        if (c->x_size % 4) {                        /* If only 1 column used */
            columns++;
        }
        
        for (i = 0; i < columns * c->y_size; i++) { /* Go through all data bytes */
            if (y >= disp->Y1 && y <= disp->Y2 && y < (draw->Y + draw->Height)) {   /* Do not draw when we are outside clipping are */            
                b = c->data[i];                     /* Get character byte */
                for (k = 0; k < 4; k++) {           /* Scan each bit in byte */
                    gui_color_t baseColor;
                    x1 = x + (i % columns) * 4 + k; /* Get new X value for pixel draw */
                    if (x1 < disp->X1 || x1 > disp->X2) {
                        continue;
                    }
                    if (x1 < (draw->X + draw->Color1Width)) {
                        baseColor = draw->Color1;
                    } else {
                        baseColor = draw->Color2;
                    }
                    tmp = (b >> (6 - 2 * k)) & 0x03;/* Get temporary bits on bottom */
                    if (tmp == 0x03) {              /* Draw solid color if both bits are enabled */
                        gui_draw_setpixel(disp, x1, y, baseColor);
                    } else if (tmp) {               /* Calculate new color */
                        float t = (float)tmp / 3.0f;
                        color = gui_draw_getpixel(disp, x1, y); /* Read current color */
                        
                        /* Calculate new values for pixel */
                        r1 = (float)t * (float)((color >> 16) & 0xFF) + (float)(1.0f - (float)t) * (float)((baseColor >> 16) & 0xFF);
                        g1 = (float)t * (float)((color >>  8) & 0xFF) + (float)(1.0f - (float)t) * (float)((baseColor >>  8) & 0xFF);
                        b1 = (float)t * (float)((color >>  0) & 0xFF) + (float)(1.0f - (float)t) * (float)((baseColor >>  0) & 0xFF);
                        
                        /* Draw actual pixel to screen */
                        gui_draw_setpixel(disp, x1, y, (color & 0xFF000000UL) | r1 << 16 | g1 << 8 | b1);
                    }
                }
            }
            if ((i % columns) == (columns - 1)) {   /* Check for next line */
                y++;
            }
        }
    } else {
        columns = c->x_size / 8;
        if (c->x_size % 8) {
            columns++;
        }
        for (i = 0; i < columns * c->y_size; i++) { /* Go through all data bytes */
            if (y >= disp->Y1 && y <= disp->Y2 && y < (draw->Y + draw->Height)) {   /* Do not draw when we are outside clipping are */
                b = c->data[i];                     /* Get character byte */
                for (k = 0; k < 8; k++) {           /* Scan each bit in byte */
                    if (b & (1 << (7 - k))) {       /* If bit is set, draw pixel */
                        x1 = x + (i % columns) * 8 + k; /* Get new X value for pixel draw */
                        if (x1 < disp->X1 || x1 > disp->X2) {
                            continue;
                        }
                        if (x1 <= (draw->X + draw->Color1Width)) {
                            gui_draw_setpixel(disp, x1, y, draw->Color1);
                        } else {
                            gui_draw_setpixel(disp, x1, y, draw->Color2);
                        }
                    }
                }
            }
            if ((i % columns) == (columns - 1)) {   /* Check for next line */
                y++;
            }
        }
    }
}

/* Get string pointer start address for specific width of rectangle */
static const gui_char *
string_get_pointer_for_width(const gui_font_t* font, GUI_STRING_t* str, GUI_DRAW_FONT_t* draw) {
    gui_dim_t tot = 0, w, h;
    uint8_t i;
    uint32_t ch;
    const gui_char* tmp = str->Str;                 /* Set start of string */
    
    gui_string_gotoend(str);                        /* Go to the end of string */
    
    while (str->Str >= tmp) {
        if (!gui_string_getchreverse(str, &ch, &i)) {   /* Get character in reverse order */
            break;
        }
        __StringGetCharSize(font, ch, &w, &h);
        if ((tot + w) < draw->Width) {
            tot += w;
        } else {
            draw->X += draw->Width - tot;           /* Add X position to align right */
            break;
        }
    }
    return str->Str + i + 1;
}

/* Fill screen with color on specific coordinates */
static void
gui_draw_fill(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_color_t color) {
    if (                                            /* Check if redraw is inside area */
        !__GUI_RECT_MATCH(  x, y, x + width, y + height,
                            disp->X1, disp->Y1, disp->X2, disp->Y2)) {
        return;
    }
        
    if (width <= 0 || height <= 0) {
        return;
    }
    
    /* We are in region */
    if (x < disp->X1) {
        width -= (disp->X1 - x);
        x = disp->X1;
    }
    if (y < disp->Y1) {
        height -= (disp->Y1 - y);
        y = disp->Y1;
    }
    
    /* Check out of regions */
    if ((x + width) > disp->X2) {
        width = disp->X2 - x;
    }
    if ((y + height) > disp->Y2) {
        height = disp->Y2 - y;
    }
    if (width > 0 && height > 0) {
        GUI.LL.FillRect(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY, width, height, color);
    }
}

/**
 * \brief           Initialize \ref GUI_DRAW_FONT_t structure for further usage
 * \param[in,out]   *f: Pointer to empty \ref GUI_DRAW_FONT_t structure 
 */
void
gui_draw_font_init(GUI_DRAW_FONT_t* f) {
    memset((void *)f, 0x00, sizeof(*f));            /* Reset structure */
}

/**
 * \brief           Fill screen with color
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       color: Color to use for filling screen 
 */
void
gui_draw_fillscreen(const gui_display_t* disp, gui_color_t color) {
    GUI.LL.Fill(&GUI.LCD, GUI.LCD.DrawingLayer, 0, GUI.LCD.DrawingLayer->Width, GUI.LCD.DrawingLayer->Height, 0, color);
}

/**
 * \brief           Set single pixel at X and Y location
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: X position on LCD
 * \param[in]       y: Y position on LCD
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_getpixel
 */
void
gui_draw_setpixel(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_color_t color) {
    if (y < disp->Y1 || y >= disp->Y2 || x < disp->X1 || x >= disp->X2) {
        return;
    }
    GUI.LL.SetPixel(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY, color);
}

/**
 * \brief           Get pixel color at X and Y location
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: X position on LCD
 * \param[in]       y: Y position on LCD
 * \return          Pixel color at desired position
 * \sa              gui_draw_setpixel
 */
gui_color_t
gui_draw_getpixel(const gui_display_t* disp, gui_dim_t x, gui_dim_t y) {
    return GUI.LL.GetPixel(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY);
}

/**
 * \brief           Draw vertical line to LCD
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Line top X position
 * \param[in]       y: Line top Y position
 * \param[in]       length: Length of vertical line
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_hline, gui_draw_line
 */
void
gui_draw_vline(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t length, gui_color_t color) {
    if (x >= disp->X2 || x < disp->X1 || y > disp->Y2 || (y + length) < disp->Y1) {
        return;
    }
    if (y < disp->Y1) {
        length -= disp->Y1 - y;
        y = disp->Y1;
    }
    if ((y + length) > disp->Y2) {
        length = disp->Y2 - y;
    }
    GUI.LL.DrawVLine(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY, length, color);
}

/**
 * \brief           Draw horizontal line to LCD
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Line left X position
 * \param[in]       y: Line left Y position
 * \param[in]       length: Length of horizontal line
 * \param[in]       color: Color used for drawing operation
 * \sa              gui_draw_vline, gui_draw_line
 */
void
gui_draw_hline(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t length, gui_color_t color) {
    if (y >= disp->Y2 || y < disp->Y1 || x > disp->X2 || (x + length) < disp->X1) {
        return;
    }
    if (x < disp->X1) {
        length -= disp->X1 - x;
        x = disp->X1;
    }
    if ((x + length) > disp->X2) {
        length = disp->X2 - x;
    }
    GUI.LL.DrawHLine(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY, length, color);
}

/******************************************************************************/
/******************************************************************************/
/***                          Functions for primitives                       **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Draw line from point 1 to point 2
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x1: Line start X position
 * \param[in]       y1: Line start Y position
 * \param[in]       x2: Line end X position
 * \param[in]       y2: Line end Y position
 * \param[in]       color: Color used for drawing operation
 * \sa              gui_draw_vline, gui_draw_hline
 */
void
gui_draw_line(const gui_display_t* disp, gui_dim_t x1, gui_dim_t y1, gui_dim_t x2, gui_dim_t y2, gui_color_t color) {
    gui_dim_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
    curpixel = 0;
    
    /* Check if coordinates are inside drawing region */
//    if (                                            /* Check if redraw is inside area */
//        !__GUI_RECT_MATCH(  x1, y1, x2, y2,
//                            disp->X1, disp->Y1, disp->X2, disp->Y2)) {
//        return;
//    }

	deltax = GUI_ABS(x2 - x1);
	deltay = GUI_ABS(y2 - y1);
    
    if (deltax == 0) {                              /* Straight vertical line */
        gui_draw_vline(disp, x1, GUI_MIN(y1, y2), deltay, color);
        return;
    }
    if (deltay == 0) {                              /* Straight horizontal line */
        gui_draw_hline(disp, GUI_MIN(x1, x2), y1, deltax, color);
        return;
    }

    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) {
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        gui_draw_setpixel(disp, x, y, color);
        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

/**
 * \brief           Draw rectangle
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Top left X position
 * \param[in]       y: Top left Y position
 * \param[in]       width: Rectangle width
 * \param[in]       height: Rectangle height
 * \param[in]       color: Color used for drawing operation
 * \sa              gui_draw_filledrectangle, gui_draw_roundedrectangle, gui_draw_filledroundedrectangle
 */
void
gui_draw_rectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_color_t color) {
    if (width == 0 || height == 0) {
        return;
    }
    gui_draw_hline(disp, x,             y,              width,  color);
    gui_draw_vline(disp, x,             y,              height, color);
    
    gui_draw_hline(disp, x,             y + height - 1, width,  color);
    gui_draw_vline(disp, x + width - 1, y,              height, color);
}

/**
 * \brief           Draw filled rectangle
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Top left X position
 * \param[in]       y: Top left Y position
 * \param[in]       width: Rectangle width
 * \param[in]       height: Rectangle height
 * \param[in]       color: Color used for drawing operation
 * \sa              gui_draw_rectangle, gui_draw_roundedrectangle, gui_draw_filledroundedrectangle
 */
void
gui_draw_filledrectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_color_t color) {
    gui_draw_fill(disp, x, y, width, height, color);
}

/**
 * \brief           Draw rectangle with 3D view
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Top left X position
 * \param[in]       y: Top left Y position
 * \param[in]       width: Rectangle width
 * \param[in]       height: Rectangle height
 * \param[in]       state: 3D state. This parameter can be a value of \ref GUI_DRAW_3D_State_t enumeration
 */
void
gui_draw_rectangle3d(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, GUI_DRAW_3D_State_t state) {
    gui_color_t c1, c2, c3;
    
    c1 = GUI_COLOR_BLACK;
    if (state == GUI_DRAW_3D_State_Raised) {
        c2 = 0xFFAAAAAA;
        c3 = 0xFF555555;
    } else {
        c2 = 0xFF555555;
        c3 = 0xFFAAAAAA;
    }
    
    gui_draw_rectangle(disp, x, y, width, height, c1);
    
    gui_draw_hline(disp, x + 1, y + 1, width - 2, c2);
    gui_draw_vline(disp, x + 1, y + 1, height - 3, c2);
    
    gui_draw_hline(disp, x + 1, y + height - 2, width - 2, c3);
    gui_draw_vline(disp, x + width - 2, y + 2, height - 4, c3);
}

/**
 * \brief           Draw rectangle with rounded corners
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Top left X position
 * \param[in]       y: Top left Y position
 * \param[in]       width: Rectangle width
 * \param[in]       height: Rectangle height
 * \param[in]       r: Corner radius, max value can be r = MIN(width, height) / 2
 * \param[in]       color: Color used for drawing operation
 * \sa              gui_draw_rectangle, gui_draw_filledrectangle, gui_draw_filledroundedrectangle
 */
void
gui_draw_roundedrectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_dim_t r, gui_color_t color) {
    if (r >= (height / 2)) {
        r = height / 2 - 1;
    }
    if (r >= (width / 2)) {
        r = width / 2 - 1;
    }
    if (r) {
        gui_draw_hline(disp, x + r,         y,              width - 2 * r,  color);
        gui_draw_vline(disp, x + width - 1, y + r,          height - 2 * r, color);
        gui_draw_hline(disp, x + r,         y + height - 1, width - 2 * r,  color);
        gui_draw_vline(disp, x,             y + r,          height - 2 * r, color);
        
        gui_draw_circlecorner(disp, x + r,             y + r,              r, GUI_DRAW_CIRCLE_TL, color);
        gui_draw_circlecorner(disp, x + width - r - 1, y + r,              r, GUI_DRAW_CIRCLE_TR, color);
        gui_draw_circlecorner(disp, x + r,             y + height - r - 1, r, GUI_DRAW_CIRCLE_BL, color);
        gui_draw_circlecorner(disp, x + width - r - 1, y + height - r - 1, r, GUI_DRAW_CIRCLE_BR, color);
    } else {
        gui_draw_rectangle(disp, x, y, width, height, color);
    }
}

/**
 * \brief           Draw filled rectangle with rounded corners
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Top left X position
 * \param[in]       y: Top left Y position
 * \param[in]       width: Rectangle width
 * \param[in]       height: Rectangle height
 * \param[in]       r: Corner radius, max value can be r = MIN(width, height) / 2
 * \param[in]       color: Color used for drawing operation
 * \sa              gui_draw_rectangle, gui_draw_filledrectangle, gui_draw_roundedrectangle
 */
void
gui_draw_filledroundedrectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_dim_t r, gui_color_t color) {
    if (r >= (height / 2)) {
        r = height / 2 - 1;
    }
    if (r >= (width / 2)) {
        r = width / 2 - 1;
    }
    if (r) {
        if ((gui_dim_t)(width - 2 * r) > 0) {
            gui_draw_filledrectangle(disp, x + r,         y,     width - 2 * r, height,         color);
        }
        if ((gui_dim_t)(height - 2 * r) > 0) {
            gui_draw_filledrectangle(disp, x,             y + r, r,             height - 2 * r, color);
            gui_draw_filledrectangle(disp, x + width - r, y + r, r,             height - 2 * r, color);
        }
        
        gui_draw_filledcirclecorner(disp, x + r,             y + r,              r, GUI_DRAW_CIRCLE_TL, color);
        gui_draw_filledcirclecorner(disp, x + width - r - 1, y + r,              r, GUI_DRAW_CIRCLE_TR, color);
        gui_draw_filledcirclecorner(disp, x + r,             y + height - r - 1, r, GUI_DRAW_CIRCLE_BL, color);
        gui_draw_filledcirclecorner(disp, x + width - r - 1, y + height - r - 1, r, GUI_DRAW_CIRCLE_BR, color);
    } else {
        gui_draw_filledrectangle(disp, x, y, width, height, color);
    }
}

/**
 * \brief           Draw circle
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: X position of circle center
 * \param[in]       y: X position of circle center
 * \param[in]       r: Circle radius
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_filledcircle, gui_draw_circlecorner, gui_draw_filledcirclecorner
 */
void
gui_draw_circle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t r, gui_color_t color) {
    gui_draw_circlecorner(disp, x, y, r, GUI_DRAW_CIRCLE_TL, color);
    gui_draw_circlecorner(disp, x - 1, y, r, GUI_DRAW_CIRCLE_TR, color);
    gui_draw_circlecorner(disp, x, y - 1, r, GUI_DRAW_CIRCLE_BL, color);
    gui_draw_circlecorner(disp, x - 1, y - 1, r, GUI_DRAW_CIRCLE_BR, color);
}

/**
 * \brief           Draw filled circle
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: X position of circle center
 * \param[in]       y: X position of circle center
 * \param[in]       r: Circle radius
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_circle, gui_draw_circlecorner, gui_draw_filledcirclecorner
 */
void
gui_draw_filledcircle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t r, gui_color_t color) {
    gui_draw_filledcirclecorner(disp, x, y, r, GUI_DRAW_CIRCLE_TL, color);
    gui_draw_filledcirclecorner(disp, x, y, r, GUI_DRAW_CIRCLE_TR, color);
    gui_draw_filledcirclecorner(disp, x, y - 1, r, GUI_DRAW_CIRCLE_BL, color);
    gui_draw_filledcirclecorner(disp, x, y - 1, r, GUI_DRAW_CIRCLE_BR, color);
}

/**
 * \brief           Draw triangle
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x1: Triangle point 1 X position
 * \param[in]       y1: Triangle point 1 Y position
 * \param[in]       x2: Triangle point 2 X position
 * \param[in]       y2: Triangle point 2 Y position
 * \param[in]       x3: Triangle point 3 X position
 * \param[in]       y3: Triangle point 3 Y position
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_filledtriangle
 */
void
gui_draw_triangle(const gui_display_t* disp, gui_dim_t x1, gui_dim_t y1,  gui_dim_t x2, gui_dim_t y2, gui_dim_t x3, gui_dim_t y3, gui_color_t color) {
    gui_draw_line(disp, x1, y1, x2, y2, color);
    gui_draw_line(disp, x1, y1, x3, y3, color);
    gui_draw_line(disp, x2, y2, x3, y3, color);
}

/**
 * \brief           Draw filled triangle
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x1: Triangle point 1 X position
 * \param[in]       y1: Triangle point 1 Y position
 * \param[in]       x2: Triangle point 2 X position
 * \param[in]       y2: Triangle point 2 Y position
 * \param[in]       x3: Triangle point 3 X position
 * \param[in]       y3: Triangle point 3 Y position
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_triangle
 */
void
gui_draw_filledtriangle(const gui_display_t* disp, gui_dim_t x1, gui_dim_t y1, gui_dim_t x2, gui_dim_t y2, gui_dim_t x3, gui_dim_t y3, gui_color_t color) {
    gui_dim_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
    curpixel = 0;

    deltax = GUI_ABS(x2 - x1);
    deltay = GUI_ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay){
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        gui_draw_line(disp, x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

/**
 * \brief           Draw circle corner, selected with parameter
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x0: X position of corner origin
 * \param[in]       y0: Y position of corner origin
 * \param[in]       r: Circle radius
 * \param[in]       c: List of corners to draw. Use BITWISE OR with these values to specify corner:
 *                     - \ref GUI_DRAW_CIRCLE_TL
 *                     - \ref GUI_DRAW_CIRCLE_TR
 *                     - \ref GUI_DRAW_CIRCLE_BL
 *                     - \ref GUI_DRAW_CIRCLE_BR
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_circle, gui_draw_filledcircle, gui_draw_filledcirclecorner
 */
void
gui_draw_circlecorner(const gui_display_t* disp, gui_dim_t x0, gui_dim_t y0, gui_dim_t r, uint8_t c, gui_color_t color) {
    gui_dim_t f = 1 - r;
    gui_dim_t ddF_x = 1;
    gui_dim_t ddF_y = -2 * r;
    gui_dim_t x = 0;
    gui_dim_t y = r;
    
    if (!__GUI_RECT_MATCH(
        x0 - r, y0 - r, x0 + r, y0 + r,
        disp->X1, disp->Y1, disp->X2, disp->Y2
    )) {
        return;
    }

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (c & GUI_DRAW_CIRCLE_TL) {               /* Top left corner */
            gui_draw_setpixel(disp, x0 - y, y0 - x, color);
            gui_draw_setpixel(disp, x0 - x, y0 - y, color);
        }

        if (c & GUI_DRAW_CIRCLE_TR) {               /* Top right corner */
            gui_draw_setpixel(disp, x0 + x, y0 - y, color);
            gui_draw_setpixel(disp, x0 + y, y0 - x, color);
        }

        if (c & GUI_DRAW_CIRCLE_BR) {               /* Bottom right corner */
            gui_draw_setpixel(disp, x0 + x, y0 + y, color);
            gui_draw_setpixel(disp, x0 + y, y0 + x, color);
        }

        if (c & GUI_DRAW_CIRCLE_BL) {               /* Bottom left corner */
            gui_draw_setpixel(disp, x0 - x, y0 + y, color);
            gui_draw_setpixel(disp, x0 - y, y0 + x, color);
        }
    }
}

/**
 * \brief           Draw filled circle corner, selected with parameter
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x0: X position of corner origin
 * \param[in]       y0: Y position of corner origin
 * \param[in]       r: Circle radius
 * \param[in]       c: List of corners to draw. Use BITWISE OR with these values to specify corner:
 *                     - \ref GUI_DRAW_CIRCLE_TL
 *                     - \ref GUI_DRAW_CIRCLE_TR
 *                     - \ref GUI_DRAW_CIRCLE_BL
 *                     - \ref GUI_DRAW_CIRCLE_BR
 * \param[in]       color: Color used for drawing operation 
 * \sa              gui_draw_circle, gui_draw_filledcircle, gui_draw_circlecorner
 */
void
gui_draw_filledcirclecorner(const gui_display_t* disp, gui_dim_t x0, gui_dim_t y0, gui_dim_t r, uint8_t c, gui_color_t color) {
    gui_dim_t f = 1 - r;
    gui_dim_t ddF_x = 1;
    gui_dim_t ddF_y = -2 * r;
    gui_dim_t x = 0;
    gui_dim_t y = r;
    
    if (!__GUI_RECT_MATCH(
        disp->X1, disp->Y1, disp->X2, disp->Y2,
        x0 - r, y0 - r, x0 + r, y0 + r
    )) {
        return;
    }

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (c & GUI_DRAW_CIRCLE_TL) {               /* Top left */            
            gui_draw_hline(disp, x0 - x, y0 - y, x, color);
            gui_draw_hline(disp, x0 - y, y0 - x, y, color);
        }
        if (c & GUI_DRAW_CIRCLE_TR) {               /* Top right */
            gui_draw_hline(disp, x0, y0 - y, x, color);
            gui_draw_hline(disp, x0, y0 - x, y, color);
        }
        if (c & GUI_DRAW_CIRCLE_BL) {               /* Bottom left */
            gui_draw_hline(disp, x0 - y, y0 + x, y, color);
            gui_draw_hline(disp, x0 - x, y0 + y, x, color);
        }
        if (c & GUI_DRAW_CIRCLE_BR) {               /* Bottom right */
            gui_draw_hline(disp, x0, y0 + x, y, color);
            gui_draw_hline(disp, x0, y0 + y, x, color);
        }
    }
}

/**
 * \brief           Draw image to display of any depth and size
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       x: Top left X position
 * \param[in]       y: Top left Y position
 * \param[in]       img: Pointer to \ref GUI_IMAGE_DESC_t structure with image description 
 */
void
gui_draw_image(gui_display_t* disp, gui_dim_t x, gui_dim_t y, const GUI_IMAGE_DESC_t* img) {
    uint8_t bytes = img->BPP >> 3;                  /* Get number of bytes per pixel on image */
    
    GUI_Layer_t* layer;
    const uint8_t* src;
    const uint8_t* dst;
    gui_dim_t width, height;
    gui_dim_t offlineSrc, offlineDst;
    
    if (!img || !__GUI_RECT_MATCH(
        disp->X1, disp->Y1, disp->X2, disp->Y2,
        x, y, x + img->xSize, y + img->ySize
    )) {
        return;
    }
    
    layer = GUI.LCD.DrawingLayer;                   /* Set layer pointer */
    
    width = img->xSize;                             /* Set default width */
    height = img->ySize;                            /* Set default height */
    
    src = (uint8_t *)(img->Image);                  /* Set source address */
    dst = (uint8_t *)(layer->StartAddress + GUI.LCD.PixelSize * ((y - layer->OffsetY) * layer->Width + (x - layer->OffsetX)));
    
    //TODO: Check proper coordinates for memory!
    if (y < disp->Y1) {
        src += (disp->Y1 - y) * img->xSize * bytes; /* Set offset for number of image lines */
        dst += (disp->Y1 - y) * GUI.LCD.Width * GUI.LCD.PixelSize;  /* Set offset for number of LCD lines */
        height -= disp->Y1 - y;                     /* Decrease effective height */
    }
    if ((y + img->ySize) > disp->Y2) {
        height -= y + img->ySize - disp->Y2;        /* Decrease effective height */
    }
    if (x < disp->X1) {                             /* Set offset start address if required */
        src += (disp->X1 - x) * bytes;              /* Set offset of start address in X direction */
        dst += (disp->X1 - x) * GUI.LCD.PixelSize;  /* Set offset of start address in X direction */
        width -= disp->X1 - x;                      /* Decrease effective width */
    }
    if ((x + img->xSize) > disp->X2) {
        width -= x + img->xSize - disp->X2;         /* Decrease effective width */
    }
    
    offlineSrc = img->xSize - width;                /* Set offline source */
    offlineDst = layer->Width - width;              /* Set offline destination */
    
    /*******************/
    /*    Draw image   */
    /*******************/
    if (bytes == 4) {                               /* Draw 32BPP image */
        if (GUI.LL.DrawImage32) {                   /* Draw image 32BPP if possible */
            GUI.LL.DrawImage32(&GUI.LCD, GUI.LCD.DrawingLayer, img, (uint8_t *)src, (uint8_t *)dst, width, height, offlineSrc, offlineDst);
        }
    } else if (bytes == 3) {                        /* Draw 24BPP image */
        if (GUI.LL.DrawImage24) {                   /* Draw image 24BPP if possible */
            GUI.LL.DrawImage24(&GUI.LCD, GUI.LCD.DrawingLayer, img, (uint8_t *)src, (uint8_t *)dst, width, height, offlineSrc, offlineDst);
        }
    } else if (bytes == 2) {                        /* Draw 16BPP image */
        if (GUI.LL.DrawImage16) {                   /* Draw image 16BPP if possible */
            GUI.LL.DrawImage16(&GUI.LCD, GUI.LCD.DrawingLayer, img, (uint8_t *)src, (uint8_t *)dst, width, height, offlineSrc, offlineDst);
        }
    }
}

/**
 * \brief           Draw polygon lines
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       points: Pointer to array of \ref GUI_DRAW_Poly_t points to draw lines between
 * \param[in]       len: Number of points in array. There must be at least 2 points
 * \param[in]       color: Color to use for drawing 
 */
void
gui_draw_poly(const gui_display_t* disp, const GUI_DRAW_Poly_t* points, size_t len, gui_color_t color) {
    gui_dim_t x = 0, y = 0;

    if (len < 2) {
        return;
    }

    gui_draw_line(disp, points->X, points->Y, (points + len - 1)->X, (points + len - 1)->Y, color);

    while (--len) {
        x = points->X;
        y = points->Y;
        points++;
        gui_draw_line(disp, x, y, points->X, points->Y, color);
    }
}

/**
 * \brief           Write text to screen
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       font: Pointer to \ref gui_font_t structure with font to use
 * \param[in]       str: Pointer to string to draw on screen
 * \param[in]       draw: Pointer to \ref GUI_DRAW_FONT_t structure with specifications about drawing style 
 */
void
gui_draw_writetext(const gui_display_t* disp, const gui_font_t* font, const gui_char* str, GUI_DRAW_FONT_t* draw) {
    gui_dim_t x, y;
    uint32_t ch;
    uint8_t i;
    size_t cnt;
    const gui_font_char_t* c;
    GUI_StringRect_t rect = {0};                    /* Get string object */
    GUI_STRING_t currStr;
    
    if (!draw->LineHeight) {                        /* When line height is not set */
        draw->LineHeight = font->size;              /* Set font size */
    }
    
    rect.Font = font;                               /* Save font structure */
    rect.StringDraw = draw;                         /* Set drawing pointer */
    rect.IsEditMode = !!(draw->Flags & GUI_FLAG_FONT_EDITMODE); /* Check if in edit mode */
      
    gui_string_prepare(&currStr, str);              /* Prepare string */
    string_rectangle(&rect, &currStr, 0);           /* Get string width for this box */
    if (rect.Width > draw->Width) {                 /* If string is wider than available rectangle */
        if (draw->Flags & GUI_FLAG_FONT_RIGHTALIGN) {   /* Check right align text */
            gui_string_prepare(&currStr, str);      /* Prepare string */
            str = string_get_pointer_for_width(font, &currStr, draw);  /* Get string pointer */
        } else {
            rect.Width = draw->Width;               /* Strip text width to available */
        }
    }
    
    x = draw->X;                                    /* Get start X position */
    y = draw->Y;                                    /* Get start Y position */
    
    if (draw->Align & GUI_VALIGN_CENTER) {          /* Check for vertical align center */
        y += (draw->Height - rect.Height) / 2;      /* Align center of drawing area */
    } else if (draw->Align & GUI_VALIGN_BOTTOM) {   /* Check for vertical align bottom */
        y += draw->Height - rect.Height;            /* Align bottom of drawing area */
    }
    
    if (y < draw->Y) {                              /* Check situation first */
        y = draw->Y;
    }
    y -= draw->ScrollY;                             /* Go scroll top */
    
    /**
     * Check Y start value in case of edit mode = allow always on bottom
     */
    if (draw->Flags & GUI_FLAG_FONT_MULTILINE && rect.IsEditMode) { /* In multi-line and edit mode */
        if (rect.Height > draw->Height) {           /* If text is greater than visible area in edit mode, set it to bottom align */
            y = draw->Y + draw->Height - rect.Height;
        }
    }    
    
    gui_string_prepare(&currStr, str);              /* Prepare string again */
    while ((cnt = string_rectangle(&rect, &currStr, 1)) > 0) {
        x = draw->X;                                       
        if (draw->Align & GUI_HALIGN_CENTER) {      /* Check for horizontal align center */
            x += (draw->Width - rect.Width) / 2;    /* Align center of drawing area */
        } else if (draw->Align & GUI_HALIGN_RIGHT) {/* Check for horizontal align right */
            x += draw->Width - rect.Width;          /* Align right of drawing area */
        }
        while (cnt-- && gui_string_getch(&currStr, &ch, &i)) {  /* Read character by character */
            if (rect.ReadDraw == 0) {               /* Anything to draw? */
                continue;
            }
            rect.ReadDraw--;                        /* Decrease number of drawn elements */
            
            if (x > disp->X2) {                     /* Check if X over line */
                continue;
            }
            
            ch = get_char_from_value(ch);           /* Get char from char value */
            if ((c = string_get_char_ptr(font, ch)) == 0) { /* Get character pointer */
                continue;                           /* Character is not known */
            }
            draw_char(disp, font, draw, x, y, c);   /* Draw actual char */
            
            x += c->x_size + c->x_margin;           /* Increase X position */
        }
        y += draw->LineHeight;                      /* Go to next line */
        if (!(draw->Flags & GUI_FLAG_FONT_MULTILINE) || y > disp->Y2) { /* Not multiline or over visible Y area */
            break;
        }
    }
}

/**
 * \brief           Initializes \ref GUI_DRAW_SB_t structure for drawing operations
 * \param[in]       sb: Pointer to \ref GUI_DRAW_SB_t to initialize to default values 
 */
void
gui_draw_scrollbar_init(GUI_DRAW_SB_t* sb) {
    memset(sb, 0x00, sizeof(*sb));                  /* Reset structure */
}

/**
 * \brief           Draw scroll bar to screen
 * \param[in,out]   *disp: Pointer to \ref gui_display_t structure for display operations
 * \param[in]       sb: Pointer to \ref GUI_DRAW_SB_t parameters for scroll bar 
 */
void
gui_draw_scrollbar(const gui_display_t* disp, GUI_DRAW_SB_t* sb) {
    gui_dim_t btnW, btnH, midHeight, rectHeight, midOffset = 0;

    btnW = sb->Width;
    btnH = (sb->Width << 1) / 3;
    
    /* Top box */
    gui_draw_rectangle3d(disp, sb->X, sb->Y, btnW, btnH, GUI_DRAW_3D_State_Raised);
    gui_draw_filledrectangle(disp, sb->X + 2, sb->Y + 2, btnW - 4, btnH - 4, GUI_COLOR_WIN_MIDDLEGRAY);
    
    /* Bottom box */
    gui_draw_rectangle3d(disp, sb->X, sb->Y + sb->Height - btnH, btnW, btnH, GUI_DRAW_3D_State_Raised);
    gui_draw_filledrectangle(disp, sb->X + 2, sb->Y + sb->Height - btnH + 2, btnW - 4, btnH - 4, GUI_COLOR_WIN_MIDDLEGRAY);
    
    /* Middle part */
    midHeight = (sb->Height - 2U * btnH);           /* Calculate middle rectangle part */
    gui_draw_filledrectangle(disp, sb->X, sb->Y + btnH, sb->Width, midHeight, GUI_COLOR_WIN_MIDDLEGRAY);
    
    /* Calculate size and offset for middle part */
    if (sb->EntriesVisible < sb->EntriesTotal) {    /* More entries than available visual space */
        rectHeight = midHeight * sb->EntriesVisible / sb->EntriesTotal; /* Entire area for drawing middle part */
        if (rectHeight < 6) {                       /* Calculate middle height */
            rectHeight = 6;
        }
        midOffset = (midHeight - rectHeight) * sb->EntriesTop / (sb->EntriesTotal - sb->EntriesVisible);
    } else {
        rectHeight = midHeight;
    }
    gui_draw_rectangle3d(disp, sb->X, sb->Y + btnH + midOffset, sb->Width, rectHeight, GUI_DRAW_3D_State_Raised); 
}

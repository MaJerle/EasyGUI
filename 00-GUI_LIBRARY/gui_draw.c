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
#include "gui_draw.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct GUI_StringRect_t {
    size_t Lines;
    GUI_iDim_t Width;
    GUI_iDim_t Height;
    
    size_t IgnoreCount;                             /*!< Number of characters to ignore before start with line drawing */
    
    GUI_DRAW_FONT_t* StringDraw;                    /*!< Pointer to object to draw string */
    const GUI_FONT_t* Font;
} GUI_StringRect_t;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define __GetCharFromValue(ch)      (uint32_t)(((uint8_t)'\r' == (uint8_t)(ch) || (uint8_t)'\n' == (uint8_t)(ch)) ? (uint32_t)' ' : (ch))

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
/* Get character from font object array */
static
const GUI_FONT_CharInfo_t* __StringGetCharPtr(const GUI_FONT_t* font, uint32_t ch) {
    ch = __GetCharFromValue(ch);                    /* Get char from char value */
    if (ch >= font->StartChar && ch <= font->EndChar) { /* Character is in font structure */
        return &font->Data[(ch) - font->StartChar]; /* Return character pointer from font */
    } else if ('?' >= font->StartChar && '?' <= font->EndChar) {    /* Try to return ? character */
        return &font->Data[(uint32_t)'?' - font->StartChar];    /* Get pointer of ? character */
    }
    return 0;                                       /* No character in font */
}

/* Get dimensions for input character */
static
void __StringGetCharSize(const GUI_FONT_t* font, uint32_t ch, GUI_iDim_t* width, GUI_iDim_t* height) {
    const GUI_FONT_CharInfo_t* c = 0;
    
    c = __StringGetCharPtr(font, ch);               /* Get character from font */
    if (c) {
        *width = c->xSize + c->xMargin;
        *height = c->ySize;
    } else {
        *width = 0;
        *height = 0;
    }
}

/* Get string rectangle width and height */
#define RECT_CONTINUE()     if (1) {                \
    var.cnt++;                                      \
    var.lastCh = var.ch;                            \
    continue;                                       \
}

static
size_t __StringRectangle(GUI_StringRect_t* rect, const GUI_Char* str, uint8_t onlyToNextLine) {
    GUI_iDim_t w, h, mW = 0, tH = 0;                /* Maximal width and total height */
    uint8_t i;
    const GUI_Char* s = str;
    struct vars {
        uint32_t ch, lastCh;                        /* Current and previous characters */
        GUI_iDim_t cW;                              /* Current line width */
        size_t cnt;                                 /* Current count in line */
        GUI_iDim_t widthAfterLastSpace;             /* Width of characters after last space detected */
        GUI_iDim_t widthOfSpace;                    /* Width of last space sequence */
        size_t lastSpaceIndex;                      /* Index number of last space sequence start */
        size_t lastSpaceCount;                      /* Number of spaces in last sequence */
        size_t fromLastSpaceIndex;                  /* Index number of non-space sequence start */
        size_t fromLastSpaceCount;                  /* Number of non-space characters in sequence */
        uint8_t isLineFeed;                         /* Status indicating character is line feed */
        uint8_t final;                              /* Status indicating we should do line check and finish */
    } var;
    memset(&var, 0x00, sizeof(var));                /* Reset structure */
        
    rect->IgnoreCount = 0;                          /* Reset ignore counter in this function */
    tH = rect->StringDraw->LineHeight;
    if (rect->StringDraw->Flags & GUI_FLAG_FONT_MULTILINE) {    /* We want to know exact rectangle for drawing multi line texts including new lines and carriage return */
        while (GUI_STRING_GetCh(&s, &var.ch, &i)) {     /* Get next character from string */
            if ((uint8_t)'\r' == (uint8_t)var.ch) { /* Check character */
                var.ch = ' ';                       /* Set it as space and don't care for more */
            }
            
            var.isLineFeed = 0;
            if ((uint8_t)'\n' == (uint8_t)var.ch) { /* LF is for new line */
                var.isLineFeed = 1;                 /* Character is line feed */
                var.ch = ' ';                       /* Set it as space */
            }
            
            /**
             * Ignore all white spaces on beginning of line
             */
            if (!var.isLineFeed && (uint8_t)' ' == (uint8_t)var.ch) {                
                var.widthAfterLastSpace = 0;        /* Reset text width after space received again */
                
                if ((uint8_t)' ' != (uint8_t)var.lastCh) {
                    var.lastSpaceIndex = var.cnt;   /* Set index when last spaces started */
                    var.lastSpaceCount = 0;         /* Reset space count */
                    var.widthOfSpace = 0;           /* Reset effective width of space */
                }
                var.lastSpaceCount++;               /* Increase number of spaces in row */
                
                /* Ignore all white spaces on start of line */
                if (var.lastSpaceIndex == 0) {      /* Character is for ignore purpose */
                    if (!var.isLineFeed) {          /* If line feed is first, do not ignore it */
                        rect->IgnoreCount++;
                        RECT_CONTINUE();            /* Continue to next character */
                    }
                }
            } else if (!var.isLineFeed) {
                if ((uint8_t)' ' == (uint8_t)var.lastCh) {  /* If last one was space but this one is not */
                    var.fromLastSpaceIndex = var.cnt;   /* Save index where non-space starts */
                    var.fromLastSpaceCount = 0;     /* Reset number of characters after last space */
                }
                var.fromLastSpaceCount++;           /* Increase number of chararacters after last space */
            }
            
            var.final = 0;
            if (var.isLineFeed) {                   
                var.final = 1;
            } else {
                __StringGetCharSize(rect->Font, var.ch, &w, &h);  /* Get character width and height */
                if (var.cW + w < rect->StringDraw->Width) { /* Check if we forced space and should stop execution or in range for new character */
                    var.cW += w;                    /* Increase X position */
                    if ((uint8_t)' ' == (uint8_t)var.ch) {  /* Check for space */
                        var.widthOfSpace += w;      /* Increase width of spaces together in a row */
                    } else {                        /* Any other element */
                        var.widthAfterLastSpace += w;   /* Increase width of text after last space */
                    }
                } else {
                    var.final = 1;                  /* Character did not pass to array, line is over */
                }
            }
                
            /**
             * Finish the drawing line of text
             */
            if (var.final) {                        /* This character does not fit in rectangle anymore */
                /* If space is the last element */
                if (var.isLineFeed) {
                    /* In case we have spaces and then new line */
                    var.cnt++;
                } else {
                    if (var.lastSpaceIndex > var.fromLastSpaceIndex) {  /* Spaces are last element */
                        if (var.lastSpaceCount) {
                            var.cnt -= var.lastSpaceCount - 1;  /* Ignore last spaces */
                            var.cW -= var.widthOfSpace; /* Decrease by width of spaces */
                        }
                    } else {                        /* Word is last element */
                        if (var.fromLastSpaceCount) {
                            var.cnt -= var.fromLastSpaceCount - 1;
                            var.cW -= var.widthAfterLastSpace;
                        }
                        
                        if ((var.lastSpaceIndex + var.lastSpaceCount) == var.fromLastSpaceIndex) {
                            var.cnt -= var.lastSpaceCount;
                            var.cW -= var.widthOfSpace;
                        }
                    }
                }
                
                if (mW < var.cW) {                  /* Current width check */
                    mW = var.cW;                    /* Save as max width currently */
                }
                if (onlyToNextLine) {               /* Check only until next line detected */
                    break;
                }
                
                /**
                 * Reset everything
                 */
                memset(&var, 0x00, sizeof(var));    /* Reset all variables and act like new function call */
                tH += rect->StringDraw->LineHeight; /* Increase line height */
                continue;
            }
            RECT_CONTINUE();                        /* Proceed to next character */
        }
        if (mW < var.cW) {                          /* Current width check */
            mW = var.cW;                            /* Save as max width currently */
        }
        rect->Width = mW;                           /* Save rectangle width */
    } else {
        var.cW = 0;
        while (GUI_STRING_GetCh(&s, &var.ch, &i)) { /* Get next character from string */
            __StringGetCharSize(rect->Font, var.ch, &w, &h);/* Get character width and height */
            if (!(rect->StringDraw->Flags & GUI_FLAG_FONT_RIGHTALIGN) && (var.cW + w) > rect->StringDraw->Width) {  /* Check if end now */
                break;
            }
            if ((uint8_t)'\r' != (uint8_t)var.ch && (uint8_t)'\n' != (uint8_t)var.ch) {
                var.cW += w;                        /* Increase width */
            }
            var.cnt++;                              /* Increase number of characters to read */
        }
        rect->Width = var.cW;                       /* Save width value */
    }
    rect->Height = tH;                              /* Save height value */
    return var.cnt;                                 /* Return number of characters processed */
}

/* Get font char object from memory with alpha values */
static
GUI_FONT_CharEntry_t* __GetCharEntryFromFont(const GUI_FONT_t* font, const GUI_FONT_CharInfo_t* c) {
    GUI_FONT_CharEntry_t* entry;
    for (entry = (GUI_FONT_CharEntry_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&GUI.RootFonts, 0); entry;
        entry = (GUI_FONT_CharEntry_t *)__GUI_LINKEDLIST_GETNEXT_GEN(0, (GUI_LinkedList_t *)entry)) {
        if (entry->Font == font && entry->Ch == c) {
            return entry;
        }
    }
    return 0;
}

/* Create char and put it to RAM for fast drawing with memory to memory copy */
static
GUI_FONT_CharEntry_t* __CreateCharEntryFromFont(const GUI_FONT_t* font, const GUI_FONT_CharInfo_t* c) {
    GUI_FONT_CharEntry_t* entry = NULL;
    uint16_t columns;
    uint16_t memsize = sizeof(*entry);              /* Get size of entry */
    uint16_t memDataSize;
    
    /* Calculate memory size for data */
    memDataSize = c->xSize * c->ySize;
    
    memsize += memDataSize;
    entry = (GUI_FONT_CharEntry_t *)__GUI_MEMALLOC(memsize);    /* Allocate memory for entry */
    if (entry) {                                    /* Allocation was successful */
        uint16_t i, x;
        uint8_t b, k, t;
        uint8_t* ptr = (uint8_t *)entry;            /* Go to memory size */
        ptr += sizeof(*entry);                      /* Go to start of data */
        
        entry->Ch = c;                              /* Set pointer to character */
        entry->Font = font;                         /* Set pointer to font structure */
        
        if (font->Flags & GUI_FLAG_FONT_AA) {       /* Anti-alliased font */
            columns = c->xSize >> 2;                /* Calculate number of bytes used for single character line */
            if (c->xSize % 4) {                     /* If only 1 column used */
                columns++;
            }
            x = 0;
            for (i = 0; i < c->ySize * columns; i++) {  /* Inspect all vertical lines */
                b = c->Data[i];                     /* Get byte of data */
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
                    if (x == c->xSize) {
                        x = 0;
                        break;
                    }
                }
            }
        } else {
            columns = c->xSize >> 3;                /* Calculate number of bytes used for single character line */
            if (c->xSize % 8) {                     /* If only 1 column used */
                columns++;
            }
            x = 0;
            for (i = 0; i < c->ySize * columns; i++) {  /* Inspect all vertical lines */
                b = c->Data[i];                     /* Get byte of data */
                for (k = 0; k < 8; k++) {           /* Scan each bit in byte */
                    if ((b >> (7 - k)) & 0x01) {
                        *ptr++ = 0xFF;
                    } else {
                        *ptr++ = 0x00;
                    }
                    x++;
                    if (x == c->xSize) {
                        x = 0;
                        break;
                    }
                }
            }
        }
        
        __GUI_LINKEDLIST_ADD_GEN(&GUI.RootFonts, (GUI_LinkedList_t *)entry);    /* Add entry to linked list */
    }
    return entry;                                   /* Return new created entry */
}

/* Draw character to screen */
/* X and Y coordinates are TOP LEFT coordinates for character */
static
void __DRAW_Char(const GUI_Display_t* disp, const GUI_FONT_t* font, const GUI_DRAW_FONT_t* draw, GUI_iDim_t x, GUI_iDim_t y, const GUI_FONT_CharInfo_t* c) {
    GUI_Byte i, b;
    GUI_iDim_t x1;
    GUI_iByte k;
    GUI_Byte columns;
    
    while (!GUI.LL.IsReady(&GUI.LCD));              /* Wait till ready */
    
    y += c->yPos;                                   /* Set Y position */
    
    if (!__GUI_RECT_MATCH(
        disp->X1, disp->Y1, disp->X2, disp->Y2,
        x, y, x + c->xSize, y + c->ySize
    )) {
        return;
    }
    
    if (GUI.LL.CopyChar) {                          /* If copying character function exists in low-level part */
        GUI_FONT_CharEntry_t* entry = NULL;
        
        entry = __GetCharEntryFromFont(font, c);    /* Get char entry from font and character for fast alpha drawing operations */
        if (!entry) {
            entry = __CreateCharEntryFromFont(font, c); /* Create new entry */
        }
        if (entry) {                                /* We have valid data */
            GUI_Dim_t width, height, offlineSrc, offlineDst;
            uint8_t* dst = 0;
            uint8_t* ptr = (uint8_t *)entry;        /* Get pointer */
            GUI_Dim_t tmpX;
            
            tmpX = x;                               /* Start X */
            
            ptr += sizeof(*entry);                  /* Go to start of data array */
            dst = (uint8_t *)(GUI.LCD.DrawingLayer->StartAddress + ((y - GUI.LCD.DrawingLayer->OffsetY) * GUI.LCD.DrawingLayer->Width + (x - GUI.LCD.DrawingLayer->OffsetX)) * GUI.LCD.PixelSize);
            
            width = c->xSize;                       /* Get X size */
            height = c->ySize;                      /* Get Y size */
            
            if (y < disp->Y1) {                     /* Start Y position if outside visible area */
                ptr += (disp->Y1 - y) * c->xSize;   /* Set offset for number of lines */
                dst += (disp->Y1 - y) * GUI.LCD.DrawingLayer->Width * GUI.LCD.PixelSize;  /* Set offset for number of LCD lines */
                height -= disp->Y1 - y;             /* Decrease effective height */
            }
            if ((y + c->ySize) > disp->Y2) {
                height -= y + c->ySize - disp->Y2;  /* Decrease effective height */
            }
            if (x < disp->X1) {                     /* Set offset start address if required */
                ptr += (disp->X1 - x);              /* Set offset of start address in X direction */
                dst += (disp->X1 - x) * GUI.LCD.PixelSize;  /* Set offset of start address in X direction */
                width -= disp->X1 - x;              /* Increase source offline */
                tmpX += disp->X1 - x;               /* Increase effective start X position */
            }
            if ((x + c->xSize) > disp->X2) {
                width -= x + c->xSize - disp->X2;   /* Decrease effective width */
            }
            
            offlineSrc = c->xSize - width;          /* Set offline source */
            offlineDst = GUI.LCD.DrawingLayer->Width - width;   /* Set offline destination */
            
            /**
             * Check if character must be drawn with 2 colors, on the middle of color switch
             */
            if (tmpX < (draw->X + draw->Color1Width) && (tmpX + width) > (draw->X + draw->Color1Width)) {
                GUI_Dim_t firstWidth = (draw->X + draw->Color1Width) - tmpX;
                
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
    
    if (font->Flags & GUI_FLAG_FONT_AA) {           /* Font has anti alliasing enabled */
        GUI_Color_t color;                          /* Temporary color for AA */
        GUI_Byte tmp, r1, g1, b1;
        
        columns = c->xSize / 4;                     /* Calculate number of bytes used for single character line */
        if (c->xSize % 4) {                         /* If only 1 column used */
            columns++;
        }
        
        for (i = 0; i < columns * c->ySize; i++) {  /* Go through all data bytes */
            if (y >= disp->Y1 && y <= disp->Y2 && y < (draw->Y + draw->Height)) {   /* Do not draw when we are outside clipping are */            
                b = c->Data[i];                     /* Get character byte */
                for (k = 0; k < 4; k++) {           /* Scan each bit in byte */
                    GUI_Color_t baseColor;
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
                        GUI_DRAW_SetPixel(disp, x1, y, baseColor);
                    } else if (tmp) {               /* Calculate new color */
                        float t = (float)tmp / 3.0f;
                        color = GUI_DRAW_GetPixel(disp, x1, y); /* Read current color */
                        
                        /* Calculate new values for pixel */
                        r1 = (float)t * (float)((color >> 16) & 0xFF) + (float)(1.0f - (float)t) * (float)((baseColor >> 16) & 0xFF);
                        g1 = (float)t * (float)((color >>  8) & 0xFF) + (float)(1.0f - (float)t) * (float)((baseColor >>  8) & 0xFF);
                        b1 = (float)t * (float)((color >>  0) & 0xFF) + (float)(1.0f - (float)t) * (float)((baseColor >>  0) & 0xFF);
                        
                        /* Draw actual pixel to screen */
                        GUI_DRAW_SetPixel(disp, x1, y, (color & 0xFF000000UL) | r1 << 16 | g1 << 8 | b1);
                    }
                }
            }
            if ((i % columns) == (columns - 1)) {   /* Check for next line */
                y++;
            }
        }
    } else {
        columns = c->xSize / 8;
        if (c->xSize % 8) {
            columns++;
        }
        for (i = 0; i < columns * c->ySize; i++) {  /* Go through all data bytes */
            if (y >= disp->Y1 && y <= disp->Y2 && y < (draw->Y + draw->Height)) {   /* Do not draw when we are outside clipping are */
                b = c->Data[i];                     /* Get character byte */
                for (k = 0; k < 8; k++) {           /* Scan each bit in byte */
                    if (b & (1 << (7 - k))) {       /* If bit is set, draw pixel */
                        x1 = x + (i % columns) * 8 + k; /* Get new X value for pixel draw */
                        if (x1 < disp->X1 || x1 > disp->X2) {
                            continue;
                        }
                        if (x1 <= (draw->X + draw->Color1Width)) {
                            GUI_DRAW_SetPixel(disp, x1, y, draw->Color1);
                        } else {
                            GUI_DRAW_SetPixel(disp, x1, y, draw->Color2);
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
static
const GUI_Char* __StringGetPointerForWidth(const GUI_FONT_t* font, const GUI_Char* str, GUI_DRAW_FONT_t* draw) {
    const GUI_Char* tmp = str;
    GUI_iDim_t tot = 0, w, h;
    uint8_t i;
    uint32_t ch;
    
    while (*tmp) {                                  /* Go to the end of string */
        tmp++;
    }
    tmp--;                                          /* Go to the last character */
    while (str <= tmp) {
        if (!GUI_STRING_GetChReverse(&tmp, &ch, &i)) {  /* Get character in reverse order */
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
    return tmp + i + 1;
}

/******************************************************************************/
/******************************************************************************/
/***                              Protothreads                               **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
void GUI_DRAW_FONT_Init(GUI_DRAW_FONT_t* f) {
    memset((void *)f, 0x00, sizeof(*f));            /* Reset structure */
}

/******************************************************************************/
/******************************************************************************/
/***                    Functions with low-level communication               **/
/******************************************************************************/
/******************************************************************************/
void GUI_DRAW_FillScreen(const GUI_Display_t* disp, GUI_Color_t color) {
    GUI.LL.Fill(&GUI.LCD, GUI.LCD.DrawingLayer, 0, GUI.LCD.DrawingLayer->Width, GUI.LCD.DrawingLayer->Height, 0, color);
}

void GUI_DRAW_Fill(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_Color_t color) {
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

void GUI_DRAW_SetPixel(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_Color_t color) {
    if (y < disp->Y1 || y >= disp->Y2 || x < disp->X1 || x >= disp->X2) {
        return;
    }
    GUI.LL.SetPixel(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY, color);
}

GUI_Color_t GUI_DRAW_GetPixel(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y) {
    return GUI.LL.GetPixel(&GUI.LCD, GUI.LCD.DrawingLayer, x - GUI.LCD.DrawingLayer->OffsetX, y - GUI.LCD.DrawingLayer->OffsetY);
}

void GUI_DRAW_VLine(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t length, GUI_Color_t color) {
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

void GUI_DRAW_HLine(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t length, GUI_Color_t color) {
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
void GUI_DRAW_Line(const GUI_Display_t* disp, GUI_iDim_t x1, GUI_iDim_t y1, GUI_iDim_t x2, GUI_iDim_t y2, GUI_Color_t color) {
    GUI_iDim_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
    curpixel = 0;
    
    /* Check if coordinates are inside drawing region */
//    if (                                            /* Check if redraw is inside area */
//        !__GUI_RECT_MATCH(  x1, y1, x2, y2,
//                            disp->X1, disp->Y1, disp->X2, disp->Y2)) {
//        return;
//    }

	deltax = __GUI_ABS(x2 - x1);
	deltay = __GUI_ABS(y2 - y1);
    
    if (deltax == 0) {                              /* Straight vertical line */
        GUI_DRAW_VLine(disp, x1, __GUI_MIN(y1, y2), deltay, color);
        return;
    }
    if (deltay == 0) {                              /* Straight horizontal line */
        GUI_DRAW_HLine(disp, __GUI_MIN(x1, x2), y1, deltax, color);
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
        GUI_DRAW_SetPixel(disp, x, y, color);
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

void GUI_DRAW_Rectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_Color_t color) {
    if (width == 0 || height == 0) {
        return;
    }
    GUI_DRAW_HLine(disp, x,             y,              width,  color);
    GUI_DRAW_VLine(disp, x,             y,              height, color);
    
    GUI_DRAW_HLine(disp, x,             y + height - 1, width,  color);
    GUI_DRAW_VLine(disp, x + width - 1, y,              height, color);
}

void GUI_DRAW_FilledRectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_Color_t color) {
    GUI_DRAW_Fill(disp, x, y, width, height, color);
}

void GUI_DRAW_Rectangle3D(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_DRAW_3D_State_t state) {
    GUI_Color_t c1, c2, c3;
    
    c1 = GUI_COLOR_BLACK;
    if (state == GUI_DRAW_3D_State_Raised) {
        c2 = 0xFFAAAAAA;
        c3 = 0xFF555555;
    } else {
        c2 = 0xFF555555;
        c3 = 0xFFAAAAAA;
    }
    
    GUI_DRAW_Rectangle(disp, x, y, width, height, c1);
    
    GUI_DRAW_HLine(disp, x + 1, y + 1, width - 2, c2);
    GUI_DRAW_VLine(disp, x + 1, y + 1, height - 3, c2);
    
    GUI_DRAW_HLine(disp, x + 1, y + height - 2, width - 2, c3);
    GUI_DRAW_VLine(disp, x + width - 2, y + 2, height - 4, c3);
}

void GUI_DRAW_RoundedRectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_iDim_t r, GUI_Color_t color) {
    if (r >= (height / 2)) {
        r = height / 2 - 1;
    }
    if (r >= (width / 2)) {
        r = width / 2 - 1;
    }
    if (r) {
        GUI_DRAW_HLine(disp, x + r,         y,              width - 2 * r,  color);
        GUI_DRAW_VLine(disp, x + width - 1, y + r,          height - 2 * r, color);
        GUI_DRAW_HLine(disp, x + r,         y + height - 1, width - 2 * r,  color);
        GUI_DRAW_VLine(disp, x,             y + r,          height - 2 * r, color);
        
        GUI_DRAW_CircleCorner(disp, x + r,             y + r,              r, GUI_DRAW_CIRCLE_TL, color);
        GUI_DRAW_CircleCorner(disp, x + width - r - 1, y + r,              r, GUI_DRAW_CIRCLE_TR, color);
        GUI_DRAW_CircleCorner(disp, x + r,             y + height - r - 1, r, GUI_DRAW_CIRCLE_BL, color);
        GUI_DRAW_CircleCorner(disp, x + width - r - 1, y + height - r - 1, r, GUI_DRAW_CIRCLE_BR, color);
    } else {
        GUI_DRAW_Rectangle(disp, x, y, width, height, color);
    }
}

void GUI_DRAW_FilledRoundedRectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_iDim_t r, GUI_Color_t color) {
    if (r >= (height / 2)) {
        r = height / 2 - 1;
    }
    if (r >= (width / 2)) {
        r = width / 2 - 1;
    }
    if (r) {
        if ((GUI_iDim_t)(width - 2 * r) > 0) {
            GUI_DRAW_FilledRectangle(disp, x + r,         y,     width - 2 * r, height,         color);
        }
        if ((GUI_iDim_t)(height - 2 * r) > 0) {
            GUI_DRAW_FilledRectangle(disp, x,             y + r, r,             height - 2 * r, color);
            GUI_DRAW_FilledRectangle(disp, x + width - r, y + r, r,             height - 2 * r, color);
        }
        
        GUI_DRAW_FilledCircleCorner(disp, x + r,             y + r,              r, GUI_DRAW_CIRCLE_TL, color);
        GUI_DRAW_FilledCircleCorner(disp, x + width - r - 1, y + r,              r, GUI_DRAW_CIRCLE_TR, color);
        GUI_DRAW_FilledCircleCorner(disp, x + r,             y + height - r - 1, r, GUI_DRAW_CIRCLE_BL, color);
        GUI_DRAW_FilledCircleCorner(disp, x + width - r - 1, y + height - r - 1, r, GUI_DRAW_CIRCLE_BR, color);
    } else {
        GUI_DRAW_FilledRectangle(disp, x, y, width, height, color);
    }
}

void GUI_DRAW_Circle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t r, GUI_Color_t color) {
    GUI_DRAW_CircleCorner(disp, x, y, r, GUI_DRAW_CIRCLE_TL, color);
    GUI_DRAW_CircleCorner(disp, x - 1, y, r, GUI_DRAW_CIRCLE_TR, color);
    GUI_DRAW_CircleCorner(disp, x, y - 1, r, GUI_DRAW_CIRCLE_BL, color);
    GUI_DRAW_CircleCorner(disp, x - 1, y - 1, r, GUI_DRAW_CIRCLE_BR, color);
}

void GUI_DRAW_FilledCircle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t r, GUI_Color_t color) {
    GUI_DRAW_FilledCircleCorner(disp, x, y, r, GUI_DRAW_CIRCLE_TL, color);
    GUI_DRAW_FilledCircleCorner(disp, x, y, r, GUI_DRAW_CIRCLE_TR, color);
    GUI_DRAW_FilledCircleCorner(disp, x, y - 1, r, GUI_DRAW_CIRCLE_BL, color);
    GUI_DRAW_FilledCircleCorner(disp, x, y - 1, r, GUI_DRAW_CIRCLE_BR, color);
}

void GUI_DRAW_Triangle(const GUI_Display_t* disp, GUI_iDim_t x1, GUI_iDim_t y1,  GUI_iDim_t x2, GUI_iDim_t y2, GUI_iDim_t x3, GUI_iDim_t y3, GUI_Color_t color) {
    GUI_DRAW_Line(disp, x1, y1, x2, y2, color);
    GUI_DRAW_Line(disp, x1, y1, x3, y3, color);
    GUI_DRAW_Line(disp, x2, y2, x3, y3, color);
}

void GUI_DRAW_FilledTriangle(const GUI_Display_t* disp, GUI_iDim_t x1, GUI_iDim_t y1, GUI_iDim_t x2, GUI_iDim_t y2, GUI_iDim_t x3, GUI_iDim_t y3, GUI_Color_t color) {
    GUI_iDim_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
    curpixel = 0;

    deltax = __GUI_ABS(x2 - x1);
    deltay = __GUI_ABS(y2 - y1);
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
        GUI_DRAW_Line(disp, x, y, x3, y3, color);

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

void GUI_DRAW_CircleCorner(const GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Byte_t c, GUI_Color_t color) {
    GUI_iDim_t f = 1 - r;
    GUI_iDim_t ddF_x = 1;
    GUI_iDim_t ddF_y = -2 * r;
    GUI_iDim_t x = 0;
    GUI_iDim_t y = r;
    
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
            GUI_DRAW_SetPixel(disp, x0 - y, y0 - x, color);
            GUI_DRAW_SetPixel(disp, x0 - x, y0 - y, color);
        }

        if (c & GUI_DRAW_CIRCLE_TR) {               /* Top right corner */
            GUI_DRAW_SetPixel(disp, x0 + x, y0 - y, color);
            GUI_DRAW_SetPixel(disp, x0 + y, y0 - x, color);
        }

        if (c & GUI_DRAW_CIRCLE_BR) {               /* Bottom right corner */
            GUI_DRAW_SetPixel(disp, x0 + x, y0 + y, color);
            GUI_DRAW_SetPixel(disp, x0 + y, y0 + x, color);
        }

        if (c & GUI_DRAW_CIRCLE_BL) {               /* Bottom left corner */
            GUI_DRAW_SetPixel(disp, x0 - x, y0 + y, color);
            GUI_DRAW_SetPixel(disp, x0 - y, y0 + x, color);
        }
    }
}

void GUI_DRAW_FilledCircleCorner(const GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Byte_t c, GUI_Color_t color) {
    GUI_iDim_t f = 1 - r;
    GUI_iDim_t ddF_x = 1;
    GUI_iDim_t ddF_y = -2 * r;
    GUI_iDim_t x = 0;
    GUI_iDim_t y = r;
    
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
            GUI_DRAW_HLine(disp, x0 - x, y0 - y, x, color);
            GUI_DRAW_HLine(disp, x0 - y, y0 - x, y, color);
        }
        if (c & GUI_DRAW_CIRCLE_TR) {               /* Top right */
            GUI_DRAW_HLine(disp, x0, y0 - y, x, color);
            GUI_DRAW_HLine(disp, x0, y0 - x, y, color);
        }
        if (c & GUI_DRAW_CIRCLE_BL) {               /* Bottom left */
            GUI_DRAW_HLine(disp, x0 - y, y0 + x, y, color);
            GUI_DRAW_HLine(disp, x0 - x, y0 + y, x, color);
        }
        if (c & GUI_DRAW_CIRCLE_BR) {               /* Bottom right */
            GUI_DRAW_HLine(disp, x0, y0 + x, y, color);
            GUI_DRAW_HLine(disp, x0, y0 + y, x, color);
        }
    }
}

void GUI_DRAW_Image(GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, const GUI_IMAGE_DESC_t* img) {
    uint8_t bytes = img->BPP >> 3;                  /* Get number of bytes per pixel on image */
    
    GUI_Layer_t* layer;
    const uint8_t* src;
    const uint8_t* dst;
    GUI_iDim_t width, height;
    GUI_iDim_t offlineSrc, offlineDst;
    
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

void GUI_DRAW_Poly(const GUI_Display_t* disp, const GUI_DRAW_Poly_t* points, size_t len, GUI_Color_t color) {
    GUI_iDim_t x = 0, y = 0;

    if (len < 2) {
        return;
    }

    GUI_DRAW_Line(disp, points->X, points->Y, (points + len - 1)->X, (points + len - 1)->Y, color);

    while (--len) {
        x = points->X;
        y = points->Y;
        points++;
        GUI_DRAW_Line(disp, x, y, points->X, points->Y, color);
    }
}

void GUI_DRAW_WriteText(const GUI_Display_t* disp, const GUI_FONT_t* font, const GUI_Char* str, GUI_DRAW_FONT_t* draw) {
    GUI_iDim_t x, y;
    uint32_t ch;
    uint8_t i;
    size_t cnt;
    const GUI_FONT_CharInfo_t* c;
    GUI_StringRect_t rect = {0};                    /* Get string object */
    
    if (!draw->LineHeight) {                        /* When line height is not set */
        draw->LineHeight = font->Size;              /* Set font size */
    }
    
    rect.Font = font;
    rect.StringDraw = draw;
    
    __StringRectangle(&rect, str, 0);               /* Get string width for this box */
    if (rect.Width > draw->Width) {                 /* If string is wider than available rectangle */
        if (draw->Flags & GUI_FLAG_FONT_RIGHTALIGN) {   /* Check right align text */
            str = __StringGetPointerForWidth(font, str, draw);
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
    
//    GUI_DRAW_Rectangle(disp, x, y, rect.Width, rect.Height, GUI_COLOR_BLUE);
    
    while ((cnt = __StringRectangle(&rect, str, 1)) > 0) {
        x = draw->X;                                       
        if (draw->Align & GUI_HALIGN_CENTER) {      /* Check for horizontal align center */
            x += (draw->Width - rect.Width) / 2;    /* Align center of drawing area */
        } else if (draw->Align & GUI_HALIGN_RIGHT) {/* Check for horizontal align right */
            x += draw->Width - rect.Width;          /* Align right of drawing area */
        }
//        GUI_DRAW_Rectangle(disp, x, y, rect.Width, rect.Height, GUI_COLOR_RED);
        while (cnt-- && GUI_STRING_GetCh(&str, &ch, &i)) {  
            if (rect.IgnoreCount) {                 /* Check if we have to ignore some characters on beginning of string */
                rect.IgnoreCount--;                 /* Proceed with ignore */
                continue;                           /* Continue with reading next character */
            }
            if (y > disp->Y2) {                     /* Check if Y over line */
                goto gui_draw_char_end;
            }
            if (x > disp->X2) {                     /* Check if X over line */
                continue;
            }
            
            ch = __GetCharFromValue(ch);            /* Get char from char value */
            if ((c = __StringGetCharPtr(font, ch)) == 0) {  /* Get character pointer */
                continue;                           /* Character is not known */
            }
            __DRAW_Char(disp, font, draw, x, y, c); /* Draw actual char */
            
            x += c->xSize + c->xMargin;             /* Increase X position */
        }
        y += draw->LineHeight;                      /* Go to next line */
        if (!(draw->Flags & GUI_FLAG_FONT_MULTILINE)) { /* Draw only first line in non-multiline environment */
            break;
        }
    }
gui_draw_char_end:
    (void)0;
}

void GUI_DRAW_ScrollBar_init(GUI_DRAW_SB_t* sb) {
    memset(sb, 0x00, sizeof(*sb));                  /* Reset structure */
}

void GUI_DRAW_ScrollBar(const GUI_Display_t* disp, GUI_DRAW_SB_t* sb) {
    GUI_Dim_t btnW, btnH, midHeight, rectHeight, midOffset = 0;

    btnW = sb->Width;
    btnH = (sb->Width << 1) / 3;
    
    /* Top box */
    GUI_DRAW_Rectangle3D(disp, sb->X, sb->Y, btnW, btnH, GUI_DRAW_3D_State_Raised);
    GUI_DRAW_FilledRectangle(disp, sb->X + 2, sb->Y + 2, btnW - 4, btnH - 4, GUI_COLOR_WIN_MIDDLEGRAY);
    
    /* Bottom box */
    GUI_DRAW_Rectangle3D(disp, sb->X, sb->Y + sb->Height - btnH, btnW, btnH, GUI_DRAW_3D_State_Raised);
    GUI_DRAW_FilledRectangle(disp, sb->X + 2, sb->Y + sb->Height - btnH + 2, btnW - 4, btnH - 4, GUI_COLOR_WIN_MIDDLEGRAY);
    
    /* Middle part */
    midHeight = (sb->Height - 2U * btnH);           /* Calculate middle rectangle part */
    GUI_DRAW_FilledRectangle(disp, sb->X, sb->Y + btnH, sb->Width, midHeight, GUI_COLOR_WIN_MIDDLEGRAY);
    
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
    GUI_DRAW_Rectangle3D(disp, sb->X, sb->Y + btnH + midOffset, sb->Width, rectHeight, GUI_DRAW_3D_State_Raised); 
}

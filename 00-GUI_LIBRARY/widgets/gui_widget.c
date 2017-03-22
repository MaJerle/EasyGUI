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
#include "gui_widget.h"
#include "gui_window.h"

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
void __GUI_WIDGET_SetClippingRegion(GUI_HANDLE_t h) {
    GUI_Dim_t x, y;
    
    x = __GUI_WIDGET_GetAbsoluteX(h);         /* Get widget absolute X */
    y = __GUI_WIDGET_GetAbsoluteY(h);         /* Get widget absolute Y */
    
    /* Set invalid clipping region */
    if (GUI.Display.X1 > x) {
        GUI.Display.X1 = x;
    }
    if (GUI.Display.X2 < (x + h->Width)) {
        GUI.Display.X2 = (x + h->Width);
    }
    if (GUI.Display.Y1 > y) {
        GUI.Display.Y1 = y;
    }
    if (GUI.Display.Y2 < (y + h->Height)) {
        GUI.Display.Y2 = (y + h->Width);
    }
}

uint8_t __GUI_WIDGET_IsInsideClippingRegion(GUI_HANDLE_t h) {
    return __GUI_RECT_MATCH(__GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h), h->Width, h->Height, GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2 - GUI.Display.X1, GUI.Display.Y2 - GUI.Display.Y1);
}

void __GUI_WIDGET_Init(void) {
    GUI_WINDOW_Create(GUI_ID_WINDOW_BASE);          /* Create base window object */
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteX(GUI_HANDLE_t h) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_t w = 0;
    GUI_Dim_t out = 0;
    
    if (h) {
        w = h->Parent;
        out = h->X;
    }
    
    while (w) {                                     /* Go through all parent windows */
        out += w->X;                                /* Add X offset from parent */
        w = (GUI_HANDLE_t)w->Parent;                /* Get next parent */
    }
    return out;
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteY(GUI_HANDLE_t h) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_t w = 0;
    GUI_Dim_t out = 0;
    
    if (h) {
        w = ((GUI_HANDLE_t)h)->Parent;
        out = ((GUI_HANDLE_t)h)->Y;
    }
    
    while (w) {                                     /* Go through all parent windows */
        out += w->Y;                                /* Add X offset from parent */
        w = (GUI_HANDLE_t)w->Parent;                /* Get next parent */
    }
    return out;
}

uint8_t __GUI_WIDGET_Invalidate(GUI_HANDLE_t h) {
    GUI_HANDLE_t h1, h2;
    
    h1 = h;                                         /* Get widget handle */
    h1->Flags |= GUI_FLAG_REDRAW;                   /* Redraw widget */
    
    __GUI_WIDGET_SetClippingRegion(h);              /* Set clipping region for widget redrawing operation */
    
    /**
     * Invalid only widget with higher Z-index (lowered on linked list) of current object
     * 
     * If widget should be redrawn, then any widget above it should be redrawn too, otherwise z-index match will fail
     *
     * Widget may not need redraw operation if positions don't match
     */
    for (; h1; h1 = __GUI_LINKEDLIST_GetNextWidget(NULL, h1)) {
        for (h2 = __GUI_LINKEDLIST_GetNextWidget(NULL, h1); h2; h2 = __GUI_LINKEDLIST_GetNextWidget(NULL, h2)) {
            if (
                h2->Flags & GUI_FLAG_REDRAW ||  /* Bit is already set */
                !__GUI_RECT_MATCH(h1->X, h1->Y, h1->Width, h1->Height, h2->X, h2->Y, h2->Width, h2->Height)
            ) {
                continue;
            }
            h2->Flags |= GUI_FLAG_REDRAW;           /* Redraw widget on next loop */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_InvalidateWithParent(GUI_HANDLE_t h) {
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate object */
    if (h->Parent) {                                /* If parent exists, invalid only parent */
        h->Parent->Flags |= GUI_FLAG_REDRAW;        /* Redraw parent widget too */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y) {
    if (h->X != x || h->Y != y) {
        GUI_iDim_t pW, pH;
        pW = GUI_WIDGET_GetParentWidth(h);
        pH = GUI_WIDGET_GetParentHeight(h);
        
        if (x < 0) {                                /* Check X value */
            x = 0;
        } else if (x > (pW - h->Width)) {
            x = pW - h->Width;
        }
        if (y < 0) {                                /* Check Y value */
            y = 0;
        } else if (y > (pH - h->Height)) {
            y = pH - h->Height;
        }
        
        if (h->X != x || h->Y != y) {            
            __GUI_WIDGET_SetClippingRegion(h);      /* Set new clipping region */
            h->X = x;                               /* Set parameter */
            h->Y = y;                               /* Set parameter */
            __GUI_WIDGET_InvalidateWithParent(h);   /* Invalidate object */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height) {
    if (h->Width != width || h->Height != height) {
        GUI_Dim_t pW, pH;
            
        pW = GUI_WIDGET_GetParentWidth(h);          /* Get parent width */
        pH = GUI_WIDGET_GetParentHeight(h);         /* Get parent height */
        
        if (h->X + width > pW) {                    /* Check if widget width is too big */
            width = pW - h->X;
        }
        if (h->Y + height > pH) {                   /* Check if widget height is too big */
            height = pH - h->Y;
        }

        __GUI_WIDGET_SetClippingRegion(h);          /* Set clipping region before changed position */
        h->Width = width;                           /* Set parameter */
        h->Height = height;                         /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Enable3DStyle(GUI_HANDLE_t h) {
    if (!(h->Flags & GUI_FLAG_3D)) {
        h->Flags |= GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Disable3DStyle(GUI_HANDLE_t h) {
    if (h->Flags & GUI_FLAG_3D) {
        h->Flags &= ~GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font) {
    if (h->Font != font) {                          /* Any parameter changed */
        h->Font = font;                             /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetText(GUI_HANDLE_t h, const GUI_Char* text) {
   if (h->Flags & GUI_FLAG_DYNAMICTEXTALLOC) {      /* Memory for text is dynamically allocated */
        if (h->TextMemSize) {
            if (GUI_STRING_Length(text) > (h->TextMemSize - 1)) { /* Check string length */
                GUI_STRING_CopyN(h->Text, text, h->TextMemSize - 1);  /* Do not copy all bytes because of memory overflow */
            } else {
                GUI_STRING_Copy(h->Text, text);     /* Copy entire string */
            }
            __GUI_WIDGET_Invalidate(h);             /* Redraw object */
        }
    } else {                                        /* Memory allocated by user */
        if (h->Text && h->Text == text) {           /* In case the same pointer is passed to WIDGET */
            if (GUI_STRING_Compare(h->Text, text)) {/* If strings does not match, source string updated? */
                __GUI_WIDGET_Invalidate(h);         /* Redraw object */
            }
        }
        
        if (h->Text != text) {                      /* Check if pointer do not match */
            h->Text = (GUI_Char *)text;             /* Set parameter */
            __GUI_WIDGET_Invalidate(h);             /* Redraw object */
        }
    }
    h->TextCursor = GUI_STRING_LengthTotal(h->Text);/* Set cursor to the end of string */
    return 1;
}

uint8_t __GUI_WIDGET_AllocateTextMemory(GUI_HANDLE_t h, uint32_t size) {
    if ((h->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && h->Text) {    /* Check if already allocated */
        __GUI_MEMFREE(h->Text);                     /* Free memory first */
        h->TextMemSize = 0;                         /* Reset memory size */
    }
    h->Text = 0;                                    /* Reset pointer */
    
    h->TextMemSize = size * sizeof(GUI_Char);       /* Allocate text memory */
    h->Text = (GUI_Char *)__GUI_MEMALLOC(h->TextMemSize);   /* Allocate memory for text */
    if (h->Text) {                                  /* Check if allocated */
        h->Flags |= GUI_FLAG_DYNAMICTEXTALLOC;      /* Dynamically allocated */
    } else {
        h->TextMemSize = 0;                         /* No dynamic bytes available */
        h->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC;     /* Not allocated */
    }
    __GUI_WIDGET_Invalidate(h);                     /* Redraw object */
    return 1;
}

uint8_t __GUI_WIDGET_FreeTextMemory(GUI_HANDLE_t h) {
    if ((h->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && h->Text) {    /* Check if dynamically alocated */
        __GUI_MEMFREE(h->Text);                     /* Free memory first */
        h->Text = 0;                                /* Reset memory */
        h->TextMemSize = 0;                         /* Reset memory size */
        h->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC;     /* Not allocated */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_IsFontAndTextSet(GUI_HANDLE_t h) {
    return h->Text && h->Font && GUI_STRING_Length(h->Text);    /* Check if conditions for drawing string match */
}

uint8_t __GUI_WIDGET_ProcessTextKey(GUI_HANDLE_t h, GUI_KeyboardData_t* kb) {
    uint16_t len, tlen;
    uint32_t ch;
    uint8_t l;
    const GUI_Char* str = kb->Keys;
    
    if (!(h->Flags & GUI_FLAG_DYNAMICTEXTALLOC)) {  /* Must be dynamically allocated memory */
        return 0;
    }
    
    if (!GUI_STRING_GetCh(&str, &ch, &l)) {         /* Get key from input data */
        return 0;                                   /* Invalid input key */
    }
    
    tlen = GUI_STRING_LengthTotal(h->Text);         /* Get total length of string */
    len = GUI_STRING_Length(h->Text);               /* Get string length */
    if (ch >= 32 && ch != 127) {                    /* Printable character */
        if (len < (h->TextMemSize - l)) {           /* Memory still available for new character */
            uint16_t pos;
            for (pos = tlen + l - 1; pos > h->TextCursor; pos--) {  /* Shift characters down */
                h->Text[pos] = h->Text[pos - l];
            }
            for (pos = 0; pos < l; pos++) {         /* Fill new characters to empty memory */
                h->Text[h->TextCursor++] = kb->Keys[pos];
            }
            h->Text[tlen + l] = 0;                  /* Add 0 to the end */
            
            __GUI_WIDGET_Invalidate(h);
            return 1;
        }
    } else if (ch == 8 || ch == 127) {              /* Backspace character */
        if (tlen && h->TextCursor) {
            const GUI_Char* end = (GUI_Char *)((uint32_t)h->Text + h->TextCursor - 1);  /* End of string pointer */
            uint16_t pos;
            
            if (!GUI_STRING_GetChReverse(&end, &ch, &l)) {  /* Get last character */
                return 0;                           
            }
            for (pos = h->TextCursor - l; pos < (tlen - l); pos++) {    /* Shift characters up */
                h->Text[pos] = h->Text[pos + l];
            }
            h->TextCursor -= l;                     /* Decrease text cursor by number of bytes for character deleted */
            h->Text[tlen - l] = 0;                  /* Set 0 to the end of string */
            
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            return 1;
        }
    }
    return 0;
}

GUI_HANDLE_t __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_HANDLE_t h;
    GUI_Byte result = 0;
    
    __GUI_ASSERTPARAMS(widget && widget->Control);  /* Check input parameters */
    
    h = (GUI_HANDLE_t)__GUI_MEMALLOC(widget->MetaData.WidgetSize);
    if (h) {  
        GUI_Dim_t pW, pH;                           /* Parent width and height values */
        
        memset(h, 0x00, widget->MetaData.WidgetSize); /* Set memory to 0 */
        
        h->Id = id;                                 /* Save ID */
        h->Widget = widget;                         /* Widget object structure */
        h->Parent = GUI.WindowActive;               /* Set parent window */

        /* Coordinates management */
        if (h->Parent) {                            /* Get object width from parent */
            pW = h->Parent->Width;
            pH = h->Parent->Height;
        } else {                                    /* Parent is LCD window */
            pW = GUI.LCD.Width;
            pH = GUI.LCD.Height;
        }
        
        if (width > pW) {                           /* Check maximum width */
            width = pW;
        }
        if (height > pH) {                          /* Check maximum height */
            height = pH;
        }
        
        if ((x + width) > pW) {                     /* Check X position if it goes over width of parent */
            x = pW - width;
        }
        if ((y + height) > pH) {                    /* Check Y position if it goes over height of parent */
            y = pH - height;
        }
        
        h->X = x;                                   /* Set X position relative to parent */
        h->Y = y;                                   /* Set Y position relative to parent */
        h->Width = width;                           /* Set widget width */
        h->Height = height;                         /* Set widget height */

        if (!widget->Control(h, GUI_WidgetControl_ExcludeLinkedList, 0, &result)) { /* Check if widget should be added to linked list */
            __GUI_LINKEDLIST_ADD((GUI_HANDLE_ROOT_t *)h->Parent, h);    /* Add entry to linkedlist of parent widget */
        }
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    } else {
        __GUI_DEBUG("Alloc failed for widget %s with %d bytes\r\n", (char *)widget->MetaData.Name, widget->MetaData.WidgetSize);
    }
    
    return h;
}
uint8_t __GUI_WIDGET_Remove(GUI_HANDLE_t* h) {
    if ((*h)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) {  /* Check memory for text */
        __GUI_MEMFREE((*h)->Text);                  /* Free text memory */
    }
    
    __GUI_LINKEDLIST_REMOVE(*h);                    /* Remove entry from linked list */
    if ((*h)->Parent) {                             /* If there is parent object */
        //TODO: Redraw only if deleted widget was visible on screen
        
        (*h)->Parent->Flags |= GUI_FLAG_REDRAW;     /* Redraw widget */
    }
    __GUI_MEMWIDFREE(h);                            /* Free memory for widget */
    return 1;
}


/******************************************************************************/
/******************************************************************************/
/***               Public API, can be used by user for any widget            **/
/******************************************************************************/
/******************************************************************************/
uint8_t GUI_WIDGET_Remove(GUI_HANDLE_t* h) {
    __GUI_ASSERTPARAMS(h && *h);                    /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;                                       /* Removev successfully */
}

uint32_t GUI_WIDGET_AllocTextMemory(GUI_HANDLE_t h, uint32_t size) {
    __GUI_ASSERTPARAMS(h && size > 1);              /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_AllocateTextMemory(h, size);       /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return h->TextMemSize;                          /* Return number of bytes allocated */
}

GUI_HANDLE_t GUI_WIDGET_FreeTextMemory(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_FreeTextMemory(h);                 /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_WIDGET_SetText(GUI_HANDLE_t h, const GUI_Char* text) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetText(h, text);                  /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_WIDGET_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetSize(h, width, height);         /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_WIDGET_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetXY(h, x, y);                    /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_WIDGET_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetFont(h, font);                  /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_WIDGET_Invalidate(GUI_HANDLE_t h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

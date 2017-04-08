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
/* Removes widget and children widgets */
static void __RemoveWidget(GUI_HANDLE_p h) {    
    /**
     * Remove children widgets first
     */
    if (__GUI_WIDGET_AllowChildren(h)) {       /* Check if children widgets are allowed */
        GUI_HANDLE_p h2;
        for (h2 = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)h, NULL); h2; h2 = __GUI_LINKEDLIST_WidgetGetNext(NULL, h2)) {
            __RemoveWidget(h2);                   /* Remove widget and all children elements */
        }
    }
    
    /**
     * Remove actual widget
     */
    __GUI_WIDGET_InvalidateWithParent(h);           /* Invalidate object and its parent */
    __GUI_WIDGET_FreeTextMemory(h);                 /* Free text memory */
    if (h->Timer) {                                 /* Check timer memory */
        __GUI_TIMER_Remove(&h->Timer);               /* Free timer memory */
    }
    __GUI_LINKEDLIST_WidgetRemove(h);               /* Remove entry from linked list */
    __GUI_MEMWIDFREE(h);                            /* Free memory for widget */
}

static void __GUI_WIDGET_SetClippingRegion(GUI_HANDLE_p h) {
    GUI_Dim_t x, y, width, height;
    
    x = __GUI_WIDGET_GetAbsoluteX(h);               /* Get widget absolute X */
    y = __GUI_WIDGET_GetAbsoluteY(h);               /* Get widget absolute Y */
    width = __GUI_WIDGET_GetWidth(h);               /* Get widget width */
    height = __GUI_WIDGET_GetHeight(h);             /* Get widget height */
    
    /* Set invalid clipping region */
    if (GUI.Display.X1 > x) {
        GUI.Display.X1 = x;
    }
    if (GUI.Display.X2 < (x + width)) {
        GUI.Display.X2 = (x + width);
    }
    if (GUI.Display.Y1 > y) {
        GUI.Display.Y1 = y;
    }
    if (GUI.Display.Y2 < (y + height)) {
        GUI.Display.Y2 = (y + height);
    }
}

static uint8_t __GUI_WIDGET_InvalidatePrivate(GUI_HANDLE_p h, uint8_t setclipping) {
    GUI_HANDLE_p h1, h2;
    
    if (!h) {
        return 0;
    }
    
    h1 = h;                                         /* Get widget handle */
    h1->Flags |= GUI_FLAG_REDRAW;                   /* Redraw widget */
    
#if GUI_USE_CLIPPING
    if (setclipping) {
        __GUI_WIDGET_SetClippingRegion(h);          /* Set clipping region for widget redrawing operation */
    }
#endif /* GUI_USE_CLIPPING */
    
    /**
     * Invalid only widget with higher Z-index (lowered on linked list) of current object
     * 
     * If widget should be redrawn, then any widget above it should be redrawn too, otherwise z-index match will fail
     *
     * Widget may not need redraw operation if positions don't match
     */
    for (; h1; h1 = __GUI_LINKEDLIST_WidgetGetNext(NULL, h1)) {
        for (h2 = __GUI_LINKEDLIST_WidgetGetNext(NULL, h1); h2; h2 = __GUI_LINKEDLIST_WidgetGetNext(NULL, h2)) {
            if (
                h2->Flags & GUI_FLAG_REDRAW ||  /* Bit is already set */
                !__GUI_RECT_MATCH(h1->X, h1->Y, h1->Width, h1->Height, h2->X, h2->Y, h2->Width, h2->Height)
            ) {
                continue;
            }
            h2->Flags |= GUI_FLAG_REDRAW;           /* Redraw widget on next loop */
        }
    }
    
    /**
     * If widget is not the last on the linked list (top z-index)
     * check status of parent widget if it is last.
     * If it is not, process parent redraw and check similar parent widgets if are over our widget
     */
    if (h->Parent && !__GUI_LINKEDLIST_IsWidgetLast(h->Parent)) {
        __GUI_WIDGET_InvalidatePrivate(h->Parent, 0);
    }
    
    return 1;
}

uint8_t __GUI_WIDGET_IsInsideClippingRegion(GUI_HANDLE_p h) {
    return __GUI_RECT_MATCH(
        __GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h), __GUI_WIDGET_GetWidth(h), __GUI_WIDGET_GetHeight(h), 
        GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2 - GUI.Display.X1, GUI.Display.Y2 - GUI.Display.Y1
    );
}

void __GUI_WIDGET_Init(void) {
    GUI_WINDOW_Create(GUI_ID_WINDOW_BASE);          /* Create base window object */
}

GUI_Dim_t __GUI_WIDGET_GetWidth(GUI_HANDLE_p h) {
    if (h->Flags & GUI_FLAG_WIDTH_FILL) {           /* "fill_parent" mode for width */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerWidth(h);
        if (parent > h->X) {
            return parent - h->X;                   /* Return widget width */
        }
    } else if (h->Flags & GUI_FLAG_WIDTH_PERCENT) { /* Percentage width */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerWidth(h);
        return h->Width * parent / 100;
    } else {                                        /* Normal width */
        return h->Width;
    }
    return 0;
}

GUI_Dim_t __GUI_WIDGET_GetHeight(GUI_HANDLE_p h) {
    if (h->Flags & GUI_FLAG_HEIGHT_FILL) {          /* "fill_parent" mode for height */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerHeight(h);
        if (parent > h->Y) {
            return parent - h->Y;                   /* Return widget width */
        }
    } else if (h->Flags & GUI_FLAG_HEIGHT_PERCENT) {/* Percentage height */
        return h->Height * __GUI_WIDGET_GetParentInnerHeight(h) / 100;
    } else {                                        /* Normal height */
        return h->Height;
    }
    return 0;
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteX(GUI_HANDLE_p h) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_p w = 0;
    GUI_Dim_t out = 0;
    
    if (h) {
        w = h->Parent;
        out = h->X;
    }
    
    while (w) {                                     /* Go through all parent windows */
        out += w->X + __GUI_WIDGET_GetPaddingLeft(w);   /* Add X offset from parent and left padding of parent */
        w = (GUI_HANDLE_p)w->Parent;                /* Get next parent */
    }
    return out;
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteY(GUI_HANDLE_p h) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_p w = 0;
    GUI_Dim_t out = 0;
    
    if (h) {
        w = ((GUI_HANDLE_p)h)->Parent;
        out = ((GUI_HANDLE_p)h)->Y;
    }
    
    while (w) {                                     /* Go through all parent windows */
        out += w->Y + __GUI_WIDGET_GetPaddingTop(w);/* Add X offset from parent + parent top padding */
        w = (GUI_HANDLE_p)w->Parent;                /* Get next parent */
    }
    return out;
}

uint8_t __GUI_WIDGET_Invalidate(GUI_HANDLE_p h) {
    uint8_t ret = __GUI_WIDGET_InvalidatePrivate(h, 1); /* Invalidate widget with clipping */
    return ret;
}

uint8_t __GUI_WIDGET_InvalidateWithParent(GUI_HANDLE_p h) {
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate object */
    if (h->Parent) {                                /* If parent exists, invalid only parent */
        __GUI_WIDGET_Invalidate(h->Parent);         /* Invalidate parent object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    if (h->X != x || h->Y != y) {
        GUI_iDim_t pW, pH, width, height;
        
        pW = __GUI_WIDGET_GetParentInnerWidth(h);   /* Get available width */
        pH = __GUI_WIDGET_GetParentInnerHeight(h);  /* Get available height */
        
        if (x < 0) {                                /* Check X value */
            x = 0;
        } else if (!(h->Flags & GUI_FLAG_WIDTH_FILL)) { /* If fill mode is not used, exclude width */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            if (x > (pW - width)) {
                x = pW - width;
            }
        }
        if (x > pW) {
            x = pW - 1;
        }
        if (y < 0) {                                /* Check Y value */
            y = 0;
        } else if (!(h->Flags & GUI_FLAG_HEIGHT_FILL)) {
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            if (y > (pH - height)) {
                y = pH - height;
            }
        }
        if (y > pH) {
            y = pH - 1;
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

uint8_t __GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi) {  
    if (wi != h->Width || hi != h->Height) { 
        GUI_Dim_t pW, pH;
        
        pW = __GUI_WIDGET_GetParentInnerWidth(h);   /* Get parent width */
        pH = __GUI_WIDGET_GetParentInnerHeight(h);  /* Get parent width */
        
        if (h->Flags & GUI_FLAG_WIDTH_PERCENT) {    /* Width is in percentage */
            if (wi > 100) {
                wi = 100;
            }
            if ((h->X + (pW * wi / 100.0f)) > pW) { /* Check if widget width is too big */
                wi = ((pW - h->X) * 100.0f) / pW;
            }
        } else {
            if ((h->X + wi) > pW) {                 /* Check if widget width is too big */
                wi = pW - h->X;
            }
        }
        
        if (h->Flags & GUI_FLAG_HEIGHT_PERCENT) {    /* Height is in percentage */
            if (hi > 100) {
                hi = 100;
            }
            if ((h->Y + (pH * hi / 100.0f)) > pH) { /* Check if widget height is too big */
                hi = ((pH - h->Y) * 100.0f) / pH;
            }
        } else {                                    /* Height is absolute number */
            if ((h->Y + hi) > pH) {                 /* Check if widget width is too big */
                hi = pH - h->Y;
            }
        }

        __GUI_WIDGET_SetClippingRegion(h);          /* Set clipping region before changed position */
        h->Width = wi;                              /* Set parameter */
        h->Height = hi;                             /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Enable3DStyle(GUI_HANDLE_p h) {
    if (!(h->Flags & GUI_FLAG_3D)) {
        h->Flags |= GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Disable3DStyle(GUI_HANDLE_p h) {
    if (h->Flags & GUI_FLAG_3D) {
        h->Flags &= ~GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetFont(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font) {
    if (h->Font != font) {                          /* Any parameter changed */
        h->Font = font;                             /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text) {
   if (h->Flags & GUI_FLAG_DYNAMICTEXTALLOC) {      /* Memory for text is dynamically allocated */
        if (h->TextMemSize) {
            if (GUI_STRING_LengthTotal(text) > (h->TextMemSize - 1)) {  /* Check string length */
                GUI_STRING_CopyN(h->Text, text, h->TextMemSize - 1);    /* Do not copy all bytes because of memory overflow */
            } else {
                GUI_STRING_Copy(h->Text, text);     /* Copy entire string */
            }
            __GUI_WIDGET_Invalidate(h);             /* Redraw object */
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    } else {                                        /* Memory allocated by user */
        if (h->Text && h->Text == text) {           /* In case the same pointer is passed to WIDGET */
            if (GUI_STRING_Compare(h->Text, text)) {/* If strings does not match, source string updated? */
                __GUI_WIDGET_Invalidate(h);         /* Redraw object */
                __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            }
        }
        
        if (h->Text != text) {                      /* Check if pointer do not match */
            h->Text = (GUI_Char *)text;             /* Set parameter */
            __GUI_WIDGET_Invalidate(h);             /* Redraw object */
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    }
    h->TextCursor = GUI_STRING_LengthTotal(h->Text);/* Set cursor to the end of string */
    return 1;
}

uint8_t __GUI_WIDGET_AllocateTextMemory(GUI_HANDLE_p h, uint32_t size) {
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
    __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    return 1;
}

uint8_t __GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h) {
    if ((h->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && h->Text) {    /* Check if dynamically alocated */
        __GUI_MEMFREE(h->Text);                     /* Free memory first */
        h->Text = 0;                                /* Reset memory */
        h->TextMemSize = 0;                         /* Reset memory size */
        h->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC;     /* Not allocated */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
        __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    }
    return 1;
}

uint8_t __GUI_WIDGET_IsFontAndTextSet(GUI_HANDLE_p h) {
    return h->Text && h->Font && GUI_STRING_Length(h->Text);    /* Check if conditions are met for drawing string */
}

uint8_t __GUI_WIDGET_ProcessTextKey(GUI_HANDLE_p h, __GUI_KeyboardData_t* kb) {
    uint16_t len, tlen;
    uint32_t ch;
    uint8_t l;
    const GUI_Char* str = kb->KB.Keys;
    
    if (!(h->Flags & GUI_FLAG_DYNAMICTEXTALLOC)) {  /* Must be dynamically allocated memory */
        return 0;
    }
    
    if (!GUI_STRING_GetCh(&str, &ch, &l)) {         /* Get key from input data */
        return 0;                                   /* Invalid input key */
    }
    
    tlen = GUI_STRING_LengthTotal(h->Text);         /* Get total length of string */
    len = GUI_STRING_Length(h->Text);               /* Get string length */
    if ((ch == GUI_KEY_CR || ch == GUI_KEY_LF || ch >= 32) && ch != 127) {  /* Check valid character character */
        if (len < (h->TextMemSize - l)) {           /* Memory still available for new character */
            uint16_t pos;
            for (pos = tlen + l - 1; pos > h->TextCursor; pos--) {  /* Shift characters down */
                h->Text[pos] = h->Text[pos - l];
            }
            for (pos = 0; pos < l; pos++) {         /* Fill new characters to empty memory */
                h->Text[h->TextCursor++] = kb->KB.Keys[pos];
            }
            h->Text[tlen + l] = 0;                  /* Add 0 to the end */
            
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
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
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            return 1;
        }
    }
    return 0;
}

GUI_HANDLE_p __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags) {
    GUI_HANDLE_p h;
    GUI_Byte result = 0;
    
    __GUI_ASSERTPARAMS(widget && widget->Callback); /* Check input parameters */
    
    h = (GUI_HANDLE_p)__GUI_MEMALLOC(widget->Size);
    if (h) {
        memset(h, 0x00, widget->Size);              /* Set memory to 0 */
        
        h->Id = id;                                 /* Save ID */
        h->Widget = widget;                         /* Widget object structure */
        if (flags & GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP) {
            h->Parent = GUI_WINDOW_GetDesktop();    /* Set parent object */
        } else {
            if (parent && __GUI_WIDGET_AllowChildren(parent)) {
                h->Parent = parent;
            } else {
                h->Parent = GUI.WindowActive;       /* Set parent object. It will be NULL on first call */
            }
        }
        
        __GUI_WIDGET_SetSize(h, width, height);     /* Set widget size */
        __GUI_WIDGET_SetXY(h, x, y);                /* Set widget position */

        result = 0;
        __GUI_WIDGET_Callback(h, GUI_WC_ExcludeLinkedList, 0, &result);
        if (!result) {                              /* Check if widget should be added to linked list */
            __GUI_LINKEDLIST_WidgetAdd((GUI_HANDLE_ROOT_t *)h->Parent, h);  /* Add entry to linkedlist of parent widget */
        }
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    } else {
        __GUI_DEBUG("Alloc failed for widget %s with %d bytes\r\n", (char *)widget->Name, widget->Size);
    }
    
    return h;
}

uint8_t __GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    h->Flags &= ~GUI_FLAG_WIDTH_PERCENT;            /* Clear percentage flag */
    h->Width = width + 1;                           /* Invalidate width */
    __GUI_WIDGET_SetSize(h, width, h->Height);      /* Set new width */
    return 1;
}

uint8_t __GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height) {
    h->Flags &= ~GUI_FLAG_HEIGHT_PERCENT;           /* Clear percentage flag */
    h->Height = height + 1;                         /* Invalidate height */
    __GUI_WIDGET_SetSize(h, h->Width, height);      /* Set new height */
    return 1;
}

uint8_t __GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width) {
    h->Flags |= GUI_FLAG_WIDTH_PERCENT;             /* Set percentage flag */
    h->Width = width + 1;                           /* Invalidate widget */
    __GUI_WIDGET_SetSize(h, width, h->Height);      /* Set new width */
    return 1;
}

uint8_t __GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height) {
    h->Flags |= GUI_FLAG_HEIGHT_PERCENT;            /* Set percentage flag */
    h->Height = height + 1;                         /* Invalidate widget */
    __GUI_WIDGET_SetSize(h, h->Width, height);      /* Set new height */
    return 1;
}

uint8_t __GUI_WIDGET_CanRemove(GUI_HANDLE_p h) {
    uint8_t r = 1;
    
    /**
     * Desktop window cannot be deleted
     */
    if (h == GUI_WINDOW_GetDesktop()) {             /* Root widget can not be deleted! */
        return 0;
    }
    
    /**
     * Check widget status itself
     */
    if (!__GUI_WIDGET_Callback(h, GUI_WC_Remove, 0, &r) || r) { /* If command was not processed, allow delete */
        r = 1;                                      /* Manually allow delete */
    }
    
    /**
     * Check children widgets recursively
     */
    if (r && __GUI_WIDGET_AllowChildren(h)) {       /* Check if we can delete all children widgets */
        GUI_HANDLE_p h1;
        for (h1 = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)h, NULL); h1; h1 = __GUI_LINKEDLIST_WidgetGetNext(NULL, h1)) {
            if (!__GUI_WIDGET_CanRemove(h1)) {      /* If we should not delete it */
                return 0;                           /* Stop on first call */
            }
        }
    }
    
    return r;
}

uint8_t __GUI_WIDGET_Remove(GUI_HANDLE_p* h) {
    if (__GUI_WIDGET_CanRemove(*h)) {               /* Check if we can delete widget */ 
        __RemoveWidget(*h);                         /* Remove widget recursively */
        *h = 0;
        return 1;                                   /* Widget deleted */
    }

    return 0;
}

uint8_t __GUI_WIDGET_Show(GUI_HANDLE_p h) {     
    if (h->Flags & GUI_FLAG_HIDDEN) {               /* If hidden, show it */
        h->Flags &= ~GUI_FLAG_HIDDEN;
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate it for redraw with parent */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Hide(GUI_HANDLE_p h) {
    if (!(h->Flags & GUI_FLAG_HIDDEN)) {            /* If visible, hide it */
        h->Flags |= GUI_FLAG_HIDDEN;
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate it for redraw with parenta */
    }
    
    /**
     * TODO: Check if active/focused widget is maybe children of this widget
     */
    
    if (GUI.FocusedWidget == h || __GUI_WIDGET_IsChildOf(GUI.FocusedWidget, h)) {   /* Clear focus */
        __GUI_FOCUS_CLEAR();
    }
    if (GUI.ActiveWidget == h || __GUI_WIDGET_IsChildOf(GUI.ActiveWidget, h)) { /* Clear active */
        __GUI_ACTIVE_CLEAR();
    }
    return 1;
}

uint8_t __GUI_WIDGET_IsChildOf(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    if (!h || !parent) {                            /* Check input parameters */
        return 0;
    }
    for (h = h->Parent; h; h = h->Parent) {         /* Check widget parent objects */
        if (parent == h) {                          /* If they matches */
            return 1;
        }
    }
    return 0;
}

void __GUI_WIDGET_MoveDownTree(GUI_HANDLE_p h) {              
    /**
     * Move widget to the end of parent linked list
     * This will allow widget to be first checked next time for touch detection
     * and will be drawn on top of al widgets as expected except if there is widget which allows children (new window or similar)
     */
    if (__GUI_LINKEDLIST_WidgetMoveToBottom(h)) {
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    
    /**
     * Since linked list is threaded, we should move our widget to the end of parent list.
     * The same should be in the parent as it should also be on the end of its parent and so on.
     * With parent recursion this can be achieved
     */
    if (h->Parent) {            /* Move its parent to the bottom of parent linked list */
        GUI_HANDLE_p parent = h->Parent;
        for (; parent; parent = parent->Parent) {
            if (__GUI_LINKEDLIST_WidgetMoveToBottom(parent)) { /* If move down was successful */
                __GUI_WIDGET_Invalidate(parent); /* Invalidate parent of widget */
            }
        }
    }
}


/******************************************************************************/
/******************************************************************************/
/***               Public API, can be used by user for any widget            **/
/******************************************************************************/
/******************************************************************************/
uint8_t GUI_WIDGET_Remove(GUI_HANDLE_p* h) {
    __GUI_ASSERTPARAMS(h && *h);                    /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;                                       /* Removev successfully */
}

uint32_t GUI_WIDGET_AllocTextMemory(GUI_HANDLE_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(h && size > 1);              /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_AllocateTextMemory(h, size);       /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return h->TextMemSize;                          /* Return number of bytes allocated */
}

GUI_HANDLE_p GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_FreeTextMemory(h);                 /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetText(h, text);                  /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetSize(h, width, height);         /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetWidth(h, width);                /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetHeight(h, height);              /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetWidthPercent(h, width);         /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetHeightPercent(h, height);       /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetXY(h, x, y);                    /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_SetFont(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_SetFont(h, font);                  /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_Invalidate(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_Show(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_Show(h);                           /* Show widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_WIDGET_Hide(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_Hide(h);                           /* Hide widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

uint8_t GUI_WIDGET_IsChildOf(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    return __GUI_WIDGET_IsChildOf(h, parent);       /* Return status */
}

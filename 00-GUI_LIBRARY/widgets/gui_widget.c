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
static 
void __RemoveWidget(GUI_HANDLE_p h) {    
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
    if (__GH(h)->Timer) {                           /* Check timer memory */
        __GUI_TIMER_Remove(&__GH(h)->Timer);        /* Free timer memory */
    }
    if (__GH(h)->Colors) {                          /* Free allocate colors if exists */
        __GUI_MEMFREE(__GH(h)->Colors);
    }
    __GUI_LINKEDLIST_WidgetRemove(h);               /* Remove entry from linked list */
    __GUI_MEMWIDFREE(h);                            /* Free memory for widget */
}

static
void __GUI_WIDGET_SetClippingRegion(GUI_HANDLE_p h) {
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

static
uint8_t __GUI_WIDGET_InvalidatePrivate(GUI_HANDLE_p h, uint8_t setclipping) {
    GUI_HANDLE_p h1, h2;
    
    if (!h) {
        return 0;
    }
    
    h1 = h;                                         /* Get widget handle */
    __GH(h1)->Flags |= GUI_FLAG_REDRAW;             /* Redraw widget */
    
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
                __GH(h2)->Flags & GUI_FLAG_REDRAW ||    /* Bit is already set */
                !__GUI_RECT_MATCH(__GH(h1)->X, __GH(h1)->Y, __GH(h1)->Width, __GH(h1)->Height, __GH(h2)->X, __GH(h2)->Y, __GH(h2)->Width, __GH(h2)->Height)
            ) {
                continue;
            }
            __GH(h2)->Flags |= GUI_FLAG_REDRAW;     /* Redraw widget on next loop */
        }
    }
    
    /**
     * If widget is not the last on the linked list (top z-index)
     * check status of parent widget if it is last.
     * If it is not, process parent redraw and check similar parent widgets if are over our widget
     */
    if (__GH(h)->Parent && !__GUI_LINKEDLIST_IsWidgetLast(__GH(h)->Parent)) {
        __GUI_WIDGET_InvalidatePrivate(__GH(h)->Parent, 0);
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
    if (__GH(h)->Flags & GUI_FLAG_WIDTH_FILL) {     /* "fill_parent" mode for width */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerWidth(h);
        if (parent > __GH(h)->X) {
            return parent - __GH(h)->X;             /* Return widget width */
        }
    } else if (__GH(h)->Flags & GUI_FLAG_WIDTH_PERCENT) { /* Percentage width */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerWidth(h);
        return __GH(h)->Width * parent / 100;
    } else {                                        /* Normal width */
        return __GH(h)->Width;
    }
    return 0;
}

GUI_Dim_t __GUI_WIDGET_GetHeight(GUI_HANDLE_p h) {
    if (__GH(h)->Flags & GUI_FLAG_HEIGHT_FILL) {    /* "fill_parent" mode for height */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerHeight(h);
        if (parent > __GH(h)->Y) {
            return parent - __GH(h)->Y;             /* Return widget width */
        }
    } else if (__GH(h)->Flags & GUI_FLAG_HEIGHT_PERCENT) {  /* Percentage height */
        return __GH(h)->Height * __GUI_WIDGET_GetParentInnerHeight(h) / 100;
    } else {                                        /* Normal height */
        return __GH(h)->Height;
    }
    return 0;
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteX(GUI_HANDLE_p h) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_p w = 0;
    GUI_Dim_t out = 0;
    
    if (h) {
        w = __GH(h)->Parent;
        out = __GH(h)->X;
    }
    
    while (w) {                                     /* Go through all parent windows */
        out += __GH(w)->X + __GUI_WIDGET_GetPaddingLeft(w);   /* Add X offset from parent and left padding of parent */
        w = __GH(w)->Parent;                        /* Get next parent */
    }
    return out;
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteY(GUI_HANDLE_p h) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_p w = 0;
    GUI_Dim_t out = 0;
    
    if (h) {
        w = __GH(h)->Parent;
        out = __GH(h)->Y;
    }
    
    while (w) {                                     /* Go through all parent windows */
        out += __GH(w)->Y + __GUI_WIDGET_GetPaddingTop(w);/* Add X offset from parent + parent top padding */
        w = __GH(w)->Parent;                        /* Get next parent */
    }
    return out;
}

uint8_t __GUI_WIDGET_Invalidate(GUI_HANDLE_p h) {
    uint8_t ret = __GUI_WIDGET_InvalidatePrivate(h, 1); /* Invalidate widget with clipping */
    return ret;
}

uint8_t __GUI_WIDGET_InvalidateWithParent(GUI_HANDLE_p h) {
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate object */
    if (__GH(h)->Parent) {                          /* If parent exists, invalid only parent */
        __GUI_WIDGET_Invalidate(__GH(h)->Parent);   /* Invalidate parent object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    if (__GH(h)->X != x || __GH(h)->Y != y) {
        GUI_iDim_t pW, pH, width, height;
        
        pW = __GUI_WIDGET_GetParentInnerWidth(h);   /* Get available width */
        pH = __GUI_WIDGET_GetParentInnerHeight(h);  /* Get available height */
        
        if (x < 0) {                                /* Check X value */
            x = 0;
        } else if (!(__GH(h)->Flags & GUI_FLAG_WIDTH_FILL)) { /* If fill mode is not used, exclude width */
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
        } else if (!(__GH(h)->Flags & GUI_FLAG_HEIGHT_FILL)) {
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            if (y > (pH - height)) {
                y = pH - height;
            }
        }
        if (y > pH) {
            y = pH - 1;
        }
        
        if (__GH(h)->X != x || __GH(h)->Y != y) {            
            __GUI_WIDGET_SetClippingRegion(h);      /* Set new clipping region */
            __GH(h)->X = x;                         /* Set parameter */
            __GH(h)->Y = y;                         /* Set parameter */
            __GUI_WIDGET_InvalidateWithParent(h);   /* Invalidate object */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi) {  
    if (wi != __GH(h)->Width || hi != __GH(h)->Height) { 
        GUI_Dim_t pW, pH;
        
        pW = __GUI_WIDGET_GetParentInnerWidth(h);   /* Get parent width */
        pH = __GUI_WIDGET_GetParentInnerHeight(h);  /* Get parent width */
        
        if (__GH(h)->Flags & GUI_FLAG_WIDTH_PERCENT) {  /* Width is in percentage */
            if (wi > 100) {
                wi = 100;
            }
            if ((__GH(h)->X + (pW * wi / 100.0f)) > pW) {   /* Check if widget width is too big */
                wi = ((pW - __GH(h)->X) * 100.0f) / pW;
            }
        } else {
            if ((__GH(h)->X + wi) > pW) {           /* Check if widget width is too big */
                wi = pW - __GH(h)->X;
            }
        }
        
        if (__GH(h)->Flags & GUI_FLAG_HEIGHT_PERCENT) { /* Height is in percentage */
            if (hi > 100) {
                hi = 100;
            }
            if ((__GH(h)->Y + (pH * hi / 100.0f)) > pH) {   /* Check if widget height is too big */
                hi = ((pH - __GH(h)->Y) * 100.0f) / pH;
            }
        } else {                                    /* Height is absolute number */
            if ((__GH(h)->Y + hi) > pH) {           /* Check if widget width is too big */
                hi = pH - __GH(h)->Y;
            }
        }

        __GUI_WIDGET_SetClippingRegion(h);          /* Set clipping region before changed position */
        __GH(h)->Width = wi;                        /* Set parameter */
        __GH(h)->Height = hi;                       /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Enable3DStyle(GUI_HANDLE_p h) {
    if (!(__GH(h)->Flags & GUI_FLAG_3D)) {
        __GH(h)->Flags |= GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Disable3DStyle(GUI_HANDLE_p h) {
    if (__GH(h)->Flags & GUI_FLAG_3D) {
        __GH(h)->Flags &= ~GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetFont(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font) {
    if (__GH(h)->Font != font) {                    /* Any parameter changed */
        __GH(h)->Font = font;                       /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text) {
   if (__GH(h)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) {    /* Memory for text is dynamically allocated */
        if (__GH(h)->TextMemSize) {
            if (GUI_STRING_LengthTotal(text) > (__GH(h)->TextMemSize - 1)) {    /* Check string length */
                GUI_STRING_CopyN(__GH(h)->Text, text, __GH(h)->TextMemSize - 1);    /* Do not copy all bytes because of memory overflow */
            } else {
                GUI_STRING_Copy(__GH(h)->Text, text);   /* Copy entire string */
            }
            __GUI_WIDGET_Invalidate(h);             /* Redraw object */
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    } else {                                        /* Memory allocated by user */
        if (__GH(h)->Text && __GH(h)->Text == text) {   /* In case the same pointer is passed to WIDGET */
            if (GUI_STRING_Compare(__GH(h)->Text, text)) {/* If strings does not match, source string updated? */
                __GUI_WIDGET_Invalidate(h);         /* Redraw object */
                __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            }
        }
        
        if (__GH(h)->Text != text) {                /* Check if pointer do not match */
            __GH(h)->Text = (GUI_Char *)text;       /* Set parameter */
            __GUI_WIDGET_Invalidate(h);             /* Redraw object */
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    }
    __GH(h)->TextCursor = GUI_STRING_LengthTotal(__GH(h)->Text);    /* Set cursor to the end of string */
    return 1;
}

uint8_t __GUI_WIDGET_AllocateTextMemory(GUI_HANDLE_p h, uint32_t size) {
    if ((__GH(h)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {    /* Check if already allocated */
        __GUI_MEMFREE(__GH(h)->Text);               /* Free memory first */
        __GH(h)->TextMemSize = 0;                   /* Reset memory size */
    }
    __GH(h)->Text = 0;                              /* Reset pointer */
    
    __GH(h)->TextMemSize = size * sizeof(GUI_Char); /* Allocate text memory */
    __GH(h)->Text = (GUI_Char *)__GUI_MEMALLOC(__GH(h)->TextMemSize);   /* Allocate memory for text */
    if (__GH(h)->Text) {                            /* Check if allocated */
        __GH(h)->Flags |= GUI_FLAG_DYNAMICTEXTALLOC;/* Dynamically allocated */
    } else {
        __GH(h)->TextMemSize = 0;                   /* No dynamic bytes available */
        __GH(h)->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC;   /* Not allocated */
    }
    __GUI_WIDGET_Invalidate(h);                     /* Redraw object */
    __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    return 1;
}

uint8_t __GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h) {
    if ((__GH(h)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {    /* Check if dynamically alocated */
        __GUI_MEMFREE(__GH(h)->Text);               /* Free memory first */
        __GH(h)->Text = 0;                          /* Reset memory */
        __GH(h)->TextMemSize = 0;                   /* Reset memory size */
        __GH(h)->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC;   /* Not allocated */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw object */
        __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    }
    return 1;
}

uint8_t __GUI_WIDGET_IsFontAndTextSet(GUI_HANDLE_p h) {
    return __GH(h)->Text && __GH(h)->Font && GUI_STRING_Length(__GH(h)->Text);  /* Check if conditions are met for drawing string */
}

uint8_t __GUI_WIDGET_ProcessTextKey(GUI_HANDLE_p h, __GUI_KeyboardData_t* kb) {
    uint16_t len, tlen;
    uint32_t ch;
    uint8_t l;
    const GUI_Char* str = kb->KB.Keys;
    
    if (!(__GH(h)->Flags & GUI_FLAG_DYNAMICTEXTALLOC)) {    /* Must be dynamically allocated memory */
        return 0;
    }
    
    if (!GUI_STRING_GetCh(&str, &ch, &l)) {         /* Get key from input data */
        return 0;                                   /* Invalid input key */
    }
    
    tlen = GUI_STRING_LengthTotal(__GH(h)->Text);   /* Get total length of string */
    len = GUI_STRING_Length(__GH(h)->Text);         /* Get string length */
    if ((ch == GUI_KEY_CR || ch == GUI_KEY_LF || ch >= 32) && ch != 127) {  /* Check valid character character */
        if (len < (__GH(h)->TextMemSize - l)) {     /* Memory still available for new character */
            uint16_t pos;
            for (pos = tlen + l - 1; pos > __GH(h)->TextCursor; pos--) {    /* Shift characters down */
                __GH(h)->Text[pos] = __GH(h)->Text[pos - l];
            }
            for (pos = 0; pos < l; pos++) {         /* Fill new characters to empty memory */
                __GH(h)->Text[__GH(h)->TextCursor++] = kb->KB.Keys[pos];
            }
            __GH(h)->Text[tlen + l] = 0;            /* Add 0 to the end */
            
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            __GUI_WIDGET_Callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            return 1;
        }
    } else if (ch == 8 || ch == 127) {              /* Backspace character */
        if (tlen && __GH(h)->TextCursor) {
            const GUI_Char* end = (GUI_Char *)((uint32_t)__GH(h)->Text + __GH(h)->TextCursor - 1);  /* End of string pointer */
            uint16_t pos;
            
            if (!GUI_STRING_GetChReverse(&end, &ch, &l)) {  /* Get last character */
                return 0;                           
            }
            for (pos = __GH(h)->TextCursor - l; pos < (tlen - l); pos++) {  /* Shift characters up */
                __GH(h)->Text[pos] = __GH(h)->Text[pos + l];
            }
            __GH(h)->TextCursor -= l;               /* Decrease text cursor by number of bytes for character deleted */
            __GH(h)->Text[tlen - l] = 0;            /* Set 0 to the end of string */
            
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
        
        __GH(h)->Id = id;                           /* Save ID */
        __GH(h)->Widget = widget;                   /* Widget object structure */
        if (flags & GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP) {
            __GH(h)->Parent = GUI_WINDOW_GetDesktop();  /* Set parent object */
        } else {
            if (parent && __GUI_WIDGET_AllowChildren(parent)) {
                __GH(h)->Parent = parent;
            } else {
                __GH(h)->Parent = GUI.WindowActive; /* Set parent object. It will be NULL on first call */
            }
        }
        
        __GUI_WIDGET_SetSize(h, width, height);     /* Set widget size */
        __GUI_WIDGET_SetXY(h, x, y);                /* Set widget position */

        result = 0;
        __GUI_WIDGET_Callback(h, GUI_WC_ExcludeLinkedList, 0, &result);
        if (!result) {                              /* Check if widget should be added to linked list */
            __GUI_LINKEDLIST_WidgetAdd((GUI_HANDLE_ROOT_t *)__GH(h)->Parent, h);    /* Add entry to linkedlist of parent widget */
        }
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate object */
    } else {
        __GUI_DEBUG("Alloc failed for widget %s with %d bytes\r\n", (char *)widget->Name, widget->Size);
    }
    
    return h;
}

uint8_t __GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GH(h)->Flags &= ~GUI_FLAG_WIDTH_PERCENT;      /* Clear percentage flag */
    __GH(h)->Width = width + 1;                     /* Invalidate width */
    __GUI_WIDGET_SetSize(h, width, __GH(h)->Height);    /* Set new width */
    return 1;
}

uint8_t __GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height) {
    __GH(h)->Flags &= ~GUI_FLAG_HEIGHT_PERCENT;     /* Clear percentage flag */
    __GH(h)->Height = height + 1;                   /* Invalidate height */
    __GUI_WIDGET_SetSize(h, __GH(h)->Width, height);    /* Set new height */
    return 1;
}

uint8_t __GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GH(h)->Flags |= GUI_FLAG_WIDTH_PERCENT;       /* Set percentage flag */
    __GH(h)->Width = width + 1;                     /* Invalidate widget */
    __GUI_WIDGET_SetSize(h, width, __GH(h)->Height);    /* Set new width */
    return 1;
}

uint8_t __GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height) {
    __GH(h)->Flags |= GUI_FLAG_HEIGHT_PERCENT;      /* Set percentage flag */
    __GH(h)->Height = height + 1;                   /* Invalidate widget */
    __GUI_WIDGET_SetSize(h, __GH(h)->Width, height);    /* Set new height */
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
    if (__GH(h)->Flags & GUI_FLAG_HIDDEN) {         /* If hidden, show it */
        __GH(h)->Flags &= ~GUI_FLAG_HIDDEN;
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate it for redraw with parent */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Hide(GUI_HANDLE_p h) {
    if (!(__GH(h)->Flags & GUI_FLAG_HIDDEN)) {      /* If visible, hide it */
        __GH(h)->Flags |= GUI_FLAG_HIDDEN;
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
    for (h = __GH(h)->Parent; h; h = __GH(h)->Parent) { /* Check widget parent objects */
        if (parent == h) {                          /* If they matches */
            return 1;
        }
    }
    return 0;
}

uint8_t __GUI_WIDGET_SetColor(GUI_HANDLE_p h, uint8_t index, GUI_Color_t color) {
    uint8_t ret = 1;
    if (!__GH(h)->Colors) {                         /* Do we need to allocate color memory? */
        if (__GH(h)->Widget->ColorsCount) {         /* Check if at least some colors should be used */
            __GH(h)->Colors = __GUI_MEMALLOC(__GH(h)->Widget->ColorsCount * sizeof(*__GH(h)->Colors));
            if (__GH(h)->Colors) {                  /* Copy all colors to new memory first */
                memcpy(__GH(h)->Colors, __GH(h)->Widget->Colors, __GH(h)->Widget->ColorsCount * sizeof(*__GH(h)->Colors));
            } else {
                ret = 0;
            }
        } else {
            ret = 0;
        }
    }
    if (ret) {
        if (index <__GH(h)->Widget->ColorsCount) {  /* Index in valid range */
            __GH(h)->Colors[index] = color;         /* Set new color */
        } else {
            ret = 0;
        }
    }
    return ret;
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
    if (__GH(h)->Parent) {                          /* Move its parent to the bottom of parent linked list */
        GUI_HANDLE_p parent = __GH(h)->Parent;
        for (; parent; parent = __GH(parent)->Parent) {
            if (__GUI_LINKEDLIST_WidgetMoveToBottom(parent)) {  /* If move down was successful */
                __GUI_WIDGET_Invalidate(parent);    /* Invalidate parent of widget */
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
    
    return __GH(h)->TextMemSize;                    /* Return number of bytes allocated */
}

uint8_t GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_FreeTextMemory(h);           /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetText(h, text);            /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

const GUI_Char* GUI_WIDGET_GetText(GUI_HANDLE_p h) {
    GUI_Char* t;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = __GH(h)->Text;                              /* Return text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;
}

const GUI_Char* GUI_WIDGET_GetTextCopy(GUI_HANDLE_p h, GUI_Char* dst, uint32_t len) {
   GUI_Char* t;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = __GH(h)->Text;                              /* Return text */
    GUI_STRING_CopyN(dst, t, len);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;  
}

uint8_t GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetSize(h, width, height);   /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetWidth(h, width);          /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetHeight(h, height);        /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetWidthPercent(h, width);   /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetHeightPercent(h, height); /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetXY(h, x, y);              /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetFont(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetFont(h, font);            /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_Invalidate(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_Invalidate(h);               /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_Show(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_Show(h);                     /* Show widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_Hide(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_Hide(h);                     /* Hide widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_IsChildOf(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    return __GUI_WIDGET_IsChildOf(h, parent);       /* Return status */
}

uint8_t GUI_WIDGET_SetCallback(GUI_HANDLE_p h, GUI_WIDGET_CALLBACK_t callback) {
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GH(h)->Callback = callback;                   /* Set callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_WIDGET_ProcessDefaultCallback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h);                          /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GH(h)->Widget->Callback(h, ctrl, param, result);    /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

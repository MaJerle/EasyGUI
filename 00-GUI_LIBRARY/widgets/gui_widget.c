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
typedef struct {
    const GUI_FONT_t* Font;
} GUI_WIDGET_Def_t;
GUI_WIDGET_Def_t WIDGET_Default;

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
    if (__GUI_WIDGET_AllowChildren(h)) {            /* Check if children widgets are allowed */
        GUI_HANDLE_p h2;
        do {
            /* Always get first widget from list because linked lists are removed later */
            /* And chain from current to next does not exists anymore */
            if ((h2 = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)h, NULL)) != 0) {
                __RemoveWidget(h2);
            }
        } while (h2);
    }
    
    /* Check focus state */
    if (GUI.FocusedWidget == h) {
        if (__GH(h)->Parent) {
            GUI.FocusedWidget = __GH(h)->Parent;    /* Set parent widget as focused */
        } else {
            __GUI_WIDGET_FOCUS_CLEAR();             /* Clear all widgets from focused */
            GUI.FocusedWidget = 0;
        }
    }
    if (GUI.FocusedWidgetPrev == h) {
        GUI.FocusedWidgetPrev = 0;
    }
    
    /**
     * Remove actual widget
     */
    if (GUI.ActiveWidget == h) {
        GUI.ActiveWidget = 0;
    }
    if (GUI.ActiveWidgetPrev == h) {
        GUI.ActiveWidgetPrev = __GH(h)->Parent;
    }
    
    __GUI_WIDGET_InvalidateWithParent(h);           /* Invalidate object and its parent */
    __GUI_WIDGET_FreeTextMemory(h);                 /* Free text memory */
    if (__GH(h)->Timer) {                           /* Check timer memory */
        __GUI_TIMER_Remove(&__GH(h)->Timer);        /* Free timer memory */
    }
    if (__GH(h)->Colors) {                          /* Free allocate colors if exists */
        __GUI_MEMFREE(__GH(h)->Colors);
    }
    __GUI_LINKEDLIST_WidgetRemove(h);               /* Remove entry from linked list */
    __GUI_MEMFREE(h);                               /* Free memory for widget */
}

/* Recursive function to delete all widgets with checking for flag */
static
void __RemoveWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h;
    if (parent && __GH(parent)->Flags & GUI_FLAG_REMOVE) {
        __RemoveWidget(parent);                     /* Remove widgets and all children */
        return;
    }

    for (h = __GUI_LINKEDLIST_WidgetGetNext((GUI_HANDLE_ROOT_t *)parent, 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(NULL, h)) {
        if (__GUI_WIDGET_AllowChildren(h)) {        /* Check children if any has flag */
            __RemoveWidgets(h);                     /* Run recursive function */
        } else if (__GH(parent)->Flags & GUI_FLAG_REMOVE) {
            __RemoveWidget(parent);                 /* Remove widget directly */
        }
    }
}

static
void __GUI_WIDGET_SetClippingRegion(GUI_HANDLE_p h) {
    GUI_Dim_t x1, y1, x2, y2;
    
    __GUI_WIDGET_GetLCDAbsPosAndVisibleWidthHeight(h, &x1, &y1, &x2, &y2);  /* Get visible widget part and absolute position on screen */
    
    /* Set invalid clipping region */
    if (GUI.Display.X1 > x1) {
        GUI.Display.X1 = x1;
    }
    if (GUI.Display.X2 < (x2)) {
        GUI.Display.X2 = (x2);
    }
    if (GUI.Display.Y1 > y1) {
        GUI.Display.Y1 = y1;
    }
    if (GUI.Display.Y2 < (y2)) {
        GUI.Display.Y2 = (y2);
    }
}

static
uint8_t __GUI_WIDGET_InvalidatePrivate(GUI_HANDLE_p h, uint8_t setclipping) {
    GUI_HANDLE_p h1, h2;
    GUI_iDim_t h1x1, h1x2, h2x1, h2x2;
    GUI_iDim_t h1y1, h1y2, h2y1, h2y2;
    
    if (!h) {
        return 0;
    }
    
    h1 = h;                                         /* Get widget handle */
    if (__GH(h1)->Flags & GUI_FLAG_IGNORE_INVALIDATE) { /* Check ignore flag */
        return 0;                                   /* Ignore invalidate process */
    }
    __GH(h1)->Flags |= GUI_FLAG_REDRAW;             /* Redraw widget */
    GUI.Flags |= GUI_FLAG_REDRAW;                   /* Notify stack about redraw operations */
    
    if (setclipping) {
        __GUI_WIDGET_SetClippingRegion(h);          /* Set clipping region for widget redrawing operation */
    }
    
    /**
     * Invalid only widget with higher Z-index (lowered on linked list) of current object
     * 
     * If widget should be redrawn, then any widget above it should be redrawn too, otherwise z-index match will fail
     *
     * Widget may not need redraw operation if positions don't match
     *
     * If widget is transparent, check all widgets, even those which are below current widget in list
     * Get first element of parent linked list for checking
     */
    if (__GUI_WIDGET_IsTransparent(h1)) {
        __GUI_WIDGET_InvalidatePrivate(__GH(h1)->Parent, 0);    /* Invalidate parent widget */
    }
    for (; h1; h1 = __GUI_LINKEDLIST_WidgetGetNext(0, h1)) {
        __GUI_WIDGET_GetLCDAbsPosAndVisibleWidthHeight(h1, &h1x1, &h1y1, &h1x2, &h1y2);
        for (h2 = __GUI_LINKEDLIST_WidgetGetNext(0, h1); h2; h2 = __GUI_LINKEDLIST_WidgetGetNext(0, h2)) {
            __GUI_WIDGET_GetLCDAbsPosAndVisibleWidthHeight(h2, &h2x1, &h2y1, &h2x2, &h2y2);
            if (
                __GH(h2)->Flags & GUI_FLAG_REDRAW ||    /* Bit is already set */
                !__GUI_RECT_MATCH(
                    h1x1, h1y1, h1x2, h1y2,
                    h2x1, h2y1, h2x2, h2y2)
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
    
    /**
     * Check if any of parent widgets has transparency = should be invalidated too
     *
     * Since recursion is used, call function only once and recursion will take care for upper level of parent widgets
     */
    for (h = __GH(h)->Parent; h; h = __GH(h)->Parent) {
        if (__GUI_WIDGET_IsTransparent(h)) {
            __GUI_WIDGET_InvalidatePrivate(h, 0);
            break;
        }
    }
    
    return 1;
}

static
GUI_HANDLE_p __GetWidgetById(GUI_HANDLE_p parent, GUI_ID_t id, uint8_t deep) {
    GUI_HANDLE_p h;
    
    for (h = __GUI_LINKEDLIST_WidgetGetNext(__GHR(parent), 0); h; h = __GUI_LINKEDLIST_WidgetGetNext(0, h)) {
        if (__GUI_WIDGET_GetId(h) == id) {          /* Compare ID values */
            return h;
        } else if (deep && __GUI_WIDGET_AllowChildren(h)) { /* Check children if possible */
            GUI_HANDLE_p tmp = __GetWidgetById(h, id, deep);
            if (tmp) {
                return tmp;
            }
        }
    }
    return 0;
}

/* Returns first common widget between 2 widgets */
static
GUI_HANDLE_p __GetCommonParentWidget(GUI_HANDLE_p h1, GUI_HANDLE_p h2) {
    GUI_HANDLE_p tmp;
    
    for (; h1; h1 = __GH(h1)->Parent) {             /* Process all entries */
        for (tmp = h2; tmp; tmp = __GH(tmp)->Parent) {
            if (h1 == tmp) {
                return tmp;
            }
        }
    }
    return GUI.Root.First;                          /* Return bottom widget on list */
}

uint8_t __GUI_WIDGET_GetLCDAbsPosAndVisibleWidthHeight(GUI_HANDLE_p h, GUI_iDim_t* x1, GUI_iDim_t* y1, GUI_iDim_t* x2, GUI_iDim_t* y2) {
    GUI_iDim_t x, y, wi, hi;
    
    x = __GUI_WIDGET_GetAbsoluteX(h);               /* Get absolute X position */
    y = __GUI_WIDGET_GetAbsoluteY(h);               /* Get absolute Y position */
    wi = __GUI_WIDGET_GetWidth(h);                  /* Get absolute width */
    hi = __GUI_WIDGET_GetHeight(h);                 /* Get absolute height */
    
    *x1 = x;
    *y1 = y;
    *x2 = x + wi;
    *y2 = y + hi;
    
    if (*x1 < x)            { *x1 = x; }
    if (*x2 > x + wi)       { *x2 = x + wi; }
    if (*y1 < y)            { *y1 = y; }
    if (*y2 > y + hi)       { *y2 = y + hi; }
    
    /* Check that widget is not drawn over any of parent widgets because of scrolling */
    for (; h; h = __GH(h)->Parent) {
        x = __GUI_WIDGET_GetParentAbsoluteX(h);     /* Parent absolute X position for inner widgets */
        y = __GUI_WIDGET_GetParentAbsoluteY(h);     /* Parent absolute Y position for inner widgets */
        wi = __GUI_WIDGET_GetParentInnerWidth(h);   /* Get parent inner width */
        hi = __GUI_WIDGET_GetParentInnerHeight(h);  /* Get parent inner height */
    
        if (*x1 < x)        { *x1 = x; }
        if (*x2 > x + wi)   { *x2 = x + wi; }
        if (*y1 < y)        { *y1 = y; }
        if (*y2 > y + hi)   { *y2 = y + hi; }
    }
    
    return 1;
}

/* Check if widget is inside LCD invalidate region */
uint8_t __GUI_WIDGET_IsInsideClippingRegion(GUI_HANDLE_p h) {
    GUI_iDim_t x1, y1, x2, y2;
    __GUI_WIDGET_GetLCDAbsPosAndVisibleWidthHeight(h, &x1, &y1, &x2, &y2);
    return __GUI_RECT_MATCH(
        x1, y1, x2, y2, 
        GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2, GUI.Display.Y2
    );
}

void __GUI_WIDGET_Init(void) {
    GUI_WINDOW_CreateDesktop(GUI_ID_WINDOW_BASE, NULL); /* Create base window object */
}

uint8_t __GUI_WIDGET_ExecuteRemove(void) {
    if (GUI.Flags & GUI_FLAG_REMOVE) {              /* Anything to remove? */
        __RemoveWidgets(NULL);                      /* Remove widgets */
        GUI.Flags &= ~GUI_FLAG_REMOVE;
        return 1;
    }
    return 0;
}

GUI_Dim_t __GUI_WIDGET_GetWidth(GUI_HANDLE_p h) {
    if (__GH(h)->Flags & GUI_FLAG_EXPANDED) {       /* Maximize window over parent */
        return __GUI_WIDGET_GetParentInnerWidth(h); /* Return parent inner width */
    } else if (__GH(h)->Flags & GUI_FLAG_WIDTH_FILL) {  /* "fill_parent" mode for width */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerWidth(h);
        if (parent > __GUI_WIDGET_GetRelativeX(h)) {
            return parent - __GUI_WIDGET_GetRelativeX(h);   /* Return widget width */
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
    if (__GH(h)->Flags & GUI_FLAG_EXPANDED) {       /* Maximize window over parent */
        return __GUI_WIDGET_GetParentInnerHeight(h);/* Return parent inner height */
    } else if (__GH(h)->Flags & GUI_FLAG_HEIGHT_FILL) { /* "fill_parent" mode for height */
        GUI_Dim_t parent = __GUI_WIDGET_GetParentInnerHeight(h);
        if (parent > __GUI_WIDGET_GetRelativeY(h)) {
            return parent - __GUI_WIDGET_GetRelativeY(h);   /* Return widget width */
        }
    } else if (__GH(h)->Flags & GUI_FLAG_HEIGHT_PERCENT) {  /* Percentage height */
        return __GH(h)->Height * __GUI_WIDGET_GetParentInnerHeight(h) / 100;
    } else {                                        /* Normal height */
        return __GH(h)->Height;
    }
    return 0;
}

GUI_iDim_t __GUI_WIDGET_GetAbsoluteX(GUI_HANDLE_p h) {
    GUI_HANDLE_p w = 0;
    GUI_Dim_t out = 0;
    
    if (!h) {                                       /* Check input value */
        return 0;                                   /* At left value */
    }
    
    /* If widget is not expanded, use actual value */
    if (!__GUI_WIDGET_IsExpanded(h)) {
        out = __GH(h)->X;                           /* Actual widget X value */
    }
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = __GH(h)->Parent; w; w = __GH(w)->Parent) { /* Go through all parent windows */
        out += __GUI_WIDGET_GetRelativeX(w) + __GUI_WIDGET_GetPaddingLeft(w);   /* Add X offset from parent and left padding of parent */
        out -= __GHR(w)->ScrollX;                   /* Decrease by scroll value */
    }
    return out;
}

GUI_iDim_t __GUI_WIDGET_GetAbsoluteY(GUI_HANDLE_p h) {
    GUI_HANDLE_p w = 0;
    GUI_Dim_t out = 0;
    
    if (!h) {                                       /* Check input value */
        return 0;                                   /* At top value */
    }
    
    /* If widget is not expanded, use actual value */
    if (!__GUI_WIDGET_IsExpanded(h)) {
        out = __GH(h)->Y;                           /* Actual widget X value */
    }
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = __GH(h)->Parent; w; w = __GH(w)->Parent) { /* Go through all parent windows */
        out += __GUI_WIDGET_GetRelativeY(w) + __GUI_WIDGET_GetPaddingTop(w);    /* Add X offset from parent and left padding of parent */
        out -= __GHR(w)->ScrollX;                   /* Decrease by scroll value */
    }
    return out;
}

GUI_iDim_t __GUI_WIDGET_GetParentAbsoluteX(GUI_HANDLE_p h) {
    GUI_Dim_t out = 0;
    
    if (h) {                                        /* Check valid widget */
        h = __GH(h)->Parent;                        /* Get parent of widget */
        if (h) {                                    /* Save left padding */
            out = __GUI_WIDGET_GetPaddingLeft(h);   /* Get left padding from parent widget */
        }
    }
    out += __GUI_WIDGET_GetAbsoluteX(h);            /* Add absolute X of parent and to current padding */
    return out;
}

GUI_iDim_t __GUI_WIDGET_GetParentAbsoluteY(GUI_HANDLE_p h) {
    GUI_Dim_t out = 0;
    
    if (h) {                                        /* Check valid widget */
        h = __GH(h)->Parent;                        /* Get parent of widget */
        if (h) {                                    /* Save left padding */
            out = __GUI_WIDGET_GetPaddingTop(h);    /* Get top padding from parent widget */
        }
    }
    out += __GUI_WIDGET_GetAbsoluteY(h);            /* Add absolute Y of parent and to current padding */
    return out;
}

uint8_t __GUI_WIDGET_Invalidate(GUI_HANDLE_p h) {
    uint8_t ret = __GUI_WIDGET_InvalidatePrivate(h, 1); /* Invalidate widget with clipping */
    
    if (
        (
            __GH(h)->Flags & GUI_FLAG_WIDGET_INVALIDATE_PARENT || 
            __GH(h)->Widget->Flags & GUI_FLAG_WIDGET_INVALIDATE_PARENT ||
            __GUI_WIDGET_IsTransparent(h)           /* At least little transparent */
        ) && __GH(h)->Parent) {
        __GUI_WIDGET_InvalidatePrivate(__GH(h)->Parent, 0); /* Invalidate parent object too but without clipping */
    }
    return ret;
}

uint8_t __GUI_WIDGET_InvalidateWithParent(GUI_HANDLE_p h) {
    __GUI_WIDGET_InvalidatePrivate(h, 1);           /* Invalidate object with clipping */
    if (__GH(h)->Parent) {                          /* If parent exists, invalid only parent */
        __GUI_WIDGET_InvalidatePrivate(__GH(h)->Parent, 0); /* Invalidate parent object without clipping */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetInvalidateWithParent(GUI_HANDLE_p h, uint8_t value) {
    if (value) {                                    /* On positive value */
        __GH(h)->Flags |= GUI_FLAG_WIDGET_INVALIDATE_PARENT;    /* Enable auto invalidation of parent widget */
    } else {                                        /* On zero */
        __GH(h)->Flags &= ~GUI_FLAG_WIDGET_INVALIDATE_PARENT;   /* Disable auto invalidation of parent widget */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {       
    if (__GH(h)->X != x || __GH(h)->Y != y) {
        if (!__GUI_WIDGET_IsExpanded(h)) {
            __GUI_WIDGET_InvalidateWithParent(h);   /* Set old clipping region first */
        }
        __GH(h)->X = x;                             /* Set parameter */
        __GH(h)->Y = y;                             /* Set parameter */
        if (!__GUI_WIDGET_IsExpanded(h)) {
            __GUI_WIDGET_InvalidateWithParent(h);   /* Set new clipping region */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi) {    
    if (wi != __GH(h)->Width || hi != __GH(h)->Height) {
        uint8_t invalidateSecond = 0;
        if (!__GUI_WIDGET_IsExpanded(h)) {
            __GUI_WIDGET_InvalidateWithParent(h);   /* Set old clipping region first */
            if (wi > __GH(h)->Width || hi > __GH(h)->Height) {
                invalidateSecond = 1;
            }
        }
        __GH(h)->Width = wi;                        /* Set parameter */
        __GH(h)->Height = hi;                       /* Set parameter */
        if (invalidateSecond) {                     /* Invalidate second time only if widget greater than before */
            __GUI_WIDGET_InvalidateWithParent(h);   /* Set new clipping region */
        }
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
    size_t len, tlen;
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

GUI_HANDLE_p __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_HANDLE_p h;
    GUI_Byte result = 0;
    
    __GUI_ASSERTPARAMS(widget && widget->Callback); /* Check input parameters */
    
    /**
     * Allocation size check:
     * 
     * - Size must be at least for widget size
     * - If widget supports children widgets, size must be for at least parent handle structure
     */
    if (widget->Size < sizeof(GUI_HANDLE) ||
        ((widget->Flags & GUI_FLAG_WIDGET_ALLOW_CHILDREN) && widget->Size < sizeof(GUI_HANDLE_ROOT_t))) { 
        return 0;
    }
    
    h = (GUI_HANDLE_p)__GUI_MEMALLOC(widget->Size); /* Allocate memory for widget */
    if (h) {
        memset(h, 0x00, widget->Size);              /* Set memory to 0 */
        
        __GH(h)->Id = id;                           /* Save ID */
        __GH(h)->Widget = widget;                   /* Widget object structure */
        __GH(h)->Footprint = GUI_WIDGET_FOOTPRINT;  /* Set widget footprint */
        __GH(h)->Callback = cb;                     /* Set widget callback */
        __GH(h)->Transparency = 0xFF;               /* Set full transparency by default */
        
        /**
         * Parent window check
         *
         * - Dialog's parent widget is desktop widget
         * - If flag for parent desktop is set, parent widget is also desktop
         * - Otherwise parent widget passed as parameter is used if it supports children widgets
         */
        if (__GUI_WIDGET_IsDialogBase(h) || flags & GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP) {/* Dialogs do not have parent widget */
            __GH(h)->Parent = GUI_WINDOW_GetDesktop();  /* Set parent object */
        } else {
            if (parent && __GUI_WIDGET_AllowChildren(parent)) {
                __GH(h)->Parent = parent;
            } else {
                __GH(h)->Parent = GUI.WindowActive; /* Set parent object. It will be NULL on first call */
            }
        }
        
        result = 1;                                 /* We are OK at starting point */
        __GUI_WIDGET_Callback(h, GUI_WC_PreInit, NULL, &result);    /* Notify internal widget library about init successful */
        
        if (!result) {                              /* Check result */
            __GUI_MEMFREE(h);                       /* Clear widget memory */
            h = 0;                                  /* Reset handle */
            return 0;                               /* Stop execution at this point */
        }
        
        /* Set widget default values */
        __GH(h)->Font = WIDGET_Default.Font;        /* Set default font */
        
        __GH(h)->Flags |= GUI_FLAG_IGNORE_INVALIDATE;   /* Ignore invalidate flag */
        __GUI_WIDGET_SetSize(h, width, height);     /* Set widget size */
        __GUI_WIDGET_SetXY(h, x, y);                /* Set widget position */
        __GH(h)->Flags &= ~GUI_FLAG_IGNORE_INVALIDATE;  /* Ignore invalidate flag */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate properly now when everything is set correctly = set for valid clipping region part */

        result = 0;
        __GUI_WIDGET_Callback(h, GUI_WC_ExcludeLinkedList, 0, &result);
        if (!result) {                              /* Check if widget should be added to linked list */
            __GUI_LINKEDLIST_WidgetAdd((GUI_HANDLE_ROOT_t *)__GH(h)->Parent, h);    /* Add entry to linkedlist of parent widget */
        }
        __GUI_WIDGET_Callback(h, GUI_WC_Init, NULL, NULL);  /* Notify user about init successful */
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

uint8_t __GUI_WIDGET_Remove(GUI_HANDLE_p h) {
    if (__GUI_WIDGET_CanRemove(h)) {                /* Check if we can delete widget */ 
        __GH(h)->Flags |= GUI_FLAG_REMOVE;          /* Set flag and delete it later */
        GUI.Flags |= GUI_FLAG_REMOVE;               /* Set flag for to remove at least one widget from tree */
        if (__GUI_WIDGET_IsFocused(h)) {
            __GUI_WIDGET_FOCUS_SET(__GH(h)->Parent);
        }
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
        __GUI_WIDGET_FOCUS_SET(__GH(h)->Parent);    /* Set parent widget as focused now */
    }
    if (GUI.ActiveWidget == h || __GUI_WIDGET_IsChildOf(GUI.ActiveWidget, h)) { /* Clear active */
        __GUI_WIDGET_ACTIVE_CLEAR();
    }
    return 1;
}

uint8_t __GUI_WIDGET_ToggleExpanded(GUI_HANDLE_p h) {
    if (__GUI_WIDGET_IsExpanded(h)) {               /* Check current status */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Redraw everything in parent */
        __GH(h)->Flags &= ~GUI_FLAG_EXPANDED;       /* Clear expanded */
    } else {
        __GH(h)->Flags |= GUI_FLAG_EXPANDED;        /* Expand widget */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw only selected widget as it is over all window */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetExpanded(GUI_HANDLE_p h, uint8_t state) {
    if (!state && __GUI_WIDGET_IsExpanded(h)) {     /* Check current status */
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate with parent first for clipping region */
        __GH(h)->Flags &= ~GUI_FLAG_EXPANDED;       /* Clear expanded */
    } else if (state && !__GUI_WIDGET_IsExpanded(h)) {
        __GH(h)->Flags |= GUI_FLAG_EXPANDED;        /* Expand widget */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw only selected widget as it is over all window */
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

uint8_t __GUI_WIDGET_SetZIndex(GUI_HANDLE_p h, int32_t zindex) {
    uint8_t ret = 1;
    if (__GH(h)->ZIndex != zindex) {                /* There was a change in z-index value */
        int32_t current = __GH(h)->ZIndex;
        __GH(h)->ZIndex = zindex;                   /* Set new index */
        if (zindex < current) {                     /* New index value is less important than before = move widget to top */
            __GUI_LINKEDLIST_WidgetMoveToTop(h);    /* Move widget to top on linked list = less important and less visible */
        } else {
            __GUI_LINKEDLIST_WidgetMoveToBottom(h); /* Move widget to bottom on linked list = most important and most visible */
        }
    }
    return ret;
}

uint8_t __GUI_WIDGET_SetTransparency(GUI_HANDLE_p h, uint8_t trans) {
    if (__GH(h)->Transparency != trans) {           /* Check transparency match */
        __GH(h)->Transparency = trans;              /* Set new transparency level */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    return 1;
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
        if (index < __GH(h)->Widget->ColorsCount) { /* Index in valid range */
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

/* Clear widget focus */
void __GUI_WIDGET_FOCUS_CLEAR(void) {
    if (GUI.FocusedWidget && GUI.FocusedWidget != GUI.Root.First) { /* First widget is always in focus */
        GUI.FocusedWidgetPrev = GUI.FocusedWidget;  /* Clear everything */
        do {
            __GUI_WIDGET_Callback(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);
            __GH(GUI.FocusedWidget)->Flags &= ~GUI_FLAG_FOCUS;
            __GUI_WIDGET_Invalidate(GUI.FocusedWidget);
            GUI.FocusedWidget = __GH(GUI.FocusedWidget)->Parent;    /* Get parent widget */
        } while (GUI.FocusedWidget != GUI.Root.First);
        GUI.FocusedWidget = 0;
    }
}

/* Set focus on widget */
void __GUI_WIDGET_FOCUS_SET(GUI_HANDLE_p h) {
    GUI_HANDLE_p common = NULL;
    
    if (GUI.FocusedWidget == h) {                   /* Check current focused widget */
        return;
    }
    
    /**
     * TODO: Check if widget is in list for remove or any parent of it
     */
    
    /**
     * Step 1:
     *
     * Identify common parent from new and old focused widget
     * Remove focused flag on widget which are not in tree between old and new widgets
     */
    if (GUI.FocusedWidget) {                        /* We already have one widget in focus */
        common = __GetCommonParentWidget(GUI.FocusedWidget, h); /* Get first widget in common */
        if (common) {                               /* We have common object, invalidate only those which are not common in tree */
            for (; GUI.FocusedWidget && common && GUI.FocusedWidget != common; GUI.FocusedWidget = __GH(GUI.FocusedWidget)->Parent) {
                __GH(GUI.FocusedWidget)->Flags &= ~GUI_FLAG_FOCUS;  /* Clear focused flag */
                __GUI_WIDGET_Callback(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);  /* Notify with callback */
                __GUI_WIDGET_Invalidate(GUI.FocusedWidget); /* Invalidate widget */
            }
        }
    } else {
        common = GUI.Root.First;                    /* Get bottom widget */
    }
    
    /**
     * Step 2:
     *
     * Set new widget as focused
     * Set all widget from common to current as focused
     */ 
    GUI.FocusedWidget = h;                          /* Set new focused widget */
    while (h && common && h != common) {
        __GH(h)->Flags |= GUI_FLAG_FOCUS;           /* Set focused flag */
        __GUI_WIDGET_Callback(h, GUI_WC_FocusIn, NULL, NULL);   /* Notify with callback */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
        h = __GH(h)->Parent;                        /* Get parent widget */
    }
}

/* Clear active widget status */
void __GUI_WIDGET_ACTIVE_CLEAR(void) {
    if (GUI.ActiveWidget) {
        __GH(GUI.ActiveWidget)->Flags &= ~(GUI_FLAG_ACTIVE | GUI_FLAG_TOUCH_MOVE);  /* Clear all widget based flags */
        __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_ActiveOut, NULL, NULL);
        GUI.ActiveWidgetPrev = GUI.ActiveWidget;
        GUI.ActiveWidget = 0;
    }
}

/* Set active status to widget */
void __GUI_WIDGET_ACTIVE_SET(GUI_HANDLE_p h) {
    __GUI_WIDGET_ACTIVE_CLEAR();
    GUI.ActiveWidget = h;
    __GH(GUI.ActiveWidget)->Flags |= GUI_FLAG_ACTIVE;
    __GUI_WIDGET_Callback(GUI.ActiveWidget, GUI_WC_ActiveIn, NULL, NULL);
}

/******************************************************************************/
/******************************************************************************/
/***                  Thread safe version of public API                      **/
/******************************************************************************/
/******************************************************************************/
uint8_t GUI_WIDGET_Remove(GUI_HANDLE_p* h) {
    __GUI_ASSERTPARAMS(h && __GUI_WIDGET_IsWidget(*h));     /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(*h);                        /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;                                       /* Removev successfully */
}

uint32_t GUI_WIDGET_AllocTextMemory(GUI_HANDLE_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h) && size > 1);   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_AllocateTextMemory(h, size);       /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return __GH(h)->TextMemSize;                    /* Return number of bytes allocated */
}

uint8_t GUI_WIDGET_FreeTextMemory(GUI_HANDLE_p h) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_FreeTextMemory(h);           /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetText(GUI_HANDLE_p h, const GUI_Char* text) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetText(h, text);            /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

const GUI_Char* GUI_WIDGET_GetText(GUI_HANDLE_p h) {
    GUI_Char* t;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = __GH(h)->Text;                              /* Return text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;
}

const GUI_Char* GUI_WIDGET_GetTextCopy(GUI_HANDLE_p h, GUI_Char* dst, uint32_t len) {
    GUI_Char* t;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = __GH(h)->Text;                              /* Return text */
    GUI_STRING_CopyN(dst, t, len);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;  
}

uint8_t GUI_WIDGET_SetSize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetSize(h, width, height);   /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetWidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetWidth(h, width);          /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetHeight(GUI_HANDLE_p h, GUI_Dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetHeight(h, height);        /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetWidthPercent(GUI_HANDLE_p h, GUI_Dim_t width) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetWidthPercent(h, width);   /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetHeightPercent(GUI_HANDLE_p h, GUI_Dim_t height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetHeightPercent(h, height); /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

GUI_Dim_t GUI_WIDGET_GetWidth(GUI_HANDLE_p h) {
    GUI_Dim_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_GetWidth(h);                 /* Get widget width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

GUI_Dim_t GUI_WIDGET_GetHeight(GUI_HANDLE_p h) {
    GUI_Dim_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_GetHeight(h);                /* Get widget height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetXY(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetXY(h, x, y);              /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_SetFont(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_SetFont(h, font);            /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_Invalidate(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_Invalidate(h);               /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_Show(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_Show(h);                     /* Show widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_PutOnFront(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GUI_WIDGET_MoveDownTree(h);                   /* Put widget on front */
    __GUI_WIDGET_FOCUS_SET(h);                      /* Set widget to focused state */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t GUI_WIDGET_Hide(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = __GUI_WIDGET_Hide(h);                     /* Hide widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

GUI_ID_t GUI_WIDGET_GetId(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    return __GUI_WIDGET_GetId(h);                   /* Return widget ID */
}

GUI_HANDLE_p GUI_WIDGET_GetById(GUI_ID_t id) {
    GUI_HANDLE_p h;
    __GUI_ENTER();                                  /* Enter GUI */
    
    h = __GetWidgetById(NULL, id, 1);               /* Find widget by ID */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h; 
}

uint8_t GUI_WIDGET_IsChildOf(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h) && __GUI_WIDGET_IsWidget(parent));  /* Check valid parameter */
    return __GUI_WIDGET_IsChildOf(h, parent);       /* Return status */
}

uint8_t GUI_WIDGET_SetCallback(GUI_HANDLE_p h, GUI_WIDGET_CALLBACK_t callback) {
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GH(h)->Callback = callback;                   /* Set callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_WIDGET_SetUserData(GUI_HANDLE_p h, void* data) {
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GH(h)->UserData = data;                       /* Set user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

void* GUI_WIDGET_GetUserData(GUI_HANDLE_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    data = __GH(h)->UserData;                       /* Set user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return data;
}

uint8_t GUI_WIDGET_ProcessDefaultCallback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GH(h)->Widget->Callback(h, ctrl, param, result);    /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_Callback(h, ctrl, param, result);    /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetScrollX(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GUI_WIDGET_AllowChildren(h) && __GHR(h)->ScrollX != scroll) { /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollX = scroll;
        __GUI_WIDGET_Invalidate(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetScrollY(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GUI_WIDGET_AllowChildren(h) && __GHR(h)->ScrollY != scroll) { /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollY = scroll;
        __GUI_WIDGET_Invalidate(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetFocus(GUI_HANDLE_p h) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    __GUI_WIDGET_FOCUS_SET(h);                      /* Put widget in focus */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetExpanded(GUI_HANDLE_p h, uint8_t state) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetExpanded(h, state);       /* Set expanded mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_IsExpanded(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_IsExpanded(h);               /* Check expanded mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetFontDefault(const GUI_FONT_t* font) {
    WIDGET_Default.Font = font;                     /* Set default font */
    return 1;
}

uint8_t GUI_WIDGET_IncSelection(GUI_HANDLE_p h, int16_t dir) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_Callback(h, GUI_WC_IncSelection, &dir, &ret);    /* Increase selection for specific amount */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetZIndex(GUI_HANDLE_p h, int32_t zindex) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetZIndex(h, zindex);        /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

int32_t GUI_WIDGET_GetZIndex(GUI_HANDLE_p h) {
    int32_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_GetZIndex(h);                /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_SetTransparency(GUI_HANDLE_p h, uint8_t trans) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetTransparency(h, trans);   /* Set widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_WIDGET_GetTransparency(GUI_HANDLE_p h) {
    uint8_t trans;
    
    __GUI_ASSERTPARAMS(__GUI_WIDGET_IsWidget(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    trans = __GUI_WIDGET_GetTransparency(h);        /* Get widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return trans;
}

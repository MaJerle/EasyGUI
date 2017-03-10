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
#define w     (__GH(ptr)->Width)
#define h     (__GH(ptr)->Height)
void __GUI_WIDGET_SetClippingRegion(void* ptr) {
    GUI_Dim_t x, y;
    
    x = __GUI_WIDGET_GetAbsoluteX(ptr);         /* Get widget absolute X */
    y = __GUI_WIDGET_GetAbsoluteY(ptr);         /* Get widget absolute Y */
    
    /* Set invalid clipping region */
    if (GUI.Display.X1 > x) {
        GUI.Display.X1 = x;
    }
    if (GUI.Display.X2 < (x + w)) {
        GUI.Display.X2 = (x + w);
    }
    if (GUI.Display.Y1 > y) {
        GUI.Display.Y1 = y;
    }
    if (GUI.Display.Y2 < (y + h)) {
        GUI.Display.Y2 = (y + h);
    }
}

uint8_t __GUI_WIDGET_IsInsideClippingRegion(void* ptr) {
    GUI_Dim_t x, y;
    
    x = __GUI_WIDGET_GetAbsoluteX(ptr);         /* Get widget absolute X */
    y = __GUI_WIDGET_GetAbsoluteY(ptr);         /* Get widget absolute Y */

    return __GUI_RECT_MATCH(x, y, w, h, GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2 - GUI.Display.X1, GUI.Display.Y2 - GUI.Display.Y1);
}
#undef w
#undef h

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
void __GUI_WIDGET_Init(void) {
    GUI_WINDOW_Create(GUI_ID_WINDOW_BASE);      /* Create base window object */
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteX(void* ptr) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_t w = 0;
    GUI_Dim_t out = 0;
    
    if (ptr) {
        w = ((GUI_HANDLE_t)ptr)->Parent;
        out = ((GUI_HANDLE_t)ptr)->X;
    }
    
    while (w) {                                 /* Go through all parent windows */
        out += w->X;                            /* Add X offset from parent */
        w = (GUI_HANDLE_t)w->Parent;            /* Get next parent */
    }
    return out;
}

GUI_Dim_t __GUI_WIDGET_GetAbsoluteY(void* ptr) {
    /* Assuming linked list is first element in structure */
    /* Second element is common structure */
    GUI_HANDLE_t w = 0;
    GUI_Dim_t out = 0;
    
    if (ptr) {
        w = ((GUI_HANDLE_t)ptr)->Parent;
        out = ((GUI_HANDLE_t)ptr)->Y;
    }
    
    while (w) {                                 /* Go through all parent windows */
        out += w->Y;                            /* Add X offset from parent */
        w = (GUI_HANDLE_t)w->Parent;            /* Get next parent */
    }
    return out;
}

uint8_t __GUI_WIDGET_Invalidate(void* ptr) {
    GUI_HANDLE_t h1, h2;
    
    h1 = __GH(ptr);                             /* Get widget handle */
    h1->Flags |= GUI_FLAG_REDRAW;               /* Redraw widget */
    
    __GUI_WIDGET_SetClippingRegion(ptr);        /* Set clipping region for widget redrawing operation */
    
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
            h2->Flags |= GUI_FLAG_REDRAW;        /* Redraw widget on next loop */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_InvalidateWithParent(void* ptr) {
    __GUI_WIDGET_Invalidate(ptr);               /* Invalidate object */
    if (__GH(ptr)->Parent) {                    /* If parent exists, invalid only parent */
        __GH(ptr)->Parent->Flags |= GUI_FLAG_REDRAW;    /* Redraw parent widget too */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetXY(void* ptr, GUI_iDim_t x, GUI_iDim_t y) {
    if (__GH(ptr)->X != x || __GH(ptr)->Y != y) {
        GUI_iDim_t pW, pH;
        pW = GUI_WIDGET_GetParentWidth(ptr);
        pH = GUI_WIDGET_GetParentHeight(ptr);
        
        if (x < 0) {                                /* Check X value */
            x = 0;
        } else if (x > (pW - __GH(ptr)->Width)) {
            x = pW - __GH(ptr)->Width;
        }
        if (y < 0) {                                /* Check Y value */
            y = 0;
        } else if (y > (pH - __GH(ptr)->Height)) {
            y = pH - __GH(ptr)->Height;
        }
        
        if (__GH(ptr)->X != x || __GH(ptr)->Y != y) {            
            __GUI_WIDGET_SetClippingRegion(ptr);    /* Set new clipping region */
            __GH(ptr)->X = x;                       /* Set parameter */
            __GH(ptr)->Y = y;                       /* Set parameter */
            __GUI_WIDGET_InvalidateWithParent(ptr); /* Invalidate object */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetSize(void* ptr, GUI_Dim_t width, GUI_Dim_t height) {
    if (__GH(ptr)->Width != width || __GH(ptr)->Height != height) {
        GUI_Dim_t pW, pH;
            
        pW = GUI_WIDGET_GetParentWidth(ptr);        /* Get parent width */
        pH = GUI_WIDGET_GetParentHeight(ptr);       /* Get parent height */
        
        if (__GH(ptr)->X + width > pW) {            /* Check if widget width is too big */
            width = pW - __GH(ptr)->X;
        }
        if (__GH(ptr)->Y + height > pH) {           /* Check if widget height is too big */
            height = pH - __GH(ptr)->Y;
        }

        __GUI_WIDGET_SetClippingRegion(ptr);        /* Set clipping region before changed position */
        __GH(ptr)->Width = width;                   /* Set parameter */
        __GH(ptr)->Height = height;                 /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(ptr);     /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Enable3DStyle(void* ptr) {
    if (!(__GH(ptr)->Flags & GUI_FLAG_3D)) {
        __GH(ptr)->Flags |= GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(ptr);               /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_Disable3DStyle(void* ptr) {
    if (__GH(ptr)->Flags & GUI_FLAG_3D) {
        __GH(ptr)->Flags &= ~GUI_FLAG_3D;
        __GUI_WIDGET_Invalidate(ptr);               /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetFont(void* ptr, GUI_Const GUI_FONT_t* font) {
    if (__GH(ptr)->Font != font) {                  /* Any parameter changed */
        __GH(ptr)->Font = font;                     /* Set parameter */
        __GUI_WIDGET_InvalidateWithParent(ptr);     /* Invalidate object */
    }
    return 1;
}

uint8_t __GUI_WIDGET_SetText(void* ptr, const char* text) {
   if (__GH(ptr)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) {  /* Memory for text is dynamically allocated */
        if (__GH(ptr)->TextMemSize) {
            if (strlen(text) > (__GH(ptr)->TextMemSize - 1)) {  /* Check string length */
                strncpy(__GH(ptr)->Text, text, __GH(ptr)->TextMemSize - 1); /* Do not copy all bytes because of memory overflow */
            } else {
                strcpy(__GH(ptr)->Text, text);      /* Copy entire string */
            }
            __GUI_WIDGET_Invalidate(__GH(ptr));     /* Redraw object */
        }
    } else {                                        /* Memory allocated by user */
        if (__GH(ptr)->Text && __GH(ptr)->Text == text) {   /* In case the same pointer is passed to button */
            if (strcmp(__GH(ptr)->Text, text)) {    /* If strings does not match, source string updated? */
                __GUI_WIDGET_Invalidate(__GH(ptr)); /* Redraw object */
            }
        }
        
        if (__GH(ptr)->Text != text) {              /* Check if pointer do not match */
            __GH(ptr)->Text = (char *)text;         /* Set parameter */
            __GUI_WIDGET_Invalidate(ptr);           /* Redraw object */
        }
    }
    return 1;
}

uint8_t __GUI_WIDGET_AllocateTextMemory(void* ptr, uint16_t size) {
    if ((__GH(ptr)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && __GH(ptr)->Text) {    /* Check if already allocated */
        __GUI_MEMFREE(__GH(ptr)->Text);             /* Free memory first */
        __GH(ptr)->TextMemSize = 0;                 /* Reset memory size */
    }
    __GH(ptr)->Text = 0;                            /* Reset pointer */
    
    __GH(ptr)->TextMemSize = size * sizeof(char);   /* Allocate text memory */
    __GH(ptr)->Text = (char *)__GUI_MEMALLOC(__GH(ptr)->TextMemSize);   /* Allocate memory for text */
    if (__GH(ptr)->Text) {                          /* Check if allocated */
        __GH(ptr)->Flags |= GUI_FLAG_DYNAMICTEXTALLOC;  /* Dynamically allocated */
    } else {
        __GH(ptr)->TextMemSize = 0;                 /* No dynamic bytes available */
        __GH(ptr)->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC; /* Not allocated */
    }
    __GUI_WIDGET_Invalidate(ptr);                   /* Redraw object */
    return 1;
}

uint8_t __GUI_WIDGET_FreeTextMemory(void* ptr) {
    if ((__GH(ptr)->Flags & GUI_FLAG_DYNAMICTEXTALLOC) && __GH(ptr)->Text) {    /* Check if dynamically alocated */
        __GUI_MEMFREE(__GH(ptr)->Text);             /* Free memory first */
        __GH(ptr)->Text = 0;                        /* Reset memory */
        __GH(ptr)->TextMemSize = 0;                 /* Reset memory size */
        __GH(ptr)->Flags &= ~GUI_FLAG_DYNAMICTEXTALLOC; /* Not allocated */
        __GUI_WIDGET_Invalidate(ptr);               /* Redraw object */
    }
    return 1;
}

GUI_HANDLE_t __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_HANDLE_t ptr;
    
    __GUI_ASSERTPARAMS(widget);                     /* Check input parameters */
    
    __GUI_MEMWIDALLOC(ptr, widget->MetaData.WidgetSize);    /* Allocate memory for widget */
    if (ptr) {  
        GUI_Dim_t pW, pH;                           /* Parent width and height values */
        
        ptr->Id = id;                               /* Save ID */
        ptr->Widget = widget;                       /* Widget object structure */
        ptr->Parent = GUI.WindowActive;             /* Set parent window */

        /* Coordinates management */
        if (ptr->Parent) {                          /* Get object width from parent */
            pW = ptr->Parent->Width;
            pH = ptr->Parent->Height;
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
        
        ptr->X = x;                                 /* Set X position relative to parent */
        ptr->Y = y;                                 /* Set Y position relative to parent */
        ptr->Width = width;                         /* Set widget width */
        ptr->Height = height;                       /* Set widget height */

        __GUI_LINKEDLIST_ADD((GUI_HANDLE_ROOT_t *)ptr->Parent, ptr);    /* Add entry to linkedlist of parent widget */
        __GUI_WIDGET_Invalidate(ptr);               /* Invalidate object */
    }
    
    return ptr;
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

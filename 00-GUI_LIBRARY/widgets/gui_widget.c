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

#if GUI_OS
static gui_mbox_msg_t msg_widget_remove = {GUI_SYS_MBOX_TYPE_REMOVE};
static gui_mbox_msg_t msg_widget_invalidate = {GUI_SYS_MBOX_TYPE_INVALIDATE};
#endif /* GUI_OS */

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
uint8_t __RemoveWidget(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /* Check focus state */
    if (GUI.FocusedWidget == h) {
        if (__GH(h)->Parent) {                      /* Should always happen as top parent (window) can't be removed */
            GUI.FocusedWidget = __GH(h)->Parent;    /* Set parent widget as focused */
        } else {
            __GUI_WIDGET_FOCUS_CLEAR();             /* Clear all widgets from focused */
            GUI.FocusedWidget = 0;
        }
    }
    if (GUI.FocusedWidgetPrev == h) {
        GUI.FocusedWidgetPrev = 0;
    }
    if (GUI.ActiveWidget == h) {
        GUI.ActiveWidget = 0;                       /* Invalidate active widget */
    }
    if (GUI.ActiveWidgetPrev == h) {
        GUI.ActiveWidgetPrev = __GH(h)->Parent;     /* Set widget as previous active */
    }
    
    gui_widget_invalidatewithparent__(h);           /* Invalidate object and its parent */
    gui_widget_freetextmemory__(h);                 /* Free text memory */
    if (__GH(h)->Timer) {                           /* Check timer memory */
        gui_timer_remove__(&__GH(h)->Timer);        /* Free timer memory */
    }
    if (__GH(h)->Colors) {                          /* Check colors memory */
        __GUI_MEMFREE(__GH(h)->Colors);             /* Free colors memory */
    }
    gui_linkedlist_widgetremove__(h);               /* Remove entry from linked list of parent widget */
    __GUI_MEMFREE(h);                               /* Free memory for widget */
    
    return 1;                                       /* Widget deleted */
}

/* Recursive function to delete all widgets with checking for flag */
static
void __RemoveWidgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h, next;
    static uint32_t lvl = 0;
    
    /**
     * Scan all widgets in system
     */
    for (h = gui_linkedlist_widgetgetnext__((GUI_HANDLE_ROOT_t *)parent, NULL); h; ) {        
        if (gui_widget_getflag__(h, GUI_FLAG_REMOVE)) { /* Widget should be deleted */
            next = gui_linkedlist_widgetgetnext__(NULL, h); /* Get next widget of current */
            if (gui_widget_allowchildren__(h)) {    /* Children widgets are supported */
                GUI_HANDLE_p tmp;
                
                /* Set remove flag to all widgets first... */
                for (tmp = gui_linkedlist_widgetgetnext__((GUI_HANDLE_ROOT_t *)h, NULL); tmp; 
                        tmp = gui_linkedlist_widgetgetnext__(NULL, tmp)) {
                    gui_widget_setflag__(tmp, GUI_FLAG_REMOVE); /* Set remove bit to all children elements */
                }
                
                /* ...process with children delete */
                lvl++;
                __RemoveWidgets(h);                 /* Remove children widgets directly */
                lvl--;
            }
            
            /**
             * Removing widget will also remove linked list chain
             * Therefore we have to save previous widget so we know next one
             */
            __RemoveWidget(h);                      /* Remove widget itself */
            
            /**
             * Move widget pointer to next widget of already deleted and continue checking
             */
            h = next;                               /* Set current pointer to next one */
            continue;                               /* Continue to prevent further execution */
        } else if (gui_widget_allowchildren__(h)) { /* Children widgets are supported */
            __RemoveWidgets(h);                     /* Check children widgets if anything to remove */
        }
        h = gui_linkedlist_widgetgetnext__(NULL, h);    /* Get next widget of current */
    }
    
#if GUI_OS
    if (lvl == 0) {                                 /* Notify about remove execution */
        gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_remove);
    }
#endif /* GUI_OS */
}

/* Get where on LCD is widget visible and what is visible width and height on screen */
static
uint8_t gui_widget_getlcdabsposandvisiblewidthheight__(GUI_HANDLE_p h, GUI_iDim_t* x1, GUI_iDim_t* y1, GUI_iDim_t* x2, GUI_iDim_t* y2) {
    GUI_iDim_t x, y, wi, hi;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    x = gui_widget_getabsolutex__(h);               /* Get absolute X position */
    y = gui_widget_getabsolutey__(h);               /* Get absolute Y position */
    wi = gui_widget_getwidth__(h);                  /* Get absolute width */
    hi = gui_widget_getheight__(h);                 /* Get absolute height */
    
    /**
     * Set widget visible positions with X and Y coordinates
     */
    *x1 = x;
    *y1 = y;
    *x2 = x + wi;
    *y2 = y + hi;
    
    /**
     * Check if widget is hidden by any parent or any parent is hidden by its parent
     */
    for (; h; h = __GH(h)->Parent) {
        x = gui_widget_getparentabsolutex__(h);     /* Parent absolute X position for inner widgets */
        y = gui_widget_getparentabsolutey__(h);     /* Parent absolute Y position for inner widgets */
        wi = gui_widget_getparentinnerwidth__(h);   /* Get parent inner width */
        hi = gui_widget_getparentinnerheight__(h);  /* Get parent inner height */
    
        if (*x1 < x)        { *x1 = x; }
        if (*x2 > x + wi)   { *x2 = x + wi; }
        if (*y1 < y)        { *y1 = y; }
        if (*y2 > y + hi)   { *y2 = y + hi; }
    }
    
    return 1;
}

/* Set widget clipping region */
static
uint8_t __SetClippingRegion(GUI_HANDLE_p h) {
    GUI_Dim_t x1, y1, x2, y2;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /* Get visible widget part and absolute position on screen */
    gui_widget_getlcdabsposandvisiblewidthheight__(h, &x1, &y1, &x2, &y2);
    
    /* Set invalid clipping region */
    if (GUI.Display.X1 > x1)    { GUI.Display.X1 = x1; }
    if (GUI.Display.X2 < x2)    { GUI.Display.X2 = x2; }
    if (GUI.Display.Y1 > y1)    { GUI.Display.Y1 = y1; }
    if (GUI.Display.Y2 < y2)    { GUI.Display.Y2 = y2; }
    
    return 1;
}

/* Invalidate widget procedure */
static
uint8_t __InvalidatePrivate(GUI_HANDLE_p h, uint8_t setclipping) {
    GUI_HANDLE_p h1, h2;
    GUI_iDim_t h1x1, h1x2, h2x1, h2x2;
    GUI_iDim_t h1y1, h1y2, h2y1, h2y2;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    h1 = h;                                         /* Get widget handle */
    if (gui_widget_getflag__(h1, GUI_FLAG_IGNORE_INVALIDATE)) { /* Check ignore flag */
        return 0;                                   /* Ignore invalidate process */
    }
    gui_widget_setflag__(h1, GUI_FLAG_REDRAW);      /* Redraw widget */
    GUI.Flags |= GUI_FLAG_REDRAW;                   /* Notify stack about redraw operations */
    
    if (setclipping) {
        __SetClippingRegion(h);                     /* Set clipping region for widget redrawing operation */
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
    if (gui_widget_istransparent__(h1)) {
        __InvalidatePrivate(__GH(h1)->Parent, 0);   /* Invalidate parent widget */
    }
    for (; h1; h1 = gui_linkedlist_widgetgetnext__(NULL, h1)) {
        gui_widget_getlcdabsposandvisiblewidthheight__(h1, &h1x1, &h1y1, &h1x2, &h1y2); /* Get visible position on LCD for widget */
        for (h2 = gui_linkedlist_widgetgetnext__(NULL, h1); h2; h2 = gui_linkedlist_widgetgetnext__(NULL, h2)) {
            gui_widget_getlcdabsposandvisiblewidthheight__(h2, &h2x1, &h2y1, &h2x2, &h2y2);
            if (
                gui_widget_getflag__(h2, GUI_FLAG_REDRAW) ||    /* Flag is already set */
                !__GUI_RECT_MATCH(                  /* Widgets are not one over another */
                    h1x1, h1y1, h1x2, h1y2,
                    h2x1, h2y1, h2x2, h2y2)
            ) {
                continue;
            }
            gui_widget_setflag__(h2, GUI_FLAG_REDRAW);  /* Redraw widget on next loop */
        }
    }
    
    /**
     * If widget is not the last on the linked list (top z-index)
     * check status of parent widget if it is last.
     * If it is not, process parent redraw and check similar parent widgets if are over our widget
     */
    if (__GH(h)->Parent && !gui_linkedlist_iswidgetlast__(__GH(h)->Parent)) {
        __InvalidatePrivate(__GH(h)->Parent, 0);
    }
    
    /**
     * Check if any of parent widgets has transparency = should be invalidated too
     *
     * Since recursion is used, call function only once and recursion will take care for upper level of parent widgets
     */
    for (h = __GH(h)->Parent; h; h = __GH(h)->Parent) {
        if (gui_widget_istransparent__(h)) {        /* If widget is transparent */
            __InvalidatePrivate(h, 0);              /* Invalidate this parent too */
            break;
        }
    }
    
    return 1;
}

/* Get first widget by given ID */
static
GUI_HANDLE_p __GetWidgetById(GUI_HANDLE_p parent, GUI_ID_t id, uint8_t deep) {
    GUI_HANDLE_p h;
    
    for (h = gui_linkedlist_widgetgetnext__(__GHR(parent), NULL); h; h = gui_linkedlist_widgetgetnext__(NULL, h)) {
        if (gui_widget_getid__(h) == id) {          /* Compare ID values */
            return h;
        } else if (deep && gui_widget_allowchildren__(h)) { /* Check children if possible */
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
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h1) && gui_widget_iswidget__(h2)); /* Check valid parameter */
    
    for (; h1; h1 = __GH(h1)->Parent) {             /* Process all entries */
        for (tmp = h2; tmp; tmp = __GH(tmp)->Parent) {
            if (h1 == tmp) {
                return tmp;
            }
        }
    }
    return GUI.Root.First;                          /* Return bottom widget on list */
}

/* Set widget size */
static
uint8_t __SetWidgetSize(GUI_HANDLE_p h, float wi, float hi) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (wi != __GH(h)->Width || hi != __GH(h)->Height) {    /* Check any differences */
        uint8_t invalidateSecond = 0;
        if (!gui_widget_isexpanded__(h)) {          /* First invalidate current position if not expanded before change of size */
            gui_widget_invalidatewithparent__(h);   /* Set old clipping region first */
            if (wi > __GH(h)->Width || hi > __GH(h)->Height) {
                invalidateSecond = 1;
            }
        }
        __GH(h)->Width = wi;                        /* Set parameter */
        __GH(h)->Height = hi;                       /* Set parameter */
        if (invalidateSecond) {                     /* Invalidate second time only if widget greater than before */
            gui_widget_invalidatewithparent__(h);   /* Set new clipping region */
        }
    }
    return 1;
}

/* Set widget position */
static
uint8_t __SetWidgetPosition(GUI_HANDLE_p h, float x, float y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (__GH(h)->X != x || __GH(h)->Y != y) {       /* Check any differences */
        if (!gui_widget_isexpanded__(h)) {
            gui_widget_invalidatewithparent__(h);   /* Set old clipping region first */
        }
        __GH(h)->X = x;                             /* Set parameter */
        __GH(h)->Y = y;                             /* Set parameter */
        if (!gui_widget_isexpanded__(h)) {
            gui_widget_invalidatewithparent__(h);   /* Set new clipping region */
        }
    }
    return 1;
}

/* Check if widget can be (or not) removed for some reason */
static
uint8_t __CanRemoveWidget(GUI_HANDLE_p h) {
    uint8_t r = 1;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /**
     * Desktop window cannot be deleted
     */
    if (h == gui_window_getdesktop()) {             /* Root widget can not be deleted! */
        return 0;
    }
    
    /**
     * Check widget status itself
     */
    if (!gui_widget_callback__(h, GUI_WC_Remove, 0, &r) || r) { /* If command was not processed, allow delete */
        r = 1;                                      /* Manually allow delete */
    }
    
    /**
     * Check children widgets recursively
     */
    if (r && gui_widget_allowchildren__(h)) {       /* Check if we can delete all children widgets */
        GUI_HANDLE_p h1;
        for (h1 = gui_linkedlist_widgetgetnext__((GUI_HANDLE_ROOT_t *)h, NULL); h1; h1 = gui_linkedlist_widgetgetnext__(NULL, h1)) {
            if (!__CanRemoveWidget(h1)) {           /* If we should not delete it */
                return 0;                           /* Stop on first call */
            }
        }
    }
    
    return r;
}

/* Check if widget is inside LCD invalidate region */
uint8_t gui_widget_isinsideclippingregion__(GUI_HANDLE_p h) {
    GUI_iDim_t x1, y1, x2, y2;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /* Get widget visible section */
    gui_widget_getlcdabsposandvisiblewidthheight__(h, &x1, &y1, &x2, &y2);
    return __GUI_RECT_MATCH(
        x1, y1, x2, y2,
        GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2, GUI.Display.Y2
    );
}

void gui_widget_init__(void) {
    gui_window_createdesktop(GUI_ID_WINDOW_BASE, NULL); /* Create base window object */
}

uint8_t gui_widget_executeremove__(void) {
    if (GUI.Flags & GUI_FLAG_REMOVE) {              /* Anything to remove? */
        __RemoveWidgets(NULL);                      /* Remove widgets */
        GUI.Flags &= ~GUI_FLAG_REMOVE;
        return 1;
    }
    return 0;
}

GUI_Dim_t gui_widget_getwidth__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (gui_widget_getflag__(h, GUI_FLAG_EXPANDED)) {   /* Maximize window over parent */
        return gui_widget_getparentinnerwidth__(h); /* Return parent inner width */
    } else if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_FILL)) {  /* "fill_parent" mode for width */
        GUI_iDim_t parent = gui_widget_getparentinnerwidth__(h);
        if (parent > gui_widget_getrelativex__(h)) {
            return parent - gui_widget_getrelativex__(h);   /* Return widget width */
        }
    } else if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT)) {   /* Percentage width */
        GUI_Dim_t parent = gui_widget_getparentinnerwidth__(h);
        return GUI_ROUND(__GH(h)->Width * parent / 100);
    } else {                                        /* Normal width */
        return __GH(h)->Width;
    }
    return 0;
}

GUI_Dim_t gui_widget_getheight__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (gui_widget_getflag__(h, GUI_FLAG_EXPANDED)) {   /* Maximize window over parent */
        return gui_widget_getparentinnerheight__(h);/* Return parent inner height */
    } else if (gui_widget_getflag__(h, GUI_FLAG_HEIGHT_FILL)) { /* "fill_parent" mode for height */
        GUI_Dim_t parent = gui_widget_getparentinnerheight__(h);
        if (parent > gui_widget_getrelativey__(h)) {
            return parent - gui_widget_getrelativey__(h);   /* Return widget width */
        }
    } else if (gui_widget_getflag__(h, GUI_FLAG_HEIGHT_PERCENT)) {  /* Percentage height */
        return GUI_ROUND(__GH(h)->Height * gui_widget_getparentinnerheight__(h) / 100);
    } else {                                        /* Normal height */
        return __GH(h)->Height;
    }
    return 0;
}

GUI_iDim_t gui_widget_getabsolutex__(GUI_HANDLE_p h) {
    GUI_HANDLE_p w = 0;
    GUI_iDim_t out = 0;
    
    if (!h) {                                       /* Check input value */
        return 0;                                   /* At left value */
    }
    
    /* If widget is not expanded, use actual value */
    out = gui_widget_getrelativex__(h);             /* Get start relative position */
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = __GH(h)->Parent; w; w = __GH(w)->Parent) { /* Go through all parent windows */
        out += gui_widget_getrelativex__(w) + gui_widget_getpaddingleft__(w);   /* Add X offset from parent and left padding of parent */
        out -= __GHR(w)->ScrollX;                   /* Decrease by scroll value */
    }
    return out;
}

GUI_iDim_t gui_widget_getabsolutey__(GUI_HANDLE_p h) {
    GUI_HANDLE_p w = 0;
    GUI_iDim_t out = 0;
    
    if (!h) {                                       /* Check input value */
        return 0;                                   /* At top value */
    }
    
    /* If widget is not expanded, use actual value */
    out = gui_widget_getrelativey__(h);             /* Get start relative position */
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = __GH(h)->Parent; w; w = __GH(w)->Parent) { /* Go through all parent windows */
        out += gui_widget_getrelativey__(w) + gui_widget_getpaddingtop__(w);    /* Add X offset from parent and left padding of parent */
        out -= __GHR(w)->ScrollX;                   /* Decrease by scroll value */
    }
    return out;
}

GUI_iDim_t gui_widget_getparentabsolutex__(GUI_HANDLE_p h) {
    GUI_Dim_t out = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (h) {                                        /* Check valid widget */
        h = __GH(h)->Parent;                        /* Get parent of widget */
        if (h) {                                    /* Save left padding */
            out = gui_widget_getpaddingleft__(h);   /* Get left padding from parent widget */
        }
    }
    out += gui_widget_getabsolutex__(h);            /* Add absolute X of parent and to current padding */
    return out;
}

GUI_iDim_t gui_widget_getparentabsolutey__(GUI_HANDLE_p h) {
    GUI_Dim_t out = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (h) {                                        /* Check valid widget */
        h = __GH(h)->Parent;                        /* Get parent of widget */
        if (h) {                                    /* Save left padding */
            out = gui_widget_getpaddingtop__(h);    /* Get top padding from parent widget */
        }
    }
    out += gui_widget_getabsolutey__(h);            /* Add absolute Y of parent and to current padding */
    return out;
}

uint8_t gui_widget_invalidate__(GUI_HANDLE_p h) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    ret = __InvalidatePrivate(h, 1);                /* Invalidate widget with clipping */
    
    if (
        (
            gui_widget_getflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT) || 
            gui_widget_getcoreflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT) ||
            gui_widget_istransparent__(h)           /* At least little transparent */
        ) && __GH(h)->Parent) {
        __InvalidatePrivate(__GH(h)->Parent, 0); /* Invalidate parent object too but without clipping */
    }
#if GUI_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_invalidate);
#endif /* GUI_OS */
    return ret;
}

uint8_t gui_widget_invalidatewithparent__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __InvalidatePrivate(h, 1);                      /* Invalidate object with clipping */
    if (__GH(h)->Parent) {                          /* If parent exists, invalid only parent */
        __InvalidatePrivate(__GH(h)->Parent, 0);    /* Invalidate parent object without clipping */
    }
    return 1;
}

uint8_t gui_widget_setinvalidatewithparent__(GUI_HANDLE_p h, uint8_t value) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (value) {                                    /* On positive value */
        gui_widget_setflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Enable auto invalidation of parent widget */
    } else {                                        /* On zero */
        gui_widget_clrflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT);   /* Disable auto invalidation of parent widget */
    }
    return 1;
}

uint8_t gui_widget_set3dstyle__(GUI_HANDLE_p h, uint8_t enable) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (enable && !gui_widget_getflag__(h, GUI_FLAG_3D)) {  /* Enable style */
        gui_widget_setflag__(h, GUI_FLAG_3D);       /* Enable 3D style */
        gui_widget_invalidate__(h);                 /* Invalidate object */
    } else if (!enable && gui_widget_getflag__(h, GUI_FLAG_3D)) {   /* Disable style */
        gui_widget_clrflag__(h, GUI_FLAG_3D);       /* Disable 3D style */
        gui_widget_invalidate__(h);                 /* Invalidate object */
    }
    return 1;
}

/*******************************************/
/**  Widget create and remove management  **/
/*******************************************/
void* gui_widget_create__(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
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
    
    h = __GUI_MEMALLOC(widget->Size);               /* Allocate memory for widget */
    if (h) {
        __GUI_ENTER();                              /* Enter GUI */
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
        if (gui_widget_isdialogbase__(h) || flags & GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP) {/* Dialogs do not have parent widget */
            __GH(h)->Parent = gui_window_getdesktop();  /* Set parent object */
        } else {
            if (parent && gui_widget_allowchildren__(parent)) {
                __GH(h)->Parent = parent;
            } else {
                __GH(h)->Parent = GUI.WindowActive; /* Set parent object. It will be NULL on first call */
            }
        }
        
        result = 1;                                 /* We are OK at starting point */
        gui_widget_callback__(h, GUI_WC_PreInit, NULL, &result);    /* Notify internal widget library about init successful */
        
        if (!result) {                              /* Check result */
            __GUI_MEMFREE(h);                       /* Clear widget memory */
            h = 0;                                  /* Reset handle */
            return 0;                               /* Stop execution at this point */
        }
        
        /* Set widget default values */
        __GH(h)->Font = WIDGET_Default.Font;        /* Set default font */
        
        gui_widget_setflag__(h, GUI_FLAG_IGNORE_INVALIDATE);    /* Ignore invalidation process */
        gui_widget_setsize__(h, width, height);     /* Set widget size */
        gui_widget_setposition__(h, x, y);          /* Set widget position */
        gui_widget_clrflag__(h, GUI_FLAG_IGNORE_INVALIDATE);    /* Include invalidation process */
        gui_widget_invalidate__(h);                 /* Invalidate properly now when everything is set correctly = set for valid clipping region part */

        result = 0;
        gui_widget_callback__(h, GUI_WC_ExcludeLinkedList, 0, &result);
        if (!result) {                              /* Check if widget should be added to linked list */
            gui_linkedlist_widgetadd__((GUI_HANDLE_ROOT_t *)__GH(h)->Parent, h);    /* Add entry to linkedlist of parent widget */
        }
        gui_widget_callback__(h, GUI_WC_Init, NULL, NULL);  /* Notify user about init successful */
        gui_widget_invalidate__(h);                 /* Invalidate object */
        
#if GUI_OS
        static gui_mbox_msg_t msg = {GUI_SYS_MBOX_TYPE_WIDGET_CREATED};
        gui_sys_mbox_putnow(&GUI.OS.mbox, &msg);    /* Post message queue */
#endif /* GUI_OS */
        __GUI_LEAVE();                              /* Leave GUI */
    }
    
    return (void *)h;
}

uint8_t gui_widget_remove__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (__CanRemoveWidget(h)) {                     /* Check if we can delete widget */
        gui_widget_setflag__(h, GUI_FLAG_REMOVE);   /* Set flag for widget delete */
        GUI.Flags |= GUI_FLAG_REMOVE;               /* Set flag for to remove at least one widget from tree */
        if (gui_widget_isfocused__(h)) {
            __GUI_WIDGET_FOCUS_SET(__GH(h)->Parent);
        }
#if GUI_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_remove);  /* Put message to queue */
#endif /* GUI_OS */
        return 1;                                   /* Widget will be deleted */
    }
    return 0;
}

/*******************************************/
/**    Widget text and font management    **/
/*******************************************/
uint8_t gui_widget_setfont__(GUI_HANDLE_p h, GUI_Const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (__GH(h)->Font != font) {                    /* Any parameter changed */
        __GH(h)->Font = font;                       /* Set parameter */
        gui_widget_invalidatewithparent__(h);       /* Invalidate object */
    }
    return 1;
}

uint8_t gui_widget_settext__(GUI_HANDLE_p h, const GUI_Char* text) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC)) {   /* Memory for text is dynamically allocated */
        if (__GH(h)->TextMemSize) {
            if (gui_string_lengthtotal(text) > (__GH(h)->TextMemSize - 1)) {    /* Check string length */
                gui_string_copyn(__GH(h)->Text, text, __GH(h)->TextMemSize - 1);    /* Do not copy all bytes because of memory overflow */
            } else {
                gui_string_copy(__GH(h)->Text, text);   /* Copy entire string */
            }
            gui_widget_invalidate__(h);             /* Redraw object */
            gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    } else {                                        /* Memory allocated by user */
        if (__GH(h)->Text && __GH(h)->Text == text) {   /* In case the same pointer is passed to WIDGET */
            if (gui_string_compare(__GH(h)->Text, text)) {  /* If strings does not match, source string updated? */
                gui_widget_invalidate__(h);         /* Redraw object */
                gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            }
        }
        
        if (__GH(h)->Text != text) {                /* Check if pointer do not match */
            __GH(h)->Text = (GUI_Char *)text;       /* Set parameter */
            gui_widget_invalidate__(h);             /* Redraw object */
            gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    }
    __GH(h)->TextCursor = gui_string_lengthtotal(__GH(h)->Text);    /* Set cursor to the end of string */
    return 1;
}

uint8_t gui_widget_allocatetextmemory__(GUI_HANDLE_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {  /* Check if already allocated */
        __GUI_MEMFREE(__GH(h)->Text);               /* Free memory first */
        __GH(h)->TextMemSize = 0;                   /* Reset memory size */
    }
    __GH(h)->Text = 0;                              /* Reset pointer */
    
    __GH(h)->TextMemSize = size * sizeof(GUI_Char); /* Allocate text memory */
    __GH(h)->Text = __GUI_MEMALLOC(__GH(h)->TextMemSize);   /* Allocate memory for text */
    if (__GH(h)->Text) {                            /* Check if allocated */
        gui_widget_setflag__(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Dynamically allocated */
    } else {
        __GH(h)->TextMemSize = 0;                   /* No dynamic bytes available */
        gui_widget_clrflag__(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
    }
    gui_widget_invalidate__(h);                     /* Redraw object */
    gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    return 1;
}

uint8_t gui_widget_freetextmemory__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {  /* Check if dynamically alocated */
        __GUI_MEMFREE(__GH(h)->Text);               /* Free memory first */
        __GH(h)->Text = 0;                          /* Reset memory */
        __GH(h)->TextMemSize = 0;                   /* Reset memory size */
        gui_widget_clrflag__(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
        gui_widget_invalidate__(h);                 /* Redraw object */
        gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    }
    return 1;
}

uint8_t gui_widget_isfontandtextset__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return __GH(h)->Text && __GH(h)->Text[0] && __GH(h)->Font && gui_string_length(__GH(h)->Text);  /* Check if conditions are met for drawing string */
}

uint8_t gui_widget_processtextkey__(GUI_HANDLE_p h, __GUI_KeyboardData_t* kb) {
    size_t len, tlen;
    uint32_t ch;
    uint8_t l;
    GUI_STRING_t currStr;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (!gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC)) {  /* Must be dynamically allocated memory */
        return 0;
    }
    
    gui_string_prepare(&currStr, kb->KB.Keys);      /* Set string to process */
    if (!gui_string_getch(&currStr, &ch, &l)) {     /* Get key from input data */
        return 0;                                   /* Invalid input key */
    }
    
    tlen = gui_string_lengthtotal(__GH(h)->Text);   /* Get total length of string */
    len = gui_string_length(__GH(h)->Text);         /* Get string length */
    if ((ch == GUI_KEY_LF || ch >= 32) && ch != 127) {  /* Check valid character character */
        if (len < (__GH(h)->TextMemSize - l)) {     /* Memory still available for new character */
            uint16_t pos;
            for (pos = tlen + l - 1; pos > __GH(h)->TextCursor; pos--) {    /* Shift characters down */
                __GH(h)->Text[pos] = __GH(h)->Text[pos - l];
            }
            for (pos = 0; pos < l; pos++) {         /* Fill new characters to empty memory */
                __GH(h)->Text[__GH(h)->TextCursor++] = kb->KB.Keys[pos];
            }
            __GH(h)->Text[tlen + l] = 0;            /* Add 0 to the end */
            
            gui_widget_invalidate__(h);             /* Invalidate widget */
            gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            return 1;
        }
    } else if (ch == 8 || ch == 127) {              /* Backspace character */
        if (tlen && __GH(h)->TextCursor) {
            uint16_t pos;
            
            gui_string_prepare(&currStr, (GUI_Char *)((uint32_t)__GH(h)->Text + __GH(h)->TextCursor - 1));  /* Set string to process */
            gui_string_gotoend(&currStr);           /* Go to the end of string */
            if (!gui_string_getchreverse(&currStr, &ch, &l)) {  /* Get last character */
                return 0;                           
            }
            for (pos = __GH(h)->TextCursor - l; pos < (tlen - l); pos++) {  /* Shift characters up */
                __GH(h)->Text[pos] = __GH(h)->Text[pos + l];
            }
            __GH(h)->TextCursor -= l;               /* Decrease text cursor by number of bytes for character deleted */
            __GH(h)->Text[tlen - l] = 0;            /* Set 0 to the end of string */
            
            gui_widget_invalidate__(h);             /* Invalidate widget */
            gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            return 1;
        }
    }
    return 0;
}

const GUI_Char* gui_widget_gettext__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* Prepare for transpate support */
#if GUI_USE_TRANSLATE
    /* For static texts only */
    if (!gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {
        return gui_translate_get(__GH(h)->Text);    /* Get translation entry */
    }
#endif /* GUI_USE_TRANSLATE */
    return __GH(h)->Text;                           /* Return text for widget */
}

const GUI_FONT_t* gui_widget_getfont__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return __GH(h)->Font;                           /* Return font for widget */
}

/*******************************************/
/**         Widget size management        **/
/*******************************************/
uint8_t gui_widget_setwidth__(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT)) {  /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_clrflag__(h, GUI_FLAG_WIDTH_PERCENT);    /* Set percentage flag */
        __GH(h)->Height = width + 1;                /* Invalidate height */
    }
    return __SetWidgetSize(h, width, __GH(h)->Height);  /* Set new height */
}

uint8_t gui_widget_setheight__(GUI_HANDLE_p h, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_HEIGHT_PERCENT)) { /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_clrflag__(h, GUI_FLAG_HEIGHT_PERCENT);   /* Set percentage flag */
        __GH(h)->Height = height + 1;               /* Invalidate height */
    }
    return __SetWidgetSize(h, __GH(h)->Width, height);  /* Set new height */
}

uint8_t gui_widget_setwidthpercent__(GUI_HANDLE_p h, float width) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT)) { /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_setflag__(h, GUI_FLAG_WIDTH_PERCENT);    /* Set percentage flag */
        __GH(h)->Width = width + 1;                 /* Invalidate widget */
    }
    return __SetWidgetSize(h, width, __GH(h)->Height);  /* Set new width */
}

uint8_t gui_widget_setheightpercent__(GUI_HANDLE_p h, float height) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_HEIGHT_PERCENT)) {    /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_setflag__(h, GUI_FLAG_HEIGHT_PERCENT);   /* Set percentage flag */
        __GH(h)->Height = height + 1;               /* Invalidate height */
    }
    return __SetWidgetSize(h, __GH(h)->Width, height);  /* Set new height */
}

uint8_t gui_widget_setsize__(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* If percentage enabled on at least one, either width or height */
    if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT)) {
        gui_widget_setflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT);  /* Set both flags */
        __GH(h)->Width = wi + 1;                    /* Invalidate width */
        __GH(h)->Height = hi + 1;                   /* Invalidate height */
    }
    return __SetWidgetSize(h, wi, hi);              /* Set widget size */
}

uint8_t gui_widget_setsizepercent__(GUI_HANDLE_p h, float wi, float hi) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* If percentage not enable on width or height */
    if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT) != (GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT)) {
        gui_widget_setflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT);  /* Set both flags */
        __GH(h)->Width = wi + 1;                    /* Invalidate width */
        __GH(h)->Height = hi + 1;                   /* Invalidate height */
    }
    return __SetWidgetSize(h, wi, hi);              /* Set widget size */
}

uint8_t gui_widget_toggleexpanded__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return gui_widget_setexpanded__(h, !gui_widget_isexpanded__(h));    /* Invert expanded mode */
}

uint8_t gui_widget_setexpanded__(GUI_HANDLE_p h, uint8_t state) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!state && gui_widget_isexpanded__(h)) {     /* Check current status */
        gui_widget_invalidatewithparent__(h);       /* Invalidate with parent first for clipping region */
        gui_widget_clrflag__(h, GUI_FLAG_EXPANDED); /* Clear expanded after invalidation */
    } else if (state && !gui_widget_isexpanded__(h)) {
        gui_widget_setflag__(h, GUI_FLAG_EXPANDED); /* Expand widget */
        gui_widget_invalidate__(h);                 /* Redraw only selected widget as it is over all window */
    }
    return 1;
}

/*******************************************/
/**       Widget position management      **/
/*******************************************/
uint8_t gui_widget_setposition__(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* If percent enabled on at least one coordinate, clear to force invalidation */
    if (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT)) {
        gui_widget_clrflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT); /* Disable percent on X and Y position */
        __GH(h)->X = x + 1;                         /* Invalidate X position */
        __GH(h)->Y = y + 1;                         /* Invalidate Y position */
    }  
    return __SetWidgetPosition(h, x, y);            /* Set widget position */
}

uint8_t gui_widget_setpositionpercent__(GUI_HANDLE_p h, float x, float y) {  
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */   
    /* If percent not set on both, enable to force invalidation */
    if (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT) != (GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT)) {
        gui_widget_setflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT); /* Enable percent on X and Y position */
        __GH(h)->X = x + 1;                         /* Invalidate X position */
        __GH(h)->Y = y + 1;                         /* Invalidate Y position */
    }
    return __SetWidgetPosition(h, x, y);            /* Set widget position */
}

uint8_t gui_widget_setxposition__(GUI_HANDLE_p h, GUI_iDim_t x) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT)) {   /* if percent enabled */
        gui_widget_clrflag__(h, GUI_FLAG_XPOS_PERCENT); /* Clear it to force invalidation */
        __GH(h)->X = x + 1;                         /* Invalidate position */
    }
    return __SetWidgetPosition(h, x, __GH(h)->Y);   /* Set widget position */
}

uint8_t gui_widget_setxpositionpercent__(GUI_HANDLE_p h, float x) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT)) {  /* if percent not enabled */
        gui_widget_setflag__(h, GUI_FLAG_XPOS_PERCENT); /* Set it to force invalidation */
        __GH(h)->X = x + 1;                         /* Invalidate position */
    }
    return __SetWidgetPosition(h, x, __GH(h)->Y);   /* Set widget position */
}

uint8_t gui_widget_setyposition__(GUI_HANDLE_p h, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_YPOS_PERCENT)) {   /* if percent enabled */
        gui_widget_clrflag__(h, GUI_FLAG_YPOS_PERCENT); /* Clear it to force invalidation */
        __GH(h)->Y = y + 1;                         /* Invalidate position */
    }
    return __SetWidgetPosition(h, __GH(h)->X, y);   /* Set widget position */
}

uint8_t gui_widget_setypositionpercent__(GUI_HANDLE_p h, float y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_YPOS_PERCENT)) {  /* if percent not enabled */
        gui_widget_setflag__(h, GUI_FLAG_YPOS_PERCENT); /* Set it to force invalidation */
        __GH(h)->X = y + 1;                         /* Invalidate position */
    }
    return __SetWidgetPosition(h, __GH(h)->X, y);   /* Set widget position */
}

/*******************************************/
/**                  .....                **/
/*******************************************/
uint8_t gui_widget_show__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (gui_widget_getflag__(h, GUI_FLAG_HIDDEN)) { /* If hidden, show it */
        gui_widget_clrflag__(h, GUI_FLAG_HIDDEN);
        gui_widget_invalidatewithparent__(h);       /* Invalidate it for redraw with parent */
    }
    return 1;
}

uint8_t gui_widget_hide__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (!gui_widget_getflag__(h, GUI_FLAG_HIDDEN)) {    /* If visible, hide it */
        gui_widget_setflag__(h, GUI_FLAG_HIDDEN);
        gui_widget_invalidatewithparent__(h);       /* Invalidate it for redraw with parenta */
    }
    
    /**
     * TODO: Check if active/focused widget is maybe children of this widget
     */
    
    if (GUI.FocusedWidget == h || gui_widget_ischildof__(GUI.FocusedWidget, h)) {   /* Clear focus */
        __GUI_WIDGET_FOCUS_SET(__GH(h)->Parent);    /* Set parent widget as focused now */
    }
    if (GUI.ActiveWidget == h || gui_widget_ischildof__(GUI.ActiveWidget, h)) { /* Clear active */
        __GUI_WIDGET_ACTIVE_CLEAR();
    }
    return 1;
}

uint8_t gui_widget_ischildof__(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_iswidget__(parent));  /* Check valid parameter */
    
    for (h = __GH(h)->Parent; h; h = __GH(h)->Parent) { /* Check widget parent objects */
        if (parent == h) {                          /* If they matches */
            return 1;
        }
    }
    return 0;
}

uint8_t gui_widget_setzindex__(GUI_HANDLE_p h, int32_t zindex) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    uint8_t ret = 1;
    if (__GH(h)->ZIndex != zindex) {                /* There was a change in z-index value */
        int32_t current = __GH(h)->ZIndex;
        __GH(h)->ZIndex = zindex;                   /* Set new index */
        if (zindex < current) {                     /* New index value is less important than before = move widget to top */
            gui_linkedlist_widgetmovetotop__(h);    /* Move widget to top on linked list = less important and less visible */
        } else {
            gui_linkedlist_widgetmovetobottom__(h); /* Move widget to bottom on linked list = most important and most visible */
        }
    }
    return ret;
}

uint8_t gui_widget_settransparency__(GUI_HANDLE_p h, uint8_t trans) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (__GH(h)->Transparency != trans) {           /* Check transparency match */
        __GH(h)->Transparency = trans;              /* Set new transparency level */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    }
    
    return 1;
}

uint8_t gui_widget_setcolor__(GUI_HANDLE_p h, uint8_t index, GUI_Color_t color) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI context */
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
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

GUI_HANDLE_p gui_widget_getbyid__(GUI_ID_t id) {
    return __GetWidgetById(NULL, id, 1);            /* Find widget by ID */ 
}

uint8_t gui_widget_setuserdata__(GUI_HANDLE_p h, void* data) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */    
    __GH(h)->UserData = data;                       /* Set user data */
    return 1;
}

void* gui_widget_getuserdata__(GUI_HANDLE_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    data = __GH(h)->UserData;                       /* Get user data */
    return data;
}

void gui_widget_movedowntree__(GUI_HANDLE_p h) {              
    /**
     * Move widget to the end of parent linked list
     * This will allow widget to be first checked next time for touch detection
     * and will be drawn on top of al widgets as expected except if there is widget which allows children (new window or similar)
     */
    if (gui_linkedlist_widgetmovetobottom__(h)) {
        gui_widget_invalidate__(h);                 /* Invalidate object */
    }
    
    /**
     * Since linked list is threaded, we should move our widget to the end of parent list.
     * The same should be in the parent as it should also be on the end of its parent and so on.
     * With parent recursion this can be achieved
     */
    if (__GH(h)->Parent) {                          /* Move its parent to the bottom of parent linked list */
        GUI_HANDLE_p parent = __GH(h)->Parent;
        for (; parent; parent = __GH(parent)->Parent) {
            if (gui_linkedlist_widgetmovetobottom__(parent)) {  /* If move down was successful */
                gui_widget_invalidate__(parent);    /* Invalidate parent of widget */
            }
        }
    }
}

/* Clear widget focus */
void __GUI_WIDGET_FOCUS_CLEAR(void) {
    if (GUI.FocusedWidget && GUI.FocusedWidget != GUI.Root.First) { /* First widget is always in focus */
        GUI.FocusedWidgetPrev = GUI.FocusedWidget;  /* Clear everything */
        do {
            gui_widget_callback__(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);
            gui_widget_clrflag__(GUI.FocusedWidget, GUI_FLAG_FOCUS);    /* Clear focused widget */
            gui_widget_invalidate__(GUI.FocusedWidget); /* Invalidate widget */
            GUI.FocusedWidget = __GH(GUI.FocusedWidget)->Parent;    /* Get parent widget */
        } while (GUI.FocusedWidget != GUI.Root.First);  /* Loop to the bottom */
        GUI.FocusedWidget = 0;                      /* Reset focused widget */
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
                gui_widget_clrflag__(GUI.FocusedWidget, GUI_FLAG_FOCUS);    /* Clear focused flag */
                gui_widget_callback__(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);  /* Notify with callback */
                gui_widget_invalidate__(GUI.FocusedWidget); /* Invalidate widget */
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
        gui_widget_setflag__(h, GUI_FLAG_FOCUS);    /* Set focused flag */
        gui_widget_callback__(h, GUI_WC_FocusIn, NULL, NULL);   /* Notify with callback */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
        h = __GH(h)->Parent;                        /* Get parent widget */
    }
}

/* Clear active widget status */
void __GUI_WIDGET_ACTIVE_CLEAR(void) {
    if (GUI.ActiveWidget) {
        gui_widget_callback__(GUI.ActiveWidget, GUI_WC_ActiveOut, NULL, NULL);  /* Notify user about change */
        gui_widget_clrflag__(GUI.ActiveWidget, GUI_FLAG_ACTIVE | GUI_FLAG_TOUCH_MOVE);  /* Clear all widget based flags */
        GUI.ActiveWidgetPrev = GUI.ActiveWidget;    /* Save as previous active */
        GUI.ActiveWidget = 0;                       /* Clear active widget handle */
    }
}

/* Set active status to widget */
void __GUI_WIDGET_ACTIVE_SET(GUI_HANDLE_p h) {
    __GUI_WIDGET_ACTIVE_CLEAR();                    /* Clear active widget flag */
    GUI.ActiveWidget = h;                           /* Set new active widget */
    if (h) {
        gui_widget_setflag__(GUI.ActiveWidget, GUI_FLAG_ACTIVE);    /* Set active widget flag */
        gui_widget_callback__(GUI.ActiveWidget, GUI_WC_ActiveIn, NULL, NULL);
    }
}

/******************************************************************************/
/******************************************************************************/
/***                  Thread safe version of public API                      **/
/******************************************************************************/
/******************************************************************************/
uint8_t gui_widget_remove(GUI_HANDLE_p* h) {
    __GUI_ASSERTPARAMS(h && gui_widget_iswidget__(*h));     /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */

    gui_widget_remove__(*h);                        /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;                                       /* Removev successfully */
}

/*******************************************/
/**         Widget text management        **/
/*******************************************/
uint32_t gui_widget_alloctextmemory(GUI_HANDLE_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && size > 1);   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    gui_widget_allocatetextmemory__(h, size);       /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return __GH(h)->TextMemSize;                    /* Return number of bytes allocated */
}

uint8_t gui_widget_freetextmemory(GUI_HANDLE_p h) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_freetextmemory__(h);           /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_settext(GUI_HANDLE_p h, const GUI_Char* text) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_settext__(h, text);            /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

const GUI_Char* gui_widget_gettext(GUI_HANDLE_p h) {
    const GUI_Char* t;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = gui_widget_gettext__(h);                    /* Return text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;
}

const GUI_Char* gui_widget_gettextcopy(GUI_HANDLE_p h, GUI_Char* dst, uint32_t len) {
    const GUI_Char* t;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = gui_widget_gettext__(h);                    /* Return text */
    gui_string_copyn(dst, t, len);                  /* Copy text after */
    dst[len] = 0;                                   /* Set trailling zero */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;  
}

uint8_t gui_widget_setfont(GUI_HANDLE_p h, const GUI_FONT_t* font) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setfont__(h, font);            /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

const GUI_FONT_t* gui_widget_getfont(GUI_HANDLE_p h) {
    const GUI_FONT_t* font;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    font = gui_widget_getfont__(h);                 /* Get widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return font;
}

uint8_t gui_widget_setparam__(GUI_HANDLE_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent) {
    GUI_WIDGET_Param_t p;
    uint8_t result = 1;
    
    p.Type = cfg;
    p.Data = (void *)data;
    __GUI_ENTER();                                  /* Enter GUI */
    gui_widget_callback__(h, GUI_WC_SetParam, &p, &result); /* Process callback function */
    if (invalidateparent) {
        gui_widget_invalidatewithparent__(h);       /* Invalidate widget and parent */
    } else if (invalidate) {
        gui_widget_invalidate__(h);                 /* Invalidate widget only */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return 1;
}

/*******************************************/
/**         Widget size management        **/
/*******************************************/
uint8_t gui_widget_setsize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setsize__(h, width, height);   /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setsizepercent(GUI_HANDLE_p h, float width, float height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setsizepercent__(h, width, height);    /* Set actual size to object in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setwidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setwidth__(h, width);          /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setheight(GUI_HANDLE_p h, GUI_Dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setheight__(h, height);        /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setwidthpercent(GUI_HANDLE_p h, float width) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setwidthpercent__(h, width);   /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setheightpercent(GUI_HANDLE_p h, float height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setheightpercent__(h, height); /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

GUI_Dim_t gui_widget_getwidth(GUI_HANDLE_p h) {
    GUI_Dim_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_getwidth__(h);                 /* Get widget width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

GUI_Dim_t gui_widget_getheight(GUI_HANDLE_p h) {
    GUI_Dim_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_getheight__(h);                /* Get widget height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setexpanded(GUI_HANDLE_p h, uint8_t state) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_setexpanded__(h, state);       /* Set expanded mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_isexpanded(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_isexpanded__(h);               /* Check expanded mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/*******************************************/
/**       Widget position management      **/
/*******************************************/
uint8_t gui_widget_setposition(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setposition__(h, x, y);        /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setpositionpercent(GUI_HANDLE_p h, float x, float y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setpositionpercent__(h, x, y); /* Set X and Y position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setxposition(GUI_HANDLE_p h, GUI_iDim_t x) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setxposition__(h, x);          /* Set X position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setxpositionpercent(GUI_HANDLE_p h, float x) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setxpositionpercent__(h, x);   /* Set X position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setyposition(GUI_HANDLE_p h, GUI_iDim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setyposition__(h, y);          /* Set Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_setypositionpercent(GUI_HANDLE_p h, float y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setypositionpercent__(h, y);   /* Set Y position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_invalidate(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_invalidate__(h);               /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_show(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_show__(h);                     /* Show widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

uint8_t gui_widget_putonfront(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    gui_widget_movedowntree__(h);                   /* Put widget on front */
    __GUI_WIDGET_FOCUS_SET(h);                      /* Set widget to focused state */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t gui_widget_hide(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_hide__(h);                     /* Hide widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

GUI_ID_t gui_widget_getid(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return gui_widget_getid__(h);                   /* Return widget ID */
}

GUI_HANDLE_p gui_widget_getbyid(GUI_ID_t id) {
    GUI_HANDLE_p h;
    __GUI_ENTER();                                  /* Enter GUI */
    
    h = __GetWidgetById(NULL, id, 1);               /* Find widget by ID */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h; 
}

uint8_t gui_widget_ischildof(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_iswidget__(parent));  /* Check valid parameter */
    return gui_widget_ischildof__(h, parent);       /* Return status */
}

uint8_t gui_widget_setcallback(GUI_HANDLE_p h, GUI_WIDGET_CALLBACK_t callback) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GH(h)->Callback = callback;                   /* Set callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t gui_widget_setuserdata(GUI_HANDLE_p h, void* data) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    gui_widget_setuserdata__(h, data);              /* Set user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

void* gui_widget_getuserdata(GUI_HANDLE_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    data = gui_widget_getuserdata__(h);             /* Get user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return data;
}

uint8_t gui_widget_processdefaultcallback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GH(h)->Widget->Callback(h, ctrl, param, result);    /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_callback__(h, ctrl, param, result);    /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_setscrollx(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (gui_widget_allowchildren__(h) && __GHR(h)->ScrollX != scroll) { /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollX = scroll;
        gui_widget_invalidate__(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_setscrolly(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (gui_widget_allowchildren__(h) && __GHR(h)->ScrollY != scroll) { /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollY = scroll;
        gui_widget_invalidate__(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_setfocus(GUI_HANDLE_p h) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    __GUI_WIDGET_FOCUS_SET(h);                      /* Put widget in focus */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_setfontdefault(const GUI_FONT_t* font) {
    WIDGET_Default.Font = font;                     /* Set default font */
    return 1;
}

uint8_t gui_widget_incselection(GUI_HANDLE_p h, int16_t dir) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_callback__(h, GUI_WC_IncSelection, &dir, &ret);    /* Increase selection for specific amount */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_setzindex(GUI_HANDLE_p h, int32_t zindex) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_setzindex__(h, zindex);        /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

int32_t gui_widget_getzindex(GUI_HANDLE_p h) {
    int32_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_getzindex__(h);                /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_settransparency(GUI_HANDLE_p h, uint8_t trans) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_settransparency__(h, trans);   /* Set widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t gui_widget_gettransparency(GUI_HANDLE_p h) {
    uint8_t trans;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    trans = gui_widget_gettransparency__(h);        /* Get widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return trans;
}

uint8_t gui_widget_set3dstyle(GUI_HANDLE_p h, uint8_t enable) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_set3dstyle__(h, enable);       /* Set 3D mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

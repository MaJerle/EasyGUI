/**	
 * \file            gui_widget.c
 * \brief           Widget specific core functions
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
#include "widget/gui_widget.h"
#include "widget/gui_window.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct {
    const GUI_FONT_t* Font;                 /*!< Pointer to font structure */
} GUI_WIDGET_Def_t;
GUI_WIDGET_Def_t WIDGET_Default;

#if GUI_CFG_OS
static gui_mbox_msg_t msg_widget_remove = {GUI_SYS_MBOX_TYPE_REMOVE};
static gui_mbox_msg_t msg_widget_invalidate = {GUI_SYS_MBOX_TYPE_INVALIDATE};
#endif /* GUI_CFG_OS */

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
static uint8_t
remove_widget(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /* Check focus state */
    if (GUI.FocusedWidget == h) {
        if (__GH(h)->Parent) {                      /* Should always happen as top parent (window) can't be removed */
            GUI.FocusedWidget = __GH(h)->Parent;    /* Set parent widget as focused */
        } else {
            gui_widget_focus_clear();               /* Clear all widgets from focused */
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
    if (GUI.WindowActive && h == GUI.WindowActive) {    /* Check for parent window */
        GUI.WindowActive = __GH(GUI.WindowActive)->Parent;
    }
    
    gui_widget_invalidatewithparent__(h);           /* Invalidate object and its parent */
    gui_widget_freetextmemory__(h);                 /* Free text memory */
    if (__GH(h)->Timer) {                           /* Check timer memory */
        gui_timer_remove__(&__GH(h)->Timer);        /* Free timer memory */
    }
    if (__GH(h)->Colors) {                          /* Check colors memory */
        GUI_MEMFREE(__GH(h)->Colors);               /* Free colors memory */
    }
    gui_linkedlist_widgetremove(h);                 /* Remove entry from linked list of parent widget */
    GUI_MEMFREE(h);                                 /* Free memory for widget */
    
    return 1;                                       /* Widget deleted */
}

/* Recursive function to delete all widgets with checking for flag */
static void
remove_widgets(GUI_HANDLE_p parent) {
    GUI_HANDLE_p h, next;
    static uint32_t lvl = 0;
    
    /**
     * Scan all widgets in system
     */
    for (h = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)parent, NULL); h; ) {        
        if (gui_widget_getflag__(h, GUI_FLAG_REMOVE)) { /* Widget should be deleted */
            next = gui_linkedlist_widgetgetnext(NULL, h);   /* Get next widget of current */
            if (gui_widget_allowchildren__(h)) {    /* Children widgets are supported */
                GUI_HANDLE_p tmp;
                
                /* Set remove flag to all widgets first... */
                for (tmp = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)h, NULL); tmp; 
                        tmp = gui_linkedlist_widgetgetnext(NULL, tmp)) {
                    gui_widget_setflag__(tmp, GUI_FLAG_REMOVE); /* Set remove bit to all children elements */
                }
                
                /* ...process with children delete */
                lvl++;
                remove_widgets(h);                  /* Remove children widgets directly */
                lvl--;
            }
            
            /**
             * Removing widget will also remove linked list chain
             * Therefore we have to save previous widget so we know next one
             */
            remove_widget(h);                       /* Remove widget itself */
            
            /**
             * Move widget pointer to next widget of already deleted and continue checking
             */
            h = next;                               /* Set current pointer to next one */
            continue;                               /* Continue to prevent further execution */
        } else if (gui_widget_allowchildren__(h)) { /* Children widgets are supported */
            remove_widgets(h);                      /* Check children widgets if anything to remove */
        }
        h = gui_linkedlist_widgetgetnext(NULL, h);  /* Get next widget of current */
    }
    
#if GUI_CFG_OS
    if (lvl == 0) {                                 /* Notify about remove execution */
        gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_remove);
    }
#endif /* GUI_CFG_OS */
}

/* Get where on LCD is widget visible and what is visible width and height on screen for this widget */
static uint8_t
get_lcd_abs_position_and_visible_width_height(GUI_HANDLE_p h, GUI_iDim_t* x1, GUI_iDim_t* y1, GUI_iDim_t* x2, GUI_iDim_t* y2) {
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
static uint8_t
set_clipping_region(GUI_HANDLE_p h) {
    GUI_Dim_t x1, y1, x2, y2;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /**
     * TODO: If widget is visible only part of it (below other widgets)
     * find this part and set clipping region according to actual visible area
     */
    
    /* Get visible widget part and absolute position on screen according to parent */
    get_lcd_abs_position_and_visible_width_height(h, &x1, &y1, &x2, &y2);
    
    /* TODO Get actual visible widget part according to other widgets above current one */
    
    /* Set invalid clipping region */
    if (GUI.Display.X1 > x1)    { GUI.Display.X1 = x1; }
    if (GUI.Display.X2 < x2)    { GUI.Display.X2 = x2; }
    if (GUI.Display.Y1 > y1)    { GUI.Display.Y1 = y1; }
    if (GUI.Display.Y2 < y2)    { GUI.Display.Y2 = y2; }
    
    return 1;
}

/* Invalidate widget procedure */
static uint8_t
invalidate_widget(GUI_HANDLE_p h, uint8_t setclipping) {
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
        set_clipping_region(h);                     /* Set clipping region for widget redrawing operation */
    }
    
    /**
     * Invalid only widget with higher Z-index (lowered on linked list) of current object
     * 
     * If widget should be redrawn, then any widget above it should be redrawn too, otherwise z-index match will fail.
     *
     * Widget may not need redraw operation if positions don't match
     *
     * If widget is transparent, check all widgets, even those which are below current widget in list
     * Get first element of parent linked list for checking
     */
#if GUI_CFG_USE_TRANSPARENCY
    if (gui_widget_istransparent__(h1)) {
        invalidate_widget(__GH(h1)->Parent, 0);     /* Invalidate parent widget */
    }
#endif /* GUI_CFG_USE_TRANSPARENCY */
    for (; h1; h1 = gui_linkedlist_widgetgetnext(NULL, h1)) {
        get_lcd_abs_position_and_visible_width_height(h1, &h1x1, &h1y1, &h1x2, &h1y2); /* Get visible position on LCD for widget */
        for (h2 = gui_linkedlist_widgetgetnext(NULL, h1); h2;
                h2 = gui_linkedlist_widgetgetnext(NULL, h2)) {
            get_lcd_abs_position_and_visible_width_height(h2, &h2x1, &h2y1, &h2x2, &h2y2);
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
    if (__GH(h)->Parent && !gui_linkedlist_iswidgetlast(__GH(h)->Parent)) {
        invalidate_widget(__GH(h)->Parent, 0);
    }

#if GUI_CFG_USE_TRANSPARENCY    
    /**
     * Check if any of parent widgets has transparency = should be invalidated too
     *
     * Since recursion is used, call function only once and recursion will take care for upper level of parent widgets
     */
    for (h = __GH(h)->Parent; h; h = __GH(h)->Parent) {
        if (gui_widget_istransparent__(h)) {        /* If widget is transparent */
            invalidate_widget(h, 0);                /* Invalidate this parent too */
            break;
        }
    }
#endif /* GUI_CFG_USE_TRANSPARENCY */
    
    return 1;
}

/* Get first widget by given ID */
static GUI_HANDLE_p
get_widget_by_id(GUI_HANDLE_p parent, GUI_ID_t id, uint8_t deep) {
    GUI_HANDLE_p h;
    
    for (h = gui_linkedlist_widgetgetnext(__GHR(parent), NULL); h;
            h = gui_linkedlist_widgetgetnext(NULL, h)) {
        if (gui_widget_getid__(h) == id) {          /* Compare ID values */
            return h;
        } else if (deep && gui_widget_allowchildren__(h)) { /* Check children if possible */
            GUI_HANDLE_p tmp = get_widget_by_id(h, id, deep);
            if (tmp) {
                return tmp;
            }
        }
    }
    return 0;
}

/* Returns first common widget between 2 widgets */
static GUI_HANDLE_p
get_common_parentwidget(GUI_HANDLE_p h1, GUI_HANDLE_p h2) {
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
static uint8_t
set_widget_size(GUI_HANDLE_p h, float wi, float hi) {
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
static uint8_t
set_widget_position(GUI_HANDLE_p h, float x, float y) {
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
static uint8_t
can_remove_widget(GUI_HANDLE_p h) {
    GUI_WIDGET_RESULT_t result = {0};
    
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
    GUI_WIDGET_RESULTTYPE_U8(&result) = 1;
    if (!gui_widget_callback__(h, GUI_WC_Remove, NULL, &result) || GUI_WIDGET_RESULTTYPE_U8(&result)) { /* If command was not processed, allow delete */
        GUI_WIDGET_RESULTTYPE_U8(&result) = 1;      /* Manually allow delete */
    }
    
    /**
     * Check children widgets recursively
     */
    if (GUI_WIDGET_RESULTTYPE_U8(&result) && gui_widget_allowchildren__(h)) {   /* Check if we can delete all children widgets */
        GUI_HANDLE_p h1;
        for (h1 = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)h, NULL); h1;
                h1 = gui_linkedlist_widgetgetnext(NULL, h1)) {
            if (!can_remove_widget(h1)) {           /* If we should not delete it */
                return 0;                           /* Stop on first call */
            }
        }
    }
    
    return GUI_WIDGET_RESULTTYPE_U8(&result);
}

/* Check if widget is inside LCD invalidate region */
uint8_t
gui_widget_isinsideclippingregion(GUI_HANDLE_p h) {
    GUI_iDim_t x1, y1, x2, y2;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    /* Get widget visible section */
    get_lcd_abs_position_and_visible_width_height(h, &x1, &y1, &x2, &y2);
    return __GUI_RECT_MATCH(
        x1, y1, x2, y2,
        GUI.Display.X1, GUI.Display.Y1, GUI.Display.X2, GUI.Display.Y2
    );
}

void
gui_widget_init(void) {
    gui_window_createdesktop(GUI_ID_WINDOW_BASE, NULL); /* Create base window object */
}

uint8_t
gui_widget_executeremove(void) {
    if (GUI.Flags & GUI_FLAG_REMOVE) {              /* Anything to remove? */
        remove_widgets(NULL);                       /* Remove widgets */
        GUI.Flags &= ~GUI_FLAG_REMOVE;
        return 1;
    }
    return 0;
}

void
gui_widget_movedowntree(GUI_HANDLE_p h) {              
    /**
     * Move widget to the end of parent linked list
     * This will allow widget to be first checked next time for touch detection
     * and will be drawn on top of al widgets as expected except if there is widget which allows children (new window or similar)
     */
    if (gui_linkedlist_widgetmovetobottom(h)) {
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
            if (gui_linkedlist_widgetmovetobottom(parent)) {/* If move down was successful */
                gui_widget_invalidate__(parent);    /* Invalidate parent of widget */
            }
        }
    }
}

/* Clear widget focus */
void
gui_widget_focus_clear(void) {
    if (GUI.FocusedWidget && GUI.FocusedWidget != GUI.Root.First) { /* First widget is always in focus */
        GUI.FocusedWidgetPrev = GUI.FocusedWidget;  /* Clear everything */
        do {
            gui_widget_callback__(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);
            gui_widget_clrflag__(GUI.FocusedWidget, GUI_FLAG_FOCUS);    /* Clear focused widget */
            gui_widget_invalidate__(GUI.FocusedWidget); /* Invalidate widget */
            GUI.FocusedWidget = __GH(GUI.FocusedWidget)->Parent;    /* Get parent widget */
        } while (GUI.FocusedWidget != GUI.Root.First);  /* Loop to the bottom */
        GUI.FocusedWidget = NULL;                   /* Reset focused widget */
    }
}

/* Set focus on widget */
void
gui_widget_focus_set(GUI_HANDLE_p h) {
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
        common = get_common_parentwidget(GUI.FocusedWidget, h); /* Get first widget in common */
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
void
gui_widget_active_clear(void) {
    if (GUI.ActiveWidget) {
        gui_widget_callback__(GUI.ActiveWidget, GUI_WC_ActiveOut, NULL, NULL);  /* Notify user about change */
        gui_widget_clrflag__(GUI.ActiveWidget, GUI_FLAG_ACTIVE | GUI_FLAG_TOUCH_MOVE);  /* Clear all widget based flags */
        GUI.ActiveWidgetPrev = GUI.ActiveWidget;    /* Save as previous active */
        GUI.ActiveWidget = NULL;                    /* Clear active widget handle */
    }
}

/* Set active status to widget */
void
gui_widget_active_set(GUI_HANDLE_p h) {
    gui_widget_active_clear();                      /* Clear active widget flag */
    GUI.ActiveWidget = h;                           /* Set new active widget */
    if (h) {
        gui_widget_setflag__(GUI.ActiveWidget, GUI_FLAG_ACTIVE);    /* Set active widget flag */
        gui_widget_callback__(GUI.ActiveWidget, GUI_WC_ActiveIn, NULL, NULL);
    }
}


/******************************************************************************/
/******************************************************************************/
/***         Public functions (thread and non-thread safe versions)          **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Get total width of widget in units of pixels
 *                     Function returns width of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total width in units of pixels
 * \sa              __gui_widget_getinnerwidth
 */
GUI_Dim_t
gui_widget_getwidth__(GUI_HANDLE_p h) {
    //__GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (!(gui_widget_iswidget__(h)) || !(GUI.Initialized)) {
        return 0;                                   \
    }
    
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

/**
 * \brief           Get total height of widget
 *                     Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total height in units of pixels
 * \sa              __gui_widget_getinnerheight
 * \hideinitializer
 */
GUI_Dim_t
gui_widget_getheight__(GUI_HANDLE_p h) {
    //__GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!(gui_widget_iswidget__(h)) || !(GUI.Initialized)) {
        return 0;                                   \
    }
    
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

/**
 * \brief           Get absolute X position on LCD for specific widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          X position on LCD
 * \hideinitializer
 */
GUI_iDim_t
gui_widget_getabsolutex__(GUI_HANDLE_p h) {
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

/**
 * \brief           Get absolute Y position on LCD for specific widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Y position on LCD
 * \hideinitializer
 */
GUI_iDim_t
gui_widget_getabsolutey__(GUI_HANDLE_p h) {
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
        out -= __GHR(w)->ScrollY;                   /* Decrease by scroll value */
    }
    return out;
}

/**
 * \brief           Get absolute inner X position of parent widget
 * \note            This function returns inner X position in absolute form.
 *                     Imagine parent absolute X is 10, and left padding is 2. Function returns 12.
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \retval          Parent absolute inner X position
 */
GUI_iDim_t
gui_widget_getparentabsolutex__(GUI_HANDLE_p h) {
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

/**
 * \brief           Get absolute inner Y position of parent widget
 * \note            This function returns inner Y position in absolute form.
 *                     Imagine parent absolute Y is 10, and top padding is 2. Function returns 12.
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \retval          Parent absolute inner Y position
 */
GUI_iDim_t
gui_widget_getparentabsolutey__(GUI_HANDLE_p h) {
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

/**
 * \brief           Invalidate widget for redraw 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t
gui_widget_invalidate__(GUI_HANDLE_p h) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    ret = invalidate_widget(h, 1);                  /* Invalidate widget with clipping */
    
    if (
        (
            gui_widget_getflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT) || 
            gui_widget_getcoreflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT)
#if GUI_CFG_USE_TRANSPARENCY
            || gui_widget_istransparent__(h)        /* At least little transparent */
#endif /* GUI_CFG_USE_TRANSPARENCY */
        ) && __GH(h)->Parent) {
        invalidate_widget(__GH(h)->Parent, 0);      /* Invalidate parent object too but without clipping */
    }
#if GUI_CFG_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_invalidate);
#endif /* GUI_CFG_OS */
    return ret;
}

/**
 * \brief           Invalidate widget and parent widget for redraw 
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \hideinitializer
 */
uint8_t
gui_widget_invalidatewithparent__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    invalidate_widget(h, 1);                        /* Invalidate object with clipping */
    if (__GH(h)->Parent) {                          /* If parent exists, invalid only parent */
        invalidate_widget(__GH(h)->Parent, 0);      /* Invalidate parent object without clipping */
    }
    return 1;
}

/**
 * \brief           Set if parent widget should be invalidated when we invalidate primary widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \note            Useful for widgets where there is no background: Transparent images, textview, slider, etc
 * \param[in]       h: Widget handle
 * \param[in]       value: Value either to enable or disable. 0 = disable, > 0 = enable
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 */
uint8_t
gui_widget_setinvalidatewithparent__(GUI_HANDLE_p h, uint8_t value) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (value) {                                    /* On positive value */
        gui_widget_setflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Enable auto invalidation of parent widget */
    } else {                                        /* On zero */
        gui_widget_clrflag__(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Disable auto invalidation of parent widget */
    }
    return 1;
}

/**
 * \brief           Set 3D mode on widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value to enable, either 1 or 0
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t
gui_widget_set3dstyle__(GUI_HANDLE_p h, uint8_t enable) {
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

/**
 * \brief           Create new widget and add it to linked list to parent object
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       *widget: Pointer to \ref GUI_WIDGET_t structure with widget description
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Widget callback function. Set to NULL to use default widget specific callback
 * \param[in]       flags: Flags for create procedure
 * \retval          > 0: Pointer to allocated memory for widget with default settings
 * \retval          0: Widget creation failed
 * \sa              __gui_widget_remove
 */
void*
gui_widget_create__(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_HANDLE_p h;
    
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
    
    h = GUI_MEMALLOC(widget->Size);                 /* Allocate memory for widget */
    if (h) {
        GUI_WIDGET_PARAM_t param = {0};
        GUI_WIDGET_RESULT_t result = {0};
    
        __GUI_ENTER();                              /* Enter GUI */
        memset(h, 0x00, widget->Size);              /* Set memory to 0 */
        
        __GH(h)->Id = id;                           /* Save ID */
        __GH(h)->Widget = widget;                   /* Widget object structure */
        __GH(h)->Footprint = GUI_WIDGET_FOOTPRINT;  /* Set widget footprint */
        __GH(h)->Callback = cb;                     /* Set widget callback */
#if GUI_CFG_USE_TRANSPARENCY
        __GH(h)->Transparency = 0xFF;               /* Set full transparency by default */
#endif /* GUI_CFG_USE_TRANSPARENCY */
        
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
        
        GUI_WIDGET_RESULTTYPE_U8(&result) = 1;
        gui_widget_callback__(h, GUI_WC_PreInit, NULL, &result);    /* Notify internal widget library about init successful */
        
        if (!GUI_WIDGET_RESULTTYPE_U8(&result)) {   /* Check result */
            GUI_MEMFREE(h);                         /* Clear widget memory */
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

        GUI_WIDGET_RESULTTYPE_U8(&result) = 0;
        gui_widget_callback__(h, GUI_WC_ExcludeLinkedList, NULL, &result);
        if (!GUI_WIDGET_RESULTTYPE_U8(&result)) {   /* Check if widget should be added to linked list */
            gui_linkedlist_widgetadd((GUI_HANDLE_ROOT_t *)__GH(h)->Parent, h);  /* Add entry to linkedlist of parent widget */
        }
        gui_widget_callback__(h, GUI_WC_Init, NULL, NULL);  /* Notify user about init successful */
        gui_widget_invalidate__(h);                 /* Invalidate object */
        
        if (__GH(h)->Parent) {                      /* If widget has parent */
            GUI_WIDGET_PARAMTYPE_HANDLE(&param) = h;
            gui_widget_callback__(__GH(h)->Parent, GUI_WC_ChildWidgetCreated, &param, NULL);    /* Notify user about init successful */
        }
        
#if GUI_CFG_OS
        static gui_mbox_msg_t msg = {GUI_SYS_MBOX_TYPE_WIDGET_CREATED};
        gui_sys_mbox_putnow(&GUI.OS.mbox, &msg);    /* Post message queue */
#endif /* GUI_CFG_OS */
        __GUI_LEAVE();                              /* Leave GUI */
    }
    
    return (void *)h;
}

/**
 * \brief           Remove widget and all of its children widgets
 *  
 *                  Function checks widget and all its children if they can be deleted. 
 *                  If so, flag for delete will be set and procedure will be executed later when all other processing is done
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param           *h: Widget handle to remove
 * \retval          0: Failed
 * \retval          1: Success
 * \sa              __gui_widget_create, gui_widget_remove
 */
uint8_t
gui_widget_remove__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (can_remove_widget(h)) {                     /* Check if we can delete widget */
        gui_widget_setflag__(h, GUI_FLAG_REMOVE);   /* Set flag for widget delete */
        GUI.Flags |= GUI_FLAG_REMOVE;               /* Set flag for to remove at least one widget from tree */
        if (gui_widget_isfocused__(h)) {            /* In case current widget is in focus */
            gui_widget_focus_set(__GH(h)->Parent);  /* Set parent as focused */
        }
#if GUI_CFG_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_remove);  /* Put message to queue */
#endif /* GUI_CFG_OS */
        return 1;                                   /* Widget will be deleted */
    }
    return 0;
}

/*******************************************/
/**    Widget text and font management    **/
/*******************************************/
/**
 * \brief           Set font used for widget drawing
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       *font: Pointer to \ref GUI_FONT_t structure with font information
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t
gui_widget_setfont__(GUI_HANDLE_p h, const GUI_FONT_t* font) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (__GH(h)->Font != font) {                    /* Any parameter changed */
        __GH(h)->Font = font;                       /* Set parameter */
        gui_widget_invalidatewithparent__(h);       /* Invalidate object */
    }
    return 1;
}

/**
 * \brief           Set text for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            When memory for text is dynamically allocated, text will be copied to allocated memory,
 *                     otherwise it will just set the pointer to new text.
 *                     Any changes on this text after function call will affect on later results
 *
 * \param[in,out]   h: Widget handle
 * \param[in]       *text: Pointer to text to set
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t
gui_widget_settext__(GUI_HANDLE_p h, const GUI_Char* text) {
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

/**
 * \brief           Allocate text memory for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \retval          1: Successful
 * \retval          0: Failed
 * \sa              gui_widget_freetextmemory__, gui_widget_alloctextmemory, gui_widget_freetextmemory
 * \hideinitializer
 */
uint8_t
gui_widget_alloctextmemory__(GUI_HANDLE_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {  /* Check if already allocated */
        GUI_MEMFREE(__GH(h)->Text);                 /* Free memory first */
        __GH(h)->TextMemSize = 0;                   /* Reset memory size */
    }
    __GH(h)->Text = 0;                              /* Reset pointer */
    
    __GH(h)->TextMemSize = size * sizeof(GUI_Char); /* Allocate text memory */
    __GH(h)->Text = GUI_MEMALLOC(__GH(h)->TextMemSize); /* Allocate memory for text */
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

/**
 * \brief           Free text memory for widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Successful
 * \retval          0: Failed
 * \sa              __gui_widget_alloctextmemory, gui_widget_alloctextmemory, gui_widget_freetextmemory
 * \hideinitializer
 */
uint8_t
gui_widget_freetextmemory__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {  /* Check if dynamically alocated */
        GUI_MEMFREE(__GH(h)->Text);                 /* Free memory first */
        __GH(h)->Text = 0;                          /* Reset memory */
        __GH(h)->TextMemSize = 0;                   /* Reset memory size */
        gui_widget_clrflag__(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
        gui_widget_invalidate__(h);                 /* Redraw object */
        gui_widget_callback__(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    }
    return 1;
}

/**
 * \brief           Check if widget has set font and text
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle
 * \retval          1: Widget has font and text set
 * \retval          0: Widget does not have font or text set
 */
uint8_t
gui_widget_isfontandtextset__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return __GH(h)->Text && __GH(h)->Text[0] && __GH(h)->Font && gui_string_length(__GH(h)->Text);  /* Check if conditions are met for drawing string */
}

/**
 * \brief           Process text key (add character, remove it, move cursor, etc)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       *kb: Pointer to \ref __GUI_KeyboardData_t structure
 * \retval          1: Key processed
 * \retval          0: Key not processed
 */
uint8_t
gui_widget_processtextkey__(GUI_HANDLE_p h, __GUI_KeyboardData_t* kb) {
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

/**
 * \brief           Get text from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to text from widget
 */
const GUI_Char*
gui_widget_gettext__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* Prepare for transpate support */
#if GUI_CFG_USE_TRANSLATE
    /* For static texts only */
    if (!gui_widget_getflag__(h, GUI_FLAG_DYNAMICTEXTALLOC) && __GH(h)->Text) {
        return gui_translate_get(__GH(h)->Text);    /* Get translation entry */
    }
#endif /* GUI_CFG_USE_TRANSLATE */
    return __GH(h)->Text;                           /* Return text for widget */
}

/**
 * \brief           Get font from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to font used for widget
 */
const GUI_FONT_t*
gui_widget_getfont__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return __GH(h)->Font;                           /* Return font for widget */
}

/*******************************************/
/**         Widget size management        **/
/*******************************************/
/**
 * \brief           Set width of widget in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              gui_widget_setheight, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setwidth__(GUI_HANDLE_p h, GUI_Dim_t width) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT)) {  /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_clrflag__(h, GUI_FLAG_WIDTH_PERCENT);    /* Set percentage flag */
        __GH(h)->Height = width + 1;                /* Invalidate height */
    }
    return set_widget_size(h, width, __GH(h)->Height);  /* Set new height */
}

/**
 * \brief           Set height of widget in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in units of pixels
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              gui_widget_setwidth, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setheight__(GUI_HANDLE_p h, GUI_Dim_t height) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_HEIGHT_PERCENT)) { /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_clrflag__(h, GUI_FLAG_HEIGHT_PERCENT);   /* Set percentage flag */
        __GH(h)->Height = height + 1;               /* Invalidate height */
    }
    return set_widget_size(h, __GH(h)->Width, height);  /* Set new height */
}

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setwidthpercent__(GUI_HANDLE_p h, float width) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT)) { /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_setflag__(h, GUI_FLAG_WIDTH_PERCENT);    /* Set percentage flag */
        __GH(h)->Width = width + 1;                 /* Invalidate widget */
    }
    return set_widget_size(h, width, __GH(h)->Height);  /* Set new width */
}

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in percentage
 * \retval          1: Successful
 * \retval          1: Failed
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setwidthpercent
 */
uint8_t
gui_widget_setheightpercent__(GUI_HANDLE_p h, float height) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_HEIGHT_PERCENT)) {    /* Invalidate if percent not yet enabled to force invalidation */
        gui_widget_setflag__(h, GUI_FLAG_HEIGHT_PERCENT);   /* Set percentage flag */
        __GH(h)->Height = height + 1;               /* Invalidate height */
    }
    return set_widget_size(h, __GH(h)->Width, height);  /* Set new height */
}

/**
 * \brief           Set widget size in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t
gui_widget_setsize__(GUI_HANDLE_p h, GUI_Dim_t wi, GUI_Dim_t hi) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* If percentage enabled on at least one, either width or height */
    if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT)) {
        gui_widget_setflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT);  /* Set both flags */
        __GH(h)->Width = wi + 1;                    /* Invalidate width */
        __GH(h)->Height = hi + 1;                   /* Invalidate height */
    }
    return set_widget_size(h, wi, hi);              /* Set widget size */
}

/**
 * \brief           Set widget size in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t
gui_widget_setsizepercent__(GUI_HANDLE_p h, float wi, float hi) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* If percentage not enable on width or height */
    if (gui_widget_getflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT) != (GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT)) {
        gui_widget_setflag__(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT);  /* Set both flags */
        __GH(h)->Width = wi + 1;                    /* Invalidate width */
        __GH(h)->Height = hi + 1;                   /* Invalidate height */
    }
    return set_widget_size(h, wi, hi);              /* Set widget size */
}

/**
 * \brief           Toggle expandend (maximized) mode of widget (mostly of windows)
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget expandend status toggled ok
 * \retval          0: Widget expandend status was not toggled
 */
uint8_t
gui_widget_toggleexpanded__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return gui_widget_setexpanded__(h, !gui_widget_isexpanded__(h));    /* Invert expanded mode */
}

/**
 * \brief           Set expandend mode on widget. When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \retval          1: Widget expandend status set ok
 * \retval          0: Widget expandend status was not set
 */
uint8_t
gui_widget_setexpanded__(GUI_HANDLE_p h, uint8_t state) {
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
/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setpositionpercent
 */
uint8_t
gui_widget_setposition__(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    /* If percent enabled on at least one coordinate, clear to force invalidation */
    if (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT)) {
        gui_widget_clrflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT); /* Disable percent on X and Y position */
        __GH(h)->X = x + 1;                         /* Invalidate X position */
        __GH(h)->Y = y + 1;                         /* Invalidate Y position */
    }  
    return set_widget_position(h, x, y);            /* Set widget position */
}
 
/**
 * \brief           Set widget position relative to parent object in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setposition
 */
uint8_t
gui_widget_setpositionpercent__(GUI_HANDLE_p h, float x, float y) {  
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */   
    /* If percent not set on both, enable to force invalidation */
    if (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT) != (GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT)) {
        gui_widget_setflag__(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT); /* Enable percent on X and Y position */
        __GH(h)->X = x + 1;                         /* Invalidate X position */
        __GH(h)->Y = y + 1;                         /* Invalidate Y position */
    }
    return set_widget_position(h, x, y);            /* Set widget position */
}
 
/**
 * \brief           Set widget X position relative to parent object in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setxpositionpercent
 */
uint8_t
gui_widget_setxposition__(GUI_HANDLE_p h, GUI_iDim_t x) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT)) {   /* if percent enabled */
        gui_widget_clrflag__(h, GUI_FLAG_XPOS_PERCENT); /* Clear it to force invalidation */
        __GH(h)->X = x + 1;                         /* Invalidate position */
    }
    return set_widget_position(h, x, __GH(h)->Y);   /* Set widget position */
}
 
/**
 * \brief           Set widget X position relative to parent object in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setxposition
 */
uint8_t
gui_widget_setxpositionpercent__(GUI_HANDLE_p h, float x) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_XPOS_PERCENT)) {  /* if percent not enabled */
        gui_widget_setflag__(h, GUI_FLAG_XPOS_PERCENT); /* Set it to force invalidation */
        __GH(h)->X = x + 1;                         /* Invalidate position */
    }
    return set_widget_position(h, x, __GH(h)->Y);   /* Set widget position */
}
 
/**
 * \brief           Set widget Y position relative to parent object in units of pixels
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setypositionpercent
 */
uint8_t
gui_widget_setyposition__(GUI_HANDLE_p h, GUI_iDim_t y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (gui_widget_getflag__(h, GUI_FLAG_YPOS_PERCENT)) {   /* if percent enabled */
        gui_widget_clrflag__(h, GUI_FLAG_YPOS_PERCENT); /* Clear it to force invalidation */
        __GH(h)->Y = y + 1;                         /* Invalidate position */
    }
    return set_widget_position(h, __GH(h)->X, y);   /* Set widget position */
}
 
/**
 * \brief           Set widget Y position relative to parent object in units of percent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setyposition
 */
uint8_t
gui_widget_setypositionpercent__(GUI_HANDLE_p h, float y) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    if (!gui_widget_getflag__(h, GUI_FLAG_YPOS_PERCENT)) {  /* if percent not enabled */
        gui_widget_setflag__(h, GUI_FLAG_YPOS_PERCENT); /* Set it to force invalidation */
        __GH(h)->X = y + 1;                         /* Invalidate position */
    }
    return set_widget_position(h, __GH(h)->X, y);   /* Set widget position */
}

/*******************************************/
/**                  .....                **/
/*******************************************/
/**
 * \brief           Show widget from visible area
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget shown
 * \retval          0: Widget not shown
 * \sa              __gui_widget_hide
 */
uint8_t
gui_widget_show__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (gui_widget_getflag__(h, GUI_FLAG_HIDDEN)) { /* If hidden, show it */
        gui_widget_clrflag__(h, GUI_FLAG_HIDDEN);
        gui_widget_invalidatewithparent__(h);       /* Invalidate it for redraw with parent */
    }
    return 1;
}

/**
 * \brief           Hide widget from visible area
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget hidden
 * \retval          0: Widget not hidden
 * \sa              __gui_widget_show
 */
uint8_t
gui_widget_hide__(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (!gui_widget_getflag__(h, GUI_FLAG_HIDDEN)) {    /* If visible, hide it */
        gui_widget_setflag__(h, GUI_FLAG_HIDDEN);
        gui_widget_invalidatewithparent__(h);       /* Invalidate it for redraw with parenta */
    }
    
    /**
     * TODO: Check if active/focused widget is maybe children of this widget
     */
    
    if (GUI.FocusedWidget && (GUI.FocusedWidget == h || gui_widget_ischildof__(GUI.FocusedWidget, h))) {    /* Clear focus */
        gui_widget_focus_set(__GH(h)->Parent);      /* Set parent widget as focused now */
    }
    if (GUI.ActiveWidget && (GUI.ActiveWidget == h || gui_widget_ischildof__(GUI.ActiveWidget, h))) {   /* Clear active */
        gui_widget_active_clear();
    }
    return 1;
}

/**
 * \brief           Check if widget is children of parent
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \retval          1: Widget handle is in tree of parent handle
 * \retval          0: Widget handle is not in tree of parent handle
 */
uint8_t
gui_widget_ischildof__(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    //__GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_iswidget__(parent));  /* Check valid parameter */
    
    if (!(gui_widget_iswidget__(h) && gui_widget_iswidget__(parent)) || !(GUI.Initialized)) {
        return 0;                                   \
    }
    
    for (h = __GH(h)->Parent; h; h = __GH(h)->Parent) { /* Check widget parent objects */
        if (parent == h) {                          /* If they matches */
            return 1;
        }
    }
    return 0;
}

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \retval          1: New value is different than previous and modification has been done
 * \retval          0: New value was not set
 */
uint8_t
gui_widget_setzindex__(GUI_HANDLE_p h, int32_t zindex) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    uint8_t ret = 1;
    
    if (__GH(h)->ZIndex != zindex) {                /* There was a change in z-index value */
        int32_t current = __GH(h)->ZIndex;
        __GH(h)->ZIndex = zindex;                   /* Set new index */
        if (zindex < current) {                     /* New index value is less important than before = move widget to top */
            gui_linkedlist_widgetmovetotop(h);      /* Move widget to top on linked list = less important and less visible */
        } else {
            gui_linkedlist_widgetmovetobottom(h);   /* Move widget to bottom on linked list = most important and most visible */
        }
    }
    return ret;
}

#if GUI_CFG_USE_TRANSPARENCY || defined(DOXYGEN)
/**
 * \brief           Set transparency level to widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       trans: Transparency level, where 0x00 means hidden and 0xFF means totally visible widget
 * \retval          1: Transparency set ok
 * \retval          0: Transparency was not set
 * \sa              __gui_widget_settransparency
 */
uint8_t
gui_widget_settransparency__(GUI_HANDLE_p h, uint8_t trans) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    
    if (__GH(h)->Transparency != trans) {           /* Check transparency match */
        __GH(h)->Transparency = trans;              /* Set new transparency level */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    }
    
    return 1;
}
#endif /* GUI_CFG_USE_TRANSPARENCY */

/**
 * \brief           Set color to widget specific index
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors
 * \param[in]       color: Actual color code to set
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_widget_setcolor__(GUI_HANDLE_p h, uint8_t index, GUI_Color_t color) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI context */
    if (!__GH(h)->Colors) {                         /* Do we need to allocate color memory? */
        if (__GH(h)->Widget->ColorsCount) {         /* Check if at least some colors should be used */
            __GH(h)->Colors = GUI_MEMALLOC(sizeof(*__GH(h)->Colors) * __GH(h)->Widget->ColorsCount);
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

/**
 * \brief           Get first widget handle by ID
 * \note            If multiple widgets have the same ID, first found will be used
 *
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   id: Widget ID to search for
 * \retval          > 0: Widget handle when widget found
 * \retval          0: Widget not found
 */
GUI_HANDLE_p
gui_widget_getbyid__(GUI_ID_t id) {
    return get_widget_by_id(NULL, id, 1);            /* Find widget by ID */ 
}

/**
 * \brief           Set custom user data to widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 *
 * \note            Specially useful in callback processing if required
 * \param[in,out]   h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \retval          1: Data were set OK
 * \retval          0: Data were not set
 * \sa              gui_widget_getuserdata__
 */
uint8_t
gui_widget_setuserdata__(GUI_HANDLE_p h, void* data) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */    
    __GH(h)->UserData = data;                       /* Set user data */
    return 1;
}

/**
 * \brief           Get custom user data from widget previously set with \ref gui_widget_setuserdata
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to user data
 * \sa              __gui_widget_setuserdata
 */
void*
gui_widget_getuserdata__(GUI_HANDLE_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    data = __GH(h)->UserData;                       /* Get user data */
    return data;
}

/**
 * \brief           Set widget parameter in OS secure way
 * \param[in,out]   h: Widget handle
 * \param[in]       cfg: Configuration to use, passed later to callback function
 * \param[in]       *data: Custom data to pass later to configuration callback
 * \param[in]       invalidate: Flag if widget should be invalidated after parameter change
 * \param[in]       invalidateparent: change if parent widget should be invalidated after parameter change
 * \retval          Pointer to user data
 * \sa              __gui_widget_setuserdata
 */
uint8_t
gui_widget_setparam__(GUI_HANDLE_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent) {
    GUI_WIDGET_Param_t p;
    GUI_WIDGET_PARAM_t param = {0};
    GUI_WIDGET_RESULT_t result = {0};
    
    GUI_WIDGET_PARAMTYPE_WIDGETPARAM(&param) = &p;
    GUI_WIDGET_RESULTTYPE_U8(&result) = 1;
    
    p.Type = cfg;
    p.Data = (void *)data;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    gui_widget_callback__(h, GUI_WC_SetParam, &param, &result); /* Process callback function */
    if (invalidateparent) {
        gui_widget_invalidatewithparent__(h);       /* Invalidate widget and parent */
    } else if (invalidate) {
        gui_widget_invalidate__(h);                 /* Invalidate widget only */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return 1;
}

/******************************************************************************/
/******************************************************************************/
/***                  Thread safe version of public API                      **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Remove widget from memory
 * \note            If widget has child widgets, they will be removed too
 * \param[in,out]   *h: Pointer to widget handle. If removed, pointer will be invalidated and set to NULL
 * \retval          1: Widget deleted
 * \retval          0: Delete failed
 */
uint8_t
gui_widget_remove(GUI_HANDLE_p* h) {
    __GUI_ASSERTPARAMS(h && gui_widget_iswidget__(*h));     /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */

    gui_widget_remove__(*h);                        /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;                                       /* Removev successfully */
}

/*******************************************/
/**         Widget text management        **/
/*******************************************/
/**
 * \brief           Allocate memory for text operations if text will be dynamic
 * \note            When unicode feature is enabled, memory should be 4x required characters because unicode can store up to 4 bytes for single character
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \retval          Number of bytes allocated
 * \sa              gui_widget_freetextmemory
 */
uint32_t gui_widget_alloctextmemory(GUI_HANDLE_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && size > 1);   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    gui_widget_alloctextmemory__(h, size);          /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return __GH(h)->TextMemSize;                    /* Return number of bytes allocated */
}

/**
 * \brief           Frees memory previously allocated for text
 * \param[in,out]   h: Widget handle to free memory on
 * \retval          1: Free was ok
 * \retval          0: Free was not ok
 * \sa              gui_widget_alloctextmemory
 */
uint8_t
gui_widget_freetextmemory(GUI_HANDLE_p h) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_freetextmemory__(h);           /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set text to widget
 * \note            If dynamic memory allocation was used then content will be copied to allocated memory
 *                     otherwise only pointer to input text will be used 
 *                     and each further change of input pointer text will affect to output
 * \param[in,out]   h: Widget handle
 * \param[in]       *text: Pointer to text to set to widget
 * \retval          1: Text was set ok
 * \retval          0: Text was not set
 * \sa              gui_widget_alloctextmemory, gui_widget_freetextmemory, gui_widget_setfont, gui_widget_gettext, gui_widget_gettextcopy
 */
uint8_t
gui_widget_settext(GUI_HANDLE_p h, const GUI_Char* text) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_settext__(h, text);            /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get text from widget
 * \note            It will return pointer to text which cannot be modified directly.
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to text from widget
 * \sa              gui_widget_settext, gui_widget_gettextcopy
 */
const GUI_Char*
gui_widget_gettext(GUI_HANDLE_p h) {
    const GUI_Char* t;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = gui_widget_gettext__(h);                    /* Return text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;
}

/**
 * \brief           Get text from widget
 * \note            Text from widget is copied to input pointer
 * \param[in,out]   h: Widget handle
 * \param[out]      *dst: Destination pointer
 * \param[in]       len: Size of output buffer in units of \ref GUI_Char
 * \retval          Pointer to text from widget
 * \sa              gui_widget_settext, gui_widget_gettext
 */
const GUI_Char*
gui_widget_gettextcopy(GUI_HANDLE_p h, GUI_Char* dst, uint32_t len) {
    const GUI_Char* t;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = gui_widget_gettext__(h);                    /* Return text */
    gui_string_copyn(dst, t, len);                  /* Copy text after */
    dst[len] = 0;                                   /* Set trailling zero */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;  
}

/**
 * \brief           Set widget font for drawing operations
 * \param[in,out]   h: Widget handle
 * \param[in]       *font: Pointer to \ref GUI_FONT_t object for font
 * \retval          1: Font was set ok
 * \retval          0: Font was not set
 * \sa              gui_widget_settext, gui_widget_gettext
 */
uint8_t
gui_widget_setfont(GUI_HANDLE_p h, const GUI_FONT_t* font) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setfont__(h, font);            /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get font from widget
 * \note            Since this function is private, it can only be used by user inside GUI library
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to font used for widget
 */
const GUI_FONT_t*
gui_widget_getfont(GUI_HANDLE_p h) {
    const GUI_FONT_t* font;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    font = gui_widget_getfont__(h);                 /* Get widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return font;
}

/*******************************************/
/**         Widget size management        **/
/*******************************************/
/**
 * \brief           Set widget size in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width value
 * \param[in]       height: Height value
 * \retval          1: Size was set ok
 * \retval          0: Size was not set
 * \sa              gui_widget_setsizepercent
 */
uint8_t
gui_widget_setsize(GUI_HANDLE_p h, GUI_Dim_t width, GUI_Dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setsize__(h, width, height);   /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget size in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width value
 * \param[in]       height: Height value
 * \retval          1: Size was set ok
 * \retval          0: Size was not set
 * \sa              gui_widget_setsize
 */
uint8_t
gui_widget_setsizepercent(GUI_HANDLE_p h, float width, float height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setsizepercent__(h, width, height);    /* Set actual size to object in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set width of widget in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \retval          1: Width was set ok
 * \retval          0: Width was not set
 * \sa              gui_widget_setheight, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setwidth(GUI_HANDLE_p h, GUI_Dim_t width) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setwidth__(h, width);          /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set height of widget in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in units of pixels
 * \retval          1: Height was set ok
 * \retval          0: Height was not set
 * \sa              gui_widget_setwidth, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setheight(GUI_HANDLE_p h, GUI_Dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setheight__(h, height);        /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \retval          1: Width was set ok
 * \retval          0: Width was not set
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setwidthpercent(GUI_HANDLE_p h, float width) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setwidthpercent__(h, width);   /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       height: Height in percentage
 * \retval          1: Height was set ok
 * \retval          0: Height was not set
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setwidthpercent
 */
uint8_t
gui_widget_setheightpercent(GUI_HANDLE_p h, float height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setheightpercent__(h, height); /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get total width of widget effective on screen in units of pixels
 *                     Function returns width of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total width in units of pixels
 * \sa              gui_widget_getheight, gui_widget_setwidth
 */
GUI_Dim_t
gui_widget_getwidth(GUI_HANDLE_p h) {
    GUI_Dim_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_getwidth__(h);                 /* Get widget width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get total height of widget effective on screen in units of pixels
 *                     Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref GUI_HANDLE_p structure
 * \retval          Total height in units of pixels
 * \sa              gui_widget_getwidth, gui_widget_setheight
 * \hideinitializer
 */
GUI_Dim_t
gui_widget_getheight(GUI_HANDLE_p h) {
    GUI_Dim_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_getheight__(h);                /* Get widget height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set expandend mode on widget. When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \retval          1: Widget expandend status set ok
 * \retval          0: Widget expandend status was not set
 * \sa              gui_widget_isexpanded
 */
uint8_t
gui_widget_setexpanded(GUI_HANDLE_p h, uint8_t state) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_setexpanded__(h, state);       /* Set expanded mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get widget expanded status
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget is expanded
 * \retval          0: Widget is not expanded
 * \sa              gui_widget_setexpanded
 */
uint8_t
gui_widget_isexpanded(GUI_HANDLE_p h) {
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

/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setpositionpercent
 */
uint8_t
gui_widget_setposition(GUI_HANDLE_p h, GUI_iDim_t x, GUI_iDim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setposition__(h, x, y);        /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setposition
 */
uint8_t
gui_widget_setpositionpercent(GUI_HANDLE_p h, float x, float y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setpositionpercent__(h, x, y); /* Set X and Y position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget X position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setxpositionpercent
 */
uint8_t
gui_widget_setxposition(GUI_HANDLE_p h, GUI_iDim_t x) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setxposition__(h, x);          /* Set X position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget X position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setxposition
 */
uint8_t
gui_widget_setxpositionpercent(GUI_HANDLE_p h, float x) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setxpositionpercent__(h, x);   /* Set X position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget Y position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setypositionpercent
 */
uint8_t
gui_widget_setyposition(GUI_HANDLE_p h, GUI_iDim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setyposition__(h, y);          /* Set Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget Y position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \retval          1: Position was set ok
 * \retval          0: Position was not set
 * \sa              gui_widget_setyposition
 */
uint8_t
gui_widget_setypositionpercent(GUI_HANDLE_p h, float y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_setypositionpercent__(h, y);   /* Set Y position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Invalidate widget object and prepare to new redraw
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was invalidated ok
 * \retval          0: Widget was not invalidated
 */
uint8_t
gui_widget_invalidate(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_invalidate__(h);               /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Show widget from visible area
 * \param[in,out]   h: Widget handle
 * \retval          1: Text was shown ok
 * \retval          0: Text was not shown
 * \sa              gui_widget_hide
 */
uint8_t
gui_widget_show(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_show__(h);                     /* Show widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Put widget to front view and put it to focused state
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was put ok
 * \retval          0: Widget was not put
 * \sa              gui_widget_hide
 */
uint8_t
gui_widget_putonfront(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    gui_widget_movedowntree(h);                     /* Put widget on front */
    gui_widget_focus_set(h);                        /* Set widget to focused state */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Hide widget from visible area
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget was hidden ok
 * \retval          0: Widget was not hidden
 * \sa              gui_widget_show, gui_widget_putonfront
 */
uint8_t
gui_widget_hide(GUI_HANDLE_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = gui_widget_hide__(h);                     /* Hide widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get widget ID
 * \param[in,out]   h: Widget handle
 * \retval          Widget ID
 */
GUI_ID_t
gui_widget_getid(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    return gui_widget_getid__(h);                   /* Return widget ID */
}

/**
 * \brief           Get first widget handle by ID
 * \note            If multiple widgets have the same ID, first found will be used
 * \param[in,out]   id: Widget ID to search for
 * \retval          > 0: Widget handle when widget found
 * \retval          0: Widget not found
 */
GUI_HANDLE_p
gui_widget_getbyid(GUI_ID_t id) {
    GUI_HANDLE_p h;
    __GUI_ENTER();                                  /* Enter GUI */
    
    h = get_widget_by_id(NULL, id, 1);               /* Find widget by ID */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h; 
}

/**
 * \brief           Check if widget is children of parent
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \retval          1: Widget handle is in tree of parent handle
 * \retval          0: Widget handle is not in tree of parent handle
 */
uint8_t
gui_widget_ischildof(GUI_HANDLE_p h, GUI_HANDLE_p parent) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_iswidget__(parent));  /* Check valid parameter */
    return gui_widget_ischildof__(h, parent);       /* Return status */
}

/**
 * \brief           Set custom user data to widget
 * \note            Specially useful in callback processing if required
 * \param[in,out]   h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \retval          1: Data were set OK
 * \retval          0: Data were not set
 * \sa              gui_widget_getuserdata
 */
uint8_t
gui_widget_setuserdata(GUI_HANDLE_p h, void* data) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    gui_widget_setuserdata__(h, data);              /* Set user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get custom user data from widget previously set with \ref gui_widget_setuserdata
 * \param[in,out]   h: Widget handle
 * \retval          Pointer to user data
 * \sa              gui_widget_setuserdata
 */
void*
gui_widget_getuserdata(GUI_HANDLE_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    data = gui_widget_getuserdata__(h);             /* Get user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return data;
}

/**
 * \brief           Widget callback function for all events
 * \note            Called either from GUI stack or from widget itself to notify user
 *
 * \note            Call this function inside custom callback widget function for unhandled events
 *                     It will automatically call required function according to input widget
 * \param[in,out]   h: Widget handle where callback occurred
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref GUI_WC_t enumeration
 * \param[in]       *param: Pointer to optional input data for command. Check \ref GUI_WC_t enumeration for more informations
 * \param[out]      *result: Pointer to optional result value. Check \ref GUI_WC_t enumeration for more informations
 * \retval          1: Command has been processed
 * \retval          0: Command has not been processed
 */
uint8_t
gui_widget_processdefaultcallback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GH(h)->Widget->Callback(h, ctrl, param, result);    /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set callback function to widget
 * \param[in,out]   h: Widget handle object
 * \param[in]       callback: Callback function for widget
 * \retval          1: Callback was set ok
 * \retval          1: Callback was not set
 * \sa              gui_widget_processdefaultcallback, gui_widget_callback
 */
uint8_t
gui_widget_setcallback(GUI_HANDLE_p h, GUI_WIDGET_CALLBACK_t callback) {
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GH(h)->Callback = callback;                   /* Set callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Widget callback function for all events
 * \note            Called from user outside callback. For calling default callback
 *                      inside custom callback for widget, use \ref gui_widget_processdefaultcallback instead.
 *                      If called from inside widget callback, it may result in recursive calls.
 *
 * \param[in,out]   h: Widget handle where callback occurred
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref GUI_WC_t enumeration
 * \param[in]       *param: Pointer to optional input data for command. Check \ref GUI_WC_t enumeration for more informations
 * \param[out]      *result: Pointer to optional result value. Check \ref GUI_WC_t enumeration for more informations
 * \retval          1: Command has been processed
 * \retval          0: Command has not been processed
 * \sa              gui_widget_setcallback
 */
uint8_t
gui_widget_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_callback__(h, ctrl, param, result);/* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set widget scroll on X axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for X direction
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              gui_widget_setscrolly
 */
uint8_t
gui_widget_setscrollx(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_allowchildren__(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GHR(h)->ScrollX != scroll) {              /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollX = scroll;
        gui_widget_invalidate__(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set widget scroll on Y axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for Y direction
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              gui_widget_setscrollx
 */
uint8_t
gui_widget_setscrolly(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_allowchildren__(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GHR(h)->ScrollY != scroll) {              /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollY = scroll;
        gui_widget_invalidate__(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Increase widget scroll on X axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll increase in units of pixels. Use negative value to decrease scroll
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              gui_widget_incscrolly
 */
uint8_t
gui_widget_incscrollx(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_allowchildren__(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (scroll) {                                   /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollX += scroll;
        gui_widget_invalidate__(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Increase widget scroll on Y axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll increase in units of pixels. Use negative value to decrease scroll
 * \retval          1: Scroll was set ok
 * \retval          0: Scroll was not set
 * \sa              gui_widget_incscrollx
 */
uint8_t
gui_widget_incscrolly(GUI_HANDLE_p h, GUI_iDim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_allowchildren__(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (scroll) {                                   /* Only widgets with children support can set scroll */
        __GHR(h)->ScrollY += scroll;
        gui_widget_invalidate__(h);
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get widget scroll on X axis
 * \param[in,out]   h: Widget handle
 * \return          Widget scroll in units of pixels
 * \sa              gui_widget_getscrolly
 */
GUI_iDim_t
gui_widget_getscrollx(GUI_HANDLE_p h) {
    GUI_iDim_t value = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_allowchildren__(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    value = __GHR(h)->ScrollX;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return value;
}

/**
 * \brief           Get widget scroll on Y axis
 * \param[in,out]   h: Widget handle
 * \return          Widget scroll in units of pixels
 * \sa              gui_widget_getscrollx
 */
GUI_iDim_t
gui_widget_getscrolly(GUI_HANDLE_p h) {
    GUI_iDim_t value = 0;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h) && gui_widget_allowchildren__(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    value = __GHR(h)->ScrollY;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return value;
}

/**
 * \brief           Manually set widget in focus
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget set to focus
 * \retval          0: Widget was not set to focus
 */
uint8_t
gui_widget_setfocus(GUI_HANDLE_p h) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    gui_widget_focus_set(h);                        /* Put widget in focus */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set default font for widgets used on widget creation
 * \param[in]       *font: Pointer to \ref GUI_FONT_t with font
 * \retval          1: Font was set ok
 * \retval          0: Font was not set
 */
uint8_t
gui_widget_setfontdefault(const GUI_FONT_t* font) {
    WIDGET_Default.Font = font;                     /* Set default font */
    return 1;
}

/**
 * \brief           Increase selection for widget
 * \note            Widget must implement \ref GUI_WC_IncSelection command in callback function and process it
 * \param[in,out]   h: Widget handle
 * \param[in]       dir: Increase direction. Positive number means number of increases, negative is number of decreases
 * \retval          1: Increase/Decrease was ok
 * \retval          0: Increase/Decrease was not ok
 */
uint8_t
gui_widget_incselection(GUI_HANDLE_p h, int16_t dir) {
    uint8_t ret = 0;
    GUI_WIDGET_PARAM_t param = {0};
    GUI_WIDGET_RESULT_t result = {0};
    
    GUI_WIDGET_PARAMTYPE_I16(&param) = dir;         /* Set parameter */
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_callback__(h, GUI_WC_IncSelection, &param, &result);   /* Increase selection for specific amount */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \retval          1: New value is different than previous and modification has been done
 * \retval          0: New value was not set
 * \sa              gui_widget_getzindex
 */
uint8_t
gui_widget_setzindex(GUI_HANDLE_p h, int32_t zindex) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_setzindex__(h, zindex);        /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get z-index value from widget
 * \param[in,out]   h: Widget handle
 * \retval          z-index value
 * \sa              gui_widget_setzindex
 */
int32_t
gui_widget_getzindex(GUI_HANDLE_p h) {
    int32_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_getzindex__(h);                /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

#if GUI_CFG_USE_TRANSPARENCY || defined(DOXYGEN)
/**
 * \brief           Set transparency level to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       trans: Transparency level, where 0x00 means hidden and 0xFF means totally visible widget
 * \retval          1: Transparency set ok
 * \retval          0: Transparency was not set
 * \sa              gui_widget_gettransparency
 */
uint8_t
gui_widget_settransparency(GUI_HANDLE_p h, uint8_t trans) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_settransparency__(h, trans);   /* Set widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get widget transparency value
 * \note            Value between 0 and 0xFF is used:
 *                      - 0x00: Widget is hidden
 *                      - 0xFF: Widget is fully visible
 *                      - between: Widget has transparency value
 * \param[in,out]   h: Widget handle
 * \retval          Trasparency value
 * \sa              gui_widget_settransparency
 */
uint8_t
gui_widget_gettransparency(GUI_HANDLE_p h) {
    uint8_t trans;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    trans = gui_widget_gettransparency__(h);        /* Get widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return trans;
}
#endif /* GUI_CFG_USE_TRANSPARENCY || defined(DOXYGEN) */

/**
 * \brief           Set 3D mode on widget
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value to enable, either 1 or 0
 * \retval          1: Successful
 * \retval          0: Failed
 */
uint8_t
gui_widget_set3dstyle(GUI_HANDLE_p h, uint8_t enable) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(gui_widget_iswidget__(h));   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = gui_widget_set3dstyle__(h, enable);       /* Set 3D mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

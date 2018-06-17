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

/**
 * \brief           Default widget settings
 */
typedef struct {
    const gui_font_t* font;                 /*!< Pointer to font structure */
} gui_widget_default_t;
gui_widget_default_t widget_default;


#if GUI_CFG_OS
static gui_mbox_msg_t msg_widget_remove = { GUI_SYS_MBOX_TYPE_REMOVE };
static gui_mbox_msg_t msg_widget_invalidate = { GUI_SYS_MBOX_TYPE_INVALIDATE };
#endif /* GUI_CFG_OS */

/**
 * \brief           Remove widget from memory
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
remove_widget(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    /*
     * Check and react on:
     *
     * Step 1: If widget is in focus, set its parent widget to focus. If there is no parent, disable focus
     * Step 2: Clear previous focused handle, if it is set to current widget
     * Step 3: If widget is active, clear it
     * Step 4: If widget is set as previous active, set its parent as previous active
     */
    if (GUI.FocusedWidget == h) {                   /* Step 1 */
        if (guii_widget_hasparent(h)) {             /* Should always happen as top parent (window) can't be removed */
            GUI.FocusedWidget = guii_widget_getparent(h);   /* Set parent widget as focused */
        } else {
            guii_widget_focus_clear();               /* Clear all widgets from focused */
            GUI.FocusedWidget = NULL;
        }
    }
    if (GUI.FocusedWidgetPrev == h) {               /* Step 2 */
        GUI.FocusedWidgetPrev = NULL;
    }
    if (GUI.ActiveWidget == h) {                    /* Step 3 */
        GUI.ActiveWidget = NULL;                    /* Invalidate active widget */
    }
    if (GUI.ActiveWidgetPrev == h) {                /* Step 4 */
        GUI.ActiveWidgetPrev = guii_widget_getparent(h);/* Set widget as previous active */
    }
    if (GUI.WindowActive != NULL && h == GUI.WindowActive) {/* Check for parent window */
        GUI.WindowActive = guii_widget_getparent(GUI.WindowActive);
    }
    
    /*
     * Final steps to remove widget are:
     * 
     * - Invalidate widget and its parent,
     *      make sure parent is redrawn on screen
     * - Free any possible memory used for text operation
     * - Remove software timer if exists
     * - Remove custom colors
     * - Remove widget from its linkedlist
     * - Free widget memory
     */
    guii_widget_invalidatewithparent(h);            /* Invalidate object and its parent */
    guii_widget_freetextmemory(h);                  /* Free text memory */
    if (h->timer != NULL) {                         /* Check timer memory */
        guii_timer_remove(&h->timer);               /* Free timer memory */
    }
    if (h->colors != NULL) {                        /* Check colors memory */
        GUI_MEMFREE(h->colors);                     /* Free colors memory */
        h->colors = NULL;
    }
    gui_linkedlist_widgetremove(h);                 /* Remove entry from linked list of parent widget */
    GUI_MEMFREE(h);                                 /* Free memory for widget */
    
    return 1;                                       /* Widget deleted */
}

/**
 * \brief           Check and remove all widgets with delete flag enabled using recursion
 * \param[in]       parent: Parent widget handle
 */
static void
remove_widgets(gui_handle_p parent) {
    gui_handle_p h, next;
    static uint32_t lvl = 0;
    
    /*
     * Scan all widgets in system
     */
    for (h = gui_linkedlist_widgetgetnext((gui_handle_root_t *)parent, NULL); h != NULL; ) {        
        if (guii_widget_getflag(h, GUI_FLAG_REMOVE)) { /* Widget should be deleted */
            next = gui_linkedlist_widgetgetnext(NULL, h);   /* Get next widget of current */
            
            /*
             * Before we can actually delete widget,
             * we have to check for its children widgets.
             *
             * In this case set remove flags to all children widgets
             * and process with delete of all
             *
             * Steps to perform are:
             *
             * - Step 1: Set remove flag to all direct children widgets
             * - Step 2: Perform mass erase of children widgets 
             *      This step is recursive operation.
             */
            if (guii_widget_allowchildren(h)) {    /* Children widgets are supported */
                gui_handle_p tmp;
                
                /* Step 1 */
                for (tmp = gui_linkedlist_widgetgetnext((gui_handle_root_t *)h, NULL); tmp != NULL; 
                        tmp = gui_linkedlist_widgetgetnext(NULL, tmp)) {
                    guii_widget_setflag(tmp, GUI_FLAG_REMOVE); /* Set remove bit to all children elements */
                }
                
                /* Step 2 */
                lvl++;
                remove_widgets(h);                  /* Remove children widgets directly */
                lvl--;
            }
            
            /*
             * Removing widget will also remove linked list chain
             * Therefore we have to save previous widget so we know next one
             */
            remove_widget(h);                       /* Remove widget itself */
            
            /*
             * Move widget pointer to next widget of already deleted and continue checking
             */
            h = next;                               /* Set current pointer to next one */
            continue;                               /* Continue to prevent further execution */
        } else if (guii_widget_allowchildren(h)) { /* Children widgets are supported */
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

/**
 * \brief           Get vidget visible X, Y and width, height values on screen
 * 
 *                  It will calculate where widget is (including scrolling).
 *
 * \param[in]       h: Widget handle
 * \param[out]      x1: Output variable to save top left X position on screen
 * \param[out]      y1: Output variable to save top left Y position on screen
 * \param[out]      x2: Output variable to save bottom right X position on screen
 * \param[out]      y2: Output variable to save bottom right Y position on screen
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
get_widget_abs_position_and_visible_width_height(gui_handle_p h, gui_dim_t* x1, gui_dim_t* y1, gui_dim_t* x2, gui_dim_t* y2) {
    gui_dim_t x, y, wi, hi;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    x = guii_widget_getabsolutex(h);                /* Get absolute X position */
    y = guii_widget_getabsolutey(h);                /* Get absolute Y position */
    wi = guii_widget_getwidth(h);                   /* Get absolute width */
    hi = guii_widget_getheight(h);                  /* Get absolute height */
    
    /*
     * Set widget visible positions with X and Y coordinates
     */
    *x1 = x;
    *y1 = y;
    *x2 = x + wi;
    *y2 = y + hi;
    
    /*
     * Check if widget is hidden by any parent or any parent is hidden by its parent
     */
    for (; h != NULL; h = guii_widget_getparent(h)) {
        x = guii_widget_getparentabsolutex(h);      /* Parent absolute X position for inner widgets */
        y = guii_widget_getparentabsolutey(h);      /* Parent absolute Y position for inner widgets */
        wi = guii_widget_getparentinnerwidth(h);    /* Get parent inner width */
        hi = guii_widget_getparentinnerheight(h);   /* Get parent inner height */
    
        if (*x1 < x)        { *x1 = x; }
        if (*x2 > x + wi)   { *x2 = x + wi; }
        if (*y1 < y)        { *y1 = y; }
        if (*y2 > y + hi)   { *y2 = y + hi; }
    }
    
    return 1;
}

/**
 * \brief           Set clipping region for visible part of widget
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
set_clipping_region(gui_handle_p h) {
    gui_dim_t x1, y1, x2, y2;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    /* Get visible widget part and absolute position on screen according to parent */
    get_widget_abs_position_and_visible_width_height(h, &x1, &y1, &x2, &y2);
    
    /* Possible improvement */
    /*
     * If widget has direct children widgets which are not transparent,
     * it is possible to limit clipping region for update process to only
     * part of widget which is actually visible on screen.
     *
     * This may only work if padding is 0 and widget position wasn't changed
     */
    
    /* Set invalid clipping region */
    if (GUI.Display.x1 > x1)    { GUI.Display.x1 = x1; }
    if (GUI.Display.x2 < x2)    { GUI.Display.x2 = x2; }
    if (GUI.Display.y1 > y1)    { GUI.Display.y1 = y1; }
    if (GUI.Display.y2 < y2)    { GUI.Display.y2 = y2; }
    
    return 1;
}

/**
 * \brief           Invalidate widget and set redraw flag
 * \note            If widget is transparent, parent must be updated too. This function will handle these cases.
 * \param[in]       h: Widget handle
 * \param[in]       setclipping: When set to 1, clipping region will be expanded to widget size
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
invalidate_widget(gui_handle_p h, uint8_t setclipping) {
    gui_handle_p h1, h2;
    gui_dim_t h1x1, h1x2, h2x1, h2x2;
    gui_dim_t h1y1, h1y2, h2y1, h2y2;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
                                                    /* Get widget handle */
    if (guii_widget_getflag(h, GUI_FLAG_IGNORE_INVALIDATE)) {   /* Check ignore flag */
        return 0;                                   /* Ignore invalidate process */
    }
    
    /*
     * First check if any of parent widgets is hidden = ignore redraw
     */
    for (h1 = guii_widget_getparent(h); h1 != NULL;
        h1 = guii_widget_getparent(h1)) {
        if (guii_widget_ishidden(h1)) {
            return 1;
        }
    }
        
    h1 = h;                                         /* Save temporary */
    guii_widget_setflag(h1, GUI_FLAG_REDRAW);       /* Redraw widget */
    GUI.flags |= GUI_FLAG_REDRAW;                   /* Notify stack about redraw operations */
    
    if (setclipping) {
        set_clipping_region(h);                     /* Set clipping region for widget redrawing operation */
    }
    
    /*
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
    if (guii_widget_istransparent(h1)) {
        invalidate_widget(guii_widget_getparent(h1), 0);    /* Invalidate parent widget */
    }
#endif /* GUI_CFG_USE_TRANSPARENCY */
    for (; h1 != NULL; h1 = gui_linkedlist_widgetgetnext(NULL, h1)) {
        get_widget_abs_position_and_visible_width_height(h1, &h1x1, &h1y1, &h1x2, &h1y2); /* Get visible position on LCD for widget */
        for (h2 = gui_linkedlist_widgetgetnext(NULL, h1); h2;
                h2 = gui_linkedlist_widgetgetnext(NULL, h2)) {
            get_widget_abs_position_and_visible_width_height(h2, &h2x1, &h2y1, &h2x2, &h2y2);
            if (
                guii_widget_getflag(h2, GUI_FLAG_REDRAW) ||    /* Flag is already set */
                !__GUI_RECT_MATCH(                  /* Widgets are not one over another */
                    h1x1, h1y1, h1x2, h1y2,
                    h2x1, h2y1, h2x2, h2y2)
            ) {
                continue;
            }
            guii_widget_setflag(h2, GUI_FLAG_REDRAW);  /* Redraw widget on next loop */
        }
    }
    
    /*
     * If widget is not the last on the linked list (top z-index)
     * check status of parent widget if it is last.
     * If it is not, process parent redraw and check similar parent widgets if are over our widget
     */
    if (guii_widget_hasparent(h)) {
        gui_handle_p ph = guii_widget_getparent(h);
        if (!gui_linkedlist_iswidgetlast(ph)) {
            invalidate_widget(guii_widget_getparent(h), 0);
        }
    }

#if GUI_CFG_USE_TRANSPARENCY    
    /*
     * Check if any of parent widgets has transparency = should be invalidated too
     *
     * Since recursion is used, call function only once and recursion will take care for upper level of parent widgets
     */
    for (h = guii_widget_getparent(h); h != NULL; h = guii_widget_getparent(h)) {
        if (guii_widget_istransparent(h)) {        /* If widget is transparent */
            invalidate_widget(h, 0);                /* Invalidate this parent too */
            break;
        }
    }
#endif /* GUI_CFG_USE_TRANSPARENCY */
    
    return 1;
}

/**
 * \brief           Get widget by specific input parameters
 * \param[in]       parent: Parent widget handle. Set to NULL to use root
 * \param[in]       id: Widget id we are searching for in parent
 * \param[in]       deep: Flag if searchi should go deeper to check for widget on parent tree
 * \return          Widget handle on success, NULL otherwise
 */
static gui_handle_p
get_widget_by_id(gui_handle_p parent, gui_id_t id, uint8_t deep) {
    gui_handle_p h;
    
    for (h = gui_linkedlist_widgetgetnext(__GHR(parent), NULL); h != NULL;
            h = gui_linkedlist_widgetgetnext(NULL, h)) {
        if (guii_widget_getid(h) == id) {          /* Compare ID values */
            return h;
        } else if (deep && guii_widget_allowchildren(h)) { /* Check children if possible */
            gui_handle_p tmp = get_widget_by_id(h, id, deep);
            if (tmp != NULL) {
                return tmp;
            }
        }
    }
    return NULL;
}

/**
 * \brief           Get first common widget between 2 widgets in a tree
 * \param[in]       h1: First widget handle
 * \param[in]       h2: Second widget handle
 * \return          First common widget handle on tree
 */
static gui_handle_p
get_common_parentwidget(gui_handle_p h1, gui_handle_p h2) {
    gui_handle_p tmp;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h1) && guii_widget_iswidget(h2)); /* Check valid parameter */
    
    for (; h1 != NULL; h1 = guii_widget_getparent(h1)) {/* Process all entries */
        for (tmp = h2; tmp != NULL; tmp = guii_widget_getparent(tmp)) {
            if (h1 == tmp) {
                return tmp;
            }
        }
    }
    return GUI.root.first;                          /* Return bottom widget on list */
}

/**
 * \brief           Set widget size and invalidate approprite widgets if necessary
 * \note            This function only sets width/height values, it does not change or modifies flags
 * \param[in]       h: Widget handle
 * \param[in]       x: Width in units of pixels or percents
 * \param[in]       y: height in units of pixels or percents
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
set_widget_size(gui_handle_p h, float wi, float hi) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (wi != h->width || hi != h->height) {        /* Check any differences */
        uint8_t invalidateSecond = 0;
        if (!guii_widget_isexpanded(h)) {           /* First invalidate current position if not expanded before change of size */
            guii_widget_invalidatewithparent(h);    /* Set old clipping region first */
            if (wi > h->width || hi > h->height) {
                invalidateSecond = 1;
            }
        }
        h->width = wi;                              /* Set parameter */
        h->height = hi;                             /* Set parameter */
        if (invalidateSecond) {                     /* Invalidate second time only if widget greater than before */
            guii_widget_invalidatewithparent(h);    /* Set new clipping region */
        }
    }
    return 1;
}

/**
 * \brief           Set widget position and invalidate approprite widgets if necessary
 * \param[in]       h: Widget handle
 * \param[in]       x: X position in units of pixels or percents
 * \param[in]       y: Y position in units of pixels or percents
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
set_widget_position(gui_handle_p h, float x, float y) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (h->x != x || h->y != y) {                   /* Check any differences */
        if (!guii_widget_isexpanded(h)) {
            guii_widget_invalidatewithparent(h);    /* Set old clipping region first */
        }
        h->x = x;                                   /* Set parameter */
        h->y = y;                                   /* Set parameter */
        if (!guii_widget_isexpanded(h)) {
            guii_widget_invalidatewithparent(h);    /* Set new clipping region */
        }
    }
    return 1;
}

/**
 * \brief           Check if widget can be removed
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
can_remove_widget(gui_handle_p h) {
    gui_widget_result_t result = {0};
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    /*
     * Desktop window cannot be deleted
     */
    if (h == gui_window_getdesktop()) {             /* Root widget can not be deleted! */
        return 0;
    }
    
    /*
     * Check widget status itself
     */
    GUI_WIDGET_RESULTTYPE_U8(&result) = 1;
    if (!guii_widget_callback(h, GUI_WC_Remove, NULL, &result) || GUI_WIDGET_RESULTTYPE_U8(&result)) { /* If command was not processed, allow delete */
        GUI_WIDGET_RESULTTYPE_U8(&result) = 1;      /* Manually allow delete */
    }
    
    /*
     * Check children widgets recursively
     */
    if (GUI_WIDGET_RESULTTYPE_U8(&result) && guii_widget_allowchildren(h)) {   /* Check if we can delete all children widgets */
        gui_handle_p h1;
        for (h1 = gui_linkedlist_widgetgetnext((gui_handle_root_t *)h, NULL); h1;
                h1 = gui_linkedlist_widgetgetnext(NULL, h1)) {
            if (!can_remove_widget(h1)) {           /* If we should not delete it */
                return 0;                           /* Stop on first call */
            }
        }
    }
    
    return GUI_WIDGET_RESULTTYPE_U8(&result);
}

/**
 * \brief           Check if visible part of widget is inside clipping region for redraw
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_isinsideclippingregion(gui_handle_p h) {
    gui_dim_t x1, y1, x2, y2;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    /* Get widget visible section */
    get_widget_abs_position_and_visible_width_height(h, &x1, &y1, &x2, &y2);
    return __GUI_RECT_MATCH(
        x1, y1, x2, y2,
        GUI.Display.x1, GUI.Display.y1, GUI.Display.x2, GUI.Display.y2
    );
}

/**
 * \brief           Init widget part of library
 */
void
guii_widget_init(void) {
    gui_window_createdesktop(GUI_ID_WINDOW_BASE, NULL); /* Create base window object */
}

/**
 * \brief           Execute remove, check all widgets with remove status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_executeremove(void) {
    if (GUI.flags & GUI_FLAG_REMOVE) {              /* Anything to remove? */
        remove_widgets(NULL);                       /* Remove widgets */
        GUI.flags &= ~GUI_FLAG_REMOVE;
        return 1;
    }
    return 0;
}

/**
 * \brief           Move widget down on linked list (put it as last, most visible on screen)
 * \param[in]       h: Widget handle
 */
void
guii_widget_movedowntree(gui_handle_p h) {              
    /*
     * Move widget to the end of parent linked list
     * This will allow widget to be first checked next time for touch detection
     * and will be drawn on top of al widgets as expected except if there is widget which allows children (new window or similar)
     */
    if (gui_linkedlist_widgetmovetobottom(h)) {
        guii_widget_invalidate(h);                  /* Invalidate object */
    }
    
    /*
     * Since linked list is threaded, we should move our widget to the end of parent list.
     * The same should be in the parent as it should also be on the end of its parent and so on.
     * With parent recursion this can be achieved
     */
    if (guii_widget_hasparent(h)) {                 /* Move its parent to the bottom of parent linked list */
        gui_handle_p parent;
        for (parent = guii_widget_getparent(h); parent != NULL;
            parent = guii_widget_getparent(parent)) {
            if (gui_linkedlist_widgetmovetobottom(parent)) {/* If move down was successful */
                guii_widget_invalidate(parent);     /* Invalidate parent of widget */
            }
        }
    }
}

/**
 * \brief           Clear focus on widget
 * \param[in]       h: Widget handle
 */
void
guii_widget_focus_clear(void) {
    if (GUI.FocusedWidget != NULL && GUI.FocusedWidget != GUI.root.first) { /* First widget is always in focus */
        GUI.FocusedWidgetPrev = GUI.FocusedWidget;  /* Clear everything */
        do {
            guii_widget_callback(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);
            guii_widget_clrflag(GUI.FocusedWidget, GUI_FLAG_FOCUS); /* Clear focused widget */
            guii_widget_invalidate(GUI.FocusedWidget); /* Invalidate widget */
            GUI.FocusedWidget = guii_widget_getparent(GUI.FocusedWidget);   /* Get parent widget */
        } while (GUI.FocusedWidget != GUI.root.first);  /* Loop to the bottom */
        GUI.FocusedWidget = NULL;                   /* Reset focused widget */
    }
}

/**
 * \brief           Set widget as focused
 * \param[in]       h: Widget handle
 */
void
guii_widget_focus_set(gui_handle_p h) {
    gui_handle_p common = NULL;
    
    if (GUI.FocusedWidget == h) {                   /* Check current focused widget */
        return;
    }
    
    /*
     * TODO: Check if widget is in list for remove or any parent of it
     */
    
    /*
     * Step 1:
     *
     * Identify common parent from new and old focused widget
     * Remove focused flag on widget which are not in tree between old and new widgets
     */
    if (GUI.FocusedWidget != NULL) {                /* We already have one widget in focus */
        common = get_common_parentwidget(GUI.FocusedWidget, h); /* Get first widget in common */
        if (common != NULL) {                       /* We have common object, invalidate only those which are not common in tree */
            for (; 
                GUI.FocusedWidget != NULL && common != NULL && GUI.FocusedWidget != common;
                GUI.FocusedWidget = guii_widget_getparent(GUI.FocusedWidget)) {
                guii_widget_clrflag(GUI.FocusedWidget, GUI_FLAG_FOCUS);    /* Clear focused flag */
                guii_widget_callback(GUI.FocusedWidget, GUI_WC_FocusOut, NULL, NULL);  /* Notify with callback */
                guii_widget_invalidate(GUI.FocusedWidget); /* Invalidate widget */
            }
        }
    } else {
        common = GUI.root.first;                    /* Get bottom widget */
    }
    
    /*
     * Step 2:
     *
     * Set new widget as focused
     * Set all widget from common to current as focused
     */ 
    GUI.FocusedWidget = h;                          /* Set new focused widget */
    while (h != NULL && common != NULL && h != common) {
        guii_widget_setflag(h, GUI_FLAG_FOCUS);     /* Set focused flag */
        guii_widget_callback(h, GUI_WC_FocusIn, NULL, NULL);   /* Notify with callback */
        guii_widget_invalidate(h);                  /* Invalidate widget */
        h = guii_widget_getparent(h);               /* Get parent widget */
    }
}

/**
 * \brief           Clear active status on widget
 */
void
guii_widget_active_clear(void) {
    if (GUI.ActiveWidget != NULL) {
        guii_widget_callback(GUI.ActiveWidget, GUI_WC_ActiveOut, NULL, NULL);  /* Notify user about change */
        guii_widget_clrflag(GUI.ActiveWidget, GUI_FLAG_ACTIVE | GUI_FLAG_TOUCH_MOVE);  /* Clear all widget based flags */
        GUI.ActiveWidgetPrev = GUI.ActiveWidget;    /* Save as previous active */
        GUI.ActiveWidget = NULL;                    /* Clear active widget handle */
    }
}

/**
 * \brief           Set widget as active
 * \param[in]       h: Widget handle. When set to NULL, current active widget is cleared
 */
void
guii_widget_active_set(gui_handle_p h) {
    guii_widget_active_clear();                      /* Clear active widget flag */
    GUI.ActiveWidget = h;                           /* Set new active widget */
    if (h != NULL) {
        guii_widget_setflag(GUI.ActiveWidget, GUI_FLAG_ACTIVE);    /* Set active widget flag */
        guii_widget_callback(GUI.ActiveWidget, GUI_WC_ActiveIn, NULL, NULL);
    }
}

/**
 * \brief           Get total width of widget in units of pixels
 *                     Function returns width of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \return          Total width in units of pixels
 * \sa              __gui_widget_getinnerwidth
 */
gui_dim_t
guii_widget_getwidth(gui_handle_p h) {
    //__GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (!(guii_widget_iswidget(h)) || !(GUI.Initialized)) {
        return 0;                                   \
    }
    
    if (guii_widget_getflag(h, GUI_FLAG_EXPANDED)) {   /* Maximize window over parent */
        return guii_widget_getparentinnerwidth(h); /* Return parent inner width */
    } else if (guii_widget_getflag(h, GUI_FLAG_WIDTH_FILL)) {  /* "fill_parent" mode for width */
        gui_dim_t parent = guii_widget_getparentinnerwidth(h);
        gui_dim_t rel_x = guii_widget_getrelativex(h);
        if (parent > rel_x) {
            return parent - rel_x;                  /* Return widget width */
        }
    } else if (guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT)) {   /* Percentage width */
        return GUI_ROUND((h->width * guii_widget_getparentinnerwidth(h)) / 100.0f);
    } else {                                        /* Normal width */
        return h->width;                            /* Width in pixels */
    }
    return 0;
}

/**
 * \brief           Get total height of widget
 *                     Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \return          Total height in units of pixels
 * \sa              __gui_widget_getinnerheight
 * \hideinitializer
 */
gui_dim_t
guii_widget_getheight(gui_handle_p h) {
    //__GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (!(guii_widget_iswidget(h)) || !(GUI.Initialized)) {
        return 0;                                   \
    }
    
    if (guii_widget_getflag(h, GUI_FLAG_EXPANDED)) {   /* Maximize window over parent */
        return guii_widget_getparentinnerheight(h);/* Return parent inner height */
    } else if (guii_widget_getflag(h, GUI_FLAG_HEIGHT_FILL)) { /* "fill_parent" mode for height */
        gui_dim_t parent = guii_widget_getparentinnerheight(h);
        gui_dim_t rel_y = guii_widget_getrelativey(h);
        if (parent > rel_y) {
            return parent - rel_y;                  /* Return widget height */
        }
    } else if (guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT)) {  /* Percentage height */
        return GUI_ROUND((h->height * guii_widget_getparentinnerheight(h)) / 100.0f);
    } else {                                        /* Normal height */
        return h->height;                           /* height in pixels */
    }
    return 0;
}

/**
 * \brief           Get absolute X position on LCD for specific widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          X position on LCD
 * \hideinitializer
 */
gui_dim_t
guii_widget_getabsolutex(gui_handle_p h) {
    gui_handle_p w;
    gui_dim_t out = 0;
    
    if (h == NULL) {                                /* Check input value */
        return 0;                                   /* At left value */
    }
    
    /* If widget is not expanded, use actual value */
    out = guii_widget_getrelativex(h);              /* Get start relative position */
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = guii_widget_getparent(h); w != NULL;
        w = guii_widget_getparent(w)) {             /* Go through all parent windows */
        out += guii_widget_getrelativex(w) + guii_widget_getpaddingleft(w);   /* Add X offset from parent and left padding of parent */
        out -= __GHR(w)->x_scroll;                  /* Decrease by scroll value */
    }
    return out;
}

/**
 * \brief           Get absolute Y position on LCD for specific widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          Y position on LCD
 * \hideinitializer
 */
gui_dim_t
guii_widget_getabsolutey(gui_handle_p h) {
    gui_handle_p w;
    gui_dim_t out = 0;
    
    if (h == NULL) {                                /* Check input value */
        return 0;                                   /* At top value */
    }
    
    /* If widget is not expanded, use actual value */
    out = guii_widget_getrelativey(h);              /* Get start relative position */
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = guii_widget_getparent(h); w != NULL;
        w = guii_widget_getparent(w)) {             /* Go through all parent windows */
        out += guii_widget_getrelativey(w) + guii_widget_getpaddingtop(w);    /* Add X offset from parent and left padding of parent */
        out -= __GHR(w)->y_scroll;                  /* Decrease by scroll value */
    }
    return out;
}

/**
 * \brief           Get absolute inner X position of parent widget
 * \note            This function returns inner X position in absolute form.
 *                     Imagine parent absolute X is 10, and left padding is 2. Function returns 12.
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \return          Parent absolute inner X position
 */
gui_dim_t
guii_widget_getparentabsolutex(gui_handle_p h) {
    gui_dim_t out = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (h != NULL) {                                /* Check input value */
        h = guii_widget_getparent(h);               /* Get parent of widget */
        if (h != NULL) {                            /* Save left padding */
            out = guii_widget_getpaddingleft(h);    /* Get left padding from parent widget */
        }
    }
    out += guii_widget_getabsolutex(h);             /* Add absolute X of parent and to current padding */
    return out;
}

/**
 * \brief           Get absolute inner Y position of parent widget
 * \note            This function returns inner Y position in absolute form.
 *                     Imagine parent absolute Y is 10, and top padding is 2. Function returns 12.
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle for which parent should be calculated
 * \return          Parent absolute inner Y position
 */
gui_dim_t
guii_widget_getparentabsolutey(gui_handle_p h) {
    gui_dim_t out = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (h != NULL) {                                /* Check valid widget */
        h = guii_widget_getparent(h);               /* Get parent of widget */
        if (h != NULL) {                            /* Save left padding */
            out = guii_widget_getpaddingtop(h);     /* Get top padding from parent widget */
        }
    }
    out += guii_widget_getabsolutey(h);             /* Add absolute Y of parent and to current padding */
    return out;
}

/**
 * \brief           Invalidate widget for redraw 
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
uint8_t
guii_widget_invalidate(gui_handle_p h) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    ret = invalidate_widget(h, 1);                  /* Invalidate widget with clipping */
    
    if (
        (
            guii_widget_getflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT) || 
            guii_widget_getcoreflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT)
#if GUI_CFG_USE_TRANSPARENCY
            || guii_widget_istransparent(h)        /* At least little transparent */
#endif /* GUI_CFG_USE_TRANSPARENCY */
        ) && guii_widget_hasparent(h)) {
        invalidate_widget(guii_widget_getparent(h), 0); /* Invalidate parent object too but without clipping */
    }
#if GUI_CFG_OS
    gui_sys_mbox_putnow(&GUI.OS.mbox, &msg_widget_invalidate);
#endif /* GUI_CFG_OS */
    return ret;
}

/**
 * \brief           Invalidate widget and parent widget for redraw 
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
uint8_t
guii_widget_invalidatewithparent(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    invalidate_widget(h, 1);                        /* Invalidate object with clipping */
    if (guii_widget_hasparent(h)) {                 /* If parent exists, invalid only parent */
        invalidate_widget(guii_widget_getparent(h), 0); /* Invalidate parent object without clipping */
    }
    return 1;
}

/**
 * \brief           Set if parent widget should be invalidated when we invalidate primary widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \note            Useful for widgets where there is no background: Transparent images, textview, slider, etc
 * \param[in]       h: Widget handle
 * \param[in]       value: Value either to enable or disable. 0 = disable, > 0 = enable
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setinvalidatewithparent(gui_handle_p h, uint8_t value) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (value) {                                    /* On positive value */
        guii_widget_setflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Enable auto invalidation of parent widget */
    } else {                                        /* On zero */
        guii_widget_clrflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Disable auto invalidation of parent widget */
    }
    return 1;
}

/**
 * \brief           Set 3D mode on widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value to enable, either 1 or 0
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_set3dstyle(gui_handle_p h, uint8_t enable) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (enable && !guii_widget_getflag(h, GUI_FLAG_3D)) {  /* Enable style */
        guii_widget_setflag(h, GUI_FLAG_3D);        /* Enable 3D style */
        guii_widget_invalidate(h);                  /* Invalidate object */
    } else if (!enable && guii_widget_getflag(h, GUI_FLAG_3D)) {/* Disable style */
        guii_widget_clrflag(h, GUI_FLAG_3D);        /* Disable 3D style */
        guii_widget_invalidate(h);                  /* Invalidate object */
    }
    return 1;
}

/*******************************************/
/**  Widget create and remove management  **/
/*******************************************/

/**
 * \brief           Create new widget and add it to linked list to parent object
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       widget: Pointer to \ref gui_widget_t structure with widget description
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Widget callback function. Set to NULL to use default widget specific callback
 * \param[in]       flags: flags for create procedure
 * \return          Created widget handle on success, NULL otherwise
 * \sa              __gui_widget_remove
 */
void *
guii_widget_create(const gui_widget_t* widget, gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags) {
    gui_handle_p h;
    
    __GUI_ASSERTPARAMS(widget != NULL && widget->callback != NULL); /* Check input parameters */
    
    /*
     * Allocation size check:
     * 
     * - Size must be at least for widget size
     * - If widget supports children widgets, size must be for at least parent handle structure
     */
    if (widget->size < sizeof(gui_handle) ||
        ((widget->flags & GUI_FLAG_WIDGET_ALLOW_CHILDREN) && widget->size < sizeof(gui_handle_root_t))) { 
        return 0;
    }
    
    h = GUI_MEMALLOC(widget->size);                 /* Allocate memory for widget */
    if (h != NULL) {
        gui_widget_param_t param = {0};
        gui_widget_result_t result = {0};
    
        memset(h, 0x00, widget->size);              /* Set memory to 0 */
        
        __GUI_ENTER();                              /* Enter GUI */
        
        h->id = id;                                 /* Save ID */
        h->widget = widget;                         /* Widget object structure */
        h->footprint = GUI_WIDGET_FOOTPRINT;        /* Set widget footprint */
        h->callback = cb;                           /* Set widget callback */
#if GUI_CFG_USE_TRANSPARENCY
        h->transparency = 0xFF;                     /* Set full transparency by default */
#endif /* GUI_CFG_USE_TRANSPARENCY */
        
        /*
         * Parent window check
         *
         * - Dialog's parent widget is desktop widget
         * - If flag for parent desktop is set, parent widget is also desktop
         * - Otherwise parent widget passed as parameter is used if it supports children widgets
         */
        if (guii_widget_isdialogbase(h) || flags & GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP) {/* Dialogs do not have parent widget */
            h->parent = gui_window_getdesktop();    /* Set parent object */
        } else {
            if (parent != NULL && guii_widget_allowchildren(parent)) {
                h->parent = parent;
            } else {
                h->parent = GUI.WindowActive;       /* Set parent object. It will be NULL on first call */
            }
        }
        
        GUI_WIDGET_RESULTTYPE_U8(&result) = 1;
        guii_widget_callback(h, GUI_WC_PreInit, NULL, &result);    /* Notify internal widget library about init successful */
        
        if (!GUI_WIDGET_RESULTTYPE_U8(&result)) {   /* Check result */
            GUI_MEMFREE(h);                         /* Clear widget memory */
            h = 0;                                  /* Reset handle */
            return 0;                               /* Stop execution at this point */
        }
        
        /* Set widget default values */
        h->font = widget_default.font;              /* Set default font */
        
        guii_widget_setflag(h, GUI_FLAG_IGNORE_INVALIDATE);    /* Ignore invalidation process */
        guii_widget_setsize(h, width, height);      /* Set widget size */
        guii_widget_setposition(h, x, y);           /* Set widget position */
        guii_widget_clrflag(h, GUI_FLAG_IGNORE_INVALIDATE);    /* Include invalidation process */
        guii_widget_invalidate(h);                  /* Invalidate properly now when everything is set correctly = set for valid clipping region part */

        GUI_WIDGET_RESULTTYPE_U8(&result) = 0;
        guii_widget_callback(h, GUI_WC_ExcludeLinkedList, NULL, &result);
        if (!GUI_WIDGET_RESULTTYPE_U8(&result)) {   /* Check if widget should be added to linked list */
            gui_linkedlist_widgetadd((gui_handle_root_t *)h->parent, h); /* Add entry to linkedlist of parent widget */
        }
        guii_widget_callback(h, GUI_WC_Init, NULL, NULL);  /* Notify user about init successful */
        guii_widget_invalidate(h);                  /* Invalidate object */
        
        if (h->parent != NULL) {                    /* If widget has parent */
            GUI_WIDGET_PARAMTYPE_HANDLE(&param) = h;
            guii_widget_callback(h->parent, GUI_WC_ChildWidgetCreated, &param, NULL);    /* Notify user about init successful */
        }
        
        __GUI_LEAVE();                              /* Leave GUI */
#if GUI_CFG_OS
        static gui_mbox_msg_t msg = {GUI_SYS_MBOX_TYPE_WIDGET_CREATED};
        gui_sys_mbox_putnow(&GUI.OS.mbox, &msg);    /* Post message queue */
#endif /* GUI_CFG_OS */
    }
    
    return (void *)h;
}

/**
 * \brief           Remove widget and all of its children widgets
 *  
 *                  Function checks widget and all its children if they can be deleted. 
 *                  If so, flag for delete will be set and procedure will be executed later when all other processing is done
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param           *h: Widget handle to remove
 * \return          `1` on success, `0` otherwise
 * \sa              __gui_widget_create, gui_widget_remove
 */
uint8_t
guii_widget_remove(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (can_remove_widget(h)) {                     /* Check if we can delete widget */
        guii_widget_setflag(h, GUI_FLAG_REMOVE);    /* Set flag for widget delete */
        GUI.flags |= GUI_FLAG_REMOVE;               /* Set flag for to remove at least one widget from tree */
        if (guii_widget_isfocused(h)) {             /* In case current widget is in focus */
            guii_widget_focus_set(guii_widget_getparent(h)); /* Set parent as focused */
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       font: Pointer to \ref gui_font_t structure with font information
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setfont(gui_handle_p h, const gui_font_t* font) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (h->font != font) {                          /* Any parameter changed */
        h->font = font;                             /* Set parameter */
        guii_widget_invalidatewithparent(h);        /* Invalidate object */
    }
    return 1;
}

/**
 * \brief           Set text for widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            When memory for text is dynamically allocated, text will be copied to allocated memory,
 *                     otherwise it will just set the pointer to new text.
 *                     Any changes on this text after function call will affect on later results
 *
 * \param[in,out]   h: Widget handle
 * \param[in]       text: Pointer to text to set
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_settext(gui_handle_p h, const gui_char* text) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC)) {   /* Memory for text is dynamically allocated */
        if (h->textmemsize) {
            if (gui_string_lengthtotal(text) > (h->textmemsize - 1)) {  /* Check string length */
                gui_string_copyn(h->text, text, h->textmemsize - 1);    /* Do not copy all bytes because of memory overflow */
            } else {
                gui_string_copy(h->text, text);     /* Copy entire string */
            }
            guii_widget_invalidate(h);              /* Redraw object */
            guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    } else {                                        /* Memory allocated by user */
        if (h->text != NULL && h->text == text) {   /* In case the same pointer is passed to WIDGET */
            guii_widget_invalidate(h);              /* Redraw object */
            guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
        
        if (h->text != text) {                      /* Check if pointer do not match */
            h->text = (gui_char *)text;             /* Set parameter */
            guii_widget_invalidate(h);              /* Redraw object */
            guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
        }
    }
    h->textcursor = gui_string_lengthtotal(h->text);/* Set cursor to the end of string */
    return 1;
}

/**
 * \brief           Allocate text memory for widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \return          `1` on success, `0` otherwise
 * \sa              guii_widget_freetextmemory, gui_widget_alloctextmemory, gui_widget_freetextmemory
 * \hideinitializer
 */
uint8_t
guii_widget_alloctextmemory(gui_handle_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC) && h->text) {  /* Check if already allocated */
        GUI_MEMFREE(h->text);                       /* Free memory first */
        h->textmemsize = 0;                         /* Reset memory size */
    }
    h->text = NULL;                                 /* Reset pointer */
    
    h->textmemsize = sizeof(gui_char) * size;       /* Allocate text memory */
    h->text = GUI_MEMALLOC(__GH(h)->textmemsize);   /* Allocate memory for text */
    if (h->text != NULL) {                          /* Check if allocated */
        guii_widget_setflag(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Dynamically allocated */
    } else {
        h->textmemsize = 0;                         /* No dynamic bytes available */
        guii_widget_clrflag(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
    }
    guii_widget_invalidate(h);                      /* Redraw object */
    guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    return 1;
}

/**
 * \brief           Free text memory for widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              __gui_widget_alloctextmemory, gui_widget_alloctextmemory, gui_widget_freetextmemory
 * \hideinitializer
 */
uint8_t
guii_widget_freetextmemory(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC) && h->text != NULL) { /* Check if dynamically alocated */
        GUI_MEMFREE(h->text);                       /* Free memory first */
        h->text = NULL;                             /* Reset memory */
        h->textmemsize = 0;                         /* Reset memory size */
        guii_widget_clrflag(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
        guii_widget_invalidate(h);                  /* Redraw object */
        guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
    }
    return 1;
}

/**
 * \brief           Check if widget has set font and text
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_isfontandtextset(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    return h->text != NULL && h->font != NULL && gui_string_length(h->text);    /* Check if conditions are met for drawing string */
}

/**
 * \brief           Process text key (add character, remove it, move cursor, etc)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       kb: Pointer to \ref guii_keyboard_data_t structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_processtextkey(gui_handle_p h, guii_keyboard_data_t* kb) {
    size_t len, tlen;
    uint32_t ch;
    uint8_t l;
    gui_string_t currStr;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (!guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC)) {  /* Must be dynamically allocated memory */
        return 0;
    }
    
    gui_string_prepare(&currStr, kb->kb.keys);      /* Set string to process */
    if (!gui_string_getch(&currStr, &ch, &l)) {     /* Get key from input data */
        return 0;                                   /* Invalid input key */
    }
    
    tlen = gui_string_lengthtotal(h->text);         /* Get total length of string */
    len = gui_string_length(h->text);               /* Get string length */
    if ((ch == GUI_KEY_LF || ch >= 32) && ch != 127) {  /* Check valid character character */
        if (len < (h->textmemsize - l)) {           /* Memory still available for new character */
            uint16_t pos;
            for (pos = tlen + l - 1; pos > h->textcursor; pos--) {  /* Shift characters down */
                h->text[pos] = h->text[pos - l];
            }
            for (pos = 0; pos < l; pos++) {         /* Fill new characters to empty memory */
                h->text[h->textcursor++] = kb->kb.keys[pos];
            }
            h->text[tlen + l] = 0;                  /* Add 0 to the end */
            
            guii_widget_invalidate(h);              /* Invalidate widget */
            guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);   /* Process callback */
            return 1;
        }
    } else if (ch == 8 || ch == 127) {              /* Backspace character */
        if (tlen && h->textcursor) {
            uint16_t pos;
            
            gui_string_prepare(&currStr, (gui_char *)((uint32_t)h->text + h->textcursor - 1));  /* Set string to process */
            gui_string_gotoend(&currStr);           /* Go to the end of string */
            if (!gui_string_getchreverse(&currStr, &ch, &l)) {  /* Get last character */
                return 0;                           
            }
            for (pos = h->textcursor - l; pos < (tlen - l); pos++) {/* Shift characters up */
                h->text[pos] = h->text[pos + l];
            }
            h->textcursor -= l;                     /* Decrease text cursor by number of bytes for character deleted */
            h->text[tlen - l] = 0;                  /* Set 0 to the end of string */
            
            guii_widget_invalidate(h);              /* Invalidate widget */
            guii_widget_callback(h, GUI_WC_TextChanged, NULL, NULL);/* Process callback */
            return 1;
        }
    }
    return 0;
}

/**
 * \brief           Get text from widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          Pointer to text from widget
 */
const gui_char *
guii_widget_gettext(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    /* Prepare for transpate support */
#if GUI_CFG_USE_TRANSLATE
    /* For static texts only */
    if (!guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC) && h->text != NULL) {
        return gui_translate_get(h->text);          /* Get translation entry */
    }
#endif /* GUI_CFG_USE_TRANSLATE */
    return h->text;                                 /* Return text for widget */
}

/**
 * \brief           Get font from widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          Pointer to font used for widget
 */
const gui_font_t *
guii_widget_getfont(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    return h->font;                                 /* Return font for widget */
}

/*******************************************/
/**         Widget size management        **/
/*******************************************/
/**
 * \brief           Set width of widget in units of pixels
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setheight, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
guii_widget_setwidth(gui_handle_p h, gui_dim_t width) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT)) {   /* Invalidate if percent not yet enabled to force invalidation */
        guii_widget_clrflag(h, GUI_FLAG_WIDTH_PERCENT); /* Set percentage flag */
        h->width = width + 1;                       /* Invalidate height */
    }
    return set_widget_size(h, width, h->height);    /* Set new height */
}

/**
 * \brief           Set height of widget in units of pixels
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       height: height in units of pixels
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setwidth, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
guii_widget_setheight(gui_handle_p h, gui_dim_t height) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT)) {  /* Invalidate if percent not yet enabled to force invalidation */
        guii_widget_clrflag(h, GUI_FLAG_HEIGHT_PERCENT);    /* Set percentage flag */
        __GH(h)->height = height + 1;               /* Invalidate height */
    }
    return set_widget_size(h, h->width, height);    /* Set new height */
}

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setheightpercent
 */
uint8_t
guii_widget_setwidthpercent(gui_handle_p h, float width) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (!guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT)) {  /* Invalidate if percent not yet enabled to force invalidation */
        guii_widget_setflag(h, GUI_FLAG_WIDTH_PERCENT); /* Set percentage flag */
        h->width = width + 1;                       /* Invalidate widget */
    }
    return set_widget_size(h, width, h->height);    /* Set new width */
}

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       height: height in percentage
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setwidthpercent
 */
uint8_t
guii_widget_setheightpercent(gui_handle_p h, float height) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (!guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT)) {    /* Invalidate if percent not yet enabled to force invalidation */
        guii_widget_setflag(h, GUI_FLAG_HEIGHT_PERCENT);   /* Set percentage flag */
        h->height = height + 1;                     /* Invalidate height */
    }
    return set_widget_size(h, h->width, height);    /* Set new height */
}

/**
 * \brief           Set widget size in units of pixels
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setsize(gui_handle_p h, gui_dim_t wi, gui_dim_t hi) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    /* If percentage enabled on at least one, either width or height */
    if (guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT)) {
        guii_widget_clrflag(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT);  /* Clear both flags */
        h->width = wi + 1;                          /* Invalidate width */
        h->height = hi + 1;                         /* Invalidate height */
    }
    return set_widget_size(h, wi, hi);              /* Set widget size */
}

/**
 * \brief           Set widget size in units of percent
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       wi: Widget width
 * \param[in]       hi: Widget height
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setsizepercent(gui_handle_p h, float wi, float hi) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    /* If percentage not enabled on both */
    if (guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT) != (GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT)) {
        guii_widget_setflag(h, GUI_FLAG_WIDTH_PERCENT | GUI_FLAG_HEIGHT_PERCENT);  /* Set both flags */
        h->width = wi + 1;                          /* Invalidate width */
        h->height = hi + 1;                         /* Invalidate height */
    }
    return set_widget_size(h, wi, hi);              /* Set widget size */
}

/**
 * \brief           Toggle expandend (maximized) mode of widget (mostly of windows)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_toggleexpanded(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    return guii_widget_setexpanded(h, !guii_widget_isexpanded(h));    /* Invert expanded mode */
}

/**
 * \brief           Set expandend mode on widget. When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setexpanded(gui_handle_p h, uint8_t state) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (!state && guii_widget_isexpanded(h)) {     /* Check current status */
        guii_widget_invalidatewithparent(h);       /* Invalidate with parent first for clipping region */
        guii_widget_clrflag(h, GUI_FLAG_EXPANDED); /* Clear expanded after invalidation */
    } else if (state && !guii_widget_isexpanded(h)) {
        guii_widget_setflag(h, GUI_FLAG_EXPANDED); /* Expand widget */
        guii_widget_invalidate(h);                 /* Redraw only selected widget as it is over all window */
    }
    return 1;
}

/*******************************************/
/**       Widget position management      **/
/*******************************************/
/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setposition(gui_handle_p h, gui_dim_t x, gui_dim_t y) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    /* If percent enabled on at least one coordinate, clear to force invalidation */
    if (guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT)) {
        guii_widget_clrflag(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT); /* Disable percent on X and Y position */
        h->x = x + 1;                               /* Invalidate X position */
        h->y = y + 1;                               /* Invalidate Y position */
    }  
    return set_widget_position(h, x, y);            /* Set widget position */
}
 
/**
 * \brief           Set widget position relative to parent object in units of percent
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setpositionpercent(gui_handle_p h, float x, float y) {  
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */   
    /* If percent not set on both, enable to force invalidation */
    if (guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT) != (GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT)) {
        guii_widget_setflag(h, GUI_FLAG_XPOS_PERCENT | GUI_FLAG_YPOS_PERCENT); /* Enable percent on X and Y position */
        h->x = x + 1;                               /* Invalidate X position */
        h->y = y + 1;                               /* Invalidate Y position */
    }
    return set_widget_position(h, x, y);            /* Set widget position */
}
 
/**
 * \brief           Set widget X position relative to parent object in units of pixels
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setxpositionpercent
 */
uint8_t
guii_widget_setxposition(gui_handle_p h, gui_dim_t x) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT)) {/* if percent enabled */
        guii_widget_clrflag(h, GUI_FLAG_XPOS_PERCENT);  /* Clear it to force invalidation */
        h->x = x + 1;                               /* Invalidate position */
    }
    return set_widget_position(h, x, h->y);         /* Set widget position */
}
 
/**
 * \brief           Set widget X position relative to parent object in units of percent
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setxposition
 */
uint8_t
guii_widget_setxpositionpercent(gui_handle_p h, float x) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (!guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT)) {  /* if percent not enabled */
        guii_widget_setflag(h, GUI_FLAG_XPOS_PERCENT); /* Set it to force invalidation */
        h->x = x + 1;                               /* Invalidate position */
    }
    return set_widget_position(h, x, h->y);         /* Set widget position */
}
 
/**
 * \brief           Set widget Y position relative to parent object in units of pixels
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setypositionpercent
 */
uint8_t
guii_widget_setyposition(gui_handle_p h, gui_dim_t y) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT)) {   /* if percent enabled */
        guii_widget_clrflag(h, GUI_FLAG_YPOS_PERCENT); /* Clear it to force invalidation */
        h->y = y + 1;                               /* Invalidate position */
    }
    return set_widget_position(h, h->x, y);         /* Set widget position */
}
 
/**
 * \brief           Set widget Y position relative to parent object in units of percent
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setyposition
 */
uint8_t
guii_widget_setypositionpercent(gui_handle_p h, float y) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    if (!guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT)) {  /* if percent not enabled */
        guii_widget_setflag(h, GUI_FLAG_YPOS_PERCENT); /* Set it to force invalidation */
        h->y = y + 1;                               /* Invalidate position */
    }
    return set_widget_position(h, h->y, y);         /* Set widget position */
}

/*******************************************/
/**                  .....                **/
/*******************************************/
/**
 * \brief           Show widget from visible area
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              guii_widget_hide
 */
uint8_t
guii_widget_show(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (guii_widget_getflag(h, GUI_FLAG_HIDDEN)) {  /* If hidden, show it */
        guii_widget_clrflag(h, GUI_FLAG_HIDDEN);
        guii_widget_invalidatewithparent(h);        /* Invalidate it for redraw with parent */
    }
    return 1;
}

/**
 * \brief           Hide widget from visible area
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              guii_widget_show
 */
uint8_t
guii_widget_hide(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (!guii_widget_getflag(h, GUI_FLAG_HIDDEN)) { /* If visible, hide it */
        guii_widget_setflag(h, GUI_FLAG_HIDDEN);
        guii_widget_invalidatewithparent(h);        /* Invalidate it for redraw with parent */
    }
    
    /*
     * TODO: Check if active/focused widget is maybe children of this widget
     */
    
    if (GUI.FocusedWidget != NULL && (GUI.FocusedWidget == h || guii_widget_ischildof(GUI.FocusedWidget, h))) {    /* Clear focus */
        guii_widget_focus_set(guii_widget_getparent(GUI.FocusedWidget)); /* Set parent widget as focused now */
    }
    if (GUI.ActiveWidget && (GUI.ActiveWidget == h || guii_widget_ischildof(GUI.ActiveWidget, h))) {   /* Clear active */
        guii_widget_active_clear();
    }
    return 1;
}

/**
 * \brief           Hide direct children widgets of current widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_hidechildren(gui_handle_p h) {
    gui_handle_p t;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    
    /*
     * Scan all widgets of current widget and hide them
     */
    for (t = gui_linkedlist_widgetgetnext((gui_handle_root_t *)h, NULL); t != NULL;
        t = gui_linkedlist_widgetgetnext(NULL, t)) {
        guii_widget_hide(t);
    }
    
    return 1;
}

/**
 * \brief           Check if widget is children of parent
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_ischildof(gui_handle_p h, gui_handle_p parent) {
    //__GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_iswidget(parent));  /* Check valid parameter */
    
    if (!(guii_widget_iswidget(h) && guii_widget_iswidget(parent)) || !(GUI.Initialized)) {
        return 0;
    }
    
    for (h = guii_widget_getparent(h); h != NULL;
        h = guii_widget_getparent(h)) {             /* Check widget parent objects */
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setzindex(gui_handle_p h, int32_t zindex) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    uint8_t ret = 1;
    
    if (h->zindex != zindex) {                      /* There was a change in z-index value */
        int32_t current = h->zindex;
        h->zindex = zindex;                         /* Set new index */
        if (zindex < current) {                     /* New index value is less important than before = move widget to top */
            gui_linkedlist_widgetmovetotop(h);      /* Move widget to top on linked list = less important and less visible */
        } else {
            gui_linkedlist_widgetmovetobottom(h);   /* Move widget to bottom on linked list = most important and most visible */
        }
    }
    return ret;
}

#if GUI_CFG_USE_TRANSPARENCY || __DOXYGEN__
/**
 * \brief           Set transparency level to widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       trans: Transparency level, where 0x00 means hidden and 0xFF means totally visible widget
 * \return          `1` on success, `0` otherwise
 * \sa              __gui_widget_settransparency
 */
uint8_t
guii_widget_settransparency(gui_handle_p h, uint8_t trans) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    
    if (h->transparency != trans) {                 /* Check transparency match */
        h->transparency = trans;                    /* Set new transparency level */
        guii_widget_invalidate(h);                  /* Invalidate widget */
    }
    
    return 1;
}
#endif /* GUI_CFG_USE_TRANSPARENCY */

/**
 * \brief           Set color to widget specific index
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors
 * \param[in]       color: Actual color code to set
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setcolor(gui_handle_p h, uint8_t index, gui_color_t color) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI context */
    if (h->colors == NULL) {                        /* Do we need to allocate color memory? */
        if (h->widget->color_count) {               /* Check if at least some colors should be used */
            h->colors = GUI_MEMALLOC(sizeof(*h->colors) * h->widget->color_count);
            if (h->colors != NULL) {          /* Copy all colors to new memory first */
                memcpy(h->colors, h->widget->colors, sizeof(*h->colors) * h->widget->color_count);
            } else {
                ret = 0;
            }
        } else {
            ret = 0;
        }
    }
    if (ret) {
        if (index < h->widget->color_count) {       /* Index in valid range */
           h->colors[index] = color;                /* Set new color */
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
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   id: Widget ID to search for
 * \return          > 0: Widget handle when widget found
 * \return          `1` on success, `0` otherwise
 */
gui_handle_p
guii_widget_getbyid(gui_id_t id) {
    return get_widget_by_id(NULL, id, 1);           /* Find widget by ID */
}

/**
 * \brief           Set custom user data to widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            Specially useful in callback processing if required
 * \param[in,out]   h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \return          `1` on success, `0` otherwise
 * \sa              guii_widget_getuserdata
 */
uint8_t
guii_widget_setuserdata(gui_handle_p h, void* data) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */    
    __GH(h)->UserData = data;                       /* Set user data */
    return 1;
}

/**
 * \brief           Get custom user data from widget previously set with \ref gui_widget_setuserdata
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          Pointer to user data
 */
void*
guii_widget_getuserdata(gui_handle_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    data = h->UserData;                             /* Get user data */
    return data;
}

/**
 * \brief           Set widget parameter in OS secure way
 * \param[in,out]   h: Widget handle
 * \param[in]       cfg: Configuration to use, passed later to callback function
 * \param[in]       data: Custom data to pass later to configuration callback
 * \param[in]       invalidate: Flag if widget should be invalidated after parameter change
 * \param[in]       invalidateparent: change if parent widget should be invalidated after parameter change
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setparam(gui_handle_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent) {
    gui_widget_param p;
    gui_widget_param_t param = {0};
    gui_widget_result_t result = {0};
    
    GUI_WIDGET_PARAMTYPE_WIDGETPARAM(&param) = &p;
    GUI_WIDGET_RESULTTYPE_U8(&result) = 1;
    
    p.type = cfg;
    p.data = (void *)data;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    guii_widget_callback(h, GUI_WC_SetParam, &param, &result); /* Process callback function */
    if (invalidateparent) {
        guii_widget_invalidatewithparent(h);        /* Invalidate widget and parent */
    } else if (invalidate) {
        guii_widget_invalidate(h);                  /* Invalidate widget only */
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
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_remove(gui_handle_p* h) {
    __GUI_ASSERTPARAMS(h != NULL && guii_widget_iswidget(*h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */

    guii_widget_remove(*h);                         /* Remove widget */
    
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
 * \return          Number of bytes allocated
 * \sa              gui_widget_freetextmemory
 */
uint32_t
gui_widget_alloctextmemory(gui_handle_p h, uint32_t size) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && size > 1);   /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_alloctextmemory(h, size);           /* Allocate memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return h->textmemsize;                          /* Return number of bytes allocated */
}

/**
 * \brief           Frees memory previously allocated for text
 * \param[in,out]   h: Widget handle to free memory on
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_alloctextmemory
 */
uint8_t
gui_widget_freetextmemory(gui_handle_p h) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_freetextmemory(h);            /* Free memory for text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set text to widget
 * \note            If dynamic memory allocation was used then content will be copied to allocated memory
 *                     otherwise only pointer to input text will be used 
 *                     and each further change of input pointer text will affect to output
 * \param[in,out]   h: Widget handle
 * \param[in]       text: Pointer to text to set to widget
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_alloctextmemory, gui_widget_freetextmemory, guii_widget_setfont, gui_widget_gettext, gui_widget_gettextcopy
 */
uint8_t
gui_widget_settext(gui_handle_p h, const gui_char* text) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_settext(h, text);            /* Set text for widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get text from widget
 * \note            It will return pointer to text which cannot be modified directly.
 * \param[in,out]   h: Widget handle
 * \return          Pointer to text from widget
 * \sa              gui_widget_settext, gui_widget_gettextcopy
 */
const gui_char *
gui_widget_gettext(gui_handle_p h) {
    const gui_char* t;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = guii_widget_gettext(h);                    /* Return text */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;
}

/**
 * \brief           Get text from widget
 * \note            Text from widget is copied to input pointer
 * \param[in,out]   h: Widget handle
 * \param[out]      dst: Destination pointer
 * \param[in]       len: Size of output buffer in units of \ref gui_char
 * \return          Pointer to text from widget
 * \sa              gui_widget_settext, gui_widget_gettext
 */
const gui_char*
gui_widget_gettextcopy(gui_handle_p h, gui_char* dst, uint32_t len) {
    const gui_char* t;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    t = guii_widget_gettext(h);                    /* Return text */
    gui_string_copyn(dst, t, len);                  /* Copy text after */
    dst[len] = 0;                                   /* Set trailling zero */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return t;  
}

/**
 * \brief           Set widget font for drawing operations
 * \param[in,out]   h: Widget handle
 * \param[in]       font: Pointer to \ref gui_font_t object for font
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_settext, gui_widget_gettext
 */
uint8_t
gui_widget_setfont(gui_handle_p h, const gui_font_t* font) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setfont(h, font);            /* Set widget font */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get font from widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in,out]   h: Widget handle
 * \return          Pointer to font used for widget
 */
const gui_font_t *
gui_widget_getfont(gui_handle_p h) {
    const gui_font_t* font;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    font = guii_widget_getfont(h);                  /* Get widget font */
    
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
 * \param[in]       height: height value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setsizepercent
 */
uint8_t
gui_widget_setsize(gui_handle_p h, gui_dim_t width, gui_dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setsize(h, width, height);   /* Set actual size to object */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget size in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width value
 * \param[in]       height: height value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setsize
 */
uint8_t
gui_widget_setsizepercent(gui_handle_p h, float width, float height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setsizepercent(h, width, height);    /* Set actual size to object in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set width of widget in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setheight, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setwidth(gui_handle_p h, gui_dim_t width) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setwidth(h, width);           /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set height of widget in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       height: height in units of pixels
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setwidth, gui_widget_setwidthpercent, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setheight(gui_handle_p h, gui_dim_t height) {
    uint8_t res;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setheight(h, height);        /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Width in percentage
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setheightpercent
 */
uint8_t
gui_widget_setwidthpercent(gui_handle_p h, float width) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setwidthpercent(h, width);    /* Set object width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       height: height in percentage
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setwidth, gui_widget_setheight, gui_widget_setwidthpercent
 */
uint8_t
gui_widget_setheightpercent(gui_handle_p h, float height) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setheightpercent(h, height);  /* Set object height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get total width of widget effective on screen in units of pixels
 *                  
 *                  Function returns width of widget according to current widget setup (expanded, fill, percent, etc.)
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \return          Total width in units of pixels
 * \sa              gui_widget_getheight, gui_widget_setwidth
 */
gui_dim_t
gui_widget_getwidth(gui_handle_p h) {
    gui_dim_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_getwidth(h);                  /* Get widget width */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get total height of widget effective on screen in units of pixels
 *
 *                  Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Pointer to \ref gui_handle_p structure
 * \return          Total height in units of pixels
 * \sa              gui_widget_getwidth, gui_widget_setheight
 * \hideinitializer
 */
gui_dim_t
gui_widget_getheight(gui_handle_p h) {
    gui_dim_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_getheight(h);                /* Get widget height */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set expandend mode on widget
 *                  
 *                  When enabled, widget will be at X,Y = 0,0 relative to parent and will have width,height = 100%,100%
 * \param[in,out]   h: Widget handle
 * \param[in]       state: State for expanded mode
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_isexpanded
 */
uint8_t
gui_widget_setexpanded(gui_handle_p h, uint8_t state) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_setexpanded(h, state);       /* Set expanded mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get widget expanded status
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setexpanded
 */
uint8_t
gui_widget_isexpanded(gui_handle_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_isexpanded(h);               /* Check expanded mode */
    
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
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setpositionpercent
 */
uint8_t
gui_widget_setposition(gui_handle_p h, gui_dim_t x, gui_dim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setposition(h, x, y);        /* Set X and Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setposition
 */
uint8_t
gui_widget_setpositionpercent(gui_handle_p h, float x, float y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setpositionpercent(h, x, y); /* Set X and Y position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget X position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setxpositionpercent
 */
uint8_t
gui_widget_setxposition(gui_handle_p h, gui_dim_t x) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setxposition(h, x);          /* Set X position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget X position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setxposition
 */
uint8_t
gui_widget_setxpositionpercent(gui_handle_p h, float x) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setxpositionpercent(h, x);   /* Set X position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget Y position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setypositionpercent
 */
uint8_t
gui_widget_setyposition(gui_handle_p h, gui_dim_t y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setyposition(h, y);          /* Set Y position */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Set widget Y position relative to parent object in units of percent
 * \param[in,out]   h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setyposition
 */
uint8_t
gui_widget_setypositionpercent(gui_handle_p h, float y) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_setypositionpercent(h, y);   /* Set Y position in percent */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Invalidate widget object and prepare to new redraw
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_invalidate(gui_handle_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */ 
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_invalidate(h);               /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Show widget from visible area
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_hide
 */
uint8_t
gui_widget_show(gui_handle_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_show(h);                     /* Show widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Put widget to front view and put it to focused state
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_hide
 */
uint8_t
gui_widget_putonfront(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_movedowntree(h);                     /* Put widget on front */
    guii_widget_focus_set(h);                        /* Set widget to focused state */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Hide widget from visible area
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_show, gui_widget_putonfront
 */
uint8_t
gui_widget_hide(gui_handle_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_hide(h);                     /* Hide widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Hide children widgets
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_show, gui_widget_putonfront
 */
uint8_t
gui_widget_hidechildren(gui_handle_p h) {
    uint8_t res;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    res = guii_widget_hidechildren(h);              /* Hide children widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return res;
}

/**
 * \brief           Get widget ID
 * \param[in,out]   h: Widget handle
 * \return          Widget ID
 */
gui_id_t
gui_widget_getid(gui_handle_p h) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    return guii_widget_getid(h);                    /* Return widget ID */
}

/**
 * \brief           Get first widget handle by ID
 * \note            If multiple widgets have the same ID, first found will be used
 * \param[in,out]   id: Widget ID to search for
 * \return          > 0: Widget handle when widget found
 * \return          `1` on success, `0` otherwise
 */
gui_handle_p
gui_widget_getbyid(gui_id_t id) {
    gui_handle_p h;
    __GUI_ENTER();                                  /* Enter GUI */
    
    h = get_widget_by_id(NULL, id, 1);              /* Find widget by ID */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h; 
}

/**
 * \brief           Check if widget is children of parent
 * \param[in]       h: Widget handle to test
 * \param[in]       parent: Parent widget handle to test if is parent
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_ischildof(gui_handle_p h, gui_handle_p parent) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_iswidget(parent));  /* Check valid parameter */
    return guii_widget_ischildof(h, parent);        /* Return status */
}

/**
 * \brief           Set custom user data to widget
 * \note            Specially useful in callback processing if required
 * \param[in,out]   h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_getuserdata
 */
uint8_t
gui_widget_setuserdata(gui_handle_p h, void* data) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setuserdata(h, data);               /* Set user data */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get custom user data from widget previously set with \ref gui_widget_setuserdata
 * \param[in,out]   h: Widget handle
 * \return          Pointer to user data
 * \sa              gui_widget_setuserdata
 */
void*
gui_widget_getuserdata(gui_handle_p h) {
    void* data;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    data = guii_widget_getuserdata(h);              /* Get user data */
    
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
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref gui_wc_t enumeration
 * \param[in]       param: Pointer to optional input data for command. Check \ref gui_wc_t enumeration for more informations
 * \param[out]      result: Pointer to optional result value. Check \ref gui_wc_t enumeration for more informations
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_processdefaultcallback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = h->widget->callback(h, ctrl, param, result);  /* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set callback function to widget
 * \param[in,out]   h: Widget handle object
 * \param[in]       callback: Callback function for widget
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_processdefaultcallback, gui_widget_callback
 */
uint8_t
gui_widget_setcallback(gui_handle_p h, gui_widget_callback_t callback) {
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    h->callback = callback;                         /* Set callback function */
    
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
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref gui_wc_t enumeration
 * \param[in]       param: Pointer to optional input data for command. Check \ref gui_wc_t enumeration for more informations
 * \param[out]      result: Pointer to optional result value. Check \ref gui_wc_t enumeration for more informations
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setcallback
 */
uint8_t
gui_widget_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_callback(h, ctrl, param, result);/* Call callback function */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set widget scroll on X axis
 * \note            This is possible on widgets with children support (windows) to have scroll on X and Y
 * \param[in,out]   h: Widget handle
 * \param[in]       scroll: Scroll value for X direction
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setscrolly
 */
uint8_t
gui_widget_setscrollx(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GHR(h)->x_scroll != scroll) {             /* Only widgets with children support can set scroll */
        __GHR(h)->x_scroll = scroll;
        guii_widget_invalidate(h);
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
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setscrollx
 */
uint8_t
gui_widget_setscrolly(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GHR(h)->y_scroll != scroll) {             /* Only widgets with children support can set scroll */
        __GHR(h)->y_scroll = scroll;
        guii_widget_invalidate(h);
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
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_incscrolly
 */
uint8_t
gui_widget_incscrollx(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (scroll) {                                   /* Only widgets with children support can set scroll */
        __GHR(h)->x_scroll += scroll;
        guii_widget_invalidate(h);
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
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_incscrollx
 */
uint8_t
gui_widget_incscrolly(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (scroll) {                                   /* Only widgets with children support can set scroll */
        __GHR(h)->y_scroll += scroll;
        guii_widget_invalidate(h);
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
gui_dim_t
gui_widget_getscrollx(gui_handle_p h) {
    gui_dim_t value = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    value = __GHR(h)->x_scroll;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return value;
}

/**
 * \brief           Get widget scroll on Y axis
 * \param[in,out]   h: Widget handle
 * \return          Widget scroll in units of pixels
 * \sa              gui_widget_getscrollx
 */
gui_dim_t
gui_widget_getscrolly(gui_handle_p h) {
    gui_dim_t value = 0;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));  /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    value = __GHR(h)->y_scroll;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return value;
}

/**
 * \brief           Manually set widget in focus
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setfocus(gui_handle_p h) {
    uint8_t ret = 1;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_focus_set(h);                       /* Put widget in focus */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set default font for widgets used on widget creation
 * \param[in]       font: Pointer to \ref gui_font_t with font
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setfontdefault(const gui_font_t* font) {
    widget_default.font = font;                     /* Set default font */
    return 1;
}

/**
 * \brief           Increase selection for widget
 * \note            Widget must implement \ref GUI_WC_IncSelection command in callback function and process it
 * \param[in,out]   h: Widget handle
 * \param[in]       dir: Increase direction. Positive number means number of increases, negative is number of decreases
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_incselection(gui_handle_p h, int16_t dir) {
    uint8_t ret = 0;
    gui_widget_param_t param = {0};
    gui_widget_result_t result = {0};
    
    GUI_WIDGET_PARAMTYPE_I16(&param) = dir;         /* Set parameter */
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_callback(h, GUI_WC_IncSelection, &param, &result);   /* Increase selection for specific amount */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 * \param[in,out]   h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_getzindex
 */
uint8_t
gui_widget_setzindex(gui_handle_p h, int32_t zindex) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_setzindex(h, zindex);         /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get z-index value from widget
 * \param[in,out]   h: Widget handle
 * \return          z-index value
 * \sa              gui_widget_setzindex
 */
int32_t
gui_widget_getzindex(gui_handle_p h) {
    int32_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_getzindex(h);                 /* Set z-index value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

#if GUI_CFG_USE_TRANSPARENCY || __DOXYGEN__
/**
 * \brief           Set transparency level to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       trans: Transparency level, where 0x00 means hidden and 0xFF means totally visible widget
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_gettransparency
 */
uint8_t
gui_widget_settransparency(gui_handle_p h, uint8_t trans) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_settransparency(h, trans);    /* Set widget transparency */
    
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
 * \return          Trasparency value
 * \sa              gui_widget_settransparency
 */
uint8_t
gui_widget_gettransparency(gui_handle_p h) {
    uint8_t trans;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    trans = guii_widget_gettransparency(h);         /* Get widget transparency */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return trans;
}
#endif /* GUI_CFG_USE_TRANSPARENCY || __DOXYGEN__ */

/**
 * \brief           Set 3D mode on widget
 * \param[in,out]   h: Widget handle
 * \param[in]       enable: Value to enable, either 1 or 0
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_set3dstyle(gui_handle_p h, uint8_t enable) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = guii_widget_set3dstyle(h, enable);        /* Set 3D mode */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set widget top padding
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingtop(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpaddingtop(h, x);                /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set widget right padding
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingright(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpaddingright(h, x);              /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set widget bottom padding
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingbottom(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpaddingbottom(h, x);             /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set widget left padding
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingleft(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpaddingleft(h, x);               /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set widget top and bottom paddings
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingtopbottom(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpaddingtopbottom(h, x);          /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set widget left and right paddings
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingleftright(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpaddingleftright(h, x);          /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set widget all paddings
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpadding(gui_handle_p h, gui_dim_t x) {    
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    guii_widget_setpadding(h, x);                   /* Set padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get widget top padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Left padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingtop(gui_handle_p h) {
    gui_dim_t padding;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    padding = guii_widget_getpaddingtop(h);         /* Get padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return padding;
}

/**
 * \brief           Get widget right padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Left padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingright(gui_handle_p h) {
    gui_dim_t padding;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    padding = guii_widget_getpaddingright(h);       /* Get padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return padding;
}

/**
 * \brief           Get widget bottom padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Left padding in units of pixels
 */
gui_dim_t
ui_widget_getpaddingbottom(gui_handle_p h) {
    gui_dim_t padding;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    padding = guii_widget_getpaddingbottom(h);      /* Get padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return padding;
}

/**
 * \brief           Get widget left padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Left padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingleft(gui_handle_p h) {
    gui_dim_t padding;
    __GUI_ASSERTPARAMS(guii_widget_iswidget(h));    /* Check valid parameter */
    __GUI_ENTER();                                  /* Enter GUI */
    
    padding = guii_widget_getpaddingleft(h);        /* Get padding */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return padding;
}

/**	
 * \file            gui_widget.c
 * \brief           Widget specific core functions
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
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

/* Widget absolute cache setup */
#if GUI_CFG_USE_POS_SIZE_CACHE
#define SET_WIDGET_ABS_VALUES(h)        set_widget_abs_values(h)
#else
#define SET_WIDGET_ABS_VALUES(h)
#endif

/**
 * \brief           Calculate widget absolute width
 *                  based on relative values from all parent widgets
 * \param[in]       h: Widget handle
 * \return          Widget width in pixels
 */
static gui_dim_t
calculate_widget_width(gui_handle_p h) {
    gui_dim_t width;
    if (guii_widget_getflag(h, GUI_FLAG_EXPANDED)) {/* Maximize window over parent */
        width = guii_widget_getparentinnerwidth(h); /* Return parent inner width */
    } else if (guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT)) {   /* Percentage width */
        width = GUI_DIM(GUI_ROUND((h->width * guii_widget_getparentinnerwidth(h)) / 100.0f));   /* Calculate percent width */
    } else {                                        /* Normal width */
        width = GUI_DIM(h->width);                  /* Width in pixels */
    }
    return width;
}

/**
 * \brief           Calculate widget absolute height
 *                  based on relative values from all parent widgets
 * \param[in]       h: Widget handle
 */
static gui_dim_t
calculate_widget_height(gui_handle_p h) {
    gui_dim_t height;
    if (guii_widget_getflag(h, GUI_FLAG_EXPANDED)) {/* Maximize window over parent */
        height = guii_widget_getparentinnerheight(h);   /* Return parent inner height */
    } else if (guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT)) {   /* Percentage width */
        height = GUI_DIM(GUI_ROUND((h->height * guii_widget_getparentinnerheight(h)) / 100.0f));  /* Calculate percent height */
    } else {                                        /* Normal height */
        height = GUI_DIM(h->height);                /* Width in pixels */
    }
    return height;
}

/**
 * \brief           Calculate widget absolute X position on screen in units of pixels
 * \param[in]       h: Widget handle
 */
static gui_dim_t
calculate_widget_absolute_x(gui_handle_p h) {
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
        out += guii_widget_getrelativex(w) + gui_widget_getpaddingleft(w);  /* Add X offset from parent and left padding of parent */
        out -= w->x_scroll;                         /* Decrease by scroll value */
    }
    return out;
}

/**
 * \brief           Calculate widget absolute Y position on screen in units of pixels
 * \param[in]       h: Widget handle
 */
static gui_dim_t
calculate_widget_absolute_y(gui_handle_p h) {
    gui_handle_p w;
    gui_dim_t out = 0;
    
    if (h == NULL) {                                /* Check input value */
        return 0;                                   /* At left value */
    }
    
    /* If widget is not expanded, use actual value */
    out = guii_widget_getrelativey(h);              /* Get start relative position */
    
    /* Process all parent widgets to get real absolute screen value */
    for (w = guii_widget_getparent(h); w != NULL;
        w = guii_widget_getparent(w)) {             /* Go through all parent windows */
        out += guii_widget_getrelativey(w) + gui_widget_getpaddingtop(w);   /* Add Y offset from parent and top padding of parent */
        out -= w->y_scroll;                         /* Decrease by scroll value */
    }
    return out;
}

/**
 * \brief           Calculates absolute visible position and size on screen.
 *                  Actual visible position may change when other widgets cover current one
 *                  which we can take as advantage when drawing widget or when calculating clipping area
 *
 * \param[in]       h: Widget handle
 */
uint8_t
calculate_widget_absolute_visible_position_size(gui_handle_p h, gui_dim_t* x1, gui_dim_t* y1, gui_dim_t* x2, gui_dim_t* y2) {
    gui_dim_t x, y, wi, hi, cx, cy, cw, ch;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    cx = gui_widget_getabsolutex(h);                /* Get absolute X position */
    cy = gui_widget_getabsolutey(h);                /* Get absolute Y position */
    cw = gui_widget_getwidth(h);                    /* Get absolute width */
    ch = gui_widget_getheight(h);                   /* Get absolute height */
    
    /* Set widget visible positions with X and Y coordinates */
    *x1 = cx;
    *y1 = cy;
    *x2 = cx + cw;
    *y2 = cy + ch;
    
    /* Check if widget is hidden by any parent or any parent is hidden by its parent */
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

#if GUI_CFG_USE_POS_SIZE_CACHE

/**
 * \brief           Set widget absolute values for position and size
 * \param[in]       h: Widget handle
 */
static uint8_t
set_widget_abs_values(gui_handle_p h) {
    /* Update widget absolute values */
    h->abs_x = calculate_widget_absolute_x(h);
    h->abs_y = calculate_widget_absolute_y(h);
    h->abs_width = calculate_widget_width(h);
    h->abs_height = calculate_widget_height(h);
    
    /* Calculate absolute visible position/size on screen */
    calculate_widget_absolute_visible_position_size(h,
        &h->abs_visible_x1, &h->abs_visible_y1,
        &h->abs_visible_x2, &h->abs_visible_y2);

    /* Update children widgets */
    if (guii_widget_haschildren(h)) {
        gui_handle_p child;
        
        /* Scan all children widgets */
        GUI_LINKEDLIST_WIDGETSLISTNEXT(h, child) {
            set_widget_abs_values(child);           /* Process child widget */
        }
    }
    return 1;
}
#endif /* GUI_CFG_USE_POS_SIZE_CACHE */

/**
 * \brief           Remove widget from memory
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
remove_widget(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    /*
     * Check and react on:
     *
     * Step 1: If widget is in focus, set its parent widget to focus. If there is no parent, disable focus
     * Step 2: Clear previous focused handle, if it is set to current widget
     * Step 3: If widget is active, clear it
     * Step 4: If widget is set as previous active, set its parent as previous active
     */
    if (GUI.focused_widget == h) {                  /* Step 1 */
        if (guii_widget_hasparent(h)) {             /* Should always happen as top parent (window) can't be removed */
            GUI.focused_widget = guii_widget_getparent(h);  /* Set parent widget as focused */
        } else {
            guii_widget_focus_clear();              /* Clear all widgets from focused */
            GUI.focused_widget = NULL;
        }
    }
    if (GUI.focused_widget_prev == h) {             /* Step 2 */
        GUI.focused_widget_prev = NULL;
    }
    if (GUI.active_widget == h) {                   /* Step 3 */
        GUI.active_widget = NULL;                   /* Invalidate active widget */
    }
    if (GUI.active_widget_prev == h) {              /* Step 4 */
        GUI.active_widget_prev = guii_widget_getparent(h);  /* Set widget as previous active */
    }
    if (GUI.window_active != NULL && h == GUI.window_active) {  /* Check for parent window */
        GUI.window_active = guii_widget_getparent(GUI.window_active);
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
    gui_widget_invalidatewithparent(h);
    gui_widget_freetextmemory(h);
    if (h->timer != NULL) {
        guii_timer_remove(&h->timer);
    }
    if (h->colors != NULL) {
        GUI_MEMFREE(h->colors);
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
    
    /* Scan all widgets in system */
    for (h = gui_linkedlist_widgetgetnext(parent, NULL); h != NULL; ) {        
        if (guii_widget_getflag(h, GUI_FLAG_REMOVE)) {  /* Widget should be deleted */
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
            if (guii_widget_haschildren(h)) {       /* Children widgets are supported */
                gui_handle_p tmp;
                
                /* Step 1 */
                GUI_LINKEDLIST_WIDGETSLISTNEXT(h, tmp) {
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
            
            /* Move widget pointer to next widget of already deleted and continue checking */
            h = next;                               /* Set current pointer to next one */
            continue;                               /* Continue to prevent further execution */
        } else if (guii_widget_haschildren(h)) {    /* Children widgets are supported */
            remove_widgets(h);                      /* Check children widgets if anything to remove */
        }
        h = gui_linkedlist_widgetgetnext(NULL, h);  /* Get next widget of current */
    }
    
#if GUI_CFG_OS
    if (lvl == 0) {                                 /* Notify about remove execution */
        gui_sys_mbox_putnow(&GUI.OS.mbox, NULL);
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
get_widget_abs_visible_position_size(gui_handle_p h, gui_dim_t* x1, gui_dim_t* y1, gui_dim_t* x2, gui_dim_t* y2) {
#if GUI_CFG_USE_POS_SIZE_CACHE
    *x1 = h->abs_visible_x1;
    *y1 = h->abs_visible_y1;
    *x2 = h->abs_visible_x2;
    *y2 = h->abs_visible_y2;
    return 1;
#else /* GUI_CFG_USE_POS_SIZE_CACHE */
    return calculate_widget_absolute_visible_position_size(h, x1, y1, x2, y2);
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
}

/**
 * \brief           Set clipping region for visible part of widget
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
set_clipping_region(gui_handle_p h) {
    gui_dim_t x1, y1, x2, y2;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    /* Get visible widget part and absolute position on screen according to parent */
    get_widget_abs_visible_position_size(h, &x1, &y1, &x2, &y2);
    
    /* Possible improvement */
    /*
     * If widget has direct children widgets which are not transparent,
     * it is possible to limit clipping region for update process to only
     * part of widget which is actually visible on screen.
     *
     * This may only work if padding is 0 and widget position wasn't changed
     */
    
    /* Set invalid clipping region */
    if (GUI.display.x1 > x1)    { GUI.display.x1 = x1; }
    if (GUI.display.x2 < x2)    { GUI.display.x2 = x2; }
    if (GUI.display.y1 > y1)    { GUI.display.y1 = y1; }
    if (GUI.display.y2 < y2)    { GUI.display.y2 = y2; }
    
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
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    /* Check ignore flag */
    if (guii_widget_getflag(h, GUI_FLAG_IGNORE_INVALIDATE)) {
        return 0;
    }

    /*
     * First check if any of parent widgets are hidden = ignore redraw
     *
     * When widget is invalidated for first first time,
     * ignore this check and force widget invalidation to create absolute visible positions for later usage
     */
#if GUI_CFG_USE_POS_SIZE_CACHE
    if (!guii_widget_getflag(h, GUI_FLAG_FIRST_INVALIDATE)) 
#endif /* GUI_CFG_USE_POS_SIZE_CACHE */
    {
        for (h1 = guii_widget_getparent(h); h1 != NULL;
            h1 = guii_widget_getparent(h1)) {
            if (guii_widget_ishidden(h1)) {
                return 1;
            }
        }
    }
    guii_widget_clrflag(h, GUI_FLAG_FIRST_INVALIDATE);  /* Clear flag */
        
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
#if GUI_CFG_USE_ALPHA
    if (guii_widget_hasalpha(h1) && guii_widget_hasparent(h1)) {
        invalidate_widget(guii_widget_getparent(h1), 0);    /* Invalidate parent widget */
    }
#endif /* GUI_CFG_USE_ALPHA */
    for (; h1 != NULL; h1 = gui_linkedlist_widgetgetnext(NULL, h1)) {
        get_widget_abs_visible_position_size(h1, &h1x1, &h1y1, &h1x2, &h1y2); /* Get visible position on LCD for widget */
        
        /* Scan widgets on top of current widget */
        for (h2 = gui_linkedlist_widgetgetnext(NULL, h1); h2 != NULL;
                h2 = gui_linkedlist_widgetgetnext(NULL, h2)) {
            /* Get visible position on second widget */
            get_widget_abs_visible_position_size(h2, &h2x1, &h2y1, &h2x2, &h2y2);
                    
            /* Check if next widget is on top of current one */
            if (
                guii_widget_getflag(h2, GUI_FLAG_REDRAW) || /* Flag is already set */
                !GUI_RECT_MATCH(                    /* Widgets are not one over another */
                    h1x1, h1y1, h1x2, h1y2,
                    h2x1, h2y1, h2x2, h2y2)
            ) {
                continue;
            }
            guii_widget_setflag(h2, GUI_FLAG_REDRAW);   /* Redraw widget on next loop */
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

#if GUI_CFG_USE_ALPHA    
    /*
     * Check if any of parent widgets has alpha = should be invalidated too
     *
     * Since recursion is used, call function only once and recursion will take care for upper level of parent widgets
     */
    for (h = guii_widget_getparent(h); h != NULL; h = guii_widget_getparent(h)) {
        if (guii_widget_hasalpha(h)) {              /* If widget has alpha */
            invalidate_widget(h, 0);                /* Invalidate parent too */
            break;
        }
    }
#endif /* GUI_CFG_USE_ALPHA */
    
    return 1;
}

/**
 * \brief           Get widget by specific input parameters
 * \param[in]       parent: Parent widget handle. Set to NULL to use root
 * \param[in]       id: Widget id we are searching for in parent
 * \param[in]       deep: Flag if search should go deeper to check for widget on parent tree
 * \return          Widget handle on success, NULL otherwise
 */
static gui_handle_p
get_widget_by_id(gui_handle_p parent, gui_id_t id, uint8_t deep) {
    gui_handle_p h;
    
    /* List all widgets of parent*/
    GUI_LINKEDLIST_WIDGETSLISTNEXT(parent, h) {
        if (gui_widget_getid(h) == id) {
            return h;
        } else if (deep && guii_widget_haschildren(h)) {    /* Check children if possible */
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
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h1) && guii_widget_iswidget(h2)); 
    
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
 * \param[in]       wp: Set to `1` if width unit is in percent
 * \param[in]       hp: Set to `1` if height unit is in percent
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
set_widget_size(gui_handle_p h, float wi, float hi, uint8_t wp, uint8_t hp) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if ( wi != h->width || hi != h->height ||       /* Check any differences */
        (wp && !guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT)) ||  /* New width is in percent, old is not */
        (!wp && guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT)) ||  /* New width is not in percent, old is */
        (hp && !guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT)) || /* New height is in percent, old is not */
        (!hp && guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT))    /* New height is not in percent, old is */
    ) {
        gui_dim_t wc, hc;
        uint8_t is_flag;
        
        /* Changing position or size must force invalidation */
        is_flag = !!guii_widget_getflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Get ignore invalidate flag */
        guii_widget_clrflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Clear flag */
        
        /* First invalidate current position if not expanded before change of size */
        if (!gui_widget_isexpanded(h) && !guii_widget_getflag(h, GUI_FLAG_FIRST_INVALIDATE)) {
            gui_widget_invalidatewithparent(h);     /* Set old clipping region first */
        }
        
        /* Get current values */
        wc = gui_widget_getwidth(h);                /* Get current width */
        hc = gui_widget_getheight(h);               /* Get current height */
        
        /* Check percent flag */
        if (wp) {
            guii_widget_setflag(h, GUI_FLAG_WIDTH_PERCENT);
        } else {
            guii_widget_clrflag(h, GUI_FLAG_WIDTH_PERCENT);
        }
        if (hp) {
            guii_widget_setflag(h, GUI_FLAG_HEIGHT_PERCENT);
        } else {
            guii_widget_clrflag(h, GUI_FLAG_HEIGHT_PERCENT);
        }
        
        /* Set values for width and height */
        h->width = wi;                              /* Set parameter */
        h->height = hi;                             /* Set parameter */
        SET_WIDGET_ABS_VALUES(h);                   /* Set widget absolute values */
        
        /* Check if any of dimensions are bigger than before */
        if (!gui_widget_isexpanded(h) && !guii_widget_getflag(h, GUI_FLAG_FIRST_INVALIDATE) &&
            (gui_widget_getwidth(h) > wc || gui_widget_getheight(h) > hc)) {
            gui_widget_invalidate(h);               /* Invalidate widget */
        }
        if (is_flag) {
            guii_widget_setflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Set flag back */
        }
    }

    return 1;
}

/**
 * \brief           Set widget position and invalidate approprite widgets if necessary
 * \param[in]       h: Widget handle
 * \param[in]       x: X position in units of pixels or percents
 * \param[in]       y: Y position in units of pixels or percents
 * \param[in]       xp: Set to `1` if X position is in percent
 * \param[in]       yp: Set to `1` if Y position is in percent
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
set_widget_position(gui_handle_p h, float x, float y, uint8_t xp, uint8_t yp) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    if (h->x != x || h->y != y ||                   /* Check any differences */
        (xp && !guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT)) ||   /* New X position is in percent, old is not */
        (!xp && guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT)) ||   /* New X position is not in percent, old is */
        (yp && !guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT)) ||   /* New Y position is in percent, old is not */
        (!yp && guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT))      /* New Y position is not in percent, old is */
    ) {
        uint8_t is_flag;

        /* Changing position or size must force invalidation */
        is_flag = !!guii_widget_getflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Get ignore invalidate flag */
        guii_widget_clrflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Clear flag */

        if (!gui_widget_isexpanded(h) && !guii_widget_getflag(h, GUI_FLAG_FIRST_INVALIDATE)) {
            gui_widget_invalidatewithparent(h);     /* Set old clipping region first */
        }
        
        /* Set position flags */
        if (xp) {
            guii_widget_setflag(h, GUI_FLAG_XPOS_PERCENT);
        } else {
            guii_widget_clrflag(h, GUI_FLAG_XPOS_PERCENT);
        }
        if (yp) {
            guii_widget_setflag(h, GUI_FLAG_YPOS_PERCENT);
        } else {
            guii_widget_clrflag(h, GUI_FLAG_YPOS_PERCENT);
        }
        
        /* Set new position coordinates */
        h->x = x;                                   /* Set parameter */
        h->y = y;                                   /* Set parameter */
        SET_WIDGET_ABS_VALUES(h);                   /* Set widget absolute values */
        
        if (!gui_widget_isexpanded(h) && !guii_widget_getflag(h, GUI_FLAG_FIRST_INVALIDATE)) {
            gui_widget_invalidatewithparent(h);     /* Set new clipping region */
        }
        if (is_flag) {
            guii_widget_setflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Set flag back */
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
    gui_evt_result_t result = {0};
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    /* Desktop window cannot be deleted */
    if (h == gui_window_getdesktop()) {             /* Root widget can not be deleted! */
        return 0;
    }
    
    /* Check widget status itself */
    GUI_EVT_RESULTTYPE_U8(&result) = 1;
    if (!guii_widget_callback(h, GUI_EVT_REMOVE, NULL, &result) || GUI_EVT_RESULTTYPE_U8(&result)) { /* If command was not processed, allow delete */
        GUI_EVT_RESULTTYPE_U8(&result) = 1;      /* Manually allow delete */
    }
    
    /* Check children widgets recursively */
    if (GUI_EVT_RESULTTYPE_U8(&result) && guii_widget_haschildren(h)) {
        gui_handle_p h1;
        GUI_LINKEDLIST_WIDGETSLISTNEXT(h, h1) {
            if (!can_remove_widget(h1)) {           /* If we should not delete it */
                return 0;                           /* Stop on first call */
            }
        }
    }
    
    return GUI_EVT_RESULTTYPE_U8(&result);
}

/**
 * \brief           Check if visible part of widget is inside clipping region for redraw
 * \param[in]       h: Widget handle
 * \param[in]       check_sib_cover: Set to `1` to check if any sibling is fully covering current widget
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_isinsideclippingregion(gui_handle_p h, uint8_t check_sib_cover) {
    gui_dim_t x1, y1, x2, y2;
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    /* Get widget visible section */
    get_widget_abs_visible_position_size(h, &x1, &y1, &x2, &y2);

    /* Check if widget is inside drawing area */
    if (!GUI_RECT_MATCH(
        x1, y1, x2, y2,
        GUI.display.x1, GUI.display.y1, GUI.display.x2, GUI.display.y2
    )) {
        return 0;
    }

    /* 
     * Seems like it is inside drawing area 
     * Now check if this widget is fully hidden by any of its siblings.
     * Check only widgets on linked list after current widget
     */
    if (check_sib_cover) {
        gui_dim_t tx1, ty1, tx2, ty2;
        gui_handle_p tmp;

        /* Process all widgets after current one */
        for (tmp = gui_linkedlist_widgetgetnext(NULL, h); tmp != NULL;
            tmp = gui_linkedlist_widgetgetnext(NULL, tmp)) {

            if (guii_widget_ishidden(tmp)) {        /* Ignore hidden widgets */
                continue;
            }

            /* Get display information for new widget */
            get_widget_abs_visible_position_size(tmp, &tx1, &ty1, &tx2, &ty2);

            /* Check if widget is inside */
            if (GUI_RECT_IS_INSIDE(x1, y1, x2, y2, tx1, ty1, tx2, ty2) &&
                !guii_widget_getflag(tmp, GUI_FLAG_WIDGET_INVALIDATE_PARENT) &&
                !guii_widget_hasalpha(tmp)          /* Must not have transparency enabled */
                ) {
                return 0;                           /* Widget fully covered by another! */
            }
        }
    }

    return 1;                                       /* We have to draw it */
}

/**
 * \brief           Init widget part of library
 */
void
guii_widget_init(void) {
    gui_window_createdesktop(GUI_ID_WINDOW_BASE, NULL);     /* Create base window object */
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
        gui_widget_invalidate(h);                   /* Invalidate object */
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
            if (gui_linkedlist_widgetmovetobottom(parent)) {
                gui_widget_invalidate(parent);
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
    if (GUI.focused_widget != NULL && GUI.focused_widget != GUI.root.first) { /* First widget is always in focus */
        GUI.focused_widget_prev = GUI.focused_widget;   /* Clear everything */
        do {
            guii_widget_callback(GUI.focused_widget, GUI_EVT_FOCUSOUT, NULL, NULL);
            guii_widget_clrflag(GUI.focused_widget, GUI_FLAG_FOCUS);
            gui_widget_invalidate(GUI.focused_widget);
            GUI.focused_widget = guii_widget_getparent(GUI.focused_widget);
        } while (GUI.focused_widget != GUI.root.first); /* Loop to the bottom */
        GUI.focused_widget = NULL;                  /* Reset focused widget */
    }
}

/**
 * \brief           Set widget as focused
 * \param[in]       h: Widget handle
 */
void
guii_widget_focus_set(gui_handle_p h) {
    gui_handle_p common = NULL;

    if (GUI.focused_widget == h || h == NULL) {     /* Check current focused widget */
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
    if (GUI.focused_widget != NULL) {                /* We already have one widget in focus */
        common = get_common_parentwidget(GUI.focused_widget, h); /* Get first widget in common */
        if (common != NULL) {                       /* We have common object, invalidate only those which are not common in tree */
            for (; 
                GUI.focused_widget != NULL && common != NULL && GUI.focused_widget != common;
                GUI.focused_widget = guii_widget_getparent(GUI.focused_widget)) {
                guii_widget_clrflag(GUI.focused_widget, GUI_FLAG_FOCUS);    /* Clear focused flag */
                guii_widget_callback(GUI.focused_widget, GUI_EVT_FOCUSOUT, NULL, NULL);  /* Notify with callback */
                gui_widget_invalidate(GUI.focused_widget);  /* Invalidate widget */
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
    GUI.focused_widget = h;                         /* Set new focused widget */
    while (h != NULL && common != NULL && h != common) {
        guii_widget_setflag(h, GUI_FLAG_FOCUS);     /* Set focused flag */
        guii_widget_callback(h, GUI_EVT_FOCUSIN, NULL, NULL);   /* Notify with callback */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        h = guii_widget_getparent(h);               /* Get parent widget */
    }
}

/**
 * \brief           Clear active status on widget
 */
void
guii_widget_active_clear(void) {
    if (GUI.active_widget != NULL) {
        guii_widget_callback(GUI.active_widget, GUI_EVT_ACTIVEOUT, NULL, NULL);  /* Notify user about change */
        guii_widget_clrflag(GUI.active_widget, GUI_FLAG_ACTIVE | GUI_FLAG_TOUCH_MOVE);  /* Clear all widget based flags */
        GUI.active_widget_prev = GUI.active_widget; /* Save as previous active */
        GUI.active_widget = NULL;                   /* Clear active widget handle */
    }
}

/**
 * \brief           Set widget as active
 * \param[in]       h: Widget handle. When set to NULL, current active widget is cleared
 */
void
guii_widget_active_set(gui_handle_p h) {
    guii_widget_active_clear();                     /* Clear active widget flag */
    GUI.active_widget = h;                          /* Set new active widget */
    if (h != NULL) {
        guii_widget_setflag(GUI.active_widget, GUI_FLAG_ACTIVE);    /* Set active widget flag */
        guii_widget_callback(GUI.active_widget, GUI_EVT_ACTIVEIN, NULL, NULL);
    }
}

/**
 * \brief           Get absolute inner X position of parent widget
 * \note            This function returns inner X position in absolute form.
 *                     Imagine parent absolute X is 10, and left padding is 2. Function returns 12.
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle for which parent should be calculated
 * \return          Parent absolute inner X position
 */
gui_dim_t
guii_widget_getparentabsolutex(gui_handle_p h) {
    gui_dim_t out = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    h = guii_widget_getparent(h);                   /* Get parent of widget */
    if (h != NULL) {                                /* Save left padding */
        out = gui_widget_getpaddingleft(h);         /* Get left padding from parent widget */
    }
    out += gui_widget_getabsolutex(h);              /* Add absolute X of parent and to current padding */
    return out;
}

/**
 * \brief           Get absolute inner Y position of parent widget
 * \note            This function returns inner Y position in absolute form.
 *                     Imagine parent absolute `Y = 10`, and `top padding = 2`. Function returns `12`
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle for which parent should be calculated
 * \return          Parent absolute inner Y position
 */
gui_dim_t
guii_widget_getparentabsolutey(gui_handle_p h) {
    gui_dim_t out = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    h = guii_widget_getparent(h);                   /* Get parent of widget */
    if (h != NULL) {                                /* Save left padding */
        out = gui_widget_getpaddingtop(h);          /* Get top padding from parent widget */
    }
    out += gui_widget_getabsolutey(h);              /* Add absolute Y of parent and to current padding */
    return out;
}

/**
 * \brief           Get absolute `X` position on LCD for specific widget
 * \param[in]       h: Widget handle
 * \return          `X` position on screen in units of pixels
 */
gui_dim_t
gui_widget_getabsolutex(gui_handle_p h) {   
    if (h == NULL) {                                /* Check input value */
        return 0;                                   /* At left value */
    }
#if GUI_CFG_USE_POS_SIZE_CACHE
    return h->abs_x;                                /* Cached value */
#else /* GUI_CFG_USE_POS_SIZE_CACHE */
    return calculate_widget_absolute_x(h);          /* Calculate value */
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
}

/**
 * \brief           Get absolute `Y` position on LCD for specific widget
 * \param[in]       h: Widget handle 
 * \return          `Y` position on screen in units of pixels
 */
gui_dim_t
gui_widget_getabsolutey(gui_handle_p h) {
    if (h == NULL) {                                /* Check input value */
        return 0;                                   /* At left value */
    }
#if GUI_CFG_USE_POS_SIZE_CACHE
    return h->abs_y;                                /* Cached value */
#else /* GUI_CFG_USE_POS_SIZE_CACHE */
    return calculate_widget_absolute_y(h);          /* Calculate value */
#endif /* !GUI_CFG_USE_POS_SIZE_CACHE */
}

/**
 * \brief           Get widget `X` position relative to parent in units of pixels
 * \param[in]       h: Widget handle
 * \return          Widget `X` position relative to parent
 */
gui_dim_t
gui_widget_getxposition(gui_handle_p h) {
    return gui_widget_getabsolutex(h) - guii_widget_getparentabsolutex(h);
}

/**
 * \brief           Get widget `Y` position relative to parent in units of pixels
 * \param[in]       h: Widget handle
 * \return          Widget `Y` position relative to parent
 */
gui_dim_t
gui_widget_getyposition(gui_handle_p h) {
    return gui_widget_getabsolutey(h) - guii_widget_getparentabsolutey(h);
}

/**
 * \brief           Invalidate widget and parent widget for redraw 
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_invalidatewithparent(gui_handle_p h) {
    uint8_t res;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    res = invalidate_widget(h, 1);                  /* Invalidate object with clipping */
    GUI_UNUSED(res);
    if (guii_widget_hasparent(h)) {                 /* If parent exists, invalid only parent */
        invalidate_widget(guii_widget_getparent(h), 0); /* Invalidate parent object without clipping */
    }

    return 1;
}

/**
 * \brief           Set if parent widget should be invalidated when we invalidate primary widget
 * \note            Useful for widgets where there is no background: Transparent images, textview, slider, etc
 * \param[in]       h: Widget handle
 * \param[in]       value: Set to `1` to invalidate parent widget when invalidating widget or `0` to invalidate only widget itself
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setinvalidatewithparent(gui_handle_p h, uint8_t value) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (value) {                                    /* On positive value */
        guii_widget_setflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Enable auto invalidation of parent widget */
    } else {                                        /* On zero */
        guii_widget_clrflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT); /* Disable auto invalidation of parent widget */
    }

    return 1;
}

/**
 * \brief           Set widget parameter in OS secure way
 * \param[in]       h: Widget handle
 * \param[in]       cfg: Configuration to use, passed later to callback function
 * \param[in]       data: Custom data to pass later to configuration callback
 * \param[in]       invalidate: Flag if widget should be invalidated after parameter change
 * \param[in]       invalidateparent: change if parent widget should be invalidated after parameter change
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_setparam(gui_handle_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent) {
    gui_widget_param p;
    gui_evt_param_t param = {0};
    gui_evt_result_t result = {0};

    GUI_ASSERTPARAMS(h != NULL && guii_widget_iswidget(h));   
    
    GUI_EVT_PARAMTYPE_WIDGETPARAM(&param) = &p;
    GUI_EVT_RESULTTYPE_U8(&result) = 1;
    
    p.type = cfg;
    p.data = (void *)data;
    
    guii_widget_callback(h, GUI_EVT_SETPARAM, &param, &result); /* Process callback function */
    if (invalidateparent) {
        gui_widget_invalidatewithparent(h);         /* Invalidate widget and parent */
    } else if (invalidate) {
        gui_widget_invalidate(h);                   /* Invalidate widget only */
    }
    
    return 1;
}

/**
 * \brief           Create new widget and add it to linked list to parent object
 * \note            This function may only be called from widget create function, such as \ref gui_button_create or similar
 * \param[in]       widget: Pointer to \ref gui_widget_t structure with widget description
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       evt_fn: Widget callback function. Set to `NULL` to use default widget specific callback
 * \param[in]       flags: flags for create procedure
 * \return          Widget handle on success, `NULL` otherwise
 */
void *
gui_widget_create(const gui_widget_t* widget, gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    gui_handle_p h;
    
    GUI_ASSERTPARAMS(widget != NULL && widget->callback != NULL);
    
    /* Allocation size check */
    if (widget->size < sizeof(gui_handle)) { 
        return 0;
    }

    h = GUI_MEMALLOC(widget->size);                 /* Allocate memory for widget */
    if (h != NULL) {
        gui_evt_param_t param = {0};
        gui_evt_result_t result = {0};

        h->id = id;                                 /* Save ID */
        h->widget = widget;                         /* Widget object structure */
        h->footprint = GUI_WIDGET_FOOTPRINT;        /* Set widget footprint */
        h->callback = evt_fn;                       /* Set widget callback */
#if GUI_CFG_USE_ALPHA
        h->alpha = 0xFF;                            /* Set full transparency by default */
#endif /* GUI_CFG_USE_ALPHA */
        
        /*
         * Parent window check
         *
         * - Dialog's parent widget is desktop widget
         * - If flag for parent desktop is set, parent widget is also desktop
         * - Otherwise parent widget passed as parameter is used if it supports children widgets
         */
        if (flags & GUI_FLAG_WIDGET_CREATE_NO_PARENT) {
            h->parent = NULL;
        } else if (guii_widget_isdialogbase(h) || flags & GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP) {  /* Dialogs do not have parent widget */
            h->parent = gui_window_getdesktop();    /* Set parent object */
        } else {
            if (parent != NULL && guii_widget_allowchildren(parent)) {
                h->parent = parent;
            } else {
                h->parent = GUI.window_active;       /* Set parent object. It will be NULL on first call */
            }
        }
        
        /* Call pre-init function to set default widget parameters */
        GUI_EVT_RESULTTYPE_U8(&result) = 1;
        guii_widget_callback(h, GUI_EVT_PRE_INIT, NULL, &result);    /* Notify internal widget library about init successful */
        
        if (!GUI_EVT_RESULTTYPE_U8(&result)) {
            GUI_MEMFREE(h);
            h = NULL;
        }
        
        if (h != NULL) {
            /* Set widget default values */
            h->font = widget_default.font;          /* Set default font */

            /* Do not invalidate widget while setting first size and position */
            guii_widget_setflag(h, GUI_FLAG_FIRST_INVALIDATE);  /* Ignore invalidation process for size and position */
            gui_widget_setsize(h, GUI_DIM(width), GUI_DIM(height)); /* Set widget size */
            gui_widget_setposition(h, GUI_DIM(x), GUI_DIM(y));  /* Set widget position */
            gui_widget_invalidate(h);               /* Force invalidation once we set size and position first time */
            
#if GUI_CFG_WIDGET_CREATE_IGNORE_INVALIDATE
            flags |= GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE;  /* Add ignore invalidate flag */
#endif /* GUI_CFG_WIDGET_CREATE_IGNORE_INVALIDATE */
            guii_widget_setflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Ignore invalidation process */
            if (!(flags & GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE)) {
                guii_widget_clrflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Include invalidation process */
            }

            /* Add widget to linked list of parent widget */
            GUI_EVT_RESULTTYPE_U8(&result) = 0;
            guii_widget_callback(h, GUI_EVT_EXCLUDELINKEDLIST, NULL, &result);
            if (!GUI_EVT_RESULTTYPE_U8(&result)) {   /* Check if widget should be added to linked list */
                gui_linkedlist_widgetadd(h->parent, h); /* Add entry to linkedlist of parent widget */
            }
            guii_widget_callback(h, GUI_EVT_INIT, NULL, NULL);  /* Notify user about init successful */
            gui_widget_invalidate(h);               /* Invalidate object */

            /* Notify parent widget if exists */
            if (guii_widget_hasparent(h)) {         /* If widget has parent */
                GUI_EVT_PARAMTYPE_HANDLE(&param) = h;   /* Set widget pointer */
                guii_widget_callback(guii_widget_getparent(h), GUI_EVT_CHILDWIDGETCREATED, &param, NULL);/* Notify parent about new child widget */
            }
        }
    }

    return (void *)h;
}

/*******************************************/
/**                  .....                **/
/*******************************************/

/**
 * \brief           Set color to widget specific index
 * \param[in]       h: Widget handle
 * \param[in]       index: Index in array of colors
 * \param[in]       color: Actual color code to set
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setcolor(gui_handle_p h, uint8_t index, gui_color_t color) {
    uint8_t ret = 1;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    if (h->colors == NULL) {                        /* Do we need to allocate color memory? */
        if (h->widget->color_count) {               /* Check if at least some colors should be used */
            h->colors = GUI_MEMALLOC(sizeof(*h->colors) * h->widget->color_count);
            if (h->colors != NULL) {                /* Copy all colors to new memory first */
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
    
    return ret;
}

/**
 * \brief           Remove widget from memory
 * \note            If widget has child widgets, they will be removed too
 * \param[in,out]   h: Pointer to widget handle. If removed, pointer will be invalidated and set to `NULL`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_remove(gui_handle_p* h) {
    uint8_t ret = 0;
    GUI_ASSERTPARAMS(h != NULL && guii_widget_iswidget(*h));  

    if (can_remove_widget(*h)) {                    /* Check if we can delete widget */
        guii_widget_setflag(*h, GUI_FLAG_REMOVE);   /* Set flag for widget delete */
        GUI.flags |= GUI_FLAG_REMOVE;               /* Set flag for to remove at least one widget from tree */
        if (guii_widget_isfocused(h)) {             /* In case current widget is in focus */
            guii_widget_focus_set(guii_widget_getparent(*h));   /* Set parent as focused */
        }
        ret = 1;
        *h = NULL;
#if GUI_CFG_OS
        gui_sys_mbox_putnow(&GUI.OS.mbox, NULL);    /* Put message to queue */
#endif /* GUI_CFG_OS */
    }

    return ret;                                     /* Removed successfully */
}

/**
 * \brief           Remove children widgets of current widget
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_empty(gui_handle_p h) {
    gui_handle_p child;
    uint8_t ret = 1;

    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    
    /* Process all children widgets */
    for (child = gui_linkedlist_widgetgetnext(h, NULL); child != NULL;
        child = gui_linkedlist_widgetgetnext(NULL, child)) {
        if (!can_remove_widget(child)) {            /* Stop execution if cannot be deleted */
            ret = 0;
            break;
        }
        guii_widget_setflag(h, GUI_FLAG_REMOVE);    /* Set remove flag */
    }
    
    return ret;
}

/***************************************************************/
/***************************************************************/
/**                Widget font & text management              **/
/***************************************************************/
/***************************************************************/
/**
 * \brief           Check if widget has set font and text
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_isfontandtextset(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return h->text != NULL && h->font != NULL && gui_string_length(h->text);    /* Check if conditions are met for drawing string */
}

/**
 * \brief           Process text key (add character, remove it, move cursor, etc)
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle
 * \param[in]       kb: Pointer to \ref guii_keyboard_data_t structure
 * \return          `1` on success, `0` otherwise
 */
uint8_t
guii_widget_processtextkey(gui_handle_p h, guii_keyboard_data_t* kb) {
    size_t len, tlen;
    uint32_t ch;
    uint8_t l;
    gui_string_t currStr;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
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
            size_t pos;
            for (pos = tlen + l - 1; pos > h->textcursor; pos--) {  /* Shift characters down */
                h->text[pos] = h->text[pos - l];
            }
            for (pos = 0; pos < l; pos++) {         /* Fill new characters to empty memory */
                h->text[h->textcursor++] = kb->kb.keys[pos];
            }
            h->text[tlen + l] = 0;                  /* Add 0 to the end */
            
            gui_widget_invalidate(h);               /* Invalidate widget */
            guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);   /* Process callback */
            return 1;
        }
    } else if (ch == 8 || ch == 127) {              /* Backspace character */
        if (tlen && h->textcursor) {
            size_t pos;
            
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
            
            gui_widget_invalidate(h);               /* Invalidate widget */
            guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);/* Process callback */
            return 1;
        }
    }
    return 0;
}

/**
 * \brief           Allocate memory for text operations if text will be dynamic
 * \note            When unicode feature is enabled, memory should be 4x required characters because unicode can store up to 4 bytes for single character
 * \param[in]       h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \return          Number of bytes allocated
 */
uint32_t
gui_widget_alloctextmemory(gui_handle_p h, uint32_t size) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && size > 1);   
    
    if (guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC) && h->text != NULL) { /* Check if already allocated */
        GUI_MEMFREE(h->text);                       /* Free memory first */
        h->textmemsize = 0;                         /* Reset memory size */
    }
    h->text = NULL;                                 /* Reset pointer */

    h->textmemsize = sizeof(gui_char) * (size + 1); /* Allocate text memory */
    h->text = GUI_MEMALLOC(h->textmemsize);
    if (h->text != NULL) {                          /* Check if allocated */
        guii_widget_setflag(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Dynamically allocated */
    } else {
        h->textmemsize = 0;                         /* No dynamic bytes available */
        guii_widget_clrflag(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
    }
    gui_widget_invalidate(h);                       /* Redraw object */
    guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);   /* Process callback */
    
    return h->textmemsize;                          /* Return number of bytes allocated */
}

/**
 * \brief           Frees widget memory previously allocated for text
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_freetextmemory(gui_handle_p h) {
    uint8_t res = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC) && h->text != NULL) { /* Check if dynamically alocated */
        GUI_MEMFREE(h->text);                       /* Free memory first */
        h->text = NULL;                             /* Reset memory */
        h->textmemsize = 0;                         /* Reset memory size */
        guii_widget_clrflag(h, GUI_FLAG_DYNAMICTEXTALLOC); /* Not allocated */
        gui_widget_invalidate(h);                   /* Redraw object */
        guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);   /* Process callback */
        res = 1;
    }
    
    return res;
}

/**
 * \brief           Set text to widget
 * \note            If dynamic memory allocation was used then content will be copied to allocated memory
 *                     otherwise only pointer to input text will be used 
 *                     and each further change of input pointer text will affect to output
 * \param[in]       h: Widget handle
 * \param[in]       text: Pointer to text to set to widget
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_settext(gui_handle_p h, const gui_char* text) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC)) {   /* Memory for text is dynamically allocated */
        if (h->textmemsize) {
            if (gui_string_lengthtotal(text) > (h->textmemsize - 1)) {  /* Check string length */
                gui_string_copyn(h->text, text, h->textmemsize - 1);    /* Do not copy all bytes because of memory overflow */
            } else {
                gui_string_copy(h->text, text);     /* Copy entire string */
            }
            gui_widget_invalidate(h);               /* Redraw object */
            guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);   /* Process callback */
        }
    } else {                                        /* Memory allocated by user */
        if (h->text != NULL && h->text == text) {   /* In case the same pointer is passed to WIDGET */
            gui_widget_invalidate(h);               /* Redraw object */
            guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);   /* Process callback */
        }
        
        if (h->text != text) {                      /* Check if pointer do not match */
            h->text = (gui_char *)text;             /* Set parameter */
            gui_widget_invalidate(h);               /* Redraw object */
            guii_widget_callback(h, GUI_EVT_TEXTCHANGED, NULL, NULL);   /* Process callback */
        }
    }
    h->textcursor = gui_string_lengthtotal(h->text);/* Set cursor to the end of string */
    
    return 1;
}

/**
 * \brief           Get text from widget
 * \note            It will return pointer to text which cannot be modified directly.
 * \param[in]       h: Widget handle
 * \return          Pointer to text from widget
 */
const gui_char *
gui_widget_gettext(gui_handle_p h) {
    const gui_char* t;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
#if GUI_CFG_USE_TRANSLATE
    /* For static texts only */
    if (!guii_widget_getflag(h, GUI_FLAG_DYNAMICTEXTALLOC) && h->text != NULL) {
        t = gui_translate_get(h->text);             /* Get translation entry */
    } else 
#endif /* GUI_CFG_USE_TRANSLATE */
    { 
        t = h->text;                                /* Get text for widget */
    }
    
    return t;
}

/**
 * \brief           Get text from widget
 * \note            Text from widget is copied to input pointer
 * \param[in]       h: Widget handle
 * \param[out]      dst: Destination pointer
 * \param[in]       len: Size of output buffer in units of \ref gui_char
 * \return          Pointer to text from widget
 */
const gui_char*
gui_widget_gettextcopy(gui_handle_p h, gui_char* dst, uint32_t len) {
    const gui_char* t;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    t = gui_widget_gettext(h);
    gui_string_copyn(dst, t, len);
    dst[len] = 0;
    
    return t;  
}

/**
 * \brief           Set widget font for drawing operations
 * \param[in]       h: Widget handle
 * \param[in]       font: Pointer to \ref gui_font_t object for font
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setfont(gui_handle_p h, const gui_font_t* font) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (h->font != font) {
        h->font = font;
        gui_widget_invalidatewithparent(h);
    }
    
    return 1;
}

/**
 * \brief           Get font from widget
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle
 * \return          Pointer to font used for widget
 */
const gui_font_t *
gui_widget_getfont(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return h->font;                                 /* Get widget font */
}

/***************************************************************/
/***************************************************************/
/**                   Widget size management                  **/
/***************************************************************/
/***************************************************************/
/**
 * \brief           Set widget size in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       width: Widget width
 * \param[in]       height: Widget height
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setsize(gui_handle_p h, gui_dim_t width, gui_dim_t height) {
    return set_widget_size(h, GUI_FLOAT(width), GUI_FLOAT(height), 0, 0);
}

/**
 * \brief           Set widget size in units of percent
 * \param[in]       h: Widget handle
 * \param[in]       width: Widget width
 * \param[in]       height: Widget height
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setsizepercent(gui_handle_p h, float width, float height) {
    return set_widget_size(h, width, height, 1, 1);
}

/**
 * \brief           Set widget size in original units
 * \note            If current position is in percents, new value will also be in percent, and so on
 * \param[in]       h: Widget handle
 * \param[in]       width: Widget width
 * \param[in]       height: Widget height
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setsizeoriginal(gui_handle_p h, float width, float height) {
    return set_widget_size(h, width, height,
        guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT) == GUI_FLAG_WIDTH_PERCENT,
        guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT) == GUI_FLAG_HEIGHT_PERCENT
    );
}

/**
 * \brief           Set width of widget in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       width: Width in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setwidth(gui_handle_p h, gui_dim_t width) {
    return set_widget_size(h, GUI_FLOAT(width), h->height,
        0,
        guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT) == GUI_FLAG_HEIGHT_PERCENT
    );
}

/**
 * \brief           Set width of widget in percentage relative to parent widget
 * \param[in]       h: Widget handle
 * \param[in]       width: Width in percentage
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setwidthpercent(gui_handle_p h, float width) {
    return set_widget_size(h, width, h->height,
        1,
        guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT) == GUI_FLAG_HEIGHT_PERCENT
    );
}

/**
 * \brief           Set width of widget in original units
 * \note            If current position is in percents, new value will also be in percent, and so on
 * \param[in]       h: Widget handle
 * \param[in]       width: Widget width
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setwidthoriginal(gui_handle_p h, float width) {
    return set_widget_size(h, width, h->height,
        guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT) == GUI_FLAG_WIDTH_PERCENT,
        guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT) == GUI_FLAG_HEIGHT_PERCENT
    );
}

/**
 * \brief           Set height of widget in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       height: Height in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setheight(gui_handle_p h, gui_dim_t height) {
    return set_widget_size(h, h->width, GUI_FLOAT(height),
        guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT) == GUI_FLAG_WIDTH_PERCENT,
        0
    );
}

/**
 * \brief           Set height of widget in percentage relative to parent widget
 * \param[in]       h: Widget handle
 * \param[in]       height: Height in percentage
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setheightpercent(gui_handle_p h, float height) {
    return set_widget_size(h, h->width, height,
        guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT) == GUI_FLAG_WIDTH_PERCENT,
        1
    );
}

/**
 * \brief           Set height of widget in original units
 * \note            If current position is in percents, new value will also be in percent, and so on
 * \param[in]       h: Widget handle
 * \param[in]       height: Widget height
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setheightoriginal(gui_handle_p h, float height) {
    return set_widget_size(h, h->width, height,
        guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT) == GUI_FLAG_WIDTH_PERCENT,
        guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT) == GUI_FLAG_HEIGHT_PERCENT
    );
}

/**
 * \brief           Toggle expandend (maximized) mode of widget (mostly of windows)
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_toggleexpanded(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return gui_widget_setexpanded(h, !gui_widget_isexpanded(h));
}

/**
 * \brief           Get total width of widget effective on screen in units of pixels
 *                  
 *                  Function returns width of widget according to current widget setup (expanded, percent, etc.)
 * \note            Even if percentage width is used, function will always return value in pixels
 * \param[in]       h: Widget handle
 * \return          Total width in units of pixels
 */
gui_dim_t
gui_widget_getwidth(gui_handle_p h) {
    gui_dim_t res;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && GUI.initialized); 
    
#if GUI_CFG_USE_POS_SIZE_CACHE
    res = h->abs_width;                             /* Cached value */
#else /* GUI_CFG_USE_POS_SIZE_CACHE */
    res = calculate_widget_width(h);                /* Calculate value */
#endif /* GUI_CFG_USE_POS_SIZE_CACHE */
    
    return res;
}

/**
 * \brief           Get total height of widget effective on screen in units of pixels
 *
 *                  Function returns height of widget according to current widget setup (expanded, fill, percent, etc.)
 *
 * \note            Even if percentage height is used, function will always return value in pixels
 * \param[in]       h: Widget handle
 * \return          Total height in units of pixels
 */
gui_dim_t
gui_widget_getheight(gui_handle_p h) {
    gui_dim_t res;

    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && GUI.initialized); 

#if GUI_CFG_USE_POS_SIZE_CACHE
    res = h->abs_height;                            /* Cached value */
#else /* GUI_CFG_USE_POS_SIZE_CACHE */
    res = calculate_widget_height(h);               /* Calculate value */
#endif /* GUI_CFG_USE_POS_SIZE_CACHE */

    return res;
}

/**
 * \brief           Get inner width `total width - padding left - padding right`
 * \param[in]       h: Widget handle
 * \retval          Inner width in units of pixels
 */
gui_dim_t
gui_widget_getinnerwidth(gui_handle_p h) {
    return gui_widget_getwidth(h) - gui_widget_getpaddingleft(h) - gui_widget_getpaddingright(h);
}

/**
 * \brief           Get inner height `total height - padding top - padding bottom`
 * \param[in]       h: Widget handle
 * \retval          Inner height in units of pixels
 */
gui_dim_t
gui_widget_getinnerheight(gui_handle_p h) {
    return gui_widget_getheight(h) - gui_widget_getpaddingtop(h) - gui_widget_getpaddingbottom(h);
}

/**
 * \brief           Get widget width relative to parent width in percent
 * \param[in]       h: Widget handle
 * \return          Widget width in units of percent
 */
float
gui_widget_getwidthpercent(gui_handle_p h) {
    float x, y;

    x = (float)gui_widget_getwidth(h);
    y = (float)guii_widget_getparentwidth(h);

    return x * 100.0f / y;
}

/**
 * \brief           Get widget height relative to parent height in percent
 * \param[in]       h: Widget handle
 * \return          Widget width in units of percent
 */
float
gui_widget_getheightpercent(gui_handle_p h) {
    float x, y;

    x = (float)gui_widget_getheight(h);
    y = (float)guii_widget_getparentheight(h);

    return x * 100.0f / y;
}

/**
 * \brief           Get widget width in original units
 * \note            If current width is in percents, returned value will be in percent units
 * \param[in]       h: Widget handle
 * \param[out]      is_percent: Optional parameter to set if widget height is in units of percent.
 *                     Set to `NULL` if not required
 * \return          Widget width in original units
 */
float
gui_widget_getwidthoriginal(gui_handle_p h, uint8_t* is_percent) {
    if (is_percent != NULL) {
        *is_percent = guii_widget_getflag(h, GUI_FLAG_WIDTH_PERCENT) == GUI_FLAG_WIDTH_PERCENT;
    }
    return h->width;
}

/**
 * \brief           Get widget height in original units
 * \note            If current height is in percents, returned value will be in percent units
 * \param[in]       h: Widget handle
 * \param[out]      is_percent: Optional parameter to set if widget height is in units of percent.
 *                     Set to `NULL` if not required
 * \return          Widget height in original units
 */
float
gui_widget_getheightoriginal(gui_handle_p h, uint8_t* is_percent) {
    if (is_percent != NULL) {
        *is_percent = guii_widget_getflag(h, GUI_FLAG_HEIGHT_PERCENT) == GUI_FLAG_HEIGHT_PERCENT;
    }
    return h->height;
}

/**
 * \brief           Set expandend mode on widget
 *                  
 *                  When enabled, widget will be at `X,Y = 0,0` relative to parent and will have `width,height = 100%,100%`
 * \param[in]       h: Widget handle
 * \param[in]       state: State for expanded mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setexpanded(gui_handle_p h, uint8_t state) {
    uint8_t is_expanded;
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    is_expanded = gui_widget_isexpanded(h);
    if (!state && is_expanded) {                    /* Check current status */
        /* TODO: Force invalidation even if ignored */
        gui_widget_invalidatewithparent(h);         /* Invalidate with parent first for clipping region */
        guii_widget_clrflag(h, GUI_FLAG_EXPANDED);  /* Clear expanded after invalidation */
        SET_WIDGET_ABS_VALUES(h);                   /* Set widget absolute values */
    } else if (state && !is_expanded) {
        guii_widget_setflag(h, GUI_FLAG_EXPANDED);  /* Expand widget */
        SET_WIDGET_ABS_VALUES(h);                   /* Set widget absolute values */
        gui_widget_invalidate(h);                   /* Redraw only selected widget as it is over all window */
    }
    
    return 1;
}

/**
 * \brief           Get widget expanded status
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_isexpanded(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return guii_widget_getflag(h, GUI_FLAG_EXPANDED) == GUI_FLAG_EXPANDED;
}

/***************************************************************/
/***************************************************************/
/**                  Widget position management               **/
/***************************************************************/
/***************************************************************/
/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setposition(gui_handle_p h, gui_dim_t x, gui_dim_t y) {
    return set_widget_position(h, GUI_FLOAT(x), GUI_FLOAT(y), 0, 0);
}

/**
 * \brief           Set widget position relative to parent object in units of percent
 * \param[in]       h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpositionpercent(gui_handle_p h, float x, float y) {
    return set_widget_position(h, x, y, 1, 1);
}

/**
 * \brief           Set widget position relative to parent object in original units
 * \note            If current position is in percents, new value will also be in percent, and so on
 * \param[in]       h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpositionoriginal(gui_handle_p h, float x, float y) {
    return set_widget_position(h, x, y,
        guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) == GUI_FLAG_XPOS_PERCENT,
        guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) == GUI_FLAG_YPOS_PERCENT
    );
}

/**
 * \brief           Set widget `X` position relative to parent object in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setxposition(gui_handle_p h, gui_dim_t x) {
    return set_widget_position(h, GUI_FLOAT(x), h->y,
        0,
        guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) == GUI_FLAG_YPOS_PERCENT
    );
}

/**
 * \brief           Set widget `X` position relative to parent object in units of percent
 * \param[in]       h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setxpositionpercent(gui_handle_p h, float x) {
    return set_widget_position(h, x, h->y,
        1,
        guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) == GUI_FLAG_YPOS_PERCENT
    );
}

/**
 * \brief           Set widget `X` position relative to parent object in current units
 * \note            If current position is in percents, new value will also be in percent, and so on
 * \param[in]       h: Widget handle
 * \param[in]       x: X position relative to parent object in original units
 * \return          `1` on success, `0` otherwise
 */uint8_t
gui_widget_setxpositionoriginal(gui_handle_p h, float x) {
    return set_widget_position(h, x, h->y,
        guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) == GUI_FLAG_XPOS_PERCENT,
        guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) == GUI_FLAG_YPOS_PERCENT
    );
}

/**
 * \brief           Set widget `Y` position relative to parent object in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setyposition(gui_handle_p h, gui_dim_t y) {
    return set_widget_position(h, h->x, GUI_FLOAT(y),
        guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) == GUI_FLAG_XPOS_PERCENT,
        0
    );
}

/**
 * \brief           Set widget `Y` position relative to parent object in units of percent
 * \param[in]       h: Widget handle
 * \param[in]       y: Y position relative to parent object
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setypositionpercent(gui_handle_p h, float y) {
    return set_widget_position(h, h->x, y,
        guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) == GUI_FLAG_XPOS_PERCENT,
        1
    );
}

/**
 * \brief           Set widget `Y` position relative to parent object in current units
 * \note            If current position is in percents, new value will also be in percent, and so on
 * \param[in]       h: Widget handle
 * \param[in]       y: Y position relative to parent object in original units
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setypositionoriginal(gui_handle_p h, float y) {
    return set_widget_position(h, h->x, y,
        guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) == GUI_FLAG_XPOS_PERCENT,
        guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) == GUI_FLAG_YPOS_PERCENT
    );
}

/**
 * \brief           Get `X` position relative to parent in original units
 * \note            If current position is in percents, returned value will be in percent units
 * \param[in]       h: Widget handle
 * \param[out]      is_percent: Optional parameter to set if widget position is in units of percent.
 *                     Set to `NULL` if not required
 * \return          Widget position in original units
 */
float
gui_widget_getxpositionoriginal(gui_handle_p h, uint8_t* is_percent) {
    if (is_percent != NULL) {
        *is_percent = guii_widget_getflag(h, GUI_FLAG_XPOS_PERCENT) == GUI_FLAG_XPOS_PERCENT;
    }
    return h->x;
}

/**
 * \brief           Get `Y` position relative to parent in original units
 * \note            If current position is in percents, returned value will be in percent units
 * \param[in]       h: Widget handle
 * \param[out]      is_percent: Optional parameter to set if widget position is in units of percent.
 *                     Set to `NULL` if not required
 * \return          Widget position in original units
 */
float
gui_widget_getypositionoriginal(gui_handle_p h, uint8_t* is_percent) {
    if (is_percent != NULL) {
        *is_percent = guii_widget_getflag(h, GUI_FLAG_YPOS_PERCENT) == GUI_FLAG_YPOS_PERCENT;
    }
    return h->y;
}

/**
 * \brief           Invalidate widget object and prepare to new redraw
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_invalidate(gui_handle_p h) {
    uint8_t res = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));     
    
    if (!guii_widget_getflag(h, GUI_FLAG_IGNORE_INVALIDATE)) {
        res = invalidate_widget(h, 1);              /* Invalidate widget with clipping */
        if (guii_widget_hasparent(h) && (
                guii_widget_getflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT) || 
                guii_widget_getcoreflag(h, GUI_FLAG_WIDGET_INVALIDATE_PARENT) ||
                guii_widget_hasalpha(h)             /* At least little alpha */
            )) {
            invalidate_widget(guii_widget_getparent(h), 0); /* Invalidate parent object too but without clipping */
        }
    }
    
    return res;
}

uint8_t
gui_widget_force_invalidate(gui_handle_p h) {
    /* TODO: Clear ignore invalidate flag, invalidate, set it back (if it was set before clearing it) */
    return gui_widget_invalidate(h);
}

/**
 * \brief           Set ignore widget option
 *                  
 *                  When setting multiple settings for a widget,
 *                  it is good to prevent auto-invalidate where each separate
 *                  setting invalidates widget and may slow-down execution
 *
 * \include         _example_ignore_invalidate_function.c
 *
 * \param[in]       h: Widget handle
 * \param[in]       en: Set to `1` to ignore invalidation, `0` to disable
 * \param[in]       invalidate: Set to `1` to invalidate before setting flag `en = 1` or after clearing it `en = 0`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setignoreinvalidate(gui_handle_p h, uint8_t en, uint8_t invalidate) {
    uint8_t res = 0;

    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    if (invalidate) {                               /* Check if we need to invalidate before setting ignore */
        guii_widget_clrflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* First clear flag if set already */
        res = gui_widget_invalidate(h);             /* Invalidate widget now */
    }

    if (en) {                                       /* Check enabling ignore */
        guii_widget_setflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Set ignore invalidate flag */
    } else {
        guii_widget_clrflag(h, GUI_FLAG_IGNORE_INVALIDATE); /* Clear ignore invalidate flag */
    }

    return res;
}

/**
 * \brief           Show widget from visible area
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_show(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (guii_widget_getflag(h, GUI_FLAG_HIDDEN)) {  /* If hidden, show it */
        guii_widget_clrflag(h, GUI_FLAG_HIDDEN);
        gui_widget_invalidatewithparent(h);         /* Invalidate it for redraw with parent */
    }
    
    return 1;
}

/**
 * \brief           Put widget to front view and put it to focused state
 * \param[in]       h: Widget handle
 * \param[in]       focus: Set to `1` to put widget to focused state
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_putonfront(gui_handle_p h, uint8_t focus) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    guii_widget_movedowntree(h);                    /* Put widget on front */
    if (focus) {
        guii_widget_focus_set(h);                   /* Set widget to focused state */
    }
    
    return 1;
}

/**
 * \brief           Hide widget from visible area
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_hide(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (!guii_widget_getflag(h, GUI_FLAG_HIDDEN)) { /* If visible, hide it */
        /* TODO: Check if active/focused widget is maybe children of this widget */
        if (GUI.focused_widget != NULL && (GUI.focused_widget == h || gui_widget_ischildof(GUI.focused_widget, h))) {   /* Clear focus */
            guii_widget_focus_set(guii_widget_getparent(GUI.focused_widget)); /* Set parent widget as focused now */
        }
        if (GUI.active_widget != NULL && (GUI.active_widget == h || gui_widget_ischildof(GUI.active_widget, h))) {      /* Clear active */
            guii_widget_active_clear();
        }
        gui_widget_invalidatewithparent(h);         /* Invalidate it for redraw with parent */
        guii_widget_setflag(h, GUI_FLAG_HIDDEN);    /* Hide widget */
    }
    
    return 1;
}

/**
 * \brief           Hide children widgets
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_hidechildren(gui_handle_p h) {
    gui_handle_p t;

    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));

    /* Scan all widgets of current widget and hide them */
    GUI_LINKEDLIST_WIDGETSLISTNEXT(h, t) {
        gui_widget_hide(t);                         /* Hide widget */
    }

    return 1;
}

/**
 * \brief           Get widget ID
 * \param[in]       h: Widget handle
 * \return          Widget ID
 */
gui_id_t
gui_widget_getid(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return h->id;
}

/**
 * \brief           Get parent widget of current
 * \param[in]       h: Widget handle
 * \return          Parent widget handle
 */
gui_handle_p
gui_widget_getparent(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return h->parent;
}

/**
 * \brief           Get first widget handle by ID
 * \note            If multiple widgets have the same ID, first found will be used
 * \param[in,out]   id: Widget ID to search for
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_widget_getbyid(gui_id_t id) {
    return get_widget_by_id(NULL, id, 1);           /* Find widget by ID */
}

/**
 * \brief           Get first widget handle by ID on specific parent widget
 * \note            If multiple widgets have the same ID, first found will be used
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in,out]   id: Widget ID to search for
 * \param[in]       parent: Parent widget handle to use as base search. Set to `NULL` to scan all widgets
 * \param[in]       deep: Set to `1` to search recursive on tree or `0` to search only first level
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_widget_getbyid_ex(gui_id_t id, gui_handle_p parent, uint8_t deep) {
    return get_widget_by_id(parent, id, deep);      /* Search widget */
}

/**
 * \brief           Check if widget handle is child of parent handle
 * \param[in]       h: Widget handle
 * \param[in]       parent: Parent widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_ischildof(gui_handle_p h, gui_handle_p parent) {
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_iswidget(parent));  
    
    if (!(guii_widget_iswidget(h) && guii_widget_iswidget(parent)) || !(GUI.initialized)) {
        ret = 0;
    } else {
        for (h = guii_widget_getparent(h); h != NULL;
            h = guii_widget_getparent(h)) {         /* Check widget parent objects */
            if (parent == h) {                      /* If they matches */
                ret = 1;
                break;
            }
        }
    }
    return ret;
}

/**
 * \brief           Set custom user data to widget
 * \note            Specially useful in callback processing if required
 * \param[in]       h: Widget handle
 * \param[in]       data: Pointer to custom user data
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setuserdata(gui_handle_p h, void* const data) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    h->arg = data;                                  /* Set user argument */
    return 1;
}

/**
 * \brief           Get custom user data from widget previously set with \ref gui_widget_setuserdata
 * \param[in]       h: Widget handle
 * \return          Pointer to user data
 */
void*
gui_widget_getuserdata(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return h->arg;                                  /* Get user argument */
}

/**
 * \brief           Widget callback function for all events
 * \note            Called either from GUI stack or from widget itself to notify user
 *
 * \note            Call this function inside custom callback widget function for unhandled events
 *                     It will automatically call required function according to input widget
 * \param[in]       h: Widget handle where callback occurred
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref gui_widget_evt_t enumeration
 * \param[in]       param: Pointer to optional input data for command. Check \ref gui_widget_evt_t enumeration for more informations
 * \param[out]      result: Pointer to optional result value. Check \ref gui_evt_result_t enumeration for more informations
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_processdefaultcallback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));
    return h->widget->callback(h, evt, param, result); /* Call default callback function */
}

/**
 * \brief           Set callback function to widget
 * \param[in]       h: Widget handle object
 * \param[in]       callback: Callback function for widget
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setcallback(gui_handle_p h, gui_widget_evt_fn callback) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    h->callback = callback;                         /* Set callback function */
    return 1;
}

/**
 * \brief           Widget callback function for all events
 * \note            Called from user outside callback. For calling default callback
 *                      inside custom callback for widget, use \ref gui_widget_processdefaultcallback instead.
 *                      If called from inside widget callback, it may result in recursive calls.
 *
 * \param[in]       h: Widget handle where callback occurred
 * \param[in]       ctrl: Control command which happened for widget. This parameter can be a value of \ref gui_widget_evt_t enumeration
 * \param[in]       param: Pointer to optional input data for command. Check \ref gui_widget_evt_t enumeration for more informations
 * \param[out]      result: Pointer to optional result value. Check \ref gui_evt_result_t enumeration for more informations
 * \return          `1` on success, `0` otherwise
 * \sa              gui_widget_setcallback
 */
uint8_t
gui_widget_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return guii_widget_callback(h, evt, param, result);/* Call callback function */
}

/**
 * \brief           Set widget scroll on `X` axis
 * \note            This is possible on widgets with children support (windows) to have scroll on `X` and `Y`
 * \param[in]       h: Widget handle
 * \param[in]       scroll: Scroll value for X direction
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setscrollx(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    
    if (h->x_scroll != scroll) {
        h->x_scroll = scroll;
        SET_WIDGET_ABS_VALUES(h);                   /* Set new absolute values */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        ret = 1;
    }
    return ret;
}

/**
 * \brief           Set widget scroll on `Y` axis
 * \note            This is possible on widgets with children support (windows) to have scroll on `X` and `Y`
 * \param[in]       h: Widget handle
 * \param[in]       scroll: Scroll value for Y direction
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setscrolly(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    
    if (h->y_scroll != scroll) {
        h->y_scroll = scroll;
        SET_WIDGET_ABS_VALUES(h);                   /* Set new absolute values */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Increase widget scroll on `X` axis
 * \note            This is possible on widgets with children support (windows) to have scroll on `X` and `Y`
 * \param[in]       h: Widget handle
 * \param[in]       scroll: Scroll increase in units of pixels. Use negative value to decrease scroll
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_incscrollx(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    
    if (scroll) {
        h->x_scroll += scroll;
        SET_WIDGET_ABS_VALUES(h);                   /* Set new absolute values */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Increase widget scroll on `Y` axis
 * \note            This is possible on widgets with children support (windows) to have scroll on `X` and `Y`
 * \param[in]       h: Widget handle
 * \param[in]       scroll: Scroll increase in units of pixels. Use negative value to decrease scroll
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_incscrolly(gui_handle_p h, gui_dim_t scroll) {
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    
    if (scroll) {
        h->y_scroll += scroll;
        SET_WIDGET_ABS_VALUES(h);                   /* Set new absolute values */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Get widget scroll on `X` axis
 * \param[in]       h: Widget handle
 * \return          Widget scroll in units of pixels
 */
gui_dim_t
gui_widget_getscrollx(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    return h->x_scroll;                             /* Get X scroll */
}

/**
 * \brief           Get widget scroll on `Y` axis
 * \param[in]       h: Widget handle
 * \return          Widget scroll in units of pixels
 */
gui_dim_t
gui_widget_getscrolly(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h) && guii_widget_allowchildren(h));
    return h->y_scroll;                             /* Get Y scroll */
}

/**
 * \brief           Manually set widget in focus
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setfocus(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    guii_widget_focus_set(h);                       /* Put widget in focus */
    return 1;
}

/**
 * \brief           Set default font for widgets used on widget creation
 * \param[in]       font: Pointer to \ref gui_font_t with font
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setfontdefault(const gui_font_t* font) {
    GUI_ASSERTPARAMS(font != NULL);               
    widget_default.font = font;                     /* Set default font */
    return 1;
}

/**
 * \brief           Increase selection for widget
 * \note            Widget must implement \ref GUI_EVT_INCSELECTION command in callback function and process it
 * \param[in]       h: Widget handle
 * \param[in]       dir: Increase direction. Positive number means number of increases, negative is number of decreases
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_incselection(gui_handle_p h, int16_t dir) {
    gui_evt_param_t param = {0};
    gui_evt_result_t result = {0};
    
    GUI_UNUSED2(result, param);

    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    GUI_EVT_PARAMTYPE_I16(&param) = dir;         /* Set parameter */
    return guii_widget_callback(h, GUI_EVT_INCSELECTION, &param, &result);   /* Increase selection for specific amount */
}

/**
 * \brief           Set z-Index for widgets on the same level. This feature applies on widgets which are not dialogs
 * \note            Larger z-index value means greater position on screen. In case of multiple widgets on same z-index level, they are automatically modified for correct display
 * \param[in]       h: Widget handle
 * \param[in]       zindex: Z-Index value for widget. Any value can be used
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setzindex(gui_handle_p h, int32_t zindex) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    if (h->zindex != zindex) {                      /* There was a change in z-index value */
        int32_t current = h->zindex;
        h->zindex = zindex;                         /* Set new index */
        if (zindex < current) {                     /* New index value is less important than before = move widget to top */
            gui_linkedlist_widgetmovetotop(h);      /* Move widget to top on linked list = less important and less visible */
        } else {
            gui_linkedlist_widgetmovetobottom(h);   /* Move widget to bottom on linked list = most important and most visible */
        }
    }
    
    return 1;
}

/**
 * \brief           Get z-index value from widget
 * \param[in]       h: Widget handle
 * \return          z-index value
 */
int32_t
gui_widget_getzindex(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return h->zindex;                               /* Get z-index value */
}

/**
 * \brief           Set widget alpha level to widget
 * \param[in]       h: Widget handle
 * \param[in]       alpha: Alpha level, where `0x00` means hidden and `0xFF` means totally visible widget
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setalpha(gui_handle_p h, uint8_t alpha) {
    uint8_t ret = 0;
    
#if GUI_CFG_USE_ALPHA
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    if (h->alpha != alpha) {                        /* Check transparency match */
        h->alpha = alpha;                           /* Set new transparency level */
        SET_WIDGET_ABS_VALUES(h);                   /* Set widget absolute values */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        ret = 1;
    }
#endif /* GUI_CFG_USE_ALPHA */
    
    return ret;
}

/**
 * \brief           Get widget transparency value
 * \note            Value between 0 and 0xFF is used:
 *                      - `0x00`: Widget is hidden
 *                      - `0xFF`: Widget is fully opaque
 *                      - `0x01 - 0xFE`: Widget has alpha value
 * \param[in]       h: Widget handle
 * \return          Alpha value
 */
uint8_t
gui_widget_getalpha(gui_handle_p h) {
    uint8_t a;
    
#if GUI_CFG_USE_ALPHA
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    a = GUI_U8(h->alpha);
#else
    a = 0xFF;
#endif
    
    return a;
}

/**
 * \brief           Set widget top padding
 * \param[in]       h: Widget handle
 * \param[in]       x: Padding in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_widget_setpaddingtop(gui_handle_p h, gui_dim_t x) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    h->padding = (uint32_t)((h->padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)x) << 24);/* Padding top */
    SET_WIDGET_ABS_VALUES(h);
    
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
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    h->padding = (uint32_t)((h->padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)x) << 16);/* Padding right */
    SET_WIDGET_ABS_VALUES(h);
    
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
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    h->padding = (uint32_t)((h->padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)x) << 8); /* Padding bottom */
    SET_WIDGET_ABS_VALUES(h);
    
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
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    h->padding = (uint32_t)((h->padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)x) << 0); /* Padding left */
    SET_WIDGET_ABS_VALUES(h);
    
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
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    h->padding = (uint32_t)((h->padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)x) << 24);/* Padding top */
    h->padding = (uint32_t)((h->padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)x) << 8); /* Padding bottom */
    SET_WIDGET_ABS_VALUES(h);
    
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
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    

    h->padding = (uint32_t)((h->padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)x) << 16);/* Padding right */
    h->padding = (uint32_t)((h->padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)x) << 0); /* Padding left */
    SET_WIDGET_ABS_VALUES(h);
    
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
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    
    h->padding = (uint32_t)((h->padding & 0x00FFFFFFUL) | (uint32_t)((uint8_t)x) << 24);/* Padding top */
    h->padding = (uint32_t)((h->padding & 0xFF00FFFFUL) | (uint32_t)((uint8_t)x) << 16);/* Padding right */
    h->padding = (uint32_t)((h->padding & 0xFFFF00FFUL) | (uint32_t)((uint8_t)x) << 8); /* Padding bottom */
    h->padding = (uint32_t)((h->padding & 0xFFFFFF00UL) | (uint32_t)((uint8_t)x) << 0); /* Padding left */
    SET_WIDGET_ABS_VALUES(h);
    
    return 1;
}

/**
 * \brief           Get widget top padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Top padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingtop(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return GUI_DIM((h->padding >> 24) & 0xFF);
}

/**
 * \brief           Get widget right padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Right padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingright(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return GUI_DIM((h->padding >> 16) & 0xFF);
}

/**
 * \brief           Get widget bottom padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Bottom padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingbottom(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return GUI_DIM((h->padding >> 8) & 0xFF);
}

/**
 * \brief           Get widget left padding in units of pixels
 * \param[in]       h: Widget handle
 * \return          Left padding in units of pixels
 */
gui_dim_t
gui_widget_getpaddingleft(gui_handle_p h) {
    GUI_ASSERTPARAMS(guii_widget_iswidget(h));    
    return GUI_DIM((h->padding >> 0) & 0xFF);
}

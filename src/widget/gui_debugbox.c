/**	
 * \file            gui_debugbox.c
 * \brief           Debug box widget
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
#include "widget/gui_debugbox.h"

#define __GL(x)             ((gui_debugbox_t *)(x))

static uint8_t gui_debugbox_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WIN_BG,
    GUI_COLOR_WIN_TEXT,
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("DEBUGBOX"),                        /*!< Widget name */
    .size = sizeof(gui_debugbox_t),                 /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_debugbox_callback,              /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Define number of colors */
};

#define o                   ((gui_debugbox_t *)(h))

/* Get item height in debugbox */
static uint16_t
item_height(gui_handle_p h, uint16_t* offset) {
    uint16_t size = 0;
    if (h->font != NULL) {
        size = GUI_U16((float)h->font->size * 1.3f);
    }
    if (offset != NULL) {                           /* Calculate top offset */
        *offset = (size - h->font->size) >> 1;
    }
    return size;                                    /* Return height for element */
}

/* Get number of entries maximal on one page */
static int16_t
nr_entries_pp(gui_handle_p h) {
    int16_t res = 0;
    if (h->font != NULL) {                          /* Font is responsible for this setup */
        res = guii_widget_getheight(h) / item_height(h, NULL);
    }
    return res;
}

/* Slide up or slide down widget elements */
static void
slide(gui_handle_p h, int16_t dir) {
    int16_t mPP = nr_entries_pp(h);
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->visiblestartindex + dir) < 0) {
            o->visiblestartindex = 0;
        } else {
            o->visiblestartindex += dir;
        }
        guii_widget_invalidate(h);
    } else if (dir > 0) {
        if ((o->visiblestartindex + dir) > (o->count - mPP - 1)) {  /* Slide elements down */
            o->visiblestartindex = o->count - mPP;
        } else {
            o->visiblestartindex += dir;
        }
        guii_widget_invalidate(h);
    }
}

/* Check values */
static void
check_values(gui_handle_p h) {
    int16_t mPP = nr_entries_pp(h);                 /* Get number of lines visible in widget at a time */
   
    if (o->visiblestartindex < 0) {                 /* Check visible start index position */
        o->visiblestartindex = 0;
    } else if (o->visiblestartindex > 0) {
        if (o->count > mPP) {
            if (o->visiblestartindex + mPP >= o->count) {
                o->visiblestartindex = o->count - mPP;
            }
        } else {
            o->visiblestartindex = 0;
        }
    }
    
    if (o->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO) {  /* Check slider mode */
        if (o->count > mPP) {
            o->flags |= GUI_FLAG_DEBUGBOX_SLIDER_ON;
        } else {
            o->flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_ON;
        }
    }
}

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       ctr: Callback type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          1 if command processed, 0 otherwise
 */
static uint8_t
gui_debugbox_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
#if GUI_CFG_USE_TOUCH
    static gui_dim_t ty;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GL(h)->sliderwidth = 30;              /* Set slider width */
            __GL(h)->maxcount = 15;                 /* Number of maximal entries for debug */
            __GL(h)->flags |= GUI_FLAG_DEBUGBOX_SLIDER_AUTO;   /* Set auto mode for slider */
            return 1;
        }
        case GUI_WC_Draw: {
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height;
            
            x = guii_widget_getabsolutex(h);       /* Get absolute X coordinate */
            y = guii_widget_getabsolutey(h);       /* Get absolute Y coordinate */
            width = guii_widget_getwidth(h);       /* Get widget width */
            height = guii_widget_getheight(h);     /* Get widget height */
            
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, guii_widget_getcolor(h, GUI_DEBUGBOX_COLOR_BG));
            
            /* Draw side scrollbar */
            if (o->flags & GUI_FLAG_DEBUGBOX_SLIDER_ON) {
                gui_draw_sb_t sb;
                gui_draw_scrollbar_init(&sb);
                
                width -= o->sliderwidth;            /* Decrease available width */
                
                sb.x = x + width - 1;
                sb.y = y + 1;
                sb.width = o->sliderwidth;
                sb.height = height - 2;
                sb.dir = GUI_DRAW_SB_DIR_VERTICAL;
                sb.entriestop = o->visiblestartindex;
                sb.entriestotal = o->count;
                sb.entriesvisible = nr_entries_pp(h);
                
                gui_draw_scrollbar(disp, &sb);      /* Draw scroll bar */
            } else {
                width--;                            /* Go one pixel down for alignment */
            }
            
            /* Draw text if possible */
            if (h->font != NULL && gui_linkedlist_hasentries(&__GL(h)->root)) { /* Is first set? */
                gui_draw_font_t f;
                gui_debugbox_item_t* item;
                uint16_t itemheight;                /* Get item height */
                uint16_t index = 0;                 /* Start index */
                gui_dim_t tmp;
                
                itemheight = item_height(h, 0);     /* Get item height and Y offset */
                
                gui_draw_font_init(&f);             /* Init structure */
                
                f.x = x + 4;
                f.y = y + 2;
                f.width = width - 4;
                f.height = itemheight;
                f.align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                
                tmp = disp->y2;                     /* Scale out drawing area */
                if (disp->y2 > (y + height - 2)) {
                    disp->y2 = y + height - 2;
                }
                
                for (index = 0, item = (gui_debugbox_item_t *)gui_linkedlist_getnext_gen(&o->root, NULL); item && f.y <= disp->y2;
                        item = (gui_debugbox_item_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)item), index++) {
                    if (index < o->visiblestartindex) { /* Check for start drawing index */
                        continue;
                    }
                    f.color1 = guii_widget_getcolor(h, GUI_DEBUGBOX_COLOR_TEXT);
                    gui_draw_writetext(disp, guii_widget_getfont(h), item->text, &f);
                    f.y += itemheight;
                }
                disp->y2 = tmp;
            }
            
            return 1;
        }
        case GUI_WC_Remove: {
            gui_debugbox_item_t* item;
            while ((item = (gui_debugbox_item_t *)gui_linkedlist_remove_gen(&o->root, (gui_linkedlist_t *)gui_linkedlist_getnext_gen(&o->root, NULL))) != NULL) {
                GUI_MEMFREE(item);                  /* Free memory */
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_WC_TouchStart: {
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            ty = ts->y_rel[0];
            
            GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (h->font != NULL) {
                gui_dim_t height = item_height(h, NULL);   /* Get element height */
                gui_dim_t diff = ty - ts->y_rel[0];
                
                if (GUI_ABS(diff) > height) {
                    slide(h, diff > 0 ? 1 : -1);    /* Slide widget */
                    ty = ts->y_rel[0];              /* Save pointer */
                }
            }
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_WC_Click: {
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            gui_dim_t width = guii_widget_getwidth(h);  /* Get widget widget */
            gui_dim_t height = guii_widget_getheight(h);/* Get widget height */
            
            if (o->flags & GUI_FLAG_DEBUGBOX_SLIDER_ON) {
                if (ts->x_rel[0] > (width - o->sliderwidth)) {  /* Touch is inside slider */
                    if (ts->y_rel[0] < o->sliderwidth) {
                        slide(h, -1);               /* Slide one value up */
                    } else if (ts->y_rel[0] > (height - o->sliderwidth)) {
                        slide(h, 1);                /* Slide one value down */
                    }
                }
            }
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new debugbox widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref gui_widget_callback_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: flags for widget creation
 * \return          \ref gui_handle_p object of created widget on success, NULL otherwise
 */
gui_handle_p
gui_debugbox_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to debugbox
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors. This parameter can be a value of \ref GUI_DEBUGBOX_COLOR_t enumeration
 * \param[in]       color: Actual color code to set
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_setcolor(gui_handle_p h, GUI_DEBUGBOX_COLOR_t index, gui_color_t color) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color, 1);   /* Set color */
}

/**
 * \brief           Add a new string to list box
 * \param[in,out]   h: Widget handle
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_addstring(gui_handle_p h, const gui_char* text) {
    gui_debugbox_item_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    
    item = GUI_MEMALLOC(sizeof(*item) + gui_string_lengthtotal(text) + 1);  /* Allocate memory for entry */
    if (item != NULL) {
        __GUI_LEAVE(1);                             /* Enter GUI */
        item->text = (void *)((char *)item + sizeof(*item));/* Add text to entry */
        gui_string_copy(item->text, text);          /* Copy text */
        gui_linkedlist_add_gen(&__GL(h)->root, &item->list);/* Add to linked list */
        __GL(h)->count++;                           /* Increase number of strings */
        
        /*
         * Use while loop in case user later changed max
         * number of entries to lower value.
         *
         * In this case we have to remove more than just one element
         */
        while (__GL(h)->count > __GL(h)->maxcount) {
            gui_debugbox_item_t* firstItem;
            firstItem = (gui_debugbox_item_t *)gui_linkedlist_getnext_gen(&__GL(h)->root, NULL);
            if (firstItem != NULL) {
                gui_linkedlist_remove_gen(&__GL(h)->root, (gui_linkedlist_t *)firstItem);
                GUI_MEMFREE(firstItem);
                __GL(h)->count--;
            } else {
                break;
            }
        }
        
        __GL(h)->visiblestartindex = __GL(h)->count;/* Invalidate visible start index */
        check_values(h);                            /* Check values */
        guii_widget_invalidate(h);                 /* Invalidate widget */
        __GUI_LEAVE(1);                             /* Leave GUI */
        
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to 1 for auto mode or 0 for manual mode
 * \return          `1` on success, `0` otherwise
 * \sa              gui_debugbox_setslidervisibility
 */
uint8_t
gui_debugbox_setsliderauto(gui_handle_p h, uint8_t autoMode) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    __GUI_LEAVE(1);                                 /* Enter GUI */
    
    if (autoMode && !(__GL(h)->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        __GL(h)->flags |= GUI_FLAG_DEBUGBOX_SLIDER_AUTO;
        guii_widget_invalidate(h);                 /* Invalidate widget */
    } else if (!autoMode && (__GL(h)->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        __GL(h)->flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_AUTO;
        guii_widget_invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE(1);                                 /* Leave GUI */
    return 1;
}

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in,out]   h: Widget handle
 * \param[in]       visible: Slider visible status, 1 or 0
 * \return          `1` on success, `0` otherwise
 * \sa              gui_debugbox_setsliderauto
 */
uint8_t
gui_debugbox_setslidervisibility(gui_handle_p h, uint8_t visible) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    __GUI_LEAVE(1);                                 /* Enter GUI */
    
    if (!(__GL(h)->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        if (visible && !(__GL(h)->flags & GUI_FLAG_DEBUGBOX_SLIDER_ON)) {
            __GL(h)->flags |= GUI_FLAG_DEBUGBOX_SLIDER_ON;
            guii_widget_invalidate(h);             /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GL(h)->flags & GUI_FLAG_DEBUGBOX_SLIDER_ON)) {
            __GL(h)->flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_ON;
            guii_widget_invalidate(h);             /* Invalidate widget */
            ret = 1;
        }
    }
    
    __GUI_LEAVE(1);                                 /* Leave GUI */
    return ret;
}

/**
 * \brief           Scroll list if possible
 * \param[in,out]   h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_scroll(gui_handle_p h, int16_t step) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    __GUI_LEAVE(1);                                 /* Enter GUI */
    
    start = __GL(h)->visiblestartindex;
    __GL(h)->visiblestartindex += step;
        
    check_values(h);                                /* Check widget values */
    
    start = start != __GL(h)->visiblestartindex;    /* Check if there was valid change */
    
    if (start) {
        guii_widget_invalidate(h);
    }
    
    __GUI_LEAVE(1);                                 /* Leave GUI */
    return GUI_U8(start);
}

/**
 * \brief           Set maximal number of items for debugbox
 * \param[in,out]   h: Widget handle
 * \param[in]       max_items: Maximal number of items
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_setmaxitems(gui_handle_p h, int16_t max_items) {    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && max_items > 0);   /* Check input parameters */
    __GUI_LEAVE(1);                                 /* Enter GUI */
    
    __GL(h)->maxcount = max_items;
    
    __GUI_LEAVE(1);                                 /* Leave GUI */
    return 0;
}

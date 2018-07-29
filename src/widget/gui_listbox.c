/**	
 * \file            gui_listbox.c
 * \brief           List box widget
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "widget/gui_listbox.h"

#define __GL(x)             ((gui_listbox_t *)(x))

static uint8_t gui_listbox_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WIN_BG,
    GUI_COLOR_WIN_TEXT,
    GUI_COLOR_WIN_SEL_FOC,
    GUI_COLOR_WIN_SEL_NOFOC,
    GUI_COLOR_WIN_SEL_FOC_BG,
    GUI_COLOR_WIN_SEL_NOFOC_BG
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("LISTBOX"),                         /*!< Widget name */
    .size = sizeof(gui_listbox_t),                  /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_listbox_callback,               /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Define number of colors */
};

#define o                   ((gui_listbox_t *)(h))

/* Get item from listbox entry */
static gui_listbox_item_t*
get_item(gui_handle_p h, uint16_t index) {
    uint16_t i = 0;
    gui_listbox_item_t* item = 0;
    
    if (index >= o->count) {                        /* Check if valid index */
        return 0;
    }
    
    if (index == 0) {                               /* Check for first element */
        return (gui_listbox_item_t *)gui_linkedlist_getnext_gen(&o->root, NULL);/* Return first element */
    } else if (index == o->count - 1) {
        return (gui_listbox_item_t *)gui_linkedlist_getprev_gen(&o->root, NULL);/* Return last element */
    }
    
    item = (gui_listbox_item_t *)gui_linkedlist_getnext_gen(&o->root, NULL);
    while (i++ < index) {
        item = (gui_listbox_item_t *)gui_linkedlist_getnext_gen(NULL, &item->list);
    }
    return item;
}

/* Get item height in listbox */
static uint16_t
item_height(gui_handle_p h, uint16_t* offset) {
    uint16_t size = GUI_U16((float)h->font->size * 1.3f);
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
        res = gui_widget_getheight(h, 0) / item_height(h, NULL);
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

/* Set selection for widget */
static void
set_selection(gui_handle_p h, int16_t selected) {
    if (o->selected != selected) {                  /* Set selected value */
        o->selected = selected;
        guii_widget_callback(h, GUI_WC_SelectionChanged, NULL, NULL);  /* Notify about selection changed */
    }                         
}

/* Increase or decrease selection */
static void
inc_selection(gui_handle_p h, int16_t dir) {
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->selected + dir) < 0) {
            set_selection(h, 0);
        } else {
            set_selection(h, o->selected + dir);
        }
        guii_widget_invalidate(h);
    } else if (dir > 0) {
        if ((o->selected + dir) > (o->count - 1)) { /* Slide elements down */
            set_selection(h, o->count - 1);
        } else {
            set_selection(h, o->selected + dir);
        }
        guii_widget_invalidate(h);
    }
}

/* Check values */
static void
check_values(gui_handle_p h) {
    int16_t mPP = nr_entries_pp(h);                 /* Get number of lines visible in widget at a time */
   
    if (o->selected >= 0) {                         /* Check for selected value range */
        if (o->selected >= o->count) {
            set_selection(h, o->count - 1);
        }
    }
    if (o->visiblestartindex < 0) {                 /* Check visible start index position */
        o->visiblestartindex = 0;
    } else if (o->visiblestartindex > 0) {
        if (o->count > mPP) {
            if (o->visiblestartindex + mPP >= o->count) {
                o->visiblestartindex = o->count - mPP;
            }
        }
    }
    
    if (o->flags & GUI_FLAG_LISTBOX_SLIDER_AUTO) {  /* Check slider mode */
        if (o->count > mPP) {
            o->flags |= GUI_FLAG_LISTBOX_SLIDER_ON;
        } else {
            o->flags &= ~GUI_FLAG_LISTBOX_SLIDER_ON;
        }
    }
}

/* Delete list item box by index */
static uint8_t
delete_item(gui_handle_p h, uint16_t index) {
    gui_listbox_item_t* item;
    
    item = get_item(h, index);                      /* Get list item from handle */
    if (item) {
        gui_linkedlist_remove_gen(&__GL(h)->root, &item->list);
        __GL(h)->count--;                           /* Decrease count */
        
        if (o->selected == index) {
            set_selection(h, -1);
        }
        
        check_values(h);                           /* Check widget values */
        guii_widget_invalidate(h);
        return 1;
    }
    return 0;
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
gui_listbox_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
#if GUI_CFG_USE_TOUCH
    static gui_dim_t ty;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GL(h)->selected = -1;                 /* No selection */
            __GL(h)->sliderwidth = 30;              /* Set slider width */
            __GL(h)->flags |= GUI_FLAG_LISTBOX_SLIDER_AUTO;   /* Set auto mode for slider */
            return 1;
        }
        case GUI_WC_Draw: {
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height;
            
            x = guii_widget_getabsolutex(h);        /* Get absolute X coordinate */
            y = guii_widget_getabsolutey(h);        /* Get absolute Y coordinate */
            width = gui_widget_getwidth(h, 0);      /* Get widget width */
            height = gui_widget_getheight(h, 0);    /* Get widget height */
            
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, guii_widget_getcolor(h, GUI_LISTBOX_COLOR_BG));
            
            /* Draw side scrollbar */
            if (o->flags & GUI_FLAG_LISTBOX_SLIDER_ON) {
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
            if (h->font != NULL && gui_linkedlist_hasentries(&__GL(h)->root)) {   /* Is first set? */
                gui_draw_font_t f;
                gui_listbox_item_t* item;
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
                
                for (index = 0, item = (gui_listbox_item_t *)gui_linkedlist_getnext_gen(&o->root, NULL); item && f.y <= disp->y2;
                        item = (gui_listbox_item_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)item), index++) {
                    if (index < o->visiblestartindex) { /* Check for start drawing index */
                        continue;
                    }
                    if (index == __GL(h)->selected) {
                        gui_draw_filledrectangle(disp, x + 2, f.y, width - 3, GUI_MIN(f.height, itemheight), guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_LISTBOX_COLOR_SEL_FOC_BG) : guii_widget_getcolor(h, GUI_LISTBOX_COLOR_SEL_NOFOC_BG));
                        f.color1 = guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_LISTBOX_COLOR_SEL_FOC) : guii_widget_getcolor(h, GUI_LISTBOX_COLOR_SEL_NOFOC);
                    } else {
                        f.color1 = guii_widget_getcolor(h, GUI_LISTBOX_COLOR_TEXT);
                    }
                    gui_draw_writetext(disp, gui_widget_getfont(h, 0), item->text, &f);
                    f.y += itemheight;
                }
                disp->y2 = tmp;
            }
            
            return 1;
        }
        case GUI_WC_Remove: {
            gui_listbox_item_t* item;
            while ((item = (gui_listbox_item_t *)gui_linkedlist_remove_gen(&o->root, (gui_linkedlist_t *)gui_linkedlist_getnext_gen(&o->root, NULL))) != NULL) {
                GUI_MEMFREE(item, 0);               /* Free memory */
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
                    ty = ts->y_rel[0];               /* Save pointer */
                }
            }
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_WC_Click: {
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            uint8_t handled = 0;
            gui_dim_t width = gui_widget_getwidth(h, 0);    /* Get widget widget */
            gui_dim_t height = gui_widget_getheight(h, 0);  /* Get widget height */
            
            if (o->flags & GUI_FLAG_LISTBOX_SLIDER_ON) {
                if (ts->x_rel[0] > (width - o->sliderwidth)) {   /* Touch is inside slider */
                    if (ts->y_rel[0] < o->sliderwidth) {
                        slide(h, -1);               /* Slide one value up */
                    } else if (ts->y_rel[0] > (height - o->sliderwidth)) {
                        slide(h, 1);                /* Slide one value down */
                    }
                    handled = 1;
                }
            }
            if (!handled && h->font != NULL) {
                uint16_t height = item_height(h, NULL);    /* Get element height */
                uint16_t tmpselected;
                
                tmpselected = ts->y_rel[0] / height; /* Get temporary selected index */
                if ((o->visiblestartindex + tmpselected) <= o->count) {
                    set_selection(h, o->visiblestartindex + tmpselected);
                    guii_widget_invalidate(h);     /* Choose new selection */
                }
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            guii_keyboard_data_t* kb = GUI_WIDGET_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_DOWN) {   /* On pressed down */
                inc_selection(h, 1);                /* Increase selection */
            } else if (kb->kb.keys[0] == GUI_KEY_UP) {
                inc_selection(h, -1);               /* Decrease selection */
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_WC_IncSelection: {
            inc_selection(h, GUI_WIDGET_PARAMTYPE_I16(param));  /* Increase selection */
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Set operation result */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new listbox widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_listbox_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags, protect);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to listbox
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_setcolor(gui_handle_p h, gui_listbox_color_t index, gui_color_t color, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color, protect); /* Set color */
}

/**
 * \brief           Add a new string to list box
 * \param[in,out]   h: Widget handle
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_addstring(gui_handle_p h, const gui_char* text, const uint8_t protect) {
    gui_listbox_item_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    item = GUI_MEMALLOC(sizeof(*item), 0);          /* Allocate memory for entry */
    if (item != NULL) {
        item->text = (gui_char *)text;              /* Add text to entry */
        gui_linkedlist_add_gen(&__GL(h)->root, &item->list);/* Add to linked list */
        __GL(h)->count++;                           /* Increase number of strings */
        
        check_values(h);                            /* Check values */
        guii_widget_invalidate(h);                  /* Invalidate widget */
        
        ret = 1;
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */
    
    return ret;
}

/**
 * \brief           Set string value to already added string index
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index (position) on list to set/change text
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_setstring(gui_handle_p h, uint16_t index, const gui_char* text, const uint8_t protect) {
    gui_listbox_item_t* item;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    item = get_item(h, index);                      /* Get list item from handle */
    if (item != NULL) {
        item->text = (gui_char *)text;              /* Set new text */
        guii_widget_invalidate(h);                  /* Invalidate widget */
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return item ? 1 : 0;
}

/**
 * \brief           Delete first string from list
 * \param[in,out]   h: Widget handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_deletefirststring(gui_handle_p h, const uint8_t protect) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    ret = delete_item(h, 0);                        /* Delete first item */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return ret;
}

/**
 * \brief           Delete last string from list
 * \param[in,out]   h: Widget handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_deletelaststring(gui_handle_p h, const uint8_t protect) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    ret = delete_item(h, __GL(h)->count - 1);       /* Delete last item */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return ret;
}

/**
 * \brief           Delete specific entry from list
 * \param[in,out]   h: Widget handle
 * \param[in]       index: List index (position) to delete
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_deletestring(gui_handle_p h, uint16_t index, const uint8_t protect) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    ret = delete_item(h, index);                    /* Delete item */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return ret;
}

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to `1` for auto mode or `0` for manual mode
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_setsliderauto(gui_handle_p h, uint8_t autoMode, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    if (autoMode && !(__GL(h)->flags & GUI_FLAG_LISTBOX_SLIDER_AUTO)) {
        __GL(h)->flags |= GUI_FLAG_LISTBOX_SLIDER_AUTO;
        guii_widget_invalidate(h);                  /* Invalidate widget */
    } else if (!autoMode && (__GL(h)->flags & GUI_FLAG_LISTBOX_SLIDER_AUTO)) {
        __GL(h)->flags &= ~GUI_FLAG_LISTBOX_SLIDER_AUTO;
        guii_widget_invalidate(h);                  /* Invalidate widget */
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in,out]   h: Widget handle
 * \param[in]       visible: Slider visible status, `1` or `0`
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_setslidervisibility(gui_handle_p h, uint8_t visible, const uint8_t protect) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    if (!(__GL(h)->flags & GUI_FLAG_LISTBOX_SLIDER_AUTO)) {
        if (visible && !(__GL(h)->flags & GUI_FLAG_LISTBOX_SLIDER_ON)) {
            __GL(h)->flags |= GUI_FLAG_LISTBOX_SLIDER_ON;
            guii_widget_invalidate(h);              /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GL(h)->flags & GUI_FLAG_LISTBOX_SLIDER_ON)) {
            __GL(h)->flags &= ~GUI_FLAG_LISTBOX_SLIDER_ON;
            guii_widget_invalidate(h);              /* Invalidate widget */
            ret = 1;
        }
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return ret;
}

/**
 * \brief           Scroll list if possible
 * \param[in,out]   h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_scroll(gui_handle_p h, int16_t step, const uint8_t protect) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    start = __GL(h)->visiblestartindex;
    __GL(h)->visiblestartindex += step;
        
    check_values(h);                                /* Check widget values */
    start = start != __GL(h)->visiblestartindex;    /* Check if there was valid change */
    
    if (start) {
        guii_widget_invalidate(h);
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return GUI_U8(start);
}

/**
 * \brief           Set selected value
 * \param[in,out]   h: Widget handle
 * \param[in]       selection: Set to `-1` to invalidate selection or `0 - count-1` for specific selection 
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listbox_setselection(gui_handle_p h, int16_t selection, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    set_selection(h, selection);                    /* Set selection */
    check_values(h);                                /* Check values */
    guii_widget_invalidate(h);                      /* Invalidate widget */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Get selected value
 * \param[in,out]   h: Widget handle
 * \param[in]       protect: Set to `1` to protect core, `0` otherwise
 * \return          Selection on success, `-1` otherwise
 */
int16_t
gui_listbox_getselection(gui_handle_p h, const uint8_t protect) {
    int16_t selection;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    selection = __GL(h)->selected;                  /* Read selection */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return selection;
}

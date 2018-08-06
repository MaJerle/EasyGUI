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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "widget/gui_debugbox.h"

/**
 * \ingroup         GUI_DEBUGBOX
 * \name            GUI_DEBUGBOX_FLAGS
 * \anchor          GUI_DEBUGBOX_FLAGS
 * \{
 */

#define GUI_FLAG_DEBUGBOX_SLIDER_ON         0x01    /*!< Slider is currently active */
#define GUI_FLAG_DEBUGBOX_SLIDER_AUTO       0x02    /*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \}
 */

/**
 * \ingroup         GUI_DEBUGBOX
 * \brief           Debugbox string item object structure
 */
typedef struct {
    gui_linkedlist_t list;                          /*!< Linked list entry, must be first on list */
    gui_char* text;                                 /*!< Text entry */
} gui_debugbox_item_t;
    
/**
 * \ingroup         GUI_DEBUGBOX
 * \brief           Debugbox object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    int16_t count;                                  /*!< Current number of strings attached to this widget */
    int16_t maxcount;                               /*!< Maximal number of lines in debug window */
    int16_t visiblestartindex;                      /*!< Index in array of string on top of visible area of widget */
    
    gui_linkedlistroot_t root;                      /*!< Root of linked list entries */
    
    gui_dim_t sliderwidth;                          /*!< Slider width in units of pixels */
    uint8_t flags;                                  /*!< Widget flags */
} gui_debugbox_t;

static uint8_t gui_debugbox_callback(gui_handle_p h, gui_we_t ctrl, gui_evt_param_t* param, gui_evt_result_t* result);

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
        res = gui_widget_getheight(h) / item_height(h, NULL);
    }
    return res;
}

/* Slide up or slide down widget elements */
static void
slide(gui_handle_p h, int16_t dir) {
    gui_debugbox_t* o = GUI_VP(h);
    int16_t mPP = nr_entries_pp(h);
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->visiblestartindex + dir) < 0) {
            o->visiblestartindex = 0;
        } else {
            o->visiblestartindex += dir;
        }
        gui_widget_invalidate(h);                   /* Invalidate widget */
    } else if (dir > 0) {
        if ((o->visiblestartindex + dir) > (o->count - mPP - 1)) {  /* Slide elements down */
            o->visiblestartindex = o->count - mPP;
        } else {
            o->visiblestartindex += dir;
        }
        gui_widget_invalidate(h);                   /* Invalidate widget */
    }
}

/* Check values */
static void
check_values(gui_handle_p h) {
    gui_debugbox_t* o = GUI_VP(h);
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
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_debugbox_callback(gui_handle_p h, gui_we_t ctrl, gui_evt_param_t* param, gui_evt_result_t* result) {
    gui_debugbox_t* o = GUI_VP(h);
    
    switch (ctrl) {
        case GUI_EVT_PRE_INIT: {
            o->sliderwidth = 30;                    /* Set slider width */
            o->maxcount = 15;                       /* Number of maximal entries for debug */
            o->flags |= GUI_FLAG_DEBUGBOX_SLIDER_AUTO;  /* Set auto mode for slider */
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height;
            
            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
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
            if (h->font != NULL && gui_linkedlist_hasentries(&o->root)) {
                gui_draw_text_t f;
                gui_debugbox_item_t* item;
                uint16_t itemheight;                /* Get item height */
                uint16_t index = 0;                 /* Start index */
                gui_dim_t tmp;
                
                itemheight = item_height(h, 0);     /* Get item height and Y offset */
                
                gui_draw_text_init(&f);             /* Init structure */
                
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
                    gui_draw_writetext(disp, gui_widget_getfont(h), item->text, &f);
                    f.y += itemheight;
                }
                disp->y2 = tmp;
            }
            
            return 1;
        }
        case GUI_EVT_REMOVE: {
            gui_debugbox_item_t* item;
            while ((item = (gui_debugbox_item_t *)gui_linkedlist_remove_gen(&o->root, (gui_linkedlist_t *)gui_linkedlist_getnext_gen(&o->root, NULL))) != NULL) {
                GUI_MEMFREE(item);                  /* Free memory */
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_EVT_TOUCHMOVE: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (h->font != NULL) {
                gui_dim_t height = item_height(h, NULL);   /* Get element height */
                
                if (GUI_ABS(ts->y_diff[0]) > height) {
                    slide(h, ts->y_diff[0] > 0 ? 1 : -1);   /* Slide widget */
                }
            }
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new debugbox widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       evt_fn: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_debugbox_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, evt_fn, flags);
}

/**
 * \brief           Set color to debugbox
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_setcolor(gui_handle_p h, gui_debugbox_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Add a new string to list box
 * \param[in]       h: Widget handle
 * \param[in]       text: Pointer to text to add to list
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_addstring(gui_handle_p h, const gui_char* text) {
    gui_debugbox_t* o = GUI_VP(h);
    gui_debugbox_item_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    item = GUI_MEMALLOC(sizeof(*item) + sizeof(*text) * (gui_string_lengthtotal(text) + 1));/* Allocate memory for entry */
    if (item != NULL) {
        item->text = (void *)((char *)item + sizeof(*item));/* Add text to entry */
        gui_string_copy(item->text, text);          /* Copy text */
        gui_linkedlist_add_gen(&o->root, &item->list);  /* Add to linked list */
        o->count++;                                 /* Increase number of strings */
        
        /*
         * Use while loop in case user later changed max
         * number of entries to lower value.
         *
         * In this case we have to remove more than just one element
         */
        while (o->count > o->maxcount) {
            gui_debugbox_item_t* firstItem;
            firstItem = (gui_debugbox_item_t *)gui_linkedlist_getnext_gen(&o->root, NULL);
            if (firstItem != NULL) {
                gui_linkedlist_remove_gen(&o->root, (gui_linkedlist_t *)firstItem);
                GUI_MEMFREE(firstItem);
                o->count--;
            } else {
                break;
            }
        }
        
        o->visiblestartindex = o->count;            /* Invalidate visible start index */
        check_values(h);                            /* Check values */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Set auto mode for slider
 * \note            When auto mode enabled, slider will show only if there are more entries than widget can display
 * \param[in]       h: Widget handle
 * \param[in]       autoMode: Set to `1` for auto mode or `0` for manual mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_setsliderauto(gui_handle_p h, uint8_t autoMode) {
    gui_debugbox_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (autoMode && !(o->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        o->flags |= GUI_FLAG_DEBUGBOX_SLIDER_AUTO;
        gui_widget_invalidate(h);
    } else if (!autoMode && (o->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        o->flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_AUTO;
        gui_widget_invalidate(h);
    }

    return 1;
}

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in]       h: Widget handle
 * \param[in]       visible: Slider visible status, 1 or 0
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_setslidervisibility(gui_handle_p h, uint8_t visible) {
    uint8_t ret = 0;
    gui_debugbox_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (!(o->flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        if (visible && !(o->flags & GUI_FLAG_DEBUGBOX_SLIDER_ON)) {
            o->flags |= GUI_FLAG_DEBUGBOX_SLIDER_ON;
            gui_widget_invalidate(h);
            ret = 1;
        } else if (!visible && (o->flags & GUI_FLAG_DEBUGBOX_SLIDER_ON)) {
            o->flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_ON;
            gui_widget_invalidate(h);
            ret = 1;
        }
    }

    return ret;
}

/**
 * \brief           Scroll list if possible
 * \param[in]       h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_scroll(gui_handle_p h, int16_t step) {
    volatile int16_t start;
    gui_debugbox_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    start = o->visiblestartindex;
    o->visiblestartindex += step;

    check_values(h);                                /* Check widget values */
    start = start != o->visiblestartindex;          /* Check if there was valid change */
    
    if (start) {
        gui_widget_invalidate(h);   
    }

    return GUI_U8(start);
}

/**
 * \brief           Set maximal number of items for debugbox
 * \param[in]       h: Widget handle
 * \param[in]       max_items: Maximal number of items
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_debugbox_setmaxitems(gui_handle_p h, int16_t max_items) {
    gui_debugbox_t* o = GUI_VP(h);
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && max_items > 0);
    o->maxcount = max_items;
    return 1;
}

/**	
 * \file            gui_listview.c
 * \brief           List view widget
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
#include "widget/gui_listview.h"
    
/**
 * \ingroup         GUI_LISTVIEW
 * \name            GUI_LISTVIEW_FLAGS
 * \anchor          GUI_LISTVIEW_FLAGS
 * \{
 */

#define GUI_FLAG_LISTVIEW_SLIDER_ON     0x01/*!< Slider is currently active */
#define GUI_FLAG_LISTVIEW_SLIDER_AUTO   0x02/*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \}
 */

/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview main row item
 */
typedef struct gui_listview_row {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_linkedlistroot_t root;              /*!< Linked list root entry for \ref gui_listview_item_t column data entries */
} gui_listview_row_t;

/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview column item
 */
typedef struct gui_listview_col {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_dim_t width;                       /*!< Column width in units of pixels */
    gui_char* text;                         /*!< Header column text size */
} gui_listview_col_t;

/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview string item object
 */
typedef struct gui_listview_item {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on list */
    gui_char* text;                         /*!< Text entry */
} gui_listview_item_t;
    
/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview object structure
 */
typedef struct {
    gui_handle C;                           /*!< GUI handle object, must always be first on list */
    
    /* Use colums as pointer to array for faster data access */
    gui_listview_col_t** cols;              /*!< Pointer to pointers of column elements */
    uint16_t col_count;                     /*!< Number of columns in listview package */
    
    /* Use linked list for rows */
    gui_linkedlistroot_t root;              /*!< Linked list root entry for \ref gui_listview_row_t for rows */
    
    int16_t count;                          /*!< Current number of strings attached to this widget */
    int16_t selected;                       /*!< selected text index */
    int16_t visiblestartindex;              /*!< Index in array of string on top of visible area of widget */
    
    gui_dim_t sliderwidth;                  /*!< Slider width in units of pixels */
    uint8_t flags;                          /*!< Widget flags */
} gui_listview_t;

static uint8_t gui_listview_callback(gui_handle_p h, gui_we_t ctrl, gui_evt_param_t* param, gui_evt_result_t* result);

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
    GUI_COLOR_WIN_SEL_NOFOC_BG,
    GUI_COLOR_GRAY
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("LISTVIEW"),                        /*!< Widget name */
    .size = sizeof(gui_listview_t),                 /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_listview_callback,              /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Define number of colors */
};

/* Get item from listview entry */
static gui_listview_row_t*
get_row(gui_handle_p h, uint16_t r) {
    gui_listview_row_t* row = NULL;
    gui_listview_t* o = GUI_VP(h);
    
    if (r >= o->count) {                            /* Check if valid index */
        return 0;
    }
    
    if (r == 0) {                                   /* Check for first element */
        row = (gui_listview_row_t *)gui_linkedlist_getnext_gen(&o->root, 0);/* Get first element */
    } else if (r == o->count - 1) {
        row = (gui_listview_row_t *)gui_linkedlist_getprev_gen(&o->root, 0);/* Get last element */
    } else {
        row = (gui_listview_row_t *)gui_linkedlist_getnext_byindex_gen(&o->root, r);/* Get row by index */
    }
    return row;
}

/* Get item pointer from row pointer and column index */
static gui_listview_item_t *
get_item_for_row(gui_handle_p h, gui_listview_row_t* row, uint16_t c) {
    if (row == NULL) {                              /* Check input value if exists */
        return NULL;
    }
    
    return (gui_listview_item_t *)gui_linkedlist_getnext_byindex_gen(&row->root, c);/* Get item by index value = column number */
}

/* Get item height in LISTVIEW */
static gui_dim_t
item_height(gui_handle_p h, gui_dim_t* offset) {
    gui_dim_t size = GUI_DIM((float)h->font->size * 1.3f);
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
        gui_dim_t height = item_height(h, 0);       /* Get item height */
        res = (gui_widget_getheight(h) - height) / height;
    }
    return res;
}

/* Slide up or slide down widget elements */
static void
slide(gui_handle_p h, int16_t dir) {
    gui_listview_t* o = GUI_VP(h);
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

/* Set selection for widget */
static void
set_selection(gui_handle_p h, int16_t selected) {
    gui_listview_t* o = GUI_VP(h);
    if (o->selected != selected) {
        o->selected = selected;
        guii_widget_callback(h, GUI_EVT_SELECTIONCHANGED, NULL, NULL);
    }                         
}

/* Increase or decrease selection */
static void
inc_selection(gui_handle_p h, int16_t dir) {
    gui_listview_t* o = GUI_VP(h);
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->selected + dir) < 0) {
            set_selection(h, 0);
        } else {
            set_selection(h, o->selected + dir);
        }
        gui_widget_invalidate(h);
    } else if (dir > 0) {
        if ((o->selected + dir) > (o->count - 1)) { /* Slide elements down */
            set_selection(h, o->count - 1);
        } else {
            set_selection(h, o->selected + dir);
        }
        gui_widget_invalidate(h);
    }
}

/* Check values */
static void
check_values(gui_handle_p h) {
    gui_listview_t* o = GUI_VP(h);
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
    
    if (o->flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO) {  /* Check slider mode */
        if (o->count > mPP) {
            o->flags |= GUI_FLAG_LISTVIEW_SLIDER_ON;
        } else {
            o->flags &= ~GUI_FLAG_LISTVIEW_SLIDER_ON;
        }
    }
}

/**
 * \brief           Remove row items 
 * \note            It does not remove row itself
 * \param[in]       row: Row to remove items
 */
static void
remove_row_items(gui_listview_row_t* row) {
    gui_listview_item_t* item;
    
    while ((item = (gui_listview_item_t *)gui_linkedlist_remove_gen(&row->root, (gui_linkedlist_t *)gui_linkedlist_getnext_gen(&row->root, NULL))) != NULL) {
        GUI_MEMFREE(item);
    }
}

/**
 * \brief           Remove all rows from list view
 * \param[in]       h: Widget handle
 */
static void
remove_rows(gui_handle_p h) {
    gui_listview_t* o = GUI_VP(h);
    gui_listview_row_t* row;
    
    /* Remove first row until any available */
    while ((row = (gui_listview_row_t *)gui_linkedlist_remove_gen(&o->root, (gui_linkedlist_t *)gui_linkedlist_getnext_gen(&o->root, NULL))) != NULL) {
        remove_row_items(row);
        GUI_MEMFREE(row);
    }
    o->count = 0;
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
gui_listview_callback(gui_handle_p h, gui_we_t ctrl, gui_evt_param_t* param, gui_evt_result_t* result) {
    gui_listview_t* o = GUI_VP(h);
#if GUI_CFG_USE_TOUCH
    static gui_dim_t tx, ty;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (ctrl) {
        case GUI_EVT_PRE_INIT: {
            o->selected = -1;                       /* Invalidate selection */
            o->sliderwidth = 30;                    /* Set slider width */
            o->flags |= GUI_FLAG_LISTVIEW_SLIDER_AUTO;    /* Enable auto mode for slider */
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height, itemheight;
            uint16_t i;
            gui_listview_row_t* row;
            gui_listview_item_t* item;

            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            check_values(h);                        /* Check values if size changed */
            
            gui_draw_rectangle(disp, x, y, width, height, guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_BORDER));
            gui_draw_filledrectangle(disp, x + 1, y + 1, width - 2, height - 2, guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_BG));

            /* Draw side scrollbar */
            if (o->flags & GUI_FLAG_LISTVIEW_SLIDER_ON) {
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
            
            itemheight = item_height(h, 0);         /* Calculate item height */
            gui_draw_filledrectangle(disp, x + 2, y + 2, width - 3, itemheight, GUI_COLOR_WIN_LIGHTGRAY);
            if (o->cols != NULL && o->cols[0] != NULL) {    /* Draw all columns to screen */
                gui_dim_t tmpX2, xTmp = x + 2;                
                gui_draw_text_t f;
                
                gui_draw_text_init(&f);             /* Init structure */
                
                f.height = itemheight;
                f.align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                
                tmpX2 = disp->x2;                   /* Save current value */
                if (disp->x2 > (x + width - 1)) {   /* Set clipping region */
                    disp->x2 = x + width - 1;
                }
                
                /* Draw header row with columns */
                f.y = y + 2;
                for (i = 0; i < o->col_count; i++) {
                    gui_draw_rectangle(disp, xTmp, f.y, o->cols[i]->width, itemheight, GUI_COLOR_WIN_DARKGRAY);
                    f.width = o->cols[i]->width - 6;/* Set width */
                    f.x = xTmp + 3;                 /* Set offset */
                    gui_draw_writetext(disp, gui_widget_getfont(h), o->cols[i]->text, &f);
                    xTmp += o->cols[i]->width;      /* Increase X value */
                }
                f.y += itemheight;                  /* Go to next line */
                
                /* Draw all rows and entry elements */           
                if (h->font != NULL && gui_linkedlist_hasentries(&o->root)) { /* Is first set? */
                    uint16_t index = 0;             /* Start index */
                    gui_dim_t tmp;
                    
                    tmp = disp->y2;                 /* Scale out drawing area */
                    if (disp->y2 > (y + height - 2)) {
                        disp->y2 = y + height - 2;
                    }
                    
                    /* Try to process all strings */
                    for (index = 0, row = (gui_listview_row_t *)gui_linkedlist_getnext_gen(&o->root, NULL); row != NULL && f.y <= disp->y2;
                            row = (gui_listview_row_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)row), index++) {
                        if (index < o->visiblestartindex) { /* Check for start visible */
                            continue;
                        }               
                        if (index == o->selected) {
                            gui_draw_filledrectangle(disp, x + 2, f.y, width - 2, GUI_MIN(f.height, itemheight), guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_FOC_BG) : guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_NOFOC_BG));
                            f.color1 = guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_FOC) : guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_NOFOC);
                        } else {
                            f.color1 = guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_TEXT);
                        }
                        xTmp = x + 2;
                        for (i = 0, item = (gui_listview_item_t *)gui_linkedlist_getnext_gen(&row->root, NULL); item != NULL && i < o->col_count;
                                item = (gui_listview_item_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)item), i++) {
                            if (item->text != NULL) {   /* Draw if text set */
                                f.width = o->cols[i]->width - 6;    /* Set width */
                                f.color1width = GUI.lcd.width;  /* Use the same color for entire width */
                                f.x = xTmp + 3;     /* Set offset */
                                gui_draw_writetext(disp, gui_widget_getfont(h), item->text, &f);
                            }
                            xTmp += o->cols[i]->width;  /* Increase X value */
                        }
                        f.y += itemheight;
                    }
                    disp->y2 = tmp;                 /* Set clipping region back */
                }
                disp->x2 = tmpX2;                   /* Reset to first value */
            }
            
            return 1;
        }
        case GUI_EVT_REMOVE: {
            uint16_t i = 0;
            
            remove_rows(h);                         /* Remove all rows from widget */
            
            /* Remove all columns */
            if (o->cols != NULL) {
                for (i = 0; i < o->col_count; i++) {
                    GUI_MEMFREE(o->cols[i]);
                }
                GUI_MEMFREE(o->cols);
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            tx = ts->x_rel[0];                      /* Save X position */
            ty = ts->y_rel[0];                      /* Save Y position */
            
            /* \todo Check if press was on header are to change width of columns */

            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_EVT_TOUCHMOVE: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (h->font != NULL) {
                gui_dim_t diff;
                gui_dim_t height = item_height(h, NULL);   /* Get element height */
                
                diff = ty - ts->y_rel[0];
                if (GUI_ABS(diff) > height) {       /* Difference must be greater than 1 height entry */
                    slide(h, diff > 0 ? 1 : -1);    /* Slide widget */
                    ty = ts->y_rel[0];
                }
                
                if (ts->y_rel[0] < height) {        /* Check if we are in top region part */
                    uint16_t i;
                    gui_dim_t sum = 0;
                    
                    diff = -ts->x_diff[0];
                    for (i = 0; i < o->col_count; i++) {/* Check X position for column first */
                        sum += o->cols[i]->width;   /* Check width */
                        if (GUI_ABS(tx - sum) < 10) {
                            break;
                        }
                    }
                    if (i != o->col_count) {
                        if (o->cols[i]->width - diff >= 4) {
                            o->cols[i]->width -= diff;  /* Set new width for difference */
                        }
                        tx = ts->x_rel[0];          /* Set new start X position for relative calculation */
                        gui_widget_invalidate(h);
                    }
                }
            }
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_EVT_CLICK: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            uint8_t handled = 0;
            uint16_t itemheight = item_height(h, NULL);
            
            gui_dim_t width = gui_widget_getwidth(h);
            gui_dim_t height = gui_widget_getheight(h);
            
            if (o->flags & GUI_FLAG_LISTVIEW_SLIDER_ON) {
                if (ts->x_rel[0] > (width - o->sliderwidth)) {  /* Touch is inside slider */
                    if (ts->y_rel[0] < o->sliderwidth) {
                        slide(h, -1);               /* Slide one value up */
                    } else if (ts->y_rel[0] > (height - o->sliderwidth)) {
                        slide(h, 1);                /* Slide one value down */
                    }
                    handled = 1;
                }
            }
            if (!handled) {
                uint16_t tmpselected;
                
                if (ts->y_rel[0] > itemheight) {    /* Check item height */
                    tmpselected = (ts->y_rel[0] - itemheight) / itemheight;  /* Get temporary selected index */
                    if ((o->visiblestartindex + tmpselected) < o->count) {
                        set_selection(h, o->visiblestartindex + tmpselected);
                        gui_widget_invalidate(h);   /* Choose new selection */
                    }
                    handled = 1;
                }
            }
            
            if (!handled && ts->y_rel[0] < itemheight) {/* We are in top region */
                uint16_t i;
                gui_dim_t sum = 0;
                for (i = 0; i < o->col_count; i++) {/* Process all columns */
                    if (ts->x_rel[0] > sum && ts->x_rel[0] < (sum + o->cols[i]->width)) {
                        break;
                    }
                        
                    sum += o->cols[i]->width;       /* Increase sum value */
                }
                if (i != o->col_count) {            /* We have value */
                    //TODO: Sort rows on press
                }
                handled = 1;
            }
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_EVT_KEYPRESS: {
            guii_keyboard_data_t* kb = GUI_EVT_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_DOWN) {   /* On pressed down */
                inc_selection(h, 1);                /* Increase selection */
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->kb.keys[0] == GUI_KEY_UP) {
                inc_selection(h, -1);               /* Decrease selection */
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_EVT_INCSELECTION: {
            inc_selection(h, GUI_EVT_PARAMTYPE_I16(param));  /* Increase selection */
            GUI_EVT_RESULTTYPE_U8(result) = 1;   /* Set operation result */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new list view widget
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
gui_listview_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, evt_fn, flags);
}

/**
 * \brief           Set color to list view widget
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_setcolor(gui_handle_p h, gui_listview_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Add new column to listview
 * \param[in]       h: Widget handle
 * \param[in]       text: Text to display on top
 * \param[in]       width: Width of column in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_addcolumn(gui_handle_p h, const gui_char* text, gui_dim_t width) {
    uint8_t ret = 0;
    gui_listview_col_t* col;
    gui_listview_col_t** cols;
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    cols = GUI_MEMREALLOC(o->cols, sizeof(*o->cols) * (o->col_count + 2));    /* Allocate new memory block for new pointers of columns */
    if (cols != NULL) {
        o->cols = cols;                       /* Save new pointer of memory */
        col = GUI_MEMALLOC(sizeof(*col));           /* Allocate memory for new column structure */
        if (col != NULL) {
            o->cols[o->col_count++] = col;  /* Add column to array list */
            o->cols[o->col_count] = 0;  /* Add zero to the end of array */
            
            col->text = (gui_char *)text;
            col->width = width > 0 ? width : 100;
            ret = 1;
        }
    }

    return ret;
}

/**
 * \brief           Set column width in units of pixels
 * \param[in]       h: Widget handle
 * \param[in]       index: Index for column number. First column has index 0
 * \param[in]       width: Width of column in units of pixels
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_setcolumnwidth(gui_handle_p h, uint16_t index, gui_dim_t width) {
    uint8_t ret = 0;
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (index < o->col_count) {
        o->cols[index]->width = width > 4 ? width : 4;    /* Set new width */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        ret = 1;
    }

    return ret;
}

/**
 * \brief           Add new empty row
 * \param[in]       h: Widget handle
 * \return          Row object handle on success, `NULL` otherwise
 */
gui_listview_row_p
gui_listview_addrow(gui_handle_p h) {
    gui_listview_row_t* row;
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    row = GUI_MEMALLOC(sizeof(*row));               /* Allocate memory for new row(s) */
    if (row != NULL) {
        gui_linkedlist_add_gen(&o->root, (gui_linkedlist_t *)row);/* Add new row to linked list */
        o->count++;                           /* Increase number of rows */
        check_values(h);                            /* Check values situation */
    }
    
    return (gui_listview_row_p)row;
}

/**
 * \brief           Remove row from list view
 * \param[in]       h: Widget handle
 * \param[in]       row: Row handle to delete from listview
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_removerow(gui_handle_p h, gui_listview_row_p row) {
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && row != NULL && h->widget == &widget);
    
    gui_linkedlist_remove_gen(&o->root, (gui_linkedlist_t *)row);
    remove_row_items(row);                          /* Remove row items */
    GUI_MEMFREE(row);
    o->count--;                               /* Decrease number of elements */
    check_values(h);

    return 1;
}

/**
 * \brief           Remove all rows from list view
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_removerows(gui_handle_p h) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    
    remove_rows(h);                                 /* Remove all rows */
    check_values(h);

    return 1;
}

/**
 * \brief           Set item string for specific row and column
 * \param[in]       h: Widget handle
 * \param[in]       row: Row object handle, previously returned with \ref gui_listview_addrow function
 * \param[in]       col: Column number to set. First column is on `index = 0`
 * \param[in]       text: Text to use for item
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_setitemstring(gui_handle_p h, gui_listview_row_p row, uint16_t col, const gui_char* text) {
    uint8_t ret = 0;
    gui_listview_item_t* item = 0;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && row != NULL);

    item = (gui_listview_item_t *)gui_linkedlist_getnext_gen(&((gui_listview_row_t *)row)->root, NULL); /* Get first in linked list */
    col++;
    while (col--) {                                 /* Find right column */
        if (item == NULL) {
            item = GUI_MEMALLOC(sizeof(*item));     /* Allocate for item */
            if (item == NULL) {
                break;
            }
            gui_linkedlist_add_gen(&((gui_listview_row_t *)row)->root, (gui_linkedlist_t *)item);   /* Add element to linked list */
        }
        if (col) {
            item = (gui_listview_item_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)item);   /* Get next in linked list */
        }
    }
    if (item != NULL) {
        item->text = (gui_char *)text;              /* Set text to item */
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
gui_listview_setsliderauto(gui_handle_p h, uint8_t autoMode) {
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (autoMode && !(o->flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO)) {
        o->flags |= GUI_FLAG_LISTVIEW_SLIDER_AUTO;
        gui_widget_invalidate(h);                   /* Invalidate widget */
    } else if (!autoMode && (o->flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO)) {
        o->flags &= ~GUI_FLAG_LISTVIEW_SLIDER_AUTO;
        gui_widget_invalidate(h);                   /* Invalidate widget */
    }
    
    return 1;
}

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in]       h: Widget handle
 * \param[in]       visible: Slider visible status, `1` or `0`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_setslidervisibility(gui_handle_p h, uint8_t visible) {
    uint8_t ret = 0;
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (!(o->flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO)) {
        if (visible && !(o->flags & GUI_FLAG_LISTVIEW_SLIDER_ON)) {
            o->flags |= GUI_FLAG_LISTVIEW_SLIDER_ON;
            gui_widget_invalidate(h);               /* Invalidate widget */
            ret = 1;
        } else if (!visible && (o->flags & GUI_FLAG_LISTVIEW_SLIDER_ON)) {
            o->flags &= ~GUI_FLAG_LISTVIEW_SLIDER_ON;
            gui_widget_invalidate(h);               /* Invalidate widget */
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
gui_listview_scroll(gui_handle_p h, int16_t step) {
    volatile int16_t start;
    gui_listview_t* o = GUI_VP(h);
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    start = o->visiblestartindex;
    o->visiblestartindex += step;
        
    check_values(h);                                /* Check widget values */
    start = start != o->visiblestartindex;    /* Check if there was valid change */

    if (start) {
        gui_widget_invalidate(h);                   /* Invalidate widget */
    }

    return (uint8_t)start;
}

/**
 * \brief           Set selected row number
 * \param[in]       h: Widget handle
 * \param[in]       selection: Set to `-1` to invalidate selection or `0 - count-1` for specific selection 
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_setselection(gui_handle_p h, int16_t selection) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    set_selection(h, selection);                    /* Set selection */
    check_values(h);                                /* Check values */
    gui_widget_invalidate(h);                       /* Invalidate widget */

    return 1;
}

/**
 * \brief           Get selected row number
 * \param[in]       h: Widget handle
 * \return          Selection number on success, -1 otherwise
 */
int16_t
gui_listview_getselection(gui_handle_p h) {
    gui_listview_t* o = GUI_VP(h);
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->selected;
}

/**
 * \brief           Get item text value from row index and column index
 * \param[in]       h: Widget handle
 * \param[in]       rindex: Row index
 * \param[in]       cindex: Column index
 * \param[in]       dst: Pointer to \ref gui_char variable to save text to it
 * \param[in]       length: Length of destination array
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_getitemvalue(gui_handle_p h, uint16_t rindex, uint16_t cindex, gui_char* dst, size_t length) {
    int16_t ret = 0;
    gui_listview_row_t* row;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && dst != NULL && length > 1);
    
    *dst = 0;
    row = (gui_listview_row_t *)get_row(h, rindex); /* Get row pointer */
    if (row != NULL) {
        gui_listview_item_t* item = get_item_for_row(h, row, cindex);   /* Get item from column */
        if (item != NULL) {                         /* In case of valid index */
            gui_string_copyn(dst, item->text, length - 1);  /* Copy text to destination */
            ret = 1;
        }
    }

    return GUI_U8(ret);
}

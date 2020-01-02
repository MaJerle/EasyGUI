/**	
 * \file            gui_listview.c
 * \brief           List view widget
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
#include "widget/gui_listview.h"
#include "widget/gui_widget_list.h"
    
/**
 * \ingroup         GUI_LISTVIEW
 * \name            GUI_LISTVIEW_FLAGS
 * \anchor          GUI_LISTVIEW_FLAGS
 * \{
 */

#define GUI_FLAG_LISTVIEW_SLIDER_ON         0x01    /*!< Slider is currently active */
#define GUI_FLAG_LISTVIEW_SLIDER_AUTO       0x02    /*!< Show right slider automatically when required, otherwise, manual mode is used */

/**
 * \}
 */

/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview main row item
 */
typedef struct gui_listview_row {
    gui_linkedlist_t list;                          /*!< Linked list entry, must be first on list */
    gui_linkedlistroot_t root;                      /*!< Linked list root entry for \ref gui_listview_item_t column data entries */
} gui_listview_row_t;

/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview column item
 */
typedef struct gui_listview_col {
    gui_linkedlist_t list;                          /*!< Linked list entry, must be first on list */
    gui_dim_t width;                                /*!< Column width in units of pixels */
    gui_char* text;                                 /*!< Header column text size */
} gui_listview_col_t;

/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview string item object
 */
typedef struct gui_listview_item {
    gui_linkedlist_t list;                          /*!< Linked list entry, must always be first on list */
    gui_char* text;                                 /*!< Text entry */
} gui_listview_item_t;
    
/**
 * \ingroup         GUI_LISTVIEW
 * \brief           Listview object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    /* Use colums as pointer to array for faster data access */
    gui_listview_col_t** cols;                      /*!< Pointer to pointers of column elements */
    uint16_t col_count;                             /*!< Number of columns in listview package */

    gui_widget_listdata_t ld;                       /*!< List data handle */
    int16_t selected;                               /*!< Selected text index */
    
    gui_dim_t sliderwidth;                          /*!< Slider width in units of pixels */
    uint8_t flags;                                  /*!< Widget flags */
} gui_listview_t;

static uint8_t gui_listview_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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

/**
 * \brief           Get column value for specific row
 * \param[in]       h: Widget handle
 * \param[in]       row: Row handle
 * \param[in]       c: Column index for row
 * \return          Item handle on success, `NULL` otherwise
 */
static gui_listview_item_t *
get_item_for_row(gui_handle_p h, gui_listview_row_t* row, uint16_t c) {
    gui_listview_t* o = GUI_VP(h);
    
    if (row == NULL || c >= o->col_count) {
        return NULL;
    }
    
    /* Get item by index value = column number */
    return (void *)gui_linkedlist_getnext_byindex_gen(&row->root, c);
}

/**
 * \brief           Get size of entry in units of pixels
 * \param[in]       h: Widget handle
 * \param[out]      offset: Offset in units of pixels for text Y position in single item
 * \return          Item height in units of pixels
 */
static gui_dim_t
item_height(gui_handle_p h, uint16_t* offset) {
    gui_dim_t size = 0;
    if (h->font != NULL) {
        size = GUI_DIM((float)h->font->size * 1.3f);
    }
    if (offset != NULL) {                           /* Calculate top offset */
        *offset = (size - h->font->size) >> 1;
    }
    return size;                                    /* Return height for element */
}

/**
 * \brief           Maximal number of entries per page callback
 * \param[in]       h: Widget handle
 * \return          Number of entries per page
 */
static int16_t
nr_entries_pp(gui_handle_p h) {
    int16_t res = 0;
    if (h->font != NULL) {                          /* Font is responsible for this setup */
        gui_dim_t height = item_height(h, 0);       /* Get item height */
        res = (gui_widget_getheight(h) - height) / height;
    }
    return res;
}

/**
 * \brief           Check new values callback
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
check_values(gui_handle_p h) {
    gui_listview_t* o = GUI_VP(h);
    
    /* Check slider mode */
    if (o->flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO) {
        if (gui_widget_list_get_count(h, &o->ld) > gui_widget_list_get_count_pp(h, &o->ld)) {
            o->flags |= GUI_FLAG_LISTVIEW_SLIDER_ON;
        } else {
            o->flags &= ~GUI_FLAG_LISTVIEW_SLIDER_ON;
        }
    }
    return 1;
}

/**
 * \brief           Remove all rows from list view
 * \param[in]       h: Widget handle
 * \param[in]       item: Item to remove from memory
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
remove_row(gui_handle_p h, void* item) {
    gui_listview_row_t* row = item;
    gui_listview_item_t* col;
    
    /* Remove row items (columns) first */
    while ((col = (gui_listview_item_t *)gui_linkedlist_remove_gen(&row->root, (gui_linkedlist_t *)gui_linkedlist_getnext_gen(&row->root, NULL))) != NULL) {
        GUI_MEMFREE(col);
    }
    GUI_MEMFREE(item);                              /* Remove item itself */
    return 1;
}

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       evt: Event type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_listview_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_listview_t* o = GUI_VP(h);
#if GUI_CFG_USE_TOUCH
    static gui_dim_t ty;
    static gui_listview_col_t* column_slide;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            o->selected = -1;                       /* Invalidate selection */
            o->sliderwidth = 30;                    /* Set slider width */
            o->flags |= GUI_FLAG_LISTVIEW_SLIDER_AUTO;    /* Enable auto mode for slider */

            gui_widget_list_init(h, &o->ld);
            o->ld.check_values_cb = check_values;
            o->ld.entries_per_page_cb = nr_entries_pp;
            o->ld.remove_item_cb = remove_row;
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height, itemheight;
            uint16_t i;
            gui_listview_row_t* item;
            gui_listview_item_t* col_item;

            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            gui_widget_list_check_values(h, &o->ld);    /* Check values if size changed */
            
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
                sb.entriestop = gui_widget_list_get_visible_start_index(h, &o->ld);
                sb.entriestotal = gui_widget_list_get_count(h, &o->ld);
                sb.entriesvisible = gui_widget_list_get_count_pp(h, &o->ld);
                
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
                if (h->font != NULL && gui_widget_list_get_count(h, &o->ld)) {/* Is first set? */
                    int16_t index;
                    gui_dim_t tmp;
                    
                    tmp = disp->y2;                 /* Scale out drawing area */
                    if (disp->y2 > (y + height - 2)) {
                        disp->y2 = y + height - 2;
                    }
                    
                    /* Draw list items */
                    for (item = gui_widget_list_get_first_visible_item(h, &o->ld, &index); item != NULL && f.y <= disp->y2;
                            item = gui_widget_list_get_next_item(h, &o->ld, item), index++) {

                        if (index == o->selected) {
                            gui_draw_filledrectangle(disp, x + 2, f.y, width - 2, GUI_MIN(f.height, itemheight), guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_FOC_BG) : guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_NOFOC_BG));
                            f.color1 = guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_FOC) : guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_SEL_NOFOC);
                        } else {
                            f.color1 = guii_widget_getcolor(h, GUI_LISTVIEW_COLOR_TEXT);
                        }
                        xTmp = x + 2;
                        for (i = 0, col_item = (gui_listview_item_t *)gui_linkedlist_getnext_gen(&item->root, NULL); col_item != NULL && i < o->col_count;
                                col_item = (gui_listview_item_t *)gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)col_item), i++) {
                            if (col_item->text != NULL) {   /* Draw if text set */
                                f.width = o->cols[i]->width - 6;
                                f.color1width = GUI.lcd.width;  /* Use the same color for entire width */
                                f.x = xTmp + 3;     /* Set offset */
                                gui_draw_writetext(disp, gui_widget_getfont(h), col_item->text, &f);
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
            /* Remove all rows */
            gui_widget_list_remove_items(h, &o->ld);

            /* Remove all columns */
            if (o->cols != NULL) {
                uint16_t i = 0;
                for (i = 0; i < o->col_count; i++) {
                    GUI_MEMFREE(o->cols[i]);
                }
                GUI_MEMFREE(o->cols);
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            size_t i;
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            gui_dim_t height = item_height(h, NULL);    /* Get element height */
            
            ty = ts->y_rel[0];                      /* Save Y position */
            
            /* Check column slide */
            column_slide = NULL;
            if (ts->y_rel[0] < height) {            /* Try to find column to slide */
                gui_dim_t sum = 0;
                for (i = 0; i < o->col_count; i++) {
                    sum += o->cols[i]->width;
                    if (GUI_ABS(ts->x_rel[0] - sum) < 10) {
                        column_slide = o->cols[i];
                        break;
                    }
                }
            }

            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_EVT_TOUCHMOVE: {
            size_t i;
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            gui_dim_t height = item_height(h, NULL);   /* Get element height */
            
            /* Horizontal slide */
            if (column_slide != NULL) {
                /* Check if column slide exists in this widget! */
                for (i = 0; i < o->col_count; i++) {
                    if (o->cols[i] == column_slide) {
                        break;
                    }
                }
                /* Check matches */
                if (i == o->col_count) {
                    column_slide = NULL;
                } else {
                    column_slide->width += ts->x_diff[0];
                    if (column_slide->width < 0) {
                        column_slide->width = 0;
                    }
                    gui_widget_invalidate(h);
                }
            }
            
            /* Possible vertical slide? */
            if (column_slide == NULL) {
                /*
                 * Check slide vertical difference
                 * and save current relative value in case of slide event
                 */
                if (GUI_ABS(ty - ts->y_rel[0]) > height) {
                    gui_widget_list_slide(h, &o->ld, (ty - ts->y_rel[0]) > 0 ? 1 : -1);
                    ty = ts->y_rel[0];
                }
            }
            return 1;
        }
        case GUI_EVT_TOUCHEND: {
            column_slide = NULL;
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
                        gui_widget_list_slide(h, &o->ld, -1);   /* Slide one value up */
                    } else if (ts->y_rel[0] > (height - o->sliderwidth)) {
                        gui_widget_list_slide(h, &o->ld, 1);    /* Slide one value down */
                    }
                    handled = 1;
                }
            }
            if (!handled) {
                int16_t tmpselected;
                
                if (ts->y_rel[0] > itemheight) {    /* Check item height */
                    tmpselected = (ts->y_rel[0] - itemheight) / itemheight;  /* Get temporary selected index */
                    if ((gui_widget_list_get_visible_start_index(h, &o->ld) + tmpselected) <= gui_widget_list_get_count(h, &o->ld)) {
                        gui_widget_list_set_selection(h, &o->ld, &o->selected, gui_widget_list_get_visible_start_index(h, &o->ld) + tmpselected);
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
            guii_keyboard_data_t* kb = GUI_EVT_PARAMTYPE_KEYBOARD(param);   /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_DOWN) {
                gui_widget_list_inc_selection(h, &o->ld, &o->selected, 1);  /* Increase selection */
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->kb.keys[0] == GUI_KEY_UP) {
                gui_widget_list_inc_selection(h, &o->ld, &o->selected, -1); /* Decrease selection */
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_EVT_INCSELECTION: {
            gui_widget_list_inc_selection(h, &o->ld, &o->selected, GUI_EVT_PARAMTYPE_I16(param));
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
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    cols = GUI_MEMREALLOC(o->cols, sizeof(*o->cols) * (o->col_count + 2));    /* Allocate new memory block for new pointers of columns */
    if (cols != NULL) {
        o->cols = cols;                       /* Save new pointer of memory */
        col = GUI_MEMALLOC(sizeof(*col));       /* Allocate memory for new column structure */
        if (col != NULL) {
            o->cols[o->col_count++] = col;  /* Add column to array list */
            o->cols[o->col_count] = NULL;   /* Add zero to the end of array */
            
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
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (index < o->col_count) {
        o->cols[index]->width = width > 4 ? width : 4;
        gui_widget_invalidate(h);
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
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    row = GUI_MEMALLOC(sizeof(*row));               /* Allocate memory for new row(s) */
    if (row != NULL) {
        gui_widget_list_add_item(h, &o->ld, row);
    }
    
    return (gui_listview_row_p)row;
}

/**
 * \brief           Remove row from list view
 * \param[in]       h: Widget handle
 * \param[in]       index: Row handle to delete from listview
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_removerow(gui_handle_p h, int16_t index) {
    gui_listview_t* o = GUI_VP(h);
    
    GUI_ASSERTPARAMS(h != NULL && index >= 0 && h->widget == &widget);
    
    return gui_widget_list_remove_item_byindex(h, &o->ld, index);
}

/**
 * \brief           Remove all rows from list view
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_removerows(gui_handle_p h) {
    gui_listview_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    return gui_widget_list_remove_items(h, &o->ld);
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
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && row != NULL);

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
 * \brief           Scroll list if possible
 * \param[in]       h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_scroll(gui_handle_p h, int16_t step) {
    gui_listview_t* o = GUI_VP(h);
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    return gui_widget_list_slide(h, &o->ld, step);
}

/**
 * \brief           Set selected row number
 * \param[in]       h: Widget handle
 * \param[in]       selection: Set to `-1` to invalidate selection or `0 - count-1` for specific selection 
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listview_setselection(gui_handle_p h, int16_t selection) {
    gui_listview_t* o = GUI_VP(h);

    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    return gui_widget_list_set_selection(h, &o->ld, &o->selected, selection);
}

/**
 * \brief           Get selected row number
 * \param[in]       h: Widget handle
 * \return          Selection number on success, -1 otherwise
 */
int16_t
gui_listview_getselection(gui_handle_p h) {
    gui_listview_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
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
gui_listview_getitemvalue(gui_handle_p h, int16_t rindex, uint16_t cindex, gui_char* dst, size_t length) {
    gui_listview_t* o = GUI_VP(h);
    int16_t ret = 0;
    gui_listview_row_t* row;
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && dst != NULL && length > 1);
    
    *dst = 0;
    row = gui_widget_list_get_item_byindex(h, &o->ld, rindex);
    if (row != NULL) {
        gui_listview_item_t* item = get_item_for_row(h, row, cindex);   /* Get item from column */
        if (item != NULL) {                         /* In case of valid index */
            gui_string_copyn(dst, item->text, length - 1);  /* Copy text to destination */
            ret = 1;
        }
    }

    return GUI_U8(ret);
}

/**	
 * \file            gui_dropdown.c
 * \brief           Dropdown widget
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
#include "widget/gui_dropdown.h"
#include "widget/gui_widget_list.h"

/**
 * \ingroup         GUI_DROPDOWN
 * \name            GUI_DROPDOWN_FLAGS Flags
 * \anchor          GUI_DROPDOWN_FLAGS
 * \{
 */

#define GUI_FLAG_DROPDOWN_OPENED            0x01    /*!< Dropdown is opened */
#define GUI_FLAG_DROPDOWN_SLIDER_ON         0x02    /*!< Slider is currently active */
#define GUI_FLAG_DROPDOWN_SLIDER_AUTO       0x04    /*!< Show right slider automatically when required, otherwise, manual mode is used */
#define GUI_FLAG_DROPDOWN_OPEN_UP           0x08    /*!< Open dropdown in top direction instead of bottom */

/**
 * \}
 */

/**
 * \ingroup         GUI_DROPDOWN
 * \brief           Dropdown string item object structure
 */
typedef struct {
    gui_linkedlist_t list;                          /*!< Linked list entry, must always be first on list */
    gui_char* text;                                 /*!< Text entry */
} gui_dropdown_item_t;
    
/**
 * \ingroup         GUI_DROPDOWN
 * \brief           Dropdown object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    gui_widget_listdata_t ld;                       /*!< List data handle */
    int16_t selected;                               /*!< Selected text index */
    
    gui_dim_t sliderwidth;                          /*!< Slider width in units of pixels */
    uint8_t flags;                                  /*!< Widget flags \ref GUI_DROPDOWN_FLAGS */
} gui_dropdown_t;

static uint8_t gui_dropdown_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

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
    .name = _GT("DROPDOWN"),                        /*!< Widget name */
    .size = sizeof(gui_dropdown_t),                 /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_dropdown_callback,              /*!< Callback function */
    .colors = colors,
    .color_count = GUI_COUNT_OF(colors),            /*!< Define number of colors */
};

/* Check if status is opened */
#define is_opened(o)        (((gui_dropdown_t *)(o))->flags & GUI_FLAG_DROPDOWN_OPENED)

/* Check if open direction is up */
#define is_dir_up(o)        (((gui_dropdown_t *)(o))->flags & GUI_FLAG_DROPDOWN_OPEN_UP)

/* height increase when opened */
#define HEIGHT_CONST(h)     4

/* Get Y position for main and expandend mode and height for both */
static void
get_opened_positions(gui_handle_p h, gui_dim_t* y, gui_dim_t* height, gui_dim_t* y1, gui_dim_t* height1) {
    *height1 = *height / HEIGHT_CONST(h);           /* height of main part */
    if (is_dir_up(h)) {
        *y1 = *y + *height - *height1;              /* Position of opened part, add difference in height values */
    } else {
        *y1 = *y;                                   /* Position of main part */
        *y += *height1;                             /* Position of opened part */
    }
    *height -= *height1;                            /* Height of opened part */
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
    if (h->font != NULL) {
        gui_dim_t height = gui_widget_getheight(h);
        if (!is_opened(h)) {
            height *= HEIGHT_CONST(h) - 1;          /* Get height of opened area part */
        } else {
            height -= height / HEIGHT_CONST(h);     /* Subtract height for default size */
        }
        res = height / item_height(h, NULL);        /* Calculate entries */
    }
    return res;
}

/**
 * \brief           Open/Close widget state
 * \param[in]       h: Widget handle
 * \param[in]       state: State to open or close
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
open_close(gui_handle_p h, uint8_t state) {
    gui_dropdown_t* o = GUI_VP(h);
    if (state && !is_opened(h)) {
        o->flags |= GUI_FLAG_DROPDOWN_OPENED;
        if (is_dir_up(h)) {                         /* Width is opening to up */
            uint8_t is_percent;

            /* In this case, we have to move Y position up */
            gui_widget_setypositionoriginal(h,
                gui_widget_getypositionoriginal(h, &is_percent) - (HEIGHT_CONST(h) - 1) * gui_widget_getheightex(h, is_percent)
            );
        }

        /* Set new height */
        gui_widget_setheightoriginal(h, HEIGHT_CONST(h) * gui_widget_getheightoriginal(h, NULL));
        return 1;
    } else if (!state && is_opened(h)) {
        o->flags &= ~GUI_FLAG_DROPDOWN_OPENED;

        /* Set height back to "normal" */
        gui_widget_setheightoriginal(h, gui_widget_getheightoriginal(h, NULL) / HEIGHT_CONST(h));

        if (is_dir_up(h)) {                         /* When open up */
            uint8_t is_percent;

            /* In this case, we have to move Y position down */
            gui_widget_setypositionoriginal(h,
                gui_widget_getypositionoriginal(h, &is_percent) + (HEIGHT_CONST(h) - 1) * gui_widget_getheightex(h, is_percent)
            );
        }

        /* Set visible display when opened */
        gui_widget_list_set_visible_start_index(h, &o->ld, o->selected);
        return 1;
    }
    return 0;
}

/**
 * \brief           Check new values callback
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
check_values(gui_handle_p h) {
    gui_dropdown_t* o = GUI_VP(h);
    
    if (o->flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO) { /* Check slider mode */
        if (gui_widget_list_get_count(h, &o->ld) > gui_widget_list_get_count_pp(h, &o->ld)) {
            o->flags |= GUI_FLAG_DROPDOWN_SLIDER_ON;
        } else {
            o->flags &= ~GUI_FLAG_DROPDOWN_SLIDER_ON;
        }
    }
    return 1;
}

/**
 * \brief           Delete item from widget and check values
 * \param[in]       h: Widget handle
 * \param[in]       index: Index to remove
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
delete_item(gui_handle_p h, int16_t index) {
    gui_dropdown_t* o = GUI_VP(h);
    
    /* Try to remove item */
    if (gui_widget_list_remove_item_byindex(h, &o->ld, index)) {
        if (o->selected == index) {
            gui_widget_list_set_selection(h, &o->ld, &o->selected, -1);
        }
        gui_widget_list_check_values(h, &o->ld);
        gui_widget_invalidate(h);
    }
    return 1;
}

/**
 * \brief           Remove memory for item callback
 * \param[in]       h: Widget handle
 * \param[in]       item: Item handle
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
remove_item_memory(gui_handle_p h, void* item) {
    GUI_MEMFREE(item);
    return 1;
}

/**
 * \brief           Process click event and decide to open/close widget or choose selection
 * \param[in]       h: Widget handle
 * \param[in]       ts: Touch data
 */
static void
process_click(gui_handle_p h, guii_touch_data_t* ts) {
    gui_dropdown_t* o = GUI_VP(h);
    gui_dim_t y, y1, height, height1;
    
    /* Click on closed widget may only open it */
    if (!is_opened(h)) {
        open_close(h, 1);
        return;
    }
    
    y = 0;                                          /* Relative Y position for touch events */
    height = gui_widget_getheight(h);        
    get_opened_positions(h, &y, &height, &y1, &height1);    /* Calculate values */
    
    /* Check if press was on normal area when widget is closed */
    if (ts->y_rel[0] >= y1 && ts->y_rel[0] <= (y1 + height1)) {   /* Check first part */
        
    } else {
        uint16_t tmpselected;
        
        if (is_dir_up(h)) {
            tmpselected = ts->y_rel[0] / item_height(h, NULL);  /* Get temporary selected index */
        } else {
            tmpselected = (ts->y_rel[0] - height1) / item_height(h, NULL);  /* Get temporary selected index */
        }
        if ((gui_widget_list_get_visible_start_index(h, &o->ld) + tmpselected) < gui_widget_list_get_count(h, &o->ld)) {
            gui_widget_list_set_selection(h, &o->ld, &o->selected, gui_widget_list_get_visible_start_index(h, &o->ld) + tmpselected);
            gui_widget_invalidate(h);
        }
        gui_widget_list_check_values(h, &o->ld);    /* Check values */
    }
    open_close(h, 0);                               /* Close widget on click */
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
gui_dropdown_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_dropdown_t* o = GUI_VP(h);
#if GUI_CFG_USE_TOUCH
    static gui_dim_t ty;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            o->selected = -1;                 /* Invalidate selection */
            o->sliderwidth = 30;              /* Set slider width */
            o->flags |= GUI_FLAG_DROPDOWN_SLIDER_AUTO;    /* Enable slider auto mode */
            
            gui_widget_list_init(h, &o->ld);
            o->ld.check_values_cb = check_values;
            o->ld.entries_per_page_cb = nr_entries_pp;
            o->ld.remove_item_cb = remove_item_memory;
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height;
            gui_dim_t y1, height1;                  /* Position of main widget part, when widget is opened */
            
            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            if (is_opened(h)) {
                get_opened_positions(h, &y, &height, &y1, &height1);
                
                gui_draw_rectangle3d(disp, x, y1, width, height1, GUI_DRAW_3D_State_Lowered);
                gui_draw_filledrectangle(disp, x + 2, y1 + 2, width - 4, height1 - 4, guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_BG));
                
                gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_BG));
                gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            } else {
                y1 = y;
                height1 = height;
                
                gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_BG));
            }
                
            if (o->selected >= 0 && h->font != NULL) {
                gui_draw_text_t f;
                gui_dropdown_item_t* item;
                gui_draw_text_init(&f);             /* Init structure */
                
                item = gui_widget_list_get_item_byindex(h, &o->ld, o->selected);
                if (item != NULL) {
                    f.x = x + 3;
                    f.y = y1 + 3;
                    f.width = width - 6;
                    f.height = height1 - 6;
                    f.align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                    f.color1width = f.width;
                    f.color1 = guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_TEXT);
                    gui_draw_writetext(disp, gui_widget_getfont(h), item->text, &f);
                }
            }
            
            if (is_opened(h) && o->flags & GUI_FLAG_DROPDOWN_SLIDER_ON) {
                gui_draw_sb_t sb;
                
                width -= o->sliderwidth;            /* Available width is decreased */
                
                gui_draw_scrollbar_init(&sb);
                
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
                width--;                            /* Go down for one for alignment on non-slider */
            }
            
            if (is_opened(h) && h->font != NULL && gui_widget_list_get_count(h, &o->ld)) {
                gui_draw_text_t f;
                gui_dropdown_item_t* item;
                uint16_t yOffset, itemheight;
                int16_t index = 0;
                gui_dim_t tmp;
                
                itemheight = item_height(h, &yOffset); /* Get item height and Y offset */
                
                gui_draw_text_init(&f);             /* Init structure */
                
                f.x = x + 4;
                f.y = y + 2;
                f.width = width - 6;
                f.height = itemheight;
                f.align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                
                tmp = disp->y2;
                if (disp->y2 > (y + height)) {      /* Set cut-off Y position for drawing operations */
                    disp->y2 = y + height;
                }
                
                /* Draw list items */
                for (item = gui_widget_list_get_first_visible_item(h, &o->ld, &index); item != NULL && f.y <= disp->y2;
                        item = gui_widget_list_get_next_item(h, &o->ld, item), index++) {

                    if (index == o->selected) {
                        gui_draw_filledrectangle(disp, x + 2, f.y, width - 3, GUI_MIN(f.height, itemheight), guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_SEL_FOC_BG) : guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_SEL_NOFOC_BG));
                        f.color1 = guii_widget_isfocused(h) ? guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_SEL_FOC) : guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_SEL_NOFOC);
                    } else {
                        f.color1 = guii_widget_getcolor(h, GUI_DROPDOWN_COLOR_TEXT);
                    }
                    gui_draw_writetext(disp, gui_widget_getfont(h), item->text, &f);
                    f.y += itemheight;
                }
                disp->y2 = tmp;                     /* Set temporary value back */
            }
            return 1;
        }
        case GUI_EVT_REMOVE: {
            gui_widget_list_remove_items(h, &o->ld);
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            ty = ts->y_rel[0];                      /* Save relative Y position */
            
            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_EVT_TOUCHMOVE: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (h->font != NULL) {
                gui_dim_t height = item_height(h, NULL);
                
                if (GUI_ABS(ty - ts->y_rel[0]) > height) {
                    gui_widget_list_slide(h, &o->ld, (ty - ts->y_rel[0]) > 0 ? 1 : -1);
                    ty = ts->y_rel[0];
                }
            }
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_EVT_CLICK: {
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (!is_opened(h)) {                    /* Check widget status */
                open_close(h, 1);                   /* Open widget */
            } else {                                /* Widget opened, process data */
                process_click(h, ts);               /* Process click event */
            }
            return 1;
        }
        case GUI_EVT_FOCUSOUT: {
            open_close(h, 0);                       /* Close widget */
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_EVT_KEYPRESS: {
            guii_keyboard_data_t* kb = GUI_EVT_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->kb.keys[0] == GUI_KEY_DOWN) {   /* On pressed down */
                gui_widget_list_set_selection(h, &o->ld, &o->selected, 1);
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->kb.keys[0] == GUI_KEY_UP) {
                gui_widget_list_set_selection(h, &o->ld, &o->selected, -1);
                GUI_EVT_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_CFG_USE_KEYBOARD */
        case GUI_EVT_INCSELECTION: {
            gui_widget_list_set_selection(h, &o->ld, &o->selected, GUI_EVT_PARAMTYPE_I16(param));
            GUI_EVT_RESULTTYPE_U8(result) = 1;      /* Set operation result */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

/**
 * \brief           Create new dropdown widget
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
gui_dropdown_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, evt_fn, flags);
}

/**
 * \brief           Set color to DROPDOWN
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_setcolor(gui_handle_p h, gui_dropdown_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Add a new string to list box
 * \param[in]       h: Widget handle
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_addstring(gui_handle_p h, const gui_char* text) {
    gui_dropdown_t* o = GUI_VP(h);
    gui_dropdown_item_t* item;
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    item = GUI_MEMALLOC(sizeof(*item));             /* Allocate memory for entry */
    if (item != NULL) {
        item->text = (gui_char *)text;
        gui_widget_list_add_item(h, &o->ld, item);  /* Add to linkedlist */
        
        gui_widget_invalidate(h);                   /* Invalidate widget */
         
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Set opening direction for dropdown list
 * \param[in]       h: Widget handle
 * \param[in]       dir: Opening direction
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_setopendirection(gui_handle_p h, gui_dropdown_opendir_t dir) {
    gui_dropdown_t* o = GUI_VP(h);
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    
    /* Must be closed */
    if (!is_opened(h)) {
        if ((o->flags & GUI_FLAG_DROPDOWN_OPEN_UP) && dir == GUI_DROPDOWN_OPENDIR_DOWN) {
            o->flags &= GUI_FLAG_DROPDOWN_OPEN_UP;
            ret = 1;
        } else if (!(o->flags & GUI_FLAG_DROPDOWN_OPEN_UP) && dir == GUI_DROPDOWN_OPENDIR_UP) {
            o->flags |= GUI_FLAG_DROPDOWN_OPEN_UP;
            ret = 1;
        }
    }

    return ret;
}

/**
 * \brief           Set string value to already added string index
 * \param[in]       h: Widget handle
 * \param[in]       index: Index (position) on list to set/change text
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_setstring(gui_handle_p h, uint16_t index, const gui_char* text) {
    gui_dropdown_t* o = GUI_VP(h);
    gui_dropdown_item_t* item;
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    item = gui_widget_list_get_item_byindex(h, &o->ld, index);  /* Get list item from handle */
    if (item != NULL) {
        item->text = (gui_char *)text;              /* Set new text */
        gui_widget_invalidate(h);                   /* Invalidate widget */
    }

    return ret;
}

/**
 * \brief           Delete first string from list
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_deletefirststring(gui_handle_p h) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return delete_item(h, 0);
}

/**
 * \brief           Delete last string from list
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_deletelaststring(gui_handle_p h) {
    gui_dropdown_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return delete_item(h, gui_widget_list_get_count(h, &o->ld) - 1);
}

/**
 * \brief           Delete specific entry from list
 * \param[in]       h: Widget handle
 * \param[in]       index: List index (position) to delete
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_deletestring(gui_handle_p h, uint16_t index) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return delete_item(h, (int16_t)index);
}

/**
 * \brief           Scroll list if possible
 * \param[in]       h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_scroll(gui_handle_p h, int16_t step) {
    gui_dropdown_t* o = GUI_VP(h);
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    return gui_widget_list_slide(h, &o->ld, step);
}

/**
 * \brief           Set selected value
 * \param[in]       h: Widget handle
 * \param[in]       selection: Set to `-1` to invalidate selection or `0 - count-1` for specific selection 
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_dropdown_setselection(gui_handle_p h, int16_t selection) {
    gui_dropdown_t* o = GUI_VP(h);
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    gui_widget_list_set_selection(h, &o->ld, &o->selected, selection);
    gui_widget_list_check_values(h, &o->ld);
    gui_widget_invalidate(h);

    return 1;
}

/**
 * \brief           Get selected value
 * \param[in]       h: Widget handle
 * \return          Selection number or -1 if no selection
 */
int16_t
gui_dropdown_getselection(gui_handle_p h) {
    gui_dropdown_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return o->selected;                       /* Read selection */
}

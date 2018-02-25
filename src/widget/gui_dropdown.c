/**	
 * \file            gui_dropdown.c
 * \brief           Dropdown widget
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
#include "widget/gui_dropdown.h"

#define __GD(x)             ((GUI_DROPDOWN_t *)(x))

static uint8_t gui_dropdown_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
GUI_Color_t Colors[] = {
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
GUI_WIDGET_t Widget = {
    .Name = _GT("DROPDOWN"),                        /*!< Widget name */
    .Size = sizeof(GUI_DROPDOWN_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_dropdown_callback,              /*!< Callback function */
    .Colors = Colors,
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

#define o                   ((GUI_DROPDOWN_t *)(h))

/* Check if status is opened */
#define is_opened(h)        (__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPENED)

/* Check if open direction is up */
#define is_dir_up(h)        (__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPEN_UP)

/* Height increase when opened */
#define HEIGHT_CONST(h)    4

/* Get Y position for main and expandend mode and height for both */
static void
get_opened_positions(GUI_HANDLE_p h, GUI_iDim_t* y, GUI_iDim_t* height, GUI_iDim_t* y1, GUI_iDim_t* height1) {
    *height1 = *height / HEIGHT_CONST(h);           /* Height of main part */
    if (is_dir_up(h)) {
        *y1 = *y + *height - *height1;              /* Position of opened part, add difference in height values */
    } else {
        *y1 = *y;                                   /* Position of main part */
        *y += *height1;                             /* Position of opened part */
    }
    *height -= *height1;                            /* Height of opened part */
}

/* Get item height in dropdown list */
static uint16_t
item_height(GUI_HANDLE_p h, uint16_t* offset) {
    uint16_t size = (float)__GH(h)->Font->Size * 1.3f;
    if (offset) {                                   /* Calculate top offset */
        *offset = (size - __GH(h)->Font->Size) >> 1;
    }
    return size;                                    /* Return height for element */
}

/* Get number of entries maximal on one page */
static int16_t
nr_entries_pp(GUI_HANDLE_p h) {
    int16_t res = 0;
    if (__GH(h)->Font) {                            /* Font is responsible for this setup */
        GUI_iDim_t height = gui_widget_getheight__(h);  /* Get item height */
        if (!is_opened(h)) {
            height *= HEIGHT_CONST(h) - 1;          /* Get height of opened area part */
        } else {
            height -= height / HEIGHT_CONST(h);     /* Subtract height for default size */
        }
        res = height / item_height(h, NULL);        /* Calculate entries */
    }
    return res;
}

/* Open or close dropdown widget */
static uint8_t
open_close(GUI_HANDLE_p h, uint8_t state) {
    if (state && !is_opened(h)) {
        o->Flags |= GUI_FLAG_DROPDOWN_OPENED;
        o->OldHeight = o->C.Height;                 /* Save height for restore */
        o->OldY = o->C.Y;                           /* Save Y position for restore */
        if (is_dir_up(h)) {                         /* On up direction */
            o->C.Y = o->C.Y - (HEIGHT_CONST(h) - 1) * o->C.Height; /* Go up for 3 height values */
        }
        o->C.Height = HEIGHT_CONST(h) * o->C.Height;
        gui_widget_invalidate__(h);                 /* Invalidate widget */
        return 1;
    } else if (!state && (o->Flags & GUI_FLAG_DROPDOWN_OPENED)) {
        gui_widget_invalidatewithparent__(h);       /* Invalidate widget */
        o->Flags &= ~GUI_FLAG_DROPDOWN_OPENED;      /* Clear flag */
        o->C.Height = o->OldHeight;                 /* Restore height value */
        o->C.Y = o->OldY;                           /* Restore position */
        if (o->Selected == -1) {                    /* Go to top selection */
            o->VisibleStartIndex = 0;               /* Start from top again */
        } else {                                    /* We have one selection */
            int16_t perPage = nr_entries_pp(h);
            o->VisibleStartIndex = o->Selected;
            if ((o->VisibleStartIndex + perPage) >= o->Count) {
                o->VisibleStartIndex = o->Count - perPage;
                if (o->VisibleStartIndex < 0) {
                    o->VisibleStartIndex = 0;
                }
            }
        }
        return 1;
    }
    return 0;
}

/* Slide up or slide down widget elements */
static void
slide(GUI_HANDLE_p h, int16_t dir) {
    int16_t mPP = nr_entries_pp(h);
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->VisibleStartIndex + dir) < 0) {
            o->VisibleStartIndex = 0;
        } else {
            o->VisibleStartIndex += dir;
        }
        gui_widget_invalidate__(h);
    } else if (dir > 0) {
        if ((o->VisibleStartIndex + dir) > (o->Count - mPP - 1)) {  /* Slide elements down */
            o->VisibleStartIndex = o->Count - mPP;
        } else {
            o->VisibleStartIndex += dir;
        }
        gui_widget_invalidate__(h);
    }
}

/* Set selection for widget */
static void
set_selection(GUI_HANDLE_p h, int16_t selected) {
    if (o->Selected != selected) {                  /* Set selected value */
        o->Selected = selected;
        gui_widget_callback__(h, GUI_WC_SelectionChanged, NULL, NULL);  /* Notify about selection changed */
    }                         
}

/* Get item from listbox entry */
static GUI_DROPDOWN_ITEM_t*
get_item(GUI_HANDLE_p h, uint16_t index) {
    uint16_t i = 0;
    GUI_DROPDOWN_ITEM_t* item = 0;
    
    if (index >= o->Count) {                        /* Check if valid index */
        return 0;
    }
    
    if (index == 0) {                               /* Check for first element */
        return (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(&o->Root, NULL);   /* Return first element */
    } else if (index == o->Count - 1) {
        return (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getprev_gen(&o->Root, NULL);   /* Return last element */
    }
    
    item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(&o->Root, NULL);
    while (i++ < index) {
        item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(NULL, &item->List);
    }
    return item;
}

/**
 * \brief           Check new values of widget.
 *
 *                  Scans new selected items, count, checks if valid selected item exists, etc
 * 
 * \param[in]       h: Widget handle
 */
static void
check_values(GUI_HANDLE_p h) {
    int16_t mPP = nr_entries_pp(h);                 /* Get number of lines visible in widget at a time */
   
    if (o->Selected >= 0) {                         /* Check for selected value range */
        if (o->Selected >= o->Count) {
            set_selection(h, o->Count - 1);
        }
    }
    if (o->VisibleStartIndex < 0) {                 /* Check visible start index position */
        o->VisibleStartIndex = 0;
    } else if (o->VisibleStartIndex > 0) {
        if (o->Count > mPP) {
            if (o->VisibleStartIndex + mPP >= o->Count) {
                o->VisibleStartIndex = o->Count - mPP;
            }
        }
    }
    
    if (o->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO) { /* Check slider mode */
        if (o->Count > mPP) {
            o->Flags |= GUI_FLAG_DROPDOWN_SLIDER_ON;
        } else {
            o->Flags &= ~GUI_FLAG_DROPDOWN_SLIDER_ON;
        }
    }
}

/* Increase or decrease selection */
static void
inc_selection(GUI_HANDLE_p h, int16_t dir) {
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->Selected + dir) < 0) {
            set_selection(h, 0);
        } else {
            set_selection(h, o->Selected + dir);
        }
        gui_widget_invalidate__(h);
    } else if (dir > 0) {
        if ((o->Selected + dir) > (o->Count - 1)) { /* Slide elements down */
            set_selection(h, o->Count - 1);
        } else {
            set_selection(h, o->Selected + dir);
        }
        gui_widget_invalidate__(h);
    }
}

/* Delete list item box by index */
static uint8_t
delete_item(GUI_HANDLE_p h, uint16_t index) {
    GUI_DROPDOWN_ITEM_t* item;
    
    item = get_item(h, index);                      /* Get list item from handle */
    if (item) {
        gui_linkedlist_remove_gen(&__GD(h)->Root, &item->List);
        __GD(h)->Count--;                           /* Decrease count */
        
        if (o->Selected == index) {
            set_selection(h, -1);
        }
        
        check_values(h);                            /* Check widget values */
        gui_widget_invalidate__(h);
        return 1;
    }
    return 0;
}

/* Process widget click event */
static void
process_click(GUI_HANDLE_p h, __GUI_TouchData_t* ts) {
    GUI_iDim_t y, y1, height, height1;
    
    if (!is_opened(h)) {                            /* Check if opened */
        open_close(h, 1);                           /* Open widget on click */
        return;
    }
    
    y = 0;                                          /* Relative Y position for touch events */
    height = gui_widget_getheight__(h);             /* Get widget height */
    get_opened_positions(h, &y, &height, &y1, &height1);    /* Calculate values */
    
    /* Check if press was on normal area when widget is closed */
    if (ts->RelY[0] >= y1 && ts->RelY[0] <= (y1 + height1)) {   /* Check first part */
        
    } else {
        uint16_t tmpSelected;
        
        if (is_dir_up(h)) {
            tmpSelected = ts->RelY[0] / item_height(h, NULL);  /* Get temporary selected index */
        } else {
            tmpSelected = (ts->RelY[0] - height1) / item_height(h, NULL);  /* Get temporary selected index */
        }
        if ((o->VisibleStartIndex + tmpSelected) < o->Count) {
            set_selection(h, o->VisibleStartIndex + tmpSelected);
            gui_widget_invalidate__(h);             /* Choose new selection */
        }
        check_values(h);                            /* Check values */
    }
    open_close(h, 0);                               /* Close widget on click */
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
gui_dropdown_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
#if GUI_CFG_USE_TOUCH
    static GUI_iDim_t tY;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GD(h)->Selected = -1;                 /* Invalidate selection */
            __GD(h)->SliderWidth = 30;              /* Set slider width */
            __GD(h)->Flags |= GUI_FLAG_DROPDOWN_SLIDER_AUTO;    /* Enable slider auto mode */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            GUI_iDim_t x, y, width, height;
            GUI_iDim_t y1, height1;                 /* Position of main widget part, when widget is opened */
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            if (is_opened(h)) {
                get_opened_positions(h, &y, &height, &y1, &height1);
                
                gui_draw_rectangle3d(disp, x, y1, width, height1, GUI_DRAW_3D_State_Lowered);
                gui_draw_filledrectangle(disp, x + 2, y1 + 2, width - 4, height1 - 4, gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_BG));
                
                gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_BG));
                gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            } else {
                y1 = y;
                height1 = height;
                
                gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Raised);
                gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_BG));
            }
                
            if (__GD(h)->Selected >= 0 && __GH(h)->Font) {
                int16_t i;
                GUI_DRAW_FONT_t f;
                GUI_DROPDOWN_ITEM_t* item;
                gui_draw_font_init(&f);             /* Init structure */
                
                item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(&__GD(h)->Root, NULL);
                for (i = 0; i < __GD(h)->Selected; i++) {
                    item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(NULL, (GUI_LinkedList_t *)item);
                }
                
                f.X = x + 3;
                f.Y = y1 + 3;
                f.Width = width - 6;
                f.Height = height1 - 6;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                f.Color1 = gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_TEXT);
                gui_draw_writetext(disp, gui_widget_getfont__(h), item->Text, &f);
            }
            
            if (is_opened(h) && __GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_ON) {
                GUI_DRAW_SB_t sb;
                
                width -= __GD(h)->SliderWidth;      /* Available width is decreased */
                
                gui_draw_scrollbar_init(&sb);
                
                sb.X = x + width - 1;
                sb.Y = y + 1;
                sb.Width = o->SliderWidth;
                sb.Height = height - 2;
                sb.Dir = GUI_DRAW_SB_DIR_VERTICAL;
                sb.EntriesTop = o->VisibleStartIndex;
                sb.EntriesTotal = o->Count;
                sb.EntriesVisible = nr_entries_pp(h);
                
                gui_draw_scrollbar(disp, &sb);      /* Draw scroll bar */
            } else {
                width--;                            /* Go down for one for alignment on non-slider */
            }
            
            if (is_opened(h) && __GH(h)->Font && gui_linkedlist_hasentries(&__GD(h)->Root)) {
                GUI_DRAW_FONT_t f;
                GUI_DROPDOWN_ITEM_t* item;
                uint16_t yOffset;
                uint16_t itemHeight;                /* Get item height */
                uint16_t index = 0;                 /* Start index */
                GUI_iDim_t tmp;
                
                itemHeight = item_height(h, &yOffset); /* Get item height and Y offset */
                
                gui_draw_font_init(&f);             /* Init structure */
                
                f.X = x + 4;
                f.Y = y + 2;
                f.Width = width - 6;
                f.Height = itemHeight;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                
                tmp = disp->Y2;
                if (disp->Y2 > (y + height)) {      /* Set cut-off Y position for drawing operations */
                    disp->Y2 = y + height;
                }
                
                /* Try to process all strings */
                for (index = 0, item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(&o->Root, NULL); item && f.Y <= disp->Y2;
                        item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_getnext_gen(NULL, (GUI_LinkedList_t *)item), index++) {
                    if (index < o->VisibleStartIndex) { /* Check for start visible */
                        continue;
                    }
                    if (index == __GD(h)->Selected) {
                        gui_draw_filledrectangle(disp, x + 2, f.Y, width - 3, GUI_MIN(f.Height, itemHeight), gui_widget_isfocused__(h) ? gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_SEL_FOC_BG) : gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_SEL_NOFOC_BG));
                        f.Color1 = gui_widget_isfocused__(h) ? gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_SEL_FOC) : gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_SEL_NOFOC);
                    } else {
                        f.Color1 = gui_widget_getcolor__(h, GUI_DROPDOWN_COLOR_TEXT);
                    }
                    gui_draw_writetext(disp, gui_widget_getfont__(h), item->Text, &f);
                    f.Y += itemHeight;
                }
                disp->Y2 = tmp;                     /* Set temporary value back */
            }
            return 1;
        }
        case GUI_WC_Remove: {
            GUI_DROPDOWN_ITEM_t* item;
            while ((item = (GUI_DROPDOWN_ITEM_t *)gui_linkedlist_remove_gen(&o->Root, (GUI_LinkedList_t *)gui_linkedlist_getnext_gen(&o->Root, NULL))) != NULL) {
                GUI_MEMFREE(item);                  /* Free memory */
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            tY = ts->RelY[0];                       /* Save relative Y position */
            
            GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (__GH(h)->Font) {
                GUI_Dim_t height = item_height(h, NULL);   /* Get element height */
                GUI_iDim_t diff = tY - ts->RelY[0];
                
                if (GUI_ABS(diff) > height) {
                    slide(h, diff > 0 ? 1 : -1);    /* Slide widget */
                    tY = ts->RelY[0];               /* Save pointer */
                }
            }
            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_WC_Click: {
            __GUI_TouchData_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            if (!is_opened(h)) {                    /* Check widget status */
                open_close(h, 1);                   /* Open widget */
            } else {                                /* Widget opened, process data */
                process_click(h, ts);               /* Process click event */
            }
            return 1;
        }
        case GUI_WC_FocusOut: {
            open_close(h, 0);                       /* Close widget */
            return 1;
        }
#if GUI_CFG_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = GUI_WIDGET_PARAMTYPE_KEYBOARD(param);    /* Get keyboard data */
            if (kb->KB.Keys[0] == GUI_KEY_DOWN) {   /* On pressed down */
                inc_selection(h, 1);                /* Increase selection */
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_UP) {
                inc_selection(h, -1);               /* Decrease selection */
                GUI_WIDGET_RESULTTYPE_KEYBOARD(result) = keyHANDLED;
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
 * \brief           Create new dropdown widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \return          \ref GUI_HANDLE_p object of created widget on success, NULL otherwise
 */
GUI_HANDLE_p
gui_dropdown_create(GUI_ID_t id, float x, float y, float width, float height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to DROPDOWN
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors. This parameter can be a value of \ref GUI_DROPDOWN_COLOR_t enumeration
 * \param[in]       color: Actual color code to set
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_dropdown_setcolor(GUI_HANDLE_p h, GUI_DROPDOWN_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Add a new string to list box
 * \param[in,out]   h: Widget handle
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_dropdown_addstring(GUI_HANDLE_p h, const GUI_Char* text) {
    GUI_DROPDOWN_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    
    item = GUI_MEMALLOC(sizeof(*item));             /* Allocate memory for entry */
    if (item != NULL) {
        __GUI_ENTER();                              /* Enter GUI */
        item->Text = (GUI_Char *)text;              /* Add text to entry */
        gui_linkedlist_add_gen(&__GD(h)->Root, &item->List);/* Add to linked list */
        __GD(h)->Count++;                           /* Increase number of strings */
        
        check_values(h);                            /* Check values */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
         
        ret = 1;
        __GUI_LEAVE();                              /* Leave GUI */
    }
    
    return ret;
}

/**
 * \brief           Set opening direction for dropdown list
 * \param[in,out]   h: Widget handle
 * \param[in]       dir: Opening direction. This parameter can be a value of \ref GUI_DROPDOWN_OPENDIR_t enumeration
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_dropdown_setopendirection(GUI_HANDLE_p h, GUI_DROPDOWN_OPENDIR_t dir) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!is_opened(h)) {                            /* Must be closed */
        if ((__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPEN_UP) && dir == GUI_DROPDOWN_OPENDIR_DOWN) {
            __GD(h)->Flags &= GUI_FLAG_DROPDOWN_OPEN_UP;
            ret = 1;
        } else if (!(__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPEN_UP) && dir == GUI_DROPDOWN_OPENDIR_UP) {
            __GD(h)->Flags |= GUI_FLAG_DROPDOWN_OPEN_UP;
            ret = 1;
        }
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set string value to already added string index
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index (position) on list to set/change text
 * \param[in]       text: Pointer to text to add to list. Only pointer is saved to memory!
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_dropdown_setstring(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text) {
    GUI_DROPDOWN_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    item = get_item(h, index);                      /* Get list item from handle */
    if (item) {
        item->Text = (GUI_Char *)text;              /* Set new text */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Delete first string from list
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise
 * \sa              gui_dropdown_deletestring, gui_dropdown_deletelaststring
 */
uint8_t
gui_dropdown_deletefirststring(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = delete_item(h, 0);                        /* Delete first item */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Delete last string from list
 * \param[in,out]   h: Widget handle
 * \return          1 on success, 0 otherwise
 * \sa              gui_dropdown_deletestring, gui_dropdown_deletefirststring
 */
uint8_t
gui_dropdown_deletelaststring(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = delete_item(h, __GD(h)->Count - 1);       /* Delete last item */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Delete specific entry from list
 * \param[in,out]   h: Widget handle
 * \param[in]       index: List index (position) to delete
 * \return          1 on success, 0 otherwise
 * \sa              gui_dropdown_deletefirststring, gui_dropdown_deletelaststring
 */
uint8_t
gui_dropdown_deletestring(GUI_HANDLE_p h, uint16_t index) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = delete_item(h, index);                    /* Delete item */

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to 1 for auto mode or 0 for manual mode
 * \return          1 on success, 0 otherwise
 * \sa              gui_dropdown_setslidervisibility
 */
uint8_t
gui_dropdown_setsliderauto(GUI_HANDLE_p h, uint8_t autoMode) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (autoMode && !(__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO)) {
        __GD(h)->Flags |= GUI_FLAG_DROPDOWN_SLIDER_AUTO;
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    } else if (!autoMode && (__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO)) {
        __GD(h)->Flags &= ~GUI_FLAG_DROPDOWN_SLIDER_AUTO;
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set manual visibility for slider
 * \note            Slider must be in manual mode in order to get this to work
 * \param[in,out]   h: Widget handle
 * \param[in]       visible: Slider visible status, 1 or 0
 * \return          1 on success, 0 otherwise
 * \sa              gui_dropdown_setsliderauto
 */
uint8_t
gui_dropdown_setslidervisibility(GUI_HANDLE_p h, uint8_t visible) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!(__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO)) {
        if (visible && !(__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_ON)) {
            __GD(h)->Flags |= GUI_FLAG_DROPDOWN_SLIDER_ON;
            gui_widget_invalidate__(h);             /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_ON)) {
            __GD(h)->Flags &= ~GUI_FLAG_DROPDOWN_SLIDER_ON;
            gui_widget_invalidate__(h);             /* Invalidate widget */
            ret = 1;
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Scroll list if possible
 * \param[in,out]   h: Widget handle
 * \param[in]       step: Step to scroll. Positive step will scroll up, negative will scroll down
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_dropdown_scroll(GUI_HANDLE_p h, int16_t step) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    start = __GD(h)->VisibleStartIndex;
    __GD(h)->VisibleStartIndex += step;
        
    check_values(h);                                /* Check widget values */
    
    start = start != __GD(h)->VisibleStartIndex;    /* Check if there was valid change */
    
    if (start) {
        gui_widget_invalidate__(h);
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return start;
}

/**
 * \brief           Set selected value
 * \param[in,out]   h: Widget handle
 * \param[in]       selection: Set to -1 to invalidate selection or 0 - count-1 for specific selection 
 * \return          1 on success, 0 otherwise
 * \sa              gui_dropdown_getselection
 */
uint8_t
gui_dropdown_setselection(GUI_HANDLE_p h, int16_t selection) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    set_selection(h, selection);                    /* Set selection */
    check_values(h);                                /* Check values */
    gui_widget_invalidate__(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get selected value
 * \param[in,out]   h: Widget handle
 * \return          Selection number or -1 if no selection
 * \sa              gui_dropdown_setselection
 */
int16_t
gui_dropdown_getselection(GUI_HANDLE_p h) {
    int16_t selection;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    selection = __GD(h)->Selected;                  /* Read selection */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return selection;
}

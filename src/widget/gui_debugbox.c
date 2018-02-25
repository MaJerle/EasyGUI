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

#define __GL(x)             ((GUI_DEBUGBOX_t *)(x))

static uint8_t gui_debugbox_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_BG,
    GUI_COLOR_WIN_TEXT,
};

/**
 * \brief           Widget initialization structure
 */
static const
GUI_WIDGET_t Widget = {
    .Name = _GT("DEBUGBOX"),                         /*!< Widget name */
    .Size = sizeof(GUI_DEBUGBOX_t),                  /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_debugbox_callback,               /*!< Callback function */
    .Colors = Colors,
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

#define o                   ((GUI_DEBUGBOX_t *)(h))

/* Get item height in debugbox */
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
        res = gui_widget_getheight__(h) / item_height(h, NULL);
    }
    return res;
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

/* Check values */
static void
check_values(GUI_HANDLE_p h) {
    int16_t mPP = nr_entries_pp(h);                 /* Get number of lines visible in widget at a time */
   
    if (o->VisibleStartIndex < 0) {                 /* Check visible start index position */
        o->VisibleStartIndex = 0;
    } else if (o->VisibleStartIndex > 0) {
        if (o->Count > mPP) {
            if (o->VisibleStartIndex + mPP >= o->Count) {
                o->VisibleStartIndex = o->Count - mPP;
            }
        } else {
            o->VisibleStartIndex = 0;
        }
    }
    
    if (o->Flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO) {  /* Check slider mode */
        if (o->Count > mPP) {
            o->Flags |= GUI_FLAG_DEBUGBOX_SLIDER_ON;
        } else {
            o->Flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_ON;
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
gui_debugbox_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
#if GUI_CFG_USE_TOUCH
    static GUI_iDim_t tY;
#endif /* GUI_CFG_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GL(h)->SliderWidth = 30;              /* Set slider width */
            __GL(h)->MaxCount = 15;                 /* Number of maximal entries for debug */
            __GL(h)->Flags |= GUI_FLAG_DEBUGBOX_SLIDER_AUTO;   /* Set auto mode for slider */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            GUI_Dim_t x, y, width, height;
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            gui_draw_rectangle3d(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            gui_draw_filledrectangle(disp, x + 2, y + 2, width - 4, height - 4, gui_widget_getcolor__(h, GUI_DEBUGBOX_COLOR_BG));
            
            /* Draw side scrollbar */
            if (o->Flags & GUI_FLAG_DEBUGBOX_SLIDER_ON) {
                GUI_DRAW_SB_t sb;
                gui_draw_scrollbar_init(&sb);
                
                width -= o->SliderWidth;            /* Decrease available width */
                
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
                width--;                            /* Go one pixel down for alignment */
            }
            
            /* Draw text if possible */
            if (__GH(h)->Font && gui_linkedlist_hasentries(&__GL(h)->Root)) {   /* Is first set? */
                GUI_DRAW_FONT_t f;
                GUI_DEBUGBOX_ITEM_t* item;
                uint16_t itemHeight;                /* Get item height */
                uint16_t index = 0;                 /* Start index */
                GUI_iDim_t tmp;
                
                itemHeight = item_height(h, 0);     /* Get item height and Y offset */
                
                gui_draw_font_init(&f);             /* Init structure */
                
                f.X = x + 4;
                f.Y = y + 2;
                f.Width = width - 4;
                f.Height = itemHeight;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                
                tmp = disp->Y2;                     /* Scale out drawing area */
                if (disp->Y2 > (y + height - 2)) {
                    disp->Y2 = y + height - 2;
                }
                
                for (index = 0, item = (GUI_DEBUGBOX_ITEM_t *)gui_linkedlist_getnext_gen(&o->Root, NULL); item && f.Y <= disp->Y2;
                        item = (GUI_DEBUGBOX_ITEM_t *)gui_linkedlist_getnext_gen(NULL, (GUI_LinkedList_t *)item), index++) {
                    if (index < o->VisibleStartIndex) { /* Check for start drawing index */
                        continue;
                    }
                    f.Color1 = gui_widget_getcolor__(h, GUI_DEBUGBOX_COLOR_TEXT);
                    gui_draw_writetext(disp, gui_widget_getfont__(h), item->Text, &f);
                    f.Y += itemHeight;
                }
                disp->Y2 = tmp;
            }
            
            return 1;
        }
        case GUI_WC_Remove: {
            GUI_DEBUGBOX_ITEM_t* item;
            while ((item = (GUI_DEBUGBOX_ITEM_t *)gui_linkedlist_remove_gen(&o->Root, (GUI_LinkedList_t *)gui_linkedlist_getnext_gen(&o->Root, NULL))) != NULL) {
                GUI_MEMFREE(item);                  /* Free memory */
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            tY = ts->RelY[0];
            
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
            GUI_Dim_t width = gui_widget_getwidth__(h); /* Get widget widget */
            GUI_Dim_t height = gui_widget_getheight__(h);   /* Get widget height */
            
            if (o->Flags & GUI_FLAG_DEBUGBOX_SLIDER_ON) {
                if (ts->RelX[0] > (width - o->SliderWidth)) {   /* Touch is inside slider */
                    if (ts->RelY[0] < o->SliderWidth) {
                        slide(h, -1);               /* Slide one value up */
                    } else if (ts->RelY[0] > (height - o->SliderWidth)) {
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
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \return          \ref GUI_HANDLE_p object of created widget on success, NULL otherwise
 */
GUI_HANDLE_p
gui_debugbox_create(GUI_ID_t id, float x, float y, float width, float height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to debugbox
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Index in array of colors. This parameter can be a value of \ref GUI_DEBUGBOX_COLOR_t enumeration
 * \param[in]       color: Actual color code to set
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_debugbox_setcolor(GUI_HANDLE_p h, GUI_DEBUGBOX_COLOR_t index, GUI_Color_t color) {
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
gui_debugbox_addstring(GUI_HANDLE_p h, const GUI_Char* text) {
    GUI_DEBUGBOX_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    
    item = GUI_MEMALLOC(sizeof(*item) + gui_string_lengthtotal(text) + 1);  /* Allocate memory for entry */
    if (item != NULL) {
        __GUI_ENTER();                              /* Enter GUI */
        item->Text = (void *)((char *)item + sizeof(*item));/* Add text to entry */
        gui_string_copy(item->Text, text);          /* Copy text */
        gui_linkedlist_add_gen(&__GL(h)->Root, &item->List);/* Add to linked list */
        __GL(h)->Count++;                           /* Increase number of strings */
        
        /*
         * Use while loop in case user later changed max
         * number of entries to lower value.
         *
         * In this case we have to remove more than just one element
         */
        while (__GL(h)->Count > __GL(h)->MaxCount) {
            GUI_DEBUGBOX_ITEM_t* firstItem;
            firstItem = (GUI_DEBUGBOX_ITEM_t *)gui_linkedlist_getnext_gen(&__GL(h)->Root, NULL);
            if (firstItem != NULL) {
                gui_linkedlist_remove_gen(&__GL(h)->Root, (GUI_LinkedList_t *)firstItem);
                GUI_MEMFREE(firstItem);
                __GL(h)->Count--;
            } else {
                break;
            }
        }
        
        __GL(h)->VisibleStartIndex = __GL(h)->Count;/* Invalidate visible start index */
        check_values(h);                            /* Check values */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
        __GUI_LEAVE();                              /* Leave GUI */
        
        ret = 1;
    }
    
    return ret;
}

/**
 * \brief           Set auto mode for slider
 * \note            When it is enabled, slider will only appear if needed to show more entries on list
 * \param[in,out]   h: Widget handle
 * \param[in]       autoMode: Auto mode status. Set to 1 for auto mode or 0 for manual mode
 * \return          1 on success, 0 otherwise
 * \sa              gui_debugbox_setslidervisibility
 */
uint8_t
gui_debugbox_setsliderauto(GUI_HANDLE_p h, uint8_t autoMode) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (autoMode && !(__GL(h)->Flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        __GL(h)->Flags |= GUI_FLAG_DEBUGBOX_SLIDER_AUTO;
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    } else if (!autoMode && (__GL(h)->Flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        __GL(h)->Flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_AUTO;
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
 * \sa              gui_debugbox_setsliderauto
 */
uint8_t
gui_debugbox_setslidervisibility(GUI_HANDLE_p h, uint8_t visible) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!(__GL(h)->Flags & GUI_FLAG_DEBUGBOX_SLIDER_AUTO)) {
        if (visible && !(__GL(h)->Flags & GUI_FLAG_DEBUGBOX_SLIDER_ON)) {
            __GL(h)->Flags |= GUI_FLAG_DEBUGBOX_SLIDER_ON;
            gui_widget_invalidate__(h);             /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GL(h)->Flags & GUI_FLAG_DEBUGBOX_SLIDER_ON)) {
            __GL(h)->Flags &= ~GUI_FLAG_DEBUGBOX_SLIDER_ON;
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
gui_debugbox_scroll(GUI_HANDLE_p h, int16_t step) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    start = __GL(h)->VisibleStartIndex;
    __GL(h)->VisibleStartIndex += step;
        
    check_values(h);                                /* Check widget values */
    
    start = start != __GL(h)->VisibleStartIndex;    /* Check if there was valid change */
    
    if (start) {
        gui_widget_invalidate__(h);
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return start;
}

/**
 * \brief           Set maximal number of items for debugbox
 * \param[in,out]   h: Widget handle
 * \param[in]       max_items: Maximal number of items
 * \return          1 on success, 0 otherwise
 */
uint8_t
gui_debugbox_setmaxitems(GUI_HANDLE_p h, int16_t max_items) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget && max_items > 0);   /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GL(h)->MaxCount = max_items;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 0;
}

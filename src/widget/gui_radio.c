/**	
 * \file            gui_radio.c
 * \brief           Radio box widget
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
#include "widget/gui_radio.h"

#define __GR(x)             ((gui_radio_t *)(x))

static uint8_t gui_radio_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color */
    GUI_COLOR_DARKGRAY,                             /*!< Default foreground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
    GUI_COLOR_GRAY,                                 /*!< Default background color when disabled */
    GUI_COLOR_BLACK,                                /*!< Text color index */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("RADIOBOX"),                        /*!< Widget name */
    .size = sizeof(gui_radio_t),                    /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_radio_callback,                 /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

static uint8_t
set_active(gui_handle_p h) {
    gui_handle_p handle;
    
    if (__GR(h)->flags & GUI_FLAG_RADIO_DISABLED) { /* Check if it can be enabled */
        return 0;
    }
    
    /*
     * Find radio widgets on the same page
     * and with the same group ID as widget to be set as active
     */
    for (handle = gui_linkedlist_widgetgetnext(guii_widget_getparent(h), NULL); handle != NULL; 
            handle = gui_linkedlist_widgetgetnext(NULL, handle)) {
        if (handle != h && handle->widget == &widget && __GR(handle)->group_id == __GR(h)->group_id) {  /* Check if widget is radio box and group is the same as clicked widget */
            __GR(handle)->selected_value = __GR(h)->value;  /* Set selected value for widget */
            if (__GR(handle)->flags & GUI_FLAG_RADIO_CHECKED) { /* Check if widget active */
                __GR(handle)->flags &= ~GUI_FLAG_RADIO_CHECKED; /* Clear flag */
                gui_widget_invalidate(handle);      /* Invalidate widget */
            }
        }
    }
    
    if (!(__GR(h)->flags & GUI_FLAG_RADIO_CHECKED)) {   /* Invalidate only if not checked already */
        gui_widget_invalidate(h);                   /* Invalidate widget */
    }
    __GR(h)->flags |= GUI_FLAG_RADIO_CHECKED;       /* Set active flag */
    __GR(h)->selected_value = __GR(h)->value;       /* Set selected value of this radio */
    guii_widget_callback(h, GUI_WC_SelectionChanged, NULL, NULL);  /* Call user function */
    
    return 1;
}

/* Set disabled status */
static uint8_t
set_disabled(gui_handle_p h, uint8_t state) {
    if (state && !(__GR(h)->flags & GUI_FLAG_RADIO_DISABLED)) {
        __GR(h)->flags |= GUI_FLAG_RADIO_DISABLED;  /* Set flag */
        gui_widget_invalidate(h);                   /* Invalidate widget */
        return 1;
    } else if (!state && (__GR(h)->flags & GUI_FLAG_RADIO_DISABLED)) {
        __GR(h)->flags &= ~GUI_FLAG_RADIO_DISABLED; /* Clear flag */
        gui_widget_invalidate(h);                   /* Invalidate widget */
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
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_radio_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_color_t c1;
            gui_dim_t x, y, width, height, size, sx, sy;
            
            x = gui_widget_getabsolutex(h);         /* Get absolute X coordinate */
            y = gui_widget_getabsolutey(h);         /* Get absolute Y coordinate */
            width = gui_widget_getwidth(h);         /* Get widget width */
            height = gui_widget_getheight(h);       /* Get widget height */
            
            size = 20;                              /* Circle size in pixels */
            
            sx = x;
            sy = y + (height - size) / 2;
            
            if (__GR(h)->flags & GUI_FLAG_RADIO_DISABLED) {
                c1 = guii_widget_getcolor(h, GUI_RADIO_COLOR_DISABLED_BG);
            } else {
                c1 = guii_widget_getcolor(h, GUI_RADIO_COLOR_BG);
            }
            
            gui_draw_filledcircle(disp, sx + size / 2, sy + size / 2, size / 2, c1);
            gui_draw_circle(disp, sx + size / 2, sy + size / 2, size / 2, guii_widget_getcolor(h, GUI_RADIO_COLOR_BORDER));
            
            if (guii_widget_isfocused(h) && !(__GR(h)->flags & GUI_FLAG_RADIO_DISABLED)) {  /* When in focus */
                gui_draw_circle(disp, sx + size / 2, sy + size / 2, size / 2 - 2, guii_widget_getcolor(h, GUI_RADIO_COLOR_FG));
            }

            if (__GR(h)->flags & GUI_FLAG_RADIO_CHECKED) {
                gui_draw_filledcircle(disp, sx + size / 2, sy + size / 2, size / 2 - 5, guii_widget_getcolor(h, GUI_RADIO_COLOR_FG));
            }
            
            /* Draw text if possible */
            if (gui_widget_isfontandtextset(h)) {
                gui_draw_font_t f;
                gui_draw_font_init(&f);             /* Init structure */
                
                f.x = sx + size + 5;
                f.y = y + 1;
                f.width = width - size - 5;
                f.height = height - 2;
                f.align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                f.color1 = guii_widget_getcolor(h, GUI_RADIO_COLOR_TEXT);
                gui_draw_writetext(disp, gui_widget_getfont(h), gui_widget_gettext(h), &f);
            }
            
            return 1;
        }
        case GUI_WC_Click: {
            if (!(__GR(h)->flags & GUI_FLAG_RADIO_DISABLED)) {
                set_active(h);                      /* Set widget as active */
                gui_widget_invalidate(h);          /* Invalidate widget */
            }
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef c


/**
 * \brief           Create new radio widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_radio_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, cb, flags);
}

/**
 * \brief           Set color to specific part of widget
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_radio_setcolor(gui_handle_p h, gui_radio_color_t index, gui_color_t color) {
    return gui_widget_setcolor(h, (uint8_t)index, color);
}

/**
 * \brief           Set radio group for widget
 * \note            Radio widgets with the same group must be on the same parent widget
 * \param[in]       h: Widget handle
 * \param[in]       groupId: Group ID for widget
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_radio_setgroup(gui_handle_p h, uint8_t groupId) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (__GR(h)->group_id != groupId) {
        gui_handle_p handle;
        
        /*
         * Find radio widgets on the same parent 
         * and with the group the same as desired group ID
         *
         * This is to set selected value to new radio with new group ID
         */
        for (handle = gui_linkedlist_widgetgetnext(guii_widget_getparent(h), NULL); handle;
                handle = gui_linkedlist_widgetgetnext(NULL, handle)) {
            if (handle != h && __GH(handle)->widget == &widget && __GR(handle)->group_id == groupId) {  /* Check if widget is radio box and group is the same as input group */
                __GR(h)->selected_value = __GR(handle)->selected_value;   /* Set selected value for widget */
                break;          
            }
        }
        
        __GR(h)->group_id = groupId;                /* Set new group id */
        gui_widget_invalidate(h);                   /* Invalidate widget */
    }

    return 1;
}

/**
 * \brief           Get radio group for widget
 * \param[in]       h: Widget handle
 * \return          Widget group
 */
uint8_t
gui_radio_getgroup(gui_handle_p h) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return __GR(h)->group_id;
}

/**
 * \brief           Set value for widget when pressed
 * \param[in]       h: Widget handle
 * \param[in]       value: Value of widget group when specific widget is selected
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_radio_setvalue(gui_handle_p h, uint32_t value) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);

    if (__GR(h)->value != value) {
        __GR(h)->value = value;                     /* Set new value */
        if (__GR(h)->flags & GUI_FLAG_RADIO_CHECKED) {
            set_active(h);                          /* Check active radio widgets */
        }
    }

    return 1;
}

/**
 * \brief           Get value for specific widget
 * \param[in]       h: Widget handle
 * \return          Widget value
 */
uint32_t
gui_radio_getvalue(gui_handle_p h) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return __GR(h)->value;
}

/**
 * \brief           Set radio widget selected in radio group
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t gui_radio_setselected(gui_handle_p h) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return set_active(h);                           /* Set radio active */
}

/**
 * \brief           Get value of selected widget from widget group
 * \note            If `3` widgets share the same group ID, no matter which widget is used in this function, result will be always the same
 * \param[in]       h: Widget handle
 * \return          Widget selected value
 */
uint32_t
gui_radio_getselectedvalue(gui_handle_p h) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return __GR(h)->selected_value;
}

/**
 * \brief           Disable widget to prevent state change
 * \param[in]       h: Widget handle
 * \param[in]       disabled: Set to `1` to disable, `0` otherwise
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_radio_setdisabled(gui_handle_p h, uint8_t disabled) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return set_disabled(h, disabled);               /* Set checked status */
}

/**
 * \brief           Check if radio is disabled
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_radio_isdisabled(gui_handle_p h) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return !!(__GR(h)->flags & GUI_FLAG_RADIO_DISABLED);
}

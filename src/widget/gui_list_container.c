/**	
 * \file            gui_list_container.c
 * \brief           List container widget
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
#include "widget/gui_list_container.h"

/**
 * \ingroup         GUI_LISTCONTAINER
 * \brief           List container object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    gui_listcontainer_mode_t mode;                  /*!< List container type */
    gui_dim_t maxscrollx;                           /*!< Maximal scroll on X axis */
    gui_dim_t maxscrolly;                           /*!< Maximal scroll on Y axis */
} gui_listcontainer_t;

#define CFG_MODE            0x01

static uint8_t gui_listcontainer_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_TRANS,                                /*!< Default color when led is on */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("LIST_CONTAINER"),                  /*!< Widget name */ 
    .size = sizeof(gui_listcontainer_t),            /*!< Size of widget for memory allocation */
    .flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN | GUI_FLAG_WIDGET_INVALIDATE_PARENT,    /*!< List of widget flags */
    .callback = gui_listcontainer_callback,         /*!< Control function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

/* Calculate scroll limits according to children widgets */
static void
calculate_limits(gui_handle_p h) {
    gui_handle_p w;
    gui_dim_t x, y, width, height, cmx = 0, cmy = 0;
    gui_listcontainer_t* o = GUI_VP(h);
    
    /* Scan all children widgets and check for maximal possible scroll */
    for (w = gui_linkedlist_widgetgetnext(h, NULL); w != NULL;
            w = gui_linkedlist_widgetgetnext(NULL, w)) {

        x = guii_widget_getrelativex(w);
        y = guii_widget_getrelativey(w);
        width = gui_widget_getwidth(w);
        height = gui_widget_getheight(w);

        cmx = GUI_MAX(cmx, x + width);
        cmy = GUI_MAX(cmy, y + height);
    }

    width = gui_widget_getinnerwidth(h);  
    height = gui_widget_getinnerheight(h);  
    
    o->maxscrollx = 0;
    o->maxscrolly = 0;
    if (width < cmx) {
        o->maxscrollx = cmx - width;
    }
    if (height < cmy) {
        o->maxscrolly = cmy - height;
    }
    
    if (gui_widget_getscrollx(h) > o->maxscrollx) {
        gui_widget_setscrollx(h, o->maxscrollx);
    }
    if (gui_widget_getscrolly(h) > o->maxscrolly) {
        gui_widget_setscrolly(h, o->maxscrolly);
    }
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
gui_listcontainer_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_listcontainer_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            return 1;
        }
        case GUI_EVT_SETPARAM: {                     /* Set parameter for widget */
            gui_widget_param* p = GUI_EVT_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_MODE:
                    o->mode = *(gui_listcontainer_mode_t *)p->data;
                    break;
                default: break;
            }
            GUI_EVT_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_EVT_CHILDWIDGETCREATED: {           /* Child widget has been created */
            return 1;
        }
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y, width, height;
            
            calculate_limits(h);                    /* Calculate new limits for scrolling */
            
            x = gui_widget_getabsolutex(h);         /* Get absolute position on screen */
            y = gui_widget_getabsolutey(h);         /* Get absolute position on screen */
            width = gui_widget_getwidth(h);
            height = gui_widget_getheight(h);
            
            gui_draw_filledrectangle(disp, x, y, width, height, guii_widget_getcolor(h, GUI_LISTCONTAINER_COLOR_BG));
            return 1;                               /* */
        }
#if GUI_CFG_USE_TOUCH
        case GUI_EVT_TOUCHSTART: {
            GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            return 1;
        }
        case GUI_EVT_TOUCHMOVE: {
            gui_dim_t sx, sy, new_diff;
            guii_touch_data_t* ts = GUI_EVT_PARAMTYPE_TOUCH(param);  /* Get touch data */

            /* Get current scroll values */
            sx = gui_widget_getscrollx(h);
            sy = gui_widget_getscrolly(h);

            /* Try to scroll widget */
            if (o->mode == GUI_LISTCONTAINER_MODE_VERTICAL || o->mode == GUI_LISTCONTAINER_MODE_VERTICAL_HORIZONTAL) {
                new_diff = -ts->y_diff[0];
                if ((new_diff + sy) <= 0) {
                    gui_widget_setscrolly(h, 0);
                } else if ((new_diff + sy) >= o->maxscrolly) {
                    gui_widget_setscrolly(h, o->maxscrolly);
                } else {
                    gui_widget_setscrolly(h, sy + new_diff);
                }
            }
            if (o->mode == GUI_LISTCONTAINER_MODE_HORIZONTAL || o->mode == GUI_LISTCONTAINER_MODE_VERTICAL_HORIZONTAL) {
                new_diff = -ts->x_diff[0];
                if ((new_diff + sx) <= 0) {
                    gui_widget_setscrollx(h, 0);
                } else if ((new_diff + sx) >= o->maxscrollx) {
                    gui_widget_setscrollx(h, o->maxscrollx);
                } else {
                    gui_widget_setscrollx(h, sx + new_diff);
                }
            }
            calculate_limits(h);                    /* Calculate scroll limits */
            
            /* Check if scroll was successful */
            /* Handle touch, otherwise send it to parent widget */
            GUI_EVT_RESULTTYPE_TOUCH(result) = touchCONTINUE;
            if (gui_widget_getscrollx(h) != sx || gui_widget_getscrolly(h) != sy) {
                GUI_EVT_RESULTTYPE_TOUCH(result) = touchHANDLED;
            }

            return 1;
        }
#endif /* GUI_CFG_USE_TOUCH */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef l

/**
 * \brief           Create new list container widget
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
gui_listcontainer_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, evt_fn, flags);
}

/**
 * \brief           Set color to specific part of widget
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listcontainer_setcolor(gui_handle_p h, gui_listcontainer_color_t index, gui_color_t color) {
    uint8_t ret;

    ret = gui_widget_setcolor(h, (uint8_t)index, color);
    if (ret && index == GUI_LISTCONTAINER_COLOR_BG) {   /* Check background color */
        gui_widget_setinvalidatewithparent(h, color == GUI_COLOR_TRANS);
    }

    return ret;
}

/**
 * \brief           Set list container mode
 * \param[in]       h: Widget handle
 * \param[in]       mode: List container mode
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_listcontainer_setmode(gui_handle_p h, gui_listcontainer_mode_t mode) {
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    return guii_widget_setparam(h, CFG_MODE, &mode, 1, 0);
}

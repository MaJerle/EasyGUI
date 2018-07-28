/**	
 * \file            gui_checkbox.c
 * \brief           Check bar widget
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
#include "widget/gui_checkbox.h"

#define __GC(x)             ((gui_checkbox_t *)(x))

#define CFG_CHECK           0x01
#define CFG_DISABLE         0x02

static uint8_t gui_checkbox_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color */
    GUI_COLOR_DARKGRAY,                             /*!< Default freground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
    GUI_COLOR_GRAY,                                 /*!< Default background color when disabled */
    GUI_COLOR_BLACK,                                /*!< Default text color for widget */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("CHECKBOX"),                        /*!< Widget name */
    .size = sizeof(gui_checkbox_t),                 /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_checkbox_callback,              /*!< Callback function */
    .colors = colors,                               /*!< List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

#define c                   ((gui_checkbox_t *)(h))

/* Set checked status */
static uint8_t
set_checked(gui_handle_p h, uint8_t state) {
    if (state && !(c->flags & GUI_FLAG_CHECKBOX_CHECKED)) {
        c->flags |= GUI_FLAG_CHECKBOX_CHECKED;      /* Set flag */
        guii_widget_callback(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        guii_widget_invalidate(h);
        return 1;
    } else if (!state && (c->flags & GUI_FLAG_CHECKBOX_CHECKED)) {
        c->flags &= ~GUI_FLAG_CHECKBOX_CHECKED;     /* Clear flag */
        guii_widget_callback(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        guii_widget_invalidate(h);
        return 1;
    }
    return 0;
}

/* Set disabled status */
static uint8_t
set_disabled(gui_handle_p h, uint8_t state) {
    if (state && !(c->flags & GUI_FLAG_CHECKBOX_DISABLED)) {
        c->flags |= GUI_FLAG_CHECKBOX_DISABLED;     /* Set flag */
        return 1;
    } else if (!state && (c->flags & GUI_FLAG_CHECKBOX_DISABLED)) {
        c->flags &= ~GUI_FLAG_CHECKBOX_DISABLED;    /* Clear flag */
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
gui_checkbox_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            gui_widget_param* p = GUI_WIDGET_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_CHECK:                     /* Set current progress value */
                    set_checked(h, *(uint8_t *)p->data);
                    break;
                case CFG_DISABLE:                   /* Set disabled status */
                    set_disabled(h, *(uint8_t *)p->data);
                    break;
                default: break;
            }
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            gui_color_t c1;
            gui_dim_t x, y, width, height, size, sx, sy;
            
            x = guii_widget_getabsolutex(h);       /* Get absolute X coordinate */
            y = guii_widget_getabsolutey(h);       /* Get absolute Y coordinate */
            width = guii_widget_getwidth(h);       /* Get widget width */
            height = guii_widget_getheight(h);     /* Get widget height */
            
            size = 20;
            
            sx = x;
            sy = y + (height - size) / 2;
            
            if (c->flags & GUI_FLAG_CHECKBOX_DISABLED) {
                c1 = guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_DISABLED_BG);
            } else {
                c1 = guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_BG);
            }
            
            gui_draw_filledrectangle(disp, sx + 1, sy + 1, size - 2, size - 2, c1);
            gui_draw_rectangle3d(disp, sx, sy, size, size, GUI_DRAW_3D_State_Lowered);
            
            if (guii_widget_isfocused(h) && !(c->flags & GUI_FLAG_CHECKBOX_DISABLED)) {    /* When in focus */
                gui_draw_rectangle(disp, sx + 2, sy + 2, size - 4, size - 4, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
            }
            
            if (c->flags & GUI_FLAG_CHECKBOX_CHECKED) {
                gui_draw_line(disp, sx + 4, sy + 5, sx + size - 4 - 2, sy + size - 4 - 1, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 4, sy + 4, sx + size - 4 - 1, sy + size - 4 - 1, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 5, sy + 4, sx + size - 4 - 1, sy + size - 4 - 2, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
                
                gui_draw_line(disp, sx + 4, sy + size - 4 - 2, sx + size - 4 - 2, sy + 4, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 4, sy + size - 4 - 1, sx + size - 4 - 1, sy + 4, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 5, sy + size - 4 - 1, sx + size - 4 - 1, sy + 5, guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_FG));
            }
            
            /* Draw text if possible */
            if (guii_widget_isfontandtextset(h)) {
                gui_draw_font_t f;
                gui_draw_font_init(&f);             /* Init structure */
                
                f.x = sx + size + 5;
                f.y = y + 1;
                f.width = width - size - 5;
                f.height = height - 2;
                f.align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.color1width = f.width;
                f.color1 = guii_widget_getcolor(h, GUI_CHECKBOX_COLOR_TEXT);
                gui_draw_writetext(disp, guii_widget_getfont(h), guii_widget_gettext(h), &f);
            }
            
            return 1;
        }
        case GUI_WC_Click: {
            if (!(c->flags & GUI_FLAG_CHECKBOX_DISABLED)) { 
                set_checked(h, (c->flags & GUI_FLAG_CHECKBOX_CHECKED) ? 0 : 1); /* Toggle checked state */
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
 * \brief           Create new checkbox widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_checkbox_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_checkbox_setcolor(gui_handle_p h, gui_checkbox_color_t index, gui_color_t color) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color, 1);   /* Set color */
}

/**
 * \brief           Set checked status to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       checked: Checked status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_checkbox_setchecked(gui_handle_p h, uint8_t checked) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_CHECK, &checked, 0, 0, 1);   /* Set parameter */
}

/**
 * \brief           Set disabled status to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       disabled: Disabled status
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_checkbox_setdisabled(gui_handle_p h, uint8_t disabled) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_DISABLE, &disabled, 0, 0, 1);/* Set parameter */
}

/**
 * \brief           Check if checkbox is checked
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_checkbox_ischecked(gui_handle_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_LEAVE(1);                                 /* Enter GUI */
    ret = !!(__GC(h)->flags & GUI_FLAG_CHECKBOX_CHECKED);
    __GUI_LEAVE(1);                                 /* Leave GUI */

    return ret;
}

/**
 * \brief           Check if checkbox is disabled
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_checkbox_isdisabled(gui_handle_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_LEAVE(1);                                 /* Enter GUI */
    ret = !!(__GC(h)->flags & GUI_FLAG_CHECKBOX_DISABLED);
    __GUI_LEAVE(1);                                 /* Leave GUI */

    return ret;
}

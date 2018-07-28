/**	
 * \file            gui_graph.c
 * \brief           Graph widget
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
#include "widget/gui_graph.h"
#include "math.h"

#define __GG(x)             ((gui_graph_t *)(x))

#define CFG_MIN_X           0x01
#define CFG_MAX_X           0x02
#define CFG_MIN_Y           0x03
#define CFG_MAX_Y           0x04
#define CFG_ZOOM_RESET      0x05

static uint8_t gui_graph_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/**
 * \brief           List of default color in the same order of widget color enumeration
 */
static const
gui_color_t colors[] = {
    GUI_COLOR_GRAY,                                 /*!< Default background color */
    GUI_COLOR_BLACK,                                /*!< Default foreground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
    0xFF002F00,                                     /*!< Default grid color */
};

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("GRAPH"),                           /*!< Widget name */
    .size = sizeof(gui_graph_t),                    /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_graph_callback,                 /*!< Callback function for various events */
    .colors = colors,                               /*<! List of default colors */
    .color_count = GUI_COUNT_OF(colors),            /*!< Number of colors */
};

/**
 * \brief           Reset zoom to `1`
 * \param[in]       h: Widget handle
 */
static void
graph_reset(gui_handle_p h) {
    gui_graph_t* g = (gui_graph_t *)h;

    g->visible_max_x = g->max_x;
    g->visible_min_x = g->min_x;
    g->visible_max_y = g->max_y;
    g->visible_min_y = g->min_y;
}

/**
 * \brief           Zoom plot with specific zoom at specific coordinates
 * \param[in]       h: Widget handle
 * \param[in]       zoom: Zoom multiplier. number > 1 is zoom, less is unzoom
 * \param[in]       xpos, ypos: X and Y position for zoom coordinate on screen
 */
static void
graph_zoom(gui_handle_p h, float zoom, float xpos, float ypos) {
    gui_graph_t* g = (gui_graph_t *)h;

    if (xpos < 0 || xpos > 1) { xpos = 0.5; }
    if (ypos < 0 || ypos > 1) { ypos = 0.5; }
           
    g->visible_min_x += (g->visible_max_x - g->visible_min_x) * (zoom - 1.0f) * xpos;
    g->visible_max_x -= (g->visible_max_x - g->visible_min_x) * (zoom - 1.0f) * (1.0f - xpos);

    g->visible_min_y += (g->visible_max_y - g->visible_min_y) * (zoom - 1.0f) * ypos;
    g->visible_max_y -= (g->visible_max_y - g->visible_min_y) * (zoom - 1.0f) * (1.0f - ypos);
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
gui_graph_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result) {
    gui_graph_t* g = (gui_graph_t *)h;
#if GUI_CFG_USE_TOUCH
    static gui_dim_t tx[GUI_CFG_TOUCH_MAX_PRESSES], ty[GUI_CFG_TOUCH_MAX_PRESSES];
#endif /* GUI_CFG_USE_TOUCH */

    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            g->border[GUI_GRAPH_BORDER_TOP] = 5;    /* Set borders */
            g->border[GUI_GRAPH_BORDER_RIGHT] = 5;
            g->border[GUI_GRAPH_BORDER_BOTTOM] = 5;
            g->border[GUI_GRAPH_BORDER_LEFT] = 5;

            g->max_x = 10;
            g->min_x = -10;
            g->max_y = 10;
            g->min_y = -10;
            graph_reset(h);                         /* Reset plot */

            g->rows = 8;                            /* Number of rows */
            g->columns = 10;                        /* Number of columns */
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter from widget */
            gui_widget_param* p = GUI_WIDGET_PARAMTYPE_WIDGETPARAM(param);
            switch (p->type) {
                case CFG_MIN_X: g->min_x = *(float *)p->data; break;/* Set min X value to widget */
                case CFG_MAX_X: g->max_x = *(float *)p->data; break;/* Set max X value to widget */
                case CFG_MIN_Y: g->min_y = *(float *)p->data; break;/* Set min Y value to widget */
                case CFG_MAX_Y: g->max_y = *(float *)p->data; break;/* Set max Y value to widget */
                case CFG_ZOOM_RESET: graph_reset(h); break; /* Reset zoom */
                default: break;
            }
            GUI_WIDGET_RESULTTYPE_U8(result) = 1;   /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {                         /* Draw widget */
            gui_graph_data_p data;
            gui_linkedlistmulti_t* link;
            gui_dim_t bt, br, bb, bl, x, y, width, height;
            uint8_t i;
            gui_display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);   /* Get display pointer */
            
            bt = g->border[GUI_GRAPH_BORDER_TOP];
            br = g->border[GUI_GRAPH_BORDER_RIGHT];
            bb = g->border[GUI_GRAPH_BORDER_BOTTOM];
            bl = g->border[GUI_GRAPH_BORDER_LEFT];
            
            x = guii_widget_getabsolutex(h);        /* Get absolute X position */
            y = guii_widget_getabsolutey(h);        /* Get absolute Y position */
            width = gui_widget_getwidth(h, 0);      /* Get widget width */
            height = gui_widget_getheight(h, 0);    /* Get widget height */
            
            gui_draw_filledrectangle(disp, x, y, bl, height, guii_widget_getcolor(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + bl, y, width - bl - br, bt, guii_widget_getcolor(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + bl, y + height - bb, width - bl - br, bb, guii_widget_getcolor(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + width - br, y, br, height, guii_widget_getcolor(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + bl, y + bt, width - bl - br, height - bt - bb, guii_widget_getcolor(h, GUI_GRAPH_COLOR_FG));
            gui_draw_rectangle(disp, x, y, width, height, guii_widget_getcolor(h, GUI_GRAPH_COLOR_BORDER));
            
            /* Draw horizontal lines */
            if (g->rows) {
                float step;
                step = (float)(height - bt - bb) / (float)g->rows;
                for (i = 1; i < g->rows; i++) {
                    gui_draw_hline(disp, GUI_DIM(x + bl), GUI_DIM(y + bt + i * step), GUI_DIM(width - bl - br), guii_widget_getcolor(h, GUI_GRAPH_COLOR_GRID));
                }
            }
            /* Draw vertical lines */
            if (g->columns) {
                float step;
                step = (float)(width - bl - br) / (float)g->columns;
                for (i = 1; i < g->columns; i++) {
                    gui_draw_vline(disp, GUI_DIM(x + bl + i * step), GUI_DIM(y + bt), GUI_DIM(height - bt - bb), guii_widget_getcolor(h, GUI_GRAPH_COLOR_GRID));
                }
            }
            
            /* Check if any data attached to this graph */
            if (gui_linkedlist_hasentries(&g->root)) {  /* We have attached plots */
                gui_display_t display;
                register float x1, y1, x2, y2;      /* Try to add these variables to core registers */
                float xSize = g->visible_max_x - g->visible_min_x;  /* Calculate X size */
                float ySize = g->visible_max_y - g->visible_min_y;  /* Calculate Y size */
                float xStep = (float)(width - bl - br) / (float)xSize;  /* Calculate X step */
                float yStep = (float)(height - bt - bb) / (float)ySize; /* calculate Y step */
                gui_dim_t yBottom = y + height - bb - 1;    /* Bottom Y value */
                gui_dim_t xLeft = x + bl;                   /* Left X position */
                uint32_t read, write;
                
                memcpy(&display, disp, sizeof(gui_display_t));  /* Save GUI display data */
                
                /* Set clipping region */
                if ((x + bl) > disp->x1) {
                    disp->x1 = x + bl;
                }
                if ((x + width - br) < disp->x2) {
                    disp->x2 = x + width - br;
                }
                if ((y + bt) > disp->y1) {
                    disp->y1 = y + bt;
                }
                if ((y + height - bb) < disp->y2) {
                    disp->y2 = y + height - bb;
                }
                
                /* Draw all plot attached to graph */
                for (link = gui_linkedlist_multi_getnext_gen(&g->root, NULL); link != NULL; 
                        link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
                    data = (gui_graph_data_p)gui_linkedlist_multi_getdata(link);/* Get data from list */
                    
                    read = data->ptr;               /* Get start read pointer */
                    write = data->ptr;              /* Get start write pointer */
                    
                    if (data->type == GUI_GRAPH_TYPE_YT) {  /* Draw YT plot */
                        /* Calculate first point */
                        x1 = xLeft - g->visible_min_x * xStep;  /* Calculate start X */
                        y1 = yBottom - (data->data[read] - g->visible_min_y) * yStep;   /* Calculate start Y */
                        if (++read == data->length) {   /* Check overflow */
                            read = 0;
                        }
                        
                        /* Outside of right || outside on left */
                        if (x1 > disp->x2 || (x1 + (data->length * xStep)) < disp->x1) {/* Plot start is on the right of active area */
                            continue;
                        }
                        
                        while (read != write && x1 <= disp->x2) {   /* Calculate next points */
                            x2 = x1 + xStep;                /* Calculate next X */
                            y2 = yBottom - ((float)data->data[read] - g->visible_min_y) * yStep;/* Calculate next Y */
                            if ((x1 >= disp->x1 || x2 >= disp->x1) && (x1 < disp->x2 || x2 < disp->x2)) {
                                gui_draw_line(disp, GUI_DIM(x1), GUI_DIM(y1), GUI_DIM(x2), GUI_DIM(y2), data->color);   /* Draw actual line */
                            }
                            x1 = x2, y1 = y2;       /* Copy values as old */
                            
                            if (++read == data->length) {   /* Check overflow */
                                read = 0;
                            }
                        }
                    } else if (data->type == GUI_GRAPH_TYPE_XY) {   /* Draw XY plot */                        
                        /* Calculate first point */
                        x1 = xLeft + ((float)data->data[2 * read + 0] - g->visible_min_x) * xStep;
                        y1 = yBottom - ((float)data->data[2 * read + 1] - g->visible_min_y) * yStep;
                        if (++read == data->length) {   /* Check overflow */
                            read = 0;
                        }
                        
                        while (read != write) {     /* Calculate next points */
                            x2 = xLeft + ((float)(data->data[2 * read + 0] - g->visible_min_x) * xStep);
                            y2 = yBottom - ((float)(data->data[2 * read + 1] - g->visible_min_y) * yStep);
                            gui_draw_line(disp, GUI_DIM(x1), GUI_DIM(y1), GUI_DIM(x2), GUI_DIM(y2), data->color);   /* Draw actual line */
                            x1 = x2, y1 = y2;       /* Check overflow */
                            
                            if (++read == data->length) {   /* Check overflow */
                                read = 0;
                            }
                        }
                    }
                }
                memcpy(disp, &display, sizeof(gui_display_t));  /* Copy data back */
            }
            return 1;
        }
#if GUI_CFG_USE_TOUCH
        case GUI_WC_TouchStart: {                   /* Touch down event */
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            uint8_t i = 0;
            for (i = 0; i < ts->ts.count; i++) {
                tx[i] = ts->x_rel[i];               /* Relative X position on widget */
                ty[i] = ts->y_rel[i];               /* Relative Y position on widget */
            }
            GUI_WIDGET_RESULTTYPE_TOUCH(result) = touchHANDLED;  /* Set touch status */
            return 1;
        }
        case GUI_WC_TouchMove: {                    /* Touch move event */
            guii_touch_data_t* ts = GUI_WIDGET_PARAMTYPE_TOUCH(param);  /* Get touch data */
            uint8_t i;
            gui_dim_t x, y;
            float diff, step;
            
            if (ts->ts.count == 1) {                /* Move graph on single widget */
                x = ts->x_rel[0];
                y = ts->y_rel[0];
                
                step = (float)(gui_widget_getwidth(h, 0) - g->border[GUI_GRAPH_BORDER_LEFT] - g->border[GUI_GRAPH_BORDER_RIGHT]) / (float)(g->visible_max_x - g->visible_min_x);
                diff = (float)(x - tx[0]) / step;
                g->visible_min_x -= diff;
                g->visible_max_x -= diff;
                
                step = (float)(gui_widget_getheight(h, 0) - g->border[GUI_GRAPH_BORDER_TOP] - g->border[GUI_GRAPH_BORDER_BOTTOM]) / (float)(g->visible_max_y - g->visible_min_y);
                diff = (float)(y - ty[0]) / step;
                g->visible_min_y += diff;
                g->visible_max_y += diff;
#if GUI_CFG_TOUCH_MAX_PRESSES > 1
            } else if (ts->ts.count == 2) {         /* Scale widget on multiple touches */
                float centerX, centerY, zoom;
                
                gui_math_centerofxy(ts->x_rel[0], ts->y_rel[0], ts->x_rel[1], ts->y_rel[1], &centerX, &centerY);    /* Calculate center position between points */
                zoom = ts->distance / ts->distance_old; /* Calculate zoom value */
                
                graph_zoom(h, zoom, (float)centerX / (float)gui_widget_getwidth(h, 0), (float)centerY / (float)gui_widget_getheight(h, 0));
#endif /* GUI_CFG_TOUCH_MAX_PRESSES > 1 */
            }
            
            for (i = 0; i < ts->ts.count; i++) {
                tx[i] = ts->x_rel[i];               /* Relative X position on widget */
                ty[i] = ts->y_rel[i];               /* Relative Y position on widget */
            }
            
            guii_widget_invalidate(h);
            return 1;
        }
        case GUI_WC_TouchEnd:
            return 1;
#endif /* GUI_CFG_USE_TOUCH */
        case GUI_WC_DblClick:
            graph_reset(h);                         /* Reset zoom */
            guii_widget_invalidate(h);              /* Invalidate widget */
            return 1;
        
#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
        case GUI_WC_Remove: {                       /* When widget is about to be removed */
            gui_graph_data_p data;
            gui_linkedlistmulti_t* link;
            
            /* Go through all data objects in this widget */
            for (link = gui_linkedlist_multi_getnext_gen(&g->root, NULL); link != NULL;
                    link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
                data = (gui_graph_data_p)gui_linkedlist_multi_getdata(link);    /* Get data from list */
                gui_linkedlist_multi_find_remove(&data->root, h);   /* Remove element from linked list with search */
            }
            
            return 1;
        }
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
/**
 * \brief           Invalidate all graphs where data plot is attached at
 * \param[in]       data: Data handle
 */
static void
graph_invalidate(gui_graph_data_p data) {
    gui_handle_p h;
    gui_linkedlistmulti_t* link;

    /* Invalidate all graphs attached to this data plot */
    for (link = gui_linkedlist_multi_getnext_gen(&data->root, NULL); link != NULL;
            link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
        /* Linked list of graph member in data structure is not on top */
        h = (gui_handle_p)gui_linkedlist_multi_getdata(link); /* Get data from linked list object */
        
        /* Invalidate each object attached to this data graph */
        guii_widget_invalidate(h);
    }
}
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */

/**
 * \brief           Create new graph widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       cb: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for create procedure
 * \param[in]       protect: Set to `1` to protect agains multiple access
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_graph_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect) {
    return (gui_handle_p)guii_widget_create(&widget, id, x, y, width, height, parent, cb, flags, protect);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index
 * \param[in]       color: Color value
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_graph_setcolor(gui_handle_p h, gui_graph_color_t index, gui_color_t color, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setcolor(h, (uint8_t)index, color, protect); /* Set color */
}

/**
 * \brief           Set minimal X value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal X value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_setmaxx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t
gui_graph_setminx(gui_handle_p h, float v, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MIN_X, &v, 1, 0, protect);   /* Set parameter */
}

/**
 * \brief           Set maximal X value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New maximal X value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_setminx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t
gui_graph_setmaxx(gui_handle_p h, float v, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MAX_X, &v, 1, 0, protect);   /* Set parameter */
}

/**
 * \brief           Set minimal Y value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal Y value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_setminx, gui_graph_setmaxx, gui_graph_setmaxy
 */
uint8_t
gui_graph_setminy(gui_handle_p h, float v, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MIN_Y, &v, 1, 0, protect);   /* Set parameter */
}

/**
 * \brief           Set maximal Y value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New maximal Y value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_setminx, gui_graph_setmaxx, gui_graph_setminy
 */
uint8_t
gui_graph_setmaxy(gui_handle_p h, float v, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_MAX_Y, &v, 1, 0, protect);   /* Set parameter */
}

/**
 * \brief           Set all axes
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal X value
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_setmaxx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t
gui_graph_setaxes(gui_handle_p h, float min_x, float max_x, float min_y, float max_y, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    guii_widget_setparam(h, CFG_MIN_X, &min_x, 0, 0, 0);
    guii_widget_setparam(h, CFG_MAX_X, &max_x, 0, 0, 0);
    guii_widget_setparam(h, CFG_MIN_Y, &min_y, 0, 0, 0);
    guii_widget_setparam(h, CFG_MAX_Y, &max_y, 1, 0, 0);    /* Set param and invalidate */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Reset zoom of widget
 * \param[in,out]   h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_graph_zoomreset(gui_handle_p h, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */
    return guii_widget_setparam(h, CFG_ZOOM_RESET, NULL, 1, 0, protect);/* Set parameter */
}

/**
 * \brief           Zoom widget display data
 * \param[in,out]   h: Widget handle
 * \param[in]       zoom: Zoom coeficient. Use 2.0f to double zoom, use 0.5f to unzoom 2 times, etc.
 * \param[in]       x: X coordinate on plot where zoom focus will apply. Valid value between 0 and 1 relative to width area. Use 0.5f to zoom into center area
 * \param[in]       y: Y coordinate on plot where zoom focus will apply. Valid value between 0 and 1 relative to height area. Use 0.5f to zoom into center area
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_graph_zoom(gui_handle_p h, float zoom, float x, float y, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    graph_zoom(h, zoom, x, y);                      /* Reset zoom */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Attach new data object to graph widget
 * \param[in,out]   h: Graph widget handle
 * \param[in]       data: Data object handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_detachdata
 */
uint8_t
gui_graph_attachdata(gui_handle_p h, gui_graph_data_p data, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    /* Linked list of data plots for this graph */
    gui_linkedlist_multi_add_gen(&__GG(h)->root, data);

#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    /*
     * Linked list of graphs for this data plot
     * This linked list is not on top!
     * Must subtract list element offset when using graphs from data
     */
    gui_linkedlist_multi_add_gen(&data->root, h);
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Detach existing data object from graph widget
 * \param[in,out]   h: Graph widget handle
 * \param[in]       data: Data object handle
 * \return          `1` on success, `0` otherwise
 * \sa              gui_graph_attachdata
 */
uint8_t
gui_graph_detachdata(gui_handle_p h, gui_graph_data_p data, const uint8_t protect) {
    __GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && data != NULL);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    /*
     * Linked list of data plots for this graph
     * Remove data from graph's linked list
     */
    gui_linkedlist_multi_find_remove(&__GG(h)->root, data);

#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    /*
     * Linked list of graphs for this data plot
     * Remove graph from data linked list
     */
    gui_linkedlist_multi_find_remove(&data->root, h);
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Creates data object according to specific type
 * \note            Data type used in graph widget is 2-byte (short int)
 * 
 * \note            When \arg GUI_GRAPH_TYPE_XY is used, 2 * length * sizeof(short int) of bytes is allocated for X and Y value
 * \param[in]       id: Graph data ID
 * \param[in]       type: Type of data. According to selected type different allocation size will occur
 * \param[in]       length: Number of points on plot.
 * \return          Graph data handle on success, NULL otherwise
 */
gui_graph_data_p
gui_graph_data_create(gui_id_t id, gui_graph_type_t type, size_t length, const uint8_t protect) {
    gui_graph_data_t* data;

    __GUI_ENTER(protect);                           /* Enter GUI */
    data = GUI_MEMALLOC(sizeof(*data), 0);          /* Allocate memory for basic widget */
    if (data != NULL) {
        data->id = id;
        data->type = type;
        data->length = length;
        if (type == GUI_GRAPH_TYPE_YT) {            /* Only Y values are stored */
            data->data = GUI_MEMALLOC(sizeof(*data->data) * length, 0); /* Store Y values for plot */
        } else {
            data->data = GUI_MEMALLOC(sizeof(*data->data) * length * 2, 0); /* Store X and Y values for plot */
        }
        if (data->data == NULL) {
            GUI_MEMFREE(data, 0);                   /* Remove widget because data memory could not be allocated */
            data = NULL;
        }
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */
    
    return (gui_graph_data_p)data;
}

/**
 * \brief           Add new value to the end of data object
 * \param[in]       data: Data object handle
 * \param[in]       x: X position for point. Used only in case data type is \ref GUI_GRAPH_TYPE_XY, otherwise it is ignored
 * \param[in]       y: Y position for point. Always used no matter of data type
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_graph_data_addvalue(gui_graph_data_p data, int16_t x, int16_t y, const uint8_t protect) {
    __GUI_ASSERTPARAMS(data);                       /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    if (data->type == GUI_GRAPH_TYPE_YT) {          /* YT plot */
        data->data[data->ptr] = y;                  /* Only Y value is relevant */
    } else if (data->type == GUI_GRAPH_TYPE_XY) {   /* XY plot */
        data->data[2 * data->ptr + 0] = x;          /* Set X value */
        data->data[2 * data->ptr + 1] = y;          /* Set Y value */
    }
    data->ptr++;                                    /* Increase write and read pointers */
    if (data->ptr >= data->length) {
        data->ptr = 0;                              /* Reset read operation */
    }
#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    graph_invalidate(data);                         /* Invalidate graphs attached to this data object */
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Set color for graph data
 * \param[in,out]   data: Pointer to \ref gui_graph_data_p structure with valid data
 * \param[in]       color: New color for data
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_graph_data_setcolor(gui_graph_data_p data, gui_color_t color, const uint8_t protect) {
    __GUI_ASSERTPARAMS(data != NULL);               /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    if (data->color != color) {                     /* Check color change */
        data->color = color;                        /* Set new color */
#if GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
        graph_invalidate(data);                     /* Invalidate graphs attached to this data object */
#endif /* GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */

    return 1;
}

/**
 * \brief           Get data collection with specific ID from graph
 * \param[in]       graph_h: Graph widget handle
 * \param[in]       id: Data collection ID
 * \return          Data collection handle
 */
gui_graph_data_p
gui_graph_data_get_by_id(gui_handle_p graph_h, gui_id_t id, const uint8_t protect) {
    gui_graph_data_p data = NULL;
    gui_linkedlistmulti_t* link;
    
    __GUI_ASSERTPARAMS(graph_h != NULL && graph_h->widget == &widget);  /* Check input parameters */

    __GUI_ENTER(protect);                           /* Enter GUI */
    /* Draw all plot attached to graph */
    for (link = gui_linkedlist_multi_getnext_gen(&__GG(graph_h)->root, NULL); link != NULL; 
            link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
        data = gui_linkedlist_multi_getdata(link);  /* Get data from list */
        if (data != NULL && data->id == id) {
            break;
        }
    }
    __GUI_LEAVE(protect);                           /* Leave GUI */
    return link != NULL ? data : NULL;
}

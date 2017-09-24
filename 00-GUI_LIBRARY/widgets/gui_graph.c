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
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui_graph.h"
#include "math.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define __GG(x)             ((GUI_GRAPH_t *)(x))

#define CFG_MIN_X           0x01
#define CFG_MAX_X           0x02
#define CFG_MIN_Y           0x03
#define CFG_MAX_Y           0x04
#define CFG_ZOOM_RESET      0x05

static uint8_t gui_graph_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_GRAY,                                 /*!< Default background color */
    GUI_COLOR_BLACK,                                /*!< Default foreground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
    0xFF002F00,                                     /*!< Default grid color */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("GRAPH"),                           /*!< Widget name */
    .Size = sizeof(GUI_GRAPH_t),                    /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_graph_callback,                 /*!< Callback function for various events */
    .Colors = Colors,                               /*<! List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define g       ((GUI_GRAPH_t *)h)

/* Reset zoom control on graph */
static void
graph_reset(GUI_HANDLE_p h) {
    g->VisibleMaxX = g->MaxX;
    g->VisibleMinX = g->MinX;
    g->VisibleMaxY = g->MaxY;
    g->VisibleMinY = g->MinY;
}

/* Zoom plot */
static void
graph_zoom(GUI_HANDLE_p h, float zoom, float xpos, float ypos) {
    if (xpos < 0 || xpos > 1) { xpos = 0.5; }
    if (ypos < 0 || ypos > 1) { ypos = 0.5; }
           
    g->VisibleMinX += (g->VisibleMaxX - g->VisibleMinX) * (zoom - 1.0f) * xpos;
    g->VisibleMaxX -= (g->VisibleMaxX - g->VisibleMinX) * (zoom - 1.0f) * (1.0f - xpos);

    g->VisibleMinY += (g->VisibleMaxY - g->VisibleMinY) * (zoom - 1.0f) * ypos;
    g->VisibleMaxY -= (g->VisibleMaxY - g->VisibleMinY) * (zoom - 1.0f) * (1.0f - ypos);
}

static uint8_t
gui_graph_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
static GUI_iDim_t tX[GUI_TOUCH_MAX_PRESSES], tY[GUI_TOUCH_MAX_PRESSES];
#endif /* GUI_USE_TOUCH */    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            g->Border[GUI_GRAPH_BORDER_TOP] = 5;    /* Set borders */
            g->Border[GUI_GRAPH_BORDER_RIGHT] = 5;
            g->Border[GUI_GRAPH_BORDER_BOTTOM] = 5;
            g->Border[GUI_GRAPH_BORDER_LEFT] = 5;

            g->MaxX = 10;
            g->MinX = -10;
            g->MaxY = 10;
            g->MinY = -10;
            graph_reset(h);                         /* Reset plot */

            g->Rows = 8;                            /* Number of rows */
            g->Columns = 10;                        /* Number of columns */
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter from widget */
            GUI_WIDGET_Param_t* p = (GUI_WIDGET_Param_t *)param;
            switch (p->Type) {
                case CFG_MIN_X: g->MinX = *(float *)p->Data; break; /* Set max X value to widget */
                case CFG_MAX_X: g->MaxX = *(float *)p->Data; break; /* Set max X value to widget */
                case CFG_MIN_Y: g->MinY = *(float *)p->Data; break; /* Set max X value to widget */
                case CFG_MAX_Y: g->MaxY = *(float *)p->Data; break; /* Set max X value to widget */
                case CFG_ZOOM_RESET: graph_reset(h); break; /* Reset zoom */
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {                         /* Draw widget */
            GUI_GRAPH_DATA_p data;
            GUI_LinkedListMulti_t* link;
            GUI_iDim_t bt, br, bb, bl, x, y, width, height;
            uint8_t i;
            GUI_Display_t* disp = (GUI_Display_t *)param;   /* Get display pointer */
            
            bt = g->Border[GUI_GRAPH_BORDER_TOP];
            br = g->Border[GUI_GRAPH_BORDER_RIGHT];
            bb = g->Border[GUI_GRAPH_BORDER_BOTTOM];
            bl = g->Border[GUI_GRAPH_BORDER_LEFT];
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute X position */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y position */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            gui_draw_filledrectangle(disp, x, y, bl, height, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + bl, y, width - bl - br, bt, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + bl, y + height - bb, width - bl - br, bb, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + width - br, y, br, height, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_BG));
            gui_draw_filledrectangle(disp, x + bl, y + bt, width - bl - br, height - bt - bb, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_FG));
            gui_draw_rectangle(disp, x, y, width, height, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_BORDER));
            
            /* Draw horizontal lines */
            if (g->Rows) {
                float step;
                step = (float)(height - bt - bb) / (float)g->Rows;
                for (i = 1; i < g->Rows; i++) {
                    gui_draw_hline(disp, x + bl, y + bt + i * step, width - bl - br, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_GRID));
                }
            }
            /* Draw vertical lines */
            if (g->Columns) {
                float step;
                step = (float)(width - bl - br) / (float)g->Columns;
                for (i = 1; i < g->Columns; i++) {
                    gui_draw_vline(disp, x + bl + i * step, y + bt, height - bt - bb, gui_widget_getcolor__(h, GUI_GRAPH_COLOR_GRID));
                }
            }
            
            /* Check if any data attached to this graph */
            if (g->Root.First) {                    /* We have attached plots */
                GUI_Display_t display;
                register float x1, y1, x2, y2;      /* Try to add these variables to core registers */
                float xSize = g->VisibleMaxX - g->VisibleMinX;  /* Calculate X size */
                float ySize = g->VisibleMaxY - g->VisibleMinY;  /* Calculate Y size */
                float xStep = (float)(width - bl - br) / (float)xSize;  /* Calculate X step */
                float yStep = (float)(height - bt - bb) / (float)ySize; /* calculate Y step */
                GUI_Dim_t yBottom = y + height - bb - 1;    /* Bottom Y value */
                GUI_Dim_t xLeft = x + bl;                   /* Left X position */
                uint32_t read, write;
                
                memcpy(&display, disp, sizeof(GUI_Display_t));  /* Save GUI display data */
                
                /* Set clipping region */
                if ((x + bl) > disp->X1) {
                    disp->X1 = x + bl;
                }
                if ((x + width - br) < disp->X2) {
                    disp->X2 = x + width - br;
                }
                if ((y + bt) > disp->Y1) {
                    disp->Y1 = y + bt;
                }
                if ((y + height - bb) < disp->Y2) {
                    disp->Y2 = y + height - bb;
                }
                
                /* Draw all plot attached to graph */
                for (link = gui_linkedlist_multi_getnext_gen(&g->Root, NULL); link; 
                        link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
                    data = (GUI_GRAPH_DATA_p)gui_linkedlist_multi_getdata(link);/* Get data from list */
                    
                    read = data->Ptr;               /* Get start read pointer */
                    write = data->Ptr;              /* Get start write pointer */
                    
                    if (data->Type == GUI_GRAPH_TYPE_YT) {  /* Draw YT plot */
                        /* Calculate first point */
                        x1 = xLeft - g->VisibleMinX * xStep;/* Calculate start X */
                        y1 = yBottom - (data->Data[read] - g->VisibleMinY) * yStep;/* Calculate start Y */
                        if (++read == data->Length) {   /* Check overflow */
                            read = 0;
                        }
                        
                        /* Outside of right || outside on left */
                        if (x1 > disp->X2 || (x1 + (data->Length * xStep)) < disp->X1) {/* Plot start is on the right of active area */
                            continue;
                        }
                        
                        while (read != write && x1 <= disp->X2) {   /* Calculate next points */
                            x2 = x1 + xStep;                /* Calculate next X */
                            y2 = yBottom - ((float)data->Data[read] - g->VisibleMinY) * yStep;  /* Calculate next Y */
                            if ((x1 >= disp->X1 || x2 >= disp->X1) && (x1 < disp->X2 || x2 < disp->X2)) {
                                gui_draw_line(disp, x1, y1, x2, y2, data->Color);   /* Draw actual line */
                            }
                            x1 = x2, y1 = y2;       /* Copy values as old */
                            
                            if (++read == data->Length) {   /* Check overflow */
                                read = 0;
                            }
                        }
                    } else if (data->Type == GUI_GRAPH_TYPE_XY) {   /* Draw XY plot */                        
                        /* Calculate first point */
                        x1 = xLeft + ((float)data->Data[2 * read + 0] - g->VisibleMinX) * xStep;
                        y1 = yBottom - ((float)data->Data[2 * read + 1] - g->VisibleMinY) * yStep;
                        if (++read == data->Length) {   /* Check overflow */
                            read = 0;
                        }
                        
                        while (read != write) {     /* Calculate next points */
                            x2 = xLeft + ((float)(data->Data[2 * read + 0] - g->VisibleMinX) * xStep);
                            y2 = yBottom - ((float)(data->Data[2 * read + 1] - g->VisibleMinY) * yStep);
                            gui_draw_line(disp, x1, y1, x2, y2, data->Color);   /* Draw actual line */
                            x1 = x2, y1 = y2;       /* Check overflow */
                            
                            if (++read == data->Length) {   /* Check overflow */
                                read = 0;
                            }
                        }
                    }
                }
                memcpy(disp, &display, sizeof(GUI_Display_t));  /* Copy data back */
            }
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {                   /* Touch down event */
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            uint8_t i = 0;
            for (i = 0; i < ts->TS.Count; i++) {
                tX[i] = ts->RelX[i];                /* Relative X position on widget */
                tY[i] = ts->RelY[i];                /* Relative Y position on widget */
            }
            *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Set touch status */
            return 1;
        }
        case GUI_WC_TouchMove: {                    /* Touch move event */
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            uint8_t i;
            GUI_iDim_t x, y;
            float diff, step;
            
            if (ts->TS.Count == 1) {                /* Move graph on single widget */
                x = ts->RelX[0];
                y = ts->RelY[0];
                
                step = (float)(gui_widget_getwidth__(h) - g->Border[GUI_GRAPH_BORDER_LEFT] - g->Border[GUI_GRAPH_BORDER_RIGHT]) / (float)(g->VisibleMaxX - g->VisibleMinX);
                diff = (float)(x - tX[0]) / step;
                g->VisibleMinX -= diff;
                g->VisibleMaxX -= diff;
                
                step = (float)(gui_widget_getheight__(h) - g->Border[GUI_GRAPH_BORDER_TOP] - g->Border[GUI_GRAPH_BORDER_BOTTOM]) / (float)(g->VisibleMaxY - g->VisibleMinY);
                diff = (float)(y - tY[0]) / step;
                g->VisibleMinY += diff;
                g->VisibleMaxY += diff;
#if GUI_TOUCH_MAX_PRESSES > 1
            } else if (ts->TS.Count == 2) {         /* Scale widget on multiple widgets */
                float centerX, centerY, zoom;
                
                gui_math_centerofxy(ts->RelX[0], ts->RelY[0], ts->RelX[1], ts->RelY[1], &centerX, &centerY);    /* Calculate center position between points */
                zoom = ts->Distance / ts->DistanceOld;  /* Calculate zoom value */
                
                graph_zoom(h, zoom, (float)centerX / (float)gui_widget_getwidth__(h), (float)centerY / (float)gui_widget_getheight__(h));
#endif /* GUI_TOUCH_MAX_PRESSES > 1 */
            }
            
            for (i = 0; i < ts->TS.Count; i++) {
                tX[i] = ts->RelX[i];                /* Relative X position on widget */
                tY[i] = ts->RelY[i];                /* Relative Y position on widget */
            }
            
            gui_widget_invalidate__(h);
            return 1;
        }
        case GUI_WC_TouchEnd:
            return 1;
#endif /* GUI_USE_TOUCH */
        case GUI_WC_DblClick:
            graph_reset(h);                         /* Reset zoom */
            gui_widget_invalidate__(h);             /* Invalidate widget */
            return 1;
#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
        case GUI_WC_Remove: {                       /* When widget is about to be removed */
            GUI_GRAPH_DATA_p data;
            GUI_LinkedListMulti_t* link;
            
            /**
             * Go through all data objects in this widget
             */
            for (link = gui_linkedlist_multi_getnext_gen(&g->Root, NULL); link;
                    link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
                data = (GUI_GRAPH_DATA_p)gui_linkedlist_multi_getdata(link);    /* Get data from list */
                gui_linkedlist_multi_find_remove(&data->Root, h);   /* Remove element from linked list with search */
            }
            
            return 1;
        }
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef g

#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
/* Invalidate graphs attached to data */
static void graph_invalidate(GUI_GRAPH_DATA_p data) {
    GUI_HANDLE_p h;
    GUI_LinkedListMulti_t* link;
    /**
     * Invalidate all graphs attached to this data plot
     */
    for (link = gui_linkedlist_multi_getnext_gen(&data->Root, NULL); link;
            link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
        /**
         * Linked list of graph member in data structure is not on top
         */
        h = (GUI_HANDLE_p)gui_linkedlist_multi_getdata(link); /* Get data from linked list object */
        
        /**
         * Invalidate each object attached to this data graph
         */
        gui_widget_invalidate__(h);
    }
}
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new graph widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for create procedure
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p
gui_graph_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_GRAPH_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t
gui_graph_setcolor(GUI_HANDLE_p h, GUI_GRAPH_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Set minimal X value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal X value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setmaxx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t
gui_graph_setminx(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MIN_X, &v, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set maximal X value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New maximal X value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setminx, gui_graph_setminy, gui_graph_setmaxy
 */
uint8_t
gui_graph_setmaxx(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MAX_X, &v, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set minimal Y value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New minimal Y value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setminx, gui_graph_setmaxx, gui_graph_setmaxy
 */
uint8_t
gui_graph_setminy(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MIN_Y, &v, 1, 0);   /* Set parameter */
}

/**
 * \brief           Set maximal Y value of plot
 * \param[in,out]   h: Widget handle
 * \param[in]       v: New maximal Y value
 * \retval          1: Value was set ok
 * \retval          0: Value was not set
 * \sa              gui_graph_setminx, gui_graph_setmaxx, gui_graph_setminy
 */
uint8_t
gui_graph_setmaxy(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_MAX_Y, &v, 1, 0);   /* Set parameter */
}

/**
 * \brief           Reset zoom of widget
 * \param[in,out]   h: Widget handle
 * \retval          1: Zoom was reseted
 * \retval          0: Zoom was not reseted
 */
uint8_t
gui_graph_zoomreset(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_ZOOM_RESET, NULL, 1, 0);  /* Set parameter */
}

/**
 * \brief           Zoom widget display data
 * \param[in,out]   h: Widget handle
 * \param[in]       zoom: Zoom coeficient. Use 2.0f to double zoom, use 0.5 to unzoom 2 times, etc.
 * \param[in]       x: X coordinate on plot where zoom focus will apply. Valid value between 0 and 1 relative to width area. Use 0.5 to zoom to center
 * \param[in]       y: Y coordinate on plot where zoom focus will apply. Valid value between 0 and 1 relative to height area. Use 0.5 to zoom to center
 * \retval          1: Zoom was reseted
 * \retval          0: Zoom was not reseted
 */
uint8_t
gui_graph_zoom(GUI_HANDLE_p h, float zoom, float x, float y) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    graph_zoom(h, zoom, x, y);                      /* Reset zoom */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Attach new data object to graph widget
 * \param[in,out]   h: Graph widget handle
 * \param[in]       data: Data object handle
 * \retval          1: Attaching was successful
 * \retval          0: Attaching failed
 * \sa              gui_graph_detachdata
 */
uint8_t
gui_graph_attachdata(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    /**
     * Linked list of data plots for this graph
     */
    gui_linkedlist_multi_add_gen(&__GG(h)->Root, data);

#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    /**
     * Linked list of graphs for this data plot
     * This linked list is not on top!
     * Must subtract list element offset when using graphs from data
     */
    gui_linkedlist_multi_add_gen(&data->Root, h);
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Detach existing data object from graph widget
 * \param[in,out]   h: Graph widget handle
 * \param[in]       data: Data object handle
 * \retval          1: Detaching was successful
 * \retval          0: Detaching failed
 * \sa              gui_graph_attachdata
 */
uint8_t
gui_graph_detachdata(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget && data);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    /**
     * Linked list of data plots for this graph
     * Remove data from graph's linked list
     */
    gui_linkedlist_multi_find_remove(&__GG(h)->Root, data);

#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    /**
     * Linked list of graphs for this data plot
     * Remove graph from data linked list
     */
    gui_linkedlist_multi_find_remove(&data->Root, h);
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/*************************/
/* GRAPH DATA functions  */
/*************************/

/**
 * \brief           Creates data object according to specific type
 * \note            Data type used in graph widget is 2-byte (short int)
 * 
 * \note            When \arg GUI_GRAPH_TYPE_XY is used, 2 * length * sizeof(short int) of bytes is allocated for X and Y value
 * \param[in]       type: Type of data. According to selected type different allocation size will occur
 * \param[in]       length: Number of points on plot.
 * \retval          > 0: \ref GUI_GRAPH_DATA_p object of created widget
 * \retval          0: Data creation failed
 */
GUI_GRAPH_DATA_p
gui_graph_data_create(GUI_GRAPH_TYPE_t type, size_t length) {
    GUI_GRAPH_DATA_t* data;

    data = GUI_MEMALLOC(sizeof(*data));             /* Allocate memory for basic widget */
    if (data) {
        __GUI_ENTER();                              /* Enter GUI */
        data->Type = type;
        data->Length = length;
        if (type == GUI_GRAPH_TYPE_YT) {            /* Only Y values are stored */
            data->Data = GUI_MEMALLOC(length * sizeof(*data->Data));    /* Store Y values for plot */
        } else {
            data->Data = GUI_MEMALLOC(length * 2 * sizeof(*data->Data));    /* Store X and Y values for plot */
        }
        if (!data->Data) {
            GUI_MEMFREE(data);                      /* Remove widget because data memory could not be allocated */
        }
        __GUI_LEAVE();                              /* Leave GUI */
    }
    
    return (GUI_GRAPH_DATA_p)data;
}

/**
 * \brief           Add new value to the end of data object
 * \param[in]       data: Data object handle
 * \param[in]       x: X position for point. Used only in case data type is \ref GUI_GRAPH_TYPE_XY, otherwise it is ignored
 * \param[in]       y: Y position for point. Always used no matter of data type
 * \retval          1: Value was added to data object ok
 * \retval          0: Value was not added to data object
 */
uint8_t
gui_graph_data_addvalue(GUI_GRAPH_DATA_p data, int16_t x, int16_t y) {
    __GUI_ASSERTPARAMS(data);                       /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (data->Type == GUI_GRAPH_TYPE_YT) {          /* YT plot */
        data->Data[data->Ptr] = y;                  /* Only Y value is relevant */
    } else if (data->Type == GUI_GRAPH_TYPE_XY) {   /* XY plot */
        data->Data[2 * data->Ptr + 0] = x;          /* Set X value */
        data->Data[2 * data->Ptr + 1] = y;          /* Set Y value */
    }
    
    data->Ptr++;                                    /* Increase write and read pointers */
    if (data->Ptr >= data->Length) {
        data->Ptr = 0;                              /* Reset read operation */
    }
    
#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    graph_invalidate(data);                         /* Invalidate graphs attached to this data object */
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Set color for graph data
 * \param[in,out]   data: Pointer to \ref GUI_GRAPH_DATA_p structure with valid data
 * \param[in]       color: New color for data
 * \retval          1: New color set ok
 * \retval          0: New color was not set
 */
uint8_t
gui_graph_data_setcolor(GUI_GRAPH_DATA_p data, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(data);                       /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (data->Color != color) {                     /* Check color change */
        data->Color = color;                        /* Set new color */
#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
        graph_invalidate(data);                     /* Invalidate graphs attached to this data object */
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

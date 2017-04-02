/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
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


/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        _T("Graph"),                                /*!< Widget name */
        sizeof(GUI_GRAPH_t),                        /*!< Size of widget for memory allocation */
        0,                                          /*!< List of widget flags */
    },
    GUI_GRAPH_Callback                              /*!< Callback function for various events */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define g       ((GUI_GRAPH_t *)h)
uint8_t GUI_GRAPH_Callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
#if GUI_USE_TOUCH
static GUI_iDim_t tX[GUI_TOUCH_MAX_PRESSES], tY[GUI_TOUCH_MAX_PRESSES];
#endif /* GUI_USE_TOUCH */    
    switch (cmd) {                                  /* Handle control function if required */
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
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X position */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y position */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            GUI_DRAW_FilledRectangle(disp, x, y, bl, height, g->Color[GUI_GRAPH_COLOR_BG]);
            GUI_DRAW_FilledRectangle(disp, x + bl, y, width - bl - br, bt, g->Color[GUI_GRAPH_COLOR_BG]);
            GUI_DRAW_FilledRectangle(disp, x + bl, y + height - bb, width - bl - br, bb, g->Color[GUI_GRAPH_COLOR_BG]);
            GUI_DRAW_FilledRectangle(disp, x + width - br, y, br, height, g->Color[GUI_GRAPH_COLOR_BG]);
            GUI_DRAW_FilledRectangle(disp, x + bl, y + bt, width - bl - br, height - bt - bb, g->Color[GUI_GRAPH_COLOR_FG]);
            GUI_DRAW_Rectangle(disp, x, y, width, height, g->Color[GUI_GRAPH_COLOR_BORDER]);
            
            /* Draw horizontal lines */
            if (g->Rows) {
                float step;
                step = (float)(height - bt - bb) / (float)g->Rows;
                for (i = 1; i < g->Rows; i++) {
                    GUI_DRAW_HLine(disp, x + bl, y + bt + i * step, width - bl - br, g->Color[GUI_GRAPH_COLOR_GRID]);
                }
            }
            /* Draw vertical lines */
            if (g->Columns) {
                float step;
                step = (float)(width - bl - br) / (float)g->Columns;
                for (i = 1; i < g->Columns; i++) {
                    GUI_DRAW_VLine(disp, x + bl + i * step, y + bt, height - bt - bb, g->Color[GUI_GRAPH_COLOR_GRID]);
                }
            }
            
            /* Check if any data attached to this graph */
            if (g->Root.First) {                            /* We have attached plots */
                GUI_Display_t display;
                register GUI_iDim_t x1, y1, x2, y2;         /* Try to add these variables to core registers */
                float xSize = g->MaxX - g->MinX;            /* Calculate X size */
                float ySize = g->MaxY - g->MinY;            /* Calculate Y size */
                float xStep = (float)(width - bl - br) / (float)xSize;   /* Calculate X step */
                float yStep = (float)(height - bt - bb) / (float)ySize;  /* calculate Y step */
                GUI_Dim_t yBottom = y + height - bb - 1;    /* Bottom Y value */
                GUI_Dim_t xLeft = x + bl;                   /* Left X position */
                uint32_t read, write;
                
                memcpy(&display, disp, sizeof(GUI_Display_t));  /* Save GUI display data */
                
                /* Set clipping region */
                disp->X1 = x + bl;
                disp->X2 = disp->X1 + width - bl - br;
                disp->Y1 = y + bt;
                disp->Y2 = disp->Y1 + height - bt - bb;
                
                /* Draw all plot attached to graph */
                for (link = __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(&g->Root, 0); link; link = __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(0, link)) {
                    data = (GUI_GRAPH_DATA_p)__GUI_LINKEDLIST_MULTI_GetData(link);  /* Get data from list */
                    
                    read = data->Ptr;                       /* Get start read pointer */
                    write = data->Ptr;                      /* Get start write pointer */
                    
                    if (data->Type == GUI_GRAPH_TYPE_YT) {  /* Draw YT plot */
                        /* Calculate first point */
                        x1 = xLeft - g->MinX * xStep;       /* Calculate start X */
                        y1 = yBottom - (data->Data[read] - g->MinY) * yStep;    /* Calculate start Y */
                        if (++read == data->Length) {       /* Check overflow */
                            read = 0;
                        }
                        
                        /* Outside of right || outside on left */
                        if (x1 > disp->X2 || (x1 + (data->Length * xStep)) < disp->X1) {    /* Plot start is on the right of active area */
                            continue;
                        }
                        
                        while (read != write && x1 <= disp->X2) {   /* Calculate next points */
                            x2 = x1 + xStep;                /* Calculate next X */
                            y2 = yBottom - ((float)data->Data[read] - g->MinY) * yStep; /* Calculate next Y */
                            if (x1 >= disp->X1 && x1 < disp->X2) {
                                GUI_DRAW_Line(disp, x1, y1, x2, y2, data->Color);   /* Draw actual line */
                            }
                            x1 = x2, y1 = y2;               /* Copy values as old */
                            
                            if (++read == data->Length) {   /* Check overflow */
                                read = 0;
                            }
                        }
                    } else if (data->Type == GUI_GRAPH_TYPE_XY) {   /* Draw XY plot */                        
                        /* Calculate first point */
                        x1 = xLeft + ((float)data->Data[2 * read + 0] - g->MinX) * xStep;
                        y1 = yBottom - ((float)data->Data[2 * read + 1] - g->MinY) * yStep;
                        if (++read == data->Length) {       /* Check overflow */
                            read = 0;
                        }
                        
                        while (read != write) {             /* Calculate next points */
                            x2 = xLeft + ((float)(data->Data[2 * read + 0] - g->MinX) * xStep);
                            y2 = yBottom - ((float)(data->Data[2 * read + 1] - g->MinY) * yStep);
                            GUI_DRAW_Line(disp, x1, y1, x2, y2, data->Color);   /* Draw actual line */
                            x1 = x2, y1 = y2;               /* Check overflow */
                            
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
        case GUI_WC_TouchStart: {                    /* Touch down event */
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            uint8_t i = 0;
            for (i = 0; i < ts->TS.Count; i++) {
                tX[i] = ts->RelX[i];                /* Relative X position on widget */
                tY[i] = ts->RelY[i];                /* Relative Y position on widget */
            }
            *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Set touch status */
            __GUI_DEBUG("TOUCH START\r\n");
            return 1;
        }
        case GUI_WC_TouchMove: {                    /* Touch move event */
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            uint8_t i;
            GUI_iDim_t x, y;
            float diff;
            float step;
            __GUI_DEBUG("TOUCH MOVE\r\n");
            
            if (ts->TS.Count == 1) {                /* Move graph on single widget */
                x = ts->RelX[0];
                y = ts->RelY[0];
                
                step = (float)(__GUI_WIDGET_GetWidth(h) - __GG(h)->Border[GUI_GRAPH_BORDER_LEFT] - __GG(h)->Border[GUI_GRAPH_BORDER_RIGHT]) / (float)(__GG(h)->MaxX - __GG(h)->MinX);
                diff = (float)(x - tX[0]) / step;
                __GG(h)->MinX -= diff;
                __GG(h)->MaxX -= diff;
                
                step = (float)(__GUI_WIDGET_GetHeight(h) - __GG(h)->Border[GUI_GRAPH_BORDER_TOP] - __GG(h)->Border[GUI_GRAPH_BORDER_BOTTOM]) / (float)(__GG(h)->MaxY - __GG(h)->MinY);
                diff = (float)(y - tY[0]) / step;
                __GG(h)->MinY += diff;
                __GG(h)->MaxY += diff;
#if GUI_TOUCH_MAX_PRESSES > 1
            } else if (ts->TS.Count == 2) {         /* Scale widget on multiple widgets */
                float centerX, centerY, zoom, pos;
                
                GUI_MATH_CenterOfXY(ts->RelX[0], ts->RelY[0], ts->RelX[1], ts->RelY[1], &centerX, &centerY);    /* Calculate center position between points */
                zoom = ts->Distance / ts->DistanceOld;  /* Calculate zoom value */
                
                __GUI_DEBUG("Zoom: %.3f\r\n", zoom);
                
                pos = (float)centerX / (float)__GUI_WIDGET_GetWidth(h); /* Calculate X position where on plot should we zoom */
                g->MinX += (g->MaxX - g->MinX) * (zoom - 1.0f) * pos;
                g->MaxX -= (g->MaxX - g->MinX) * (zoom - 1.0f) * (1.0f - pos);
                
                pos = (float)centerY / (float)__GUI_WIDGET_GetHeight(h);/* Calculate Y position where on plot should we zoom */
                g->MinY += (g->MaxY - g->MinY) * (zoom - 1.0f) * pos;
                g->MaxY -= (g->MaxY - g->MinY) * (zoom - 1.0f) * (1.0f - pos);
#endif /* GUI_TOUCH_MAX_PRESSES > 1 */
            }
            
            for (i = 0; i < ts->TS.Count; i++) {
                tX[i] = ts->RelX[i];                /* Relative X position on widget */
                tY[i] = ts->RelY[i];                /* Relative Y position on widget */
            }
            
            __GUI_WIDGET_Invalidate(h);
            return 1;
        }
        case GUI_WC_TouchEnd:
            __GUI_DEBUG("TOUCH END\r\n");
            return 1;
        case GUI_WC_Click:
            __GUI_DEBUG("CLICK\r\n");
            return 1;
        case GUI_WC_DblClick:
            __GUI_DEBUG("DBL CLICK\r\n");
            return 1;
        case GUI_WC_LongClick:
            __GUI_DEBUG("LONG CLICK\r\n");
            return 1;
#endif /* GUI_USE_TOUCH */
#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
        case GUI_WC_Remove: {                       /* When widget is about to be removed */
            GUI_GRAPH_DATA_p data;
            GUI_LinkedListMulti_t* link;
            
            /**
             * Go through all data objects in this widget
             */
            for (link = __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(&g->Root, NULL); link; link = __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(0, link)) {
                data = (GUI_GRAPH_DATA_p)__GUI_LINKEDLIST_MULTI_GetData(link);  /* Get data from list */
                __GUI_LINKEDLIST_MULTI_FIND_REMOVE(&data->Root, h); /* Remove element from linked list with search */
            }
            
            return 1;
        }
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef g

#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
/* Invalidate graphs attached to data */
static void InvalidateGraphs(GUI_GRAPH_DATA_p data) {
    GUI_HANDLE_p h;
    GUI_LinkedListMulti_t* link;
    /**
     * Invalidate all graphs attached to this data plot
     */
    for (link = __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(&data->Root, NULL); link; link = __GUI_LINKEDLIST_MULTI_GETNEXT_GEN(NULL, link)) {
        /**
         * Linked list of graph member in data structure is not on top
         */
        h = (GUI_HANDLE_p)__GUI_LINKEDLIST_MULTI_GetData(link); /* Get data from linked list object */
        
        /**
         * Invalidate each object attached to this data graph
         */
        __GUI_WIDGET_Invalidate(h);
    }
}
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_GRAPH_Create(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_GRAPH_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_GRAPH_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, 0);  /* Allocate memory for basic widget */
    if (ptr) {        
        /* Color setup */
        ptr->Color[GUI_GRAPH_COLOR_BG] = GUI_COLOR_GRAY;    /* Set background color */
        ptr->Color[GUI_GRAPH_COLOR_FG] = GUI_COLOR_BLACK;   /* Set foreground color */
        ptr->Color[GUI_GRAPH_COLOR_BORDER] = GUI_COLOR_BLACK;   /* Set foreground color */
        ptr->Color[GUI_GRAPH_COLOR_GRID] = 0xFF002F00;  /* Set grid color */
        
        ptr->Border[GUI_GRAPH_BORDER_TOP] = 5;      /* Set borders */
        ptr->Border[GUI_GRAPH_BORDER_RIGHT] = 5;
        ptr->Border[GUI_GRAPH_BORDER_BOTTOM] = 5;
        ptr->Border[GUI_GRAPH_BORDER_LEFT] = 5;
        
        ptr->MaxX = 100;
        ptr->MinX = 1;
        ptr->MaxY = 100;
        ptr->MinY = 0;
        
        ptr->Rows = 8;                              /* Number of rows */
        ptr->Columns = 10;                          /* Number of columns */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

GUI_HANDLE_p GUI_GRAPH_SetColor(GUI_HANDLE_p h, GUI_GRAPH_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GG(h)->Color[index] != color) {
        __GG(h)->Color[index] = color;              /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_GRAPH_AttachData(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data) {
    __GUI_ASSERTPARAMS(h && data);
    __GUI_ENTER();                                  /* Enter GUI */
    
    /**
     * Linked list of data plots for this graph
     */
    __GUI_LINKEDLIST_MULTI_ADD_GEN(&__GG(h)->Root, data);

#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    /**
     * Linked list of graphs for this data plot
     * This linked list is not on top!
     * Must subtract list element offset when using graphs from data
     */
    __GUI_LINKEDLIST_MULTI_ADD_GEN(&data->Root, h);
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_GRAPH_DetachData(GUI_HANDLE_p h, GUI_GRAPH_DATA_p data) {
    __GUI_ASSERTPARAMS(h && data);
    __GUI_ENTER();                                  /* Enter GUI */
    
    /**
     * Linked list of data plots for this graph
     * Remove data from graph's linked list
     */
    __GUI_LINKEDLIST_MULTI_FIND_REMOVE(&__GG(h)->Root, data);

#if GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE
    /**
     * Linked list of graphs for this data plot
     * Remove graph from data linked list
     */
    __GUI_LINKEDLIST_MULTI_FIND_REMOVE(&data->Root, h);
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_GRAPH_SetMinX(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    if (__GG(h)->MinX != v) {
        __GG(h)->MinX = v;                          /* Set new parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_GRAPH_SetMaxX(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    if (__GG(h)->MaxX != v) {
        __GG(h)->MaxX = v;                          /* Set new parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_GRAPH_SetMinY(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    if (__GG(h)->MinY != v) {
        __GG(h)->MinY = v;                          /* Set new parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_p GUI_GRAPH_SetMaxY(GUI_HANDLE_p h, float v) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    if (__GG(h)->MaxY != v) {
        __GG(h)->MaxY = v;                          /* Set new parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}


/*************************/
/* GRAPH DATA functions  */
/*************************/
GUI_GRAPH_DATA_p GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_t type, uint16_t length) {
    GUI_GRAPH_DATA_p data;
    __GUI_ENTER();                                  /* Enter GUI */

    data = (GUI_GRAPH_DATA_p)__GUI_MEMALLOC(sizeof(GUI_GRAPH_DATA_t));  /* Allocate memory for basic widget */
    if (data) {
        memset((void *)data, 0x00, sizeof(GUI_GRAPH_DATA_t));   /* Reset memory */
        
        data->Type = type;
        data->Length = length;
        if (type == GUI_GRAPH_TYPE_YT) {            /* Only Y values are stored */
            data->Data = __GUI_MEMALLOC(length * sizeof(int16_t));
            if (data->Data) {
                memset(data->Data, 0x00, length * sizeof(int16_t));
            }
        } else {
            data->Data = __GUI_MEMALLOC(length * 2 * sizeof(int16_t));  /* Store X and Y value for plot */
            if (data->Data) {
                memset(data->Data, 0x00, length * 2 * sizeof(int16_t));
            }
        }
        if (!data->Data) {
            __GUI_MEMFREE(data);                    /* Remove widget because data memory could not be allocated */
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return data;
}

void GUI_GRAPH_DATA_AddValue(GUI_GRAPH_DATA_p data, int16_t x, int16_t y) {
    __GUI_ASSERTPARAMSVOID(data);                   /* Check parameters */
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
    InvalidateGraphs(data);                         /* Invalidate graphs attached to this data object */
#endif /* GUI_WIDGET_GRAPH_DATA_AUTO_INVALIDATE */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

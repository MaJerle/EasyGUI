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
#include "gui_graph.h"

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
#define __GD(x)             ((GUI_GRAPH_DATA_t *)(x))

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp);
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result);

#if GUI_USE_TOUCH  
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts);
#endif /* GUI_USE_TOUCH */

/* Control for graph data */
static uint8_t __ControlData(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    {
        "Graph",                                    /*!< Widget name */
        sizeof(GUI_GRAPH_t),                        /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    __Control,
    __Draw,                                         /*!< Widget draw function */
#if GUI_USE_TOUCH
    {
        __TouchDown,                                /*!< Touch down callback function */
        0,                                          /*!< Touch up callback function */
        0                                           /*!< Touch move callback function */
    }
#endif /* GUI_USE_TOUCH */
};
const static GUI_WIDGET_t WidgetData = {
    {
        "GraphData",                                /*!< Widget name */
        sizeof(GUI_GRAPH_DATA_t),                   /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    __ControlData,                                  /*!< Control function */
    0,                                              /*!< Widget draw function */
#if GUI_USE_TOUCH
    {
        0,                                          /*!< Touch down callback function */
        0,                                          /*!< Touch up callback function */
        0                                           /*!< Touch move callback function */
    },
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    {
        0,                                          /*!< Keyboard key pressed callback function */
    }
#endif /* GUI_USE_KEYBOARD */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define g          ((GUI_GRAPH_t *)h)
static uint8_t __Control(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result) {
    __GUI_UNUSED3(h, param, result);
    switch (ctrl) {                                 /* Handle control function if required */
        default:                                    /* Handle default option */
            return 0;                               /* Command was not processed */
    }
}

static void __Draw(GUI_HANDLE_t h, GUI_Display_t* disp) {
    GUI_GRAPH_DATA_t* data;
    GUI_Dim_t bt, br, bb, bl, x, y;
    uint8_t i;
    
    bt = g->Border[GUI_GRAPH_BORDER_TOP];
    br = g->Border[GUI_GRAPH_BORDER_RIGHT];
    bb = g->Border[GUI_GRAPH_BORDER_BOTTOM];
    bl = g->Border[GUI_GRAPH_BORDER_LEFT];
    x = __GUI_WIDGET_GetAbsoluteX(h);
    y = __GUI_WIDGET_GetAbsoluteY(h);
    
    GUI_DRAW_FilledRectangle(disp, x, y, bl, h->Height, g->Color[GUI_GRAPH_COLOR_BG]);
    GUI_DRAW_FilledRectangle(disp, x + bl, y, h->Width - bl - br, bt, g->Color[GUI_GRAPH_COLOR_BG]);
    GUI_DRAW_FilledRectangle(disp, x + bl, y + h->Height - bb, h->Width - bl - br, bb, g->Color[GUI_GRAPH_COLOR_BG]);
    GUI_DRAW_FilledRectangle(disp, x + h->Width - br, y, br, h->Height, g->Color[GUI_GRAPH_COLOR_BG]);
    GUI_DRAW_FilledRectangle(disp, x + bl, y + bt, h->Width - bl - br, h->Height - bt - bb, g->Color[GUI_GRAPH_COLOR_FG]);
    GUI_DRAW_Rectangle(disp, x, y, h->Width, h->Height, g->Color[GUI_GRAPH_COLOR_BORDER]);
    
    /* Draw horizontal lines */
    if (g->Rows) {
        float step;
        uint8_t i;
        step = (float)(h->Height - bt - bb) / (float)g->Rows;
        for (i = 0; i < g->Rows - 1; i++) {
            GUI_DRAW_HLine(disp, x + bl, y + bt + (i + 1) * step, h->Width - bl - br, g->Color[GUI_GRAPH_COLOR_GRID]);
        }
    }
    /* Draw vertical lines */
    if (g->Columns) {
        float step;
        step = (float)(h->Width - bl - br) / (float)g->Columns;
        for (i = 0; i < g->Columns - 1; i++) {
            GUI_DRAW_VLine(disp, x + bl + (i + 1) * step, y + bt, h->Height - bt - bb, g->Color[GUI_GRAPH_COLOR_GRID]);
        }
    }
    
    /* Check if any data attached to this graph */
    if (g->Root.First) {                            /* We have attached plots */
        register GUI_Dim_t x1, y1, x2, y2;          /* Try to add these variables to core registers */
        uint16_t xSize = g->MaxX - g->MinX;         /* Calculate X size */
        uint16_t ySize = g->MaxY - g->MinY;         /* Calculate Y size */
        float xStep = (float)(h->Width - bl - br) / (float)xSize;   /* Calculate X step */
        float yStep = (float)(h->Height - bt - bb) / (float)ySize;  /* calculate Y step */
        GUI_Dim_t yBottom = y + h->Height - bb - 1; /* Bottom Y value */
        GUI_Dim_t xLeft = x + bl;                   /* Left X position */
        uint16_t i, l;
        
        /* Check data array */
        for (data = (GUI_GRAPH_DATA_t *)__GUI_LINKEDLIST_GETNEXT(&g->Root, 0); data; data = (GUI_GRAPH_DATA_t *)__GUI_LINKEDLIST_GETNEXT(&g->Root, &data->C.List)) {
            i = 0;
            if (data->Length > xSize) {             /* If more data than allowed for plot, print only end of array */
                i = data->Length - xSize - 1;       /* Calculate new start index position */
            }
            l = 1;
            x1 = x + bl;                            /* Calculate start X */
            y1 = yBottom - (data->Data[i] - g->MinY) * yStep;   /* Calculate start Y */
            for (i++; i < data->Length; i++, l++) { /* Go through all elements, start on second valid line index */
                x2 = xLeft + l * xStep;             /* Calculate next X */
                y2 = yBottom - (data->Data[i] - g->MinY) * yStep;   /* Calculate next Y */
                GUI_DRAW_Line(disp, x1, y1, x2, y2, data->Color);   /* Draw actual line */
                GUI_DRAW_Circle(disp, x2, y2, 2, GUI_COLOR_BLUE);
                x1 = x2;                            /* Copy X */
                y1 = y2;                            /* Copy Y */
            }
        }
    }
}

#if GUI_USE_TOUCH
static __GUI_TouchStatus_t __TouchDown(GUI_HANDLE_t h, GUI_TouchData_t* ts) {
    __GUI_UNUSED2(h, ts);
    return touchHANDLEDNOFOCUS;                     /* Handle touch without focus */
}
#endif /* GUI_USE_TOUCH */
#undef g

/* Function to control graph data */
static uint8_t __ControlData(GUI_HANDLE_t h, GUI_WidgetControl_t ctrl, void* param, void* result) {
    __GUI_UNUSED3(h, param, result);
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WidgetControl_ExcludeLinkedList:   /* Check if widget should be excluded from adding to linked list */
            *(GUI_Byte *)result = 1;                /* Exclude from linked list */
            return 1;                               /* Command processed */
        default:                                    /* Handle default option */
            return 0;                               /* Command was not processed */
    }
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_t GUI_GRAPH_Create(GUI_ID_t id, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height) {
    GUI_GRAPH_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_GRAPH_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height);    /* Allocate memory for basic widget */
    if (ptr) {        
        /* Color setup */
        ptr->Color[GUI_GRAPH_COLOR_BG] = GUI_COLOR_GRAY;    /* Set background color */
        ptr->Color[GUI_GRAPH_COLOR_FG] = GUI_COLOR_BLACK;   /* Set foreground color */
        ptr->Color[GUI_GRAPH_COLOR_BORDER] = GUI_COLOR_BLACK;   /* Set foreground color */
        ptr->Color[GUI_GRAPH_COLOR_GRID] = GUI_COLOR_ALPHA(GUI_COLOR_GREEN, GUI_COLOR_ALPHA_50); /* Set foreground color */
        
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
    
    return (GUI_HANDLE_t)ptr;
}

void GUI_GRAPH_Remove(GUI_HANDLE_t* h) {
    __GUI_ASSERTPARAMSVOID(h && *h);                /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    __GUI_WIDGET_Remove(h);                         /* Remove widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

GUI_HANDLE_t GUI_GRAPH_AttachData(GUI_HANDLE_t h, GUI_HANDLE_t hd) {
    GUI_Color_t col;
    __GUI_ASSERTPARAMS(h && hd);
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GG(h)->Root.First) {
        col = GUI_COLOR_BLUE;
    } else {
        col = GUI_COLOR_RED;
    }
    __GD(hd)->Graph = h;                            /* Set graph */
    __GD(hd)->Color = col;                          /* Set graph */
    __GUI_LINKEDLIST_ADD_GEN(&__GG(h)->Root, &__GH(hd)->List);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_GRAPH_SetColor(GUI_HANDLE_t h, GUI_GRAPH_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h);                          /* Check parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GG(h)->Color[index] != color) {
        __GG(h)->Color[index] = color;              /* Set parameter */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

GUI_HANDLE_t GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_t type, uint16_t length) {
    GUI_GRAPH_DATA_t* data;
    __GUI_ENTER();                                  /* Enter GUI */

    data = (GUI_GRAPH_DATA_t *)__GUI_WIDGET_Create(&WidgetData, 0, 0, 0, 0, 0); /* Allocate memory for basic widget */
    if (data) {
        data->Type = type;
        data->Length = length;
        if (type == GUI_GRAPH_TYPE_YT) {            /* Only Y values are stored */
            data->Data = __GUI_MEMALLOC(length * sizeof(uint16_t));
            if (data->Data) {
                memset(data->Data, 0x00, length * sizeof(uint16_t));
            }
        } else {
            data->Data = __GUI_MEMALLOC(length * 2 * sizeof(uint16_t));   /* Store X and Y value for plot */
            if (data->Data) {
                memset(data->Data, 0x00, length * 2 * sizeof(uint16_t));
            }
        }
        if (!data->Data) {
            __GUI_WIDGET_Remove((GUI_HANDLE_t *)&data); /* Remove widget because data memory could not be allocated */
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return (GUI_HANDLE_t)data;
}

GUI_HANDLE_t GUI_GRAPH_DATA_AddValue(GUI_HANDLE_t h, uint16_t val) {
    uint16_t i;
    __GUI_ASSERTPARAMS(h);
    __GUI_ENTER();                                  /* Enter GUI */
    
    //TODO: Consider using cyclic buffer for data storage
    for (i = 1; i < __GD(h)->Length; i++) {         /* Shift data up */
        __GD(h)->Data[i - 1] = __GD(h)->Data[i];    
    }
    __GD(h)->Data[__GD(h)->Length - 1] = val;       /* Add new value */
    
    if (__GD(h)->Graph) {                           /* If data attached to any graph */
        __GUI_WIDGET_Invalidate(__GD(h)->Graph);    /* Invalidate plot object */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return h;
}

void GUI_GRAPH_DATA_Remove(GUI_HANDLE_t* h) {

}

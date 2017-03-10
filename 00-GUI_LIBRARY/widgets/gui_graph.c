/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
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

static void __Draw(GUI_Display_t* disp, void* ptr);
static __GUI_TouchStatus_t __TouchDown(void* ptr, GUI_TouchData_t* ts, __GUI_TouchStatus_t status);

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
    __Draw,                                         /*!< Widget draw function */
    {
        __TouchDown,                                /*!< Touch down callback function */
        0,                                          /*!< Touch up callback function */
        0                                           /*!< Touch move callback function */
    }
};
const static GUI_WIDGET_t WidgetData = {
    {
        "GraphData",                                /*!< Widget name */
        sizeof(GUI_GRAPH_DATA_t),                   /*!< Size of widget for memory allocation */
        0,                                          /*!< Allow children objects on widget */
    },
    0,                                              /*!< Widget draw function */
    {
        0,                                          /*!< Touch down callback function */
        0,                                          /*!< Touch up callback function */
        0                                           /*!< Touch move callback function */
    }
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define h          ((GUI_HANDLE_t)ptr)
#define g          ((GUI_GRAPH_t *)ptr)
static void __Draw(GUI_Display_t* disp, void* ptr) {
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
}

static __GUI_TouchStatus_t __TouchDown(void* ptr, GUI_TouchData_t* ts, __GUI_TouchStatus_t status) {
    return touchHANDLEDNOFOCUS;                     /* Handle touch without focus */
}
#undef h
#undef g

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
        ptr->Color[GUI_GRAPH_COLOR_GRID] = 0x00002000; /* Set foreground color */
        
        ptr->Border[GUI_GRAPH_BORDER_TOP] = 5;      /* Set borders */
        ptr->Border[GUI_GRAPH_BORDER_RIGHT] = 5;
        ptr->Border[GUI_GRAPH_BORDER_BOTTOM] = 5;
        ptr->Border[GUI_GRAPH_BORDER_LEFT] = 5;
        
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

GUI_HANDLE_t GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_t type, uint16_t Length) {
    __GUI_ENTER();                                  /* Enter GUI */

    
    
    __GUI_LEAVE();                                  /* Leave GUI */
}

void GUI_GRAPH_DATA_Remove(GUI_HANDLE_t* h) {

}

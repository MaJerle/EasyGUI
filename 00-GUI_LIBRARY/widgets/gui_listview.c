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
#include "gui_listview.h"

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
#define __GL(x)             ((GUI_LISTVIEW_t *)(x))

static
uint8_t GUI_LISTVIEW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_BG,
    GUI_COLOR_WIN_TEXT,
    GUI_COLOR_WIN_SEL_FOC,
    GUI_COLOR_WIN_SEL_NOFOC,
    GUI_COLOR_WIN_SEL_FOC_BG,
    GUI_COLOR_WIN_SEL_NOFOC_BG,
    GUI_COLOR_GRAY
};

const static GUI_WIDGET_t Widget = {
    .Name = _GT("LISTVIEW"),                        /*!< Widget name */
    .Size = sizeof(GUI_LISTVIEW_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_LISTVIEW_Callback,              /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o                   ((GUI_LISTVIEW_t *)(h))

/* Get item from LISTVIEW entry */
static
GUI_LISTVIEW_ROW_t* __GetRow(GUI_HANDLE_p h, uint16_t r) {
    GUI_LISTVIEW_ROW_t* row = 0;
    
    if (r >= o->Count) {                          /* Check if valid index */
        return 0;
    }
    
    if (r == 0) {                                 /* Check for first element */
        row = (GUI_LISTVIEW_ROW_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, 0);  /* Get first element */
    } else if (r == o->Count - 1) {
        row = (GUI_LISTVIEW_ROW_t *)__GUI_LINKEDLIST_GETPREV_GEN(&o->Root, 0);  /* Get last element */
    } else {
        row = (GUI_LISTVIEW_ROW_t *)__GUI_LINKEDLIST_GETNEXT_BYINDEX_GEN(&o->Root, r);  /* Get row by index */
    }
    return row;
}

/* Get item pointer from row pointer and column index */
static
GUI_LISTVIEW_ITEM_t* __GetItemFromRow(GUI_HANDLE_p h, GUI_LISTVIEW_ROW_t* row, uint8_t c) {
    if (!row) {                                     /* Check input value if exists */
        return NULL;
    }
    
    return (GUI_LISTVIEW_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_BYINDEX_GEN(&row->Root, c);  /* Get item by index value = column number */
}

/* Get item height in LISTVIEW */
static
GUI_Dim_t __ItemHeight(GUI_HANDLE_p h, GUI_Dim_t* offset) {
    GUI_Dim_t size = (float)__GH(h)->Font->Size * 1.3f;
    if (offset) {                                   /* Calculate top offset */
        *offset = (size - __GH(h)->Font->Size) >> 1;
    }
    return size;                                    /* Return height for element */
}

/* Get number of entries maximal on one page */
static
int16_t __NumberOfEntriesPerPage(GUI_HANDLE_p h) {
    int16_t res = 0;
    if (__GH(h)->Font) {                            /* Font is responsible for this setup */
        GUI_Dim_t height = __ItemHeight(h, 0);      /* Get item height */
        res = (__GUI_WIDGET_GetHeight(h) - height) / height;
    }
    return res;
}

/* Slide up or slide down widget elements */
static
void __Slide(GUI_HANDLE_p h, int16_t dir) {
    int16_t mPP = __NumberOfEntriesPerPage(h);
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->VisibleStartIndex + dir) < 0) {
            o->VisibleStartIndex = 0;
        } else {
            o->VisibleStartIndex += dir;
        }
        __GUI_WIDGET_Invalidate(h);
    } else if (dir > 0) {
        if ((o->VisibleStartIndex + dir) > (o->Count - mPP - 1)) {  /* Slide elements down */
            o->VisibleStartIndex = o->Count - mPP;
        } else {
            o->VisibleStartIndex += dir;
        }
        __GUI_WIDGET_Invalidate(h);
    }
}

/* Set selection for widget */
static
void __SetSelection(GUI_HANDLE_p h, int16_t selected) {
    if (o->Selected != selected) {                  /* Set selected value */
        o->Selected = selected;
        __GUI_WIDGET_Callback(h, GUI_WC_SelectionChanged, NULL, NULL);  /* Notify about selection changed */
    }                         
}

/* Increase or decrease selection */
static
void __IncSelection(GUI_HANDLE_p h, int16_t dir) {
    if (dir < 0) {                                  /* Slide elements up */
        if ((o->Selected + dir) < 0) {
            __SetSelection(h, 0);
        } else {
            __SetSelection(h, o->Selected + dir);
        }
        __GUI_WIDGET_Invalidate(h);
    } else if (dir > 0) {
        if ((o->Selected + dir) > (o->Count - 1)) { /* Slide elements down */
            __SetSelection(h, o->Count - 1);
        } else {
            __SetSelection(h, o->Selected + dir);
        }
        __GUI_WIDGET_Invalidate(h);
    }
}

/* Check values */
static
void __CheckValues(GUI_HANDLE_p h) {
    int16_t mPP = __NumberOfEntriesPerPage(h);      /* Get number of lines visible in widget at a time */
   
    if (o->Selected >= 0) {                         /* Check for selected value range */
        if (o->Selected >= o->Count) {
            __SetSelection(h, o->Count - 1);
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
    
    if (o->Flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO) {  /* Check slider mode */
        if (o->Count > mPP) {
            o->Flags |= GUI_FLAG_LISTVIEW_SLIDER_ON;
        } else {
            o->Flags &= ~GUI_FLAG_LISTVIEW_SLIDER_ON;
        }
    }
}

static
uint8_t GUI_LISTVIEW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
    static GUI_iDim_t tX, tY;
#endif /* GUI_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GL(h)->Selected = -1;                 /* Invalidate selection */
            __GL(h)->SliderWidth = 30;              /* Set slider width */
            __GL(h)->Flags |= GUI_FLAG_LISTVIEW_SLIDER_AUTO;    /* Enable auto mode for slider */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, width, height;
            uint16_t i;
            GUI_Dim_t itemHeight;
            uint8_t is3D = __GUI_WIDGET_Is3D(h);    /* Is 3D mode enabled */
            
            GUI_LISTVIEW_ROW_t* row;
            GUI_LISTVIEW_ITEM_t* item;

            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            __CheckValues(h);                       /* Check values if size changed */
            
            if (is3D) {
                GUI_DRAW_Rectangle3D(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
                GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, width - 4, height - 4, __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_BG));
            } else {
                GUI_DRAW_Rectangle(disp, x, y, width, height, __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_BORDER));
                GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, width - 2, height - 2, __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_BG));
            }

            
            /* Draw side scrollbar */
            if (o->Flags & GUI_FLAG_LISTVIEW_SLIDER_ON) {
                GUI_DRAW_SB_t sb;
                GUI_DRAW_ScrollBar_init(&sb);
                
                width -= o->SliderWidth;            /* Decrease available width */
                
                sb.X = x + width - 1;
                sb.Y = y + 1;
                sb.Width = o->SliderWidth;
                sb.Height = height - 2;
                sb.Dir = GUI_DRAW_SB_DIR_VERTICAL;
                sb.EntriesTop = o->VisibleStartIndex;
                sb.EntriesTotal = o->Count;
                sb.EntriesVisible = __NumberOfEntriesPerPage(h);
                
                GUI_DRAW_ScrollBar(disp, &sb);      /* Draw scroll bar */
            } else {
                width--;                            /* Go one pixel down for alignment */
            }
            
            itemHeight = __ItemHeight(h, 0);        /* Calculate item height */
            GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, width - 3, itemHeight, GUI_COLOR_WIN_LIGHTGRAY);
            if (o->Cols && o->Cols[0]) {            /* Draw all columns to screen */
                GUI_Dim_t xTmp = x + 2;                
                GUI_DRAW_FONT_t f;
                GUI_iDim_t tmpX2;
                
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.Height = itemHeight;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                
                tmpX2 = disp->X2;                   /* Save current value */
                if (disp->X2 > (x + width - 1)) {   /* Set clipping region */
                    disp->X2 = x + width - 1;
                }
                
                /* Draw header row with columns */
                f.Y = y + 2;
                for (i = 0; i < o->ColsCount; i++) {
                    if (is3D) {
                        GUI_DRAW_Rectangle3D(disp, xTmp, f.Y, o->Cols[i]->Width, itemHeight, GUI_DRAW_3D_State_Raised);
                        f.Width = o->Cols[i]->Width - 4;    /* Set width */
                        f.X = xTmp + 2;             /* Set offset */
                    } else {
                        GUI_DRAW_Rectangle(disp, xTmp, f.Y, o->Cols[i]->Width, itemHeight, GUI_COLOR_WIN_DARKGRAY);
                        f.Width = o->Cols[i]->Width - 6;    /* Set width */
                        f.X = xTmp + 3;             /* Set offset */
                    }
                    GUI_DRAW_WriteText(disp, __GH(h)->Font, o->Cols[i]->Text, &f);
                    xTmp += o->Cols[i]->Width;      /* Increase X value */
                }
                f.Y += itemHeight;                  /* Go to next line */
                
                /* Draw all rows and entry elements */           
                if (__GH(h)->Font && __GUI_LINKEDLIST_HasEntries(&__GL(h)->Root)) { /* Is first set? */
                    uint16_t index = 0;             /* Start index */
                    GUI_iDim_t tmp;
                    
                    tmp = disp->Y2;                 /* Scale out drawing area */
                    if (disp->Y2 > (y + height - 2)) {
                        disp->Y2 = y + height - 2;
                    }
                    
                    /* Try to process all strings */
                    for (index = 0, row = (GUI_LISTVIEW_ROW_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, 0); row && f.Y <= disp->Y2; row = (GUI_LISTVIEW_ROW_t *)__GUI_LINKEDLIST_GETNEXT_GEN(0, (GUI_LinkedList_t *)row), index++) {
                        if (index < o->VisibleStartIndex) { /* Check for start visible */
                            continue;
                        }               
                        if (index == __GL(h)->Selected) {
                            GUI_DRAW_FilledRectangle(disp, x + 2, f.Y, width - 2, __GUI_MIN(f.Height, itemHeight), __GUI_WIDGET_IsFocused(h) ? __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_SEL_FOC_BG) : __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_SEL_NOFOC_BG));
                            f.Color1 = __GUI_WIDGET_IsFocused(h) ? __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_SEL_FOC) : __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_SEL_NOFOC);
                        } else {
                            f.Color1 = __GUI_WIDGET_GetColor(h, GUI_LISTVIEW_COLOR_TEXT);
                        }
                        xTmp = x + 2;
                        for (i = 0, item = (GUI_LISTVIEW_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&row->Root, 0); item && i < o->ColsCount; item = (GUI_LISTVIEW_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(0, (GUI_LinkedList_t *)item), i++) {
                            if (item->Text) {       /* Draw if text set */
                                f.Width = o->Cols[i]->Width - 6;    /* Set width */
                                f.Color1Width = GUI.LCD.Width;  /* Use the same color for entire width */
                                f.X = xTmp + 3;     /* Set offset */
                                GUI_DRAW_WriteText(disp, __GH(h)->Font, item->Text, &f);
                            }
                            xTmp += o->Cols[i]->Width;  /* Increase X value */
                        }
                        f.Y += itemHeight;
                    }
                    disp->Y2 = tmp;                 /* Set clipping region back */
                }
                disp->X2 = tmpX2;                   /* Reset to first value */
            }
            
            return 1;
        }
        case GUI_WC_Remove: {
            GUI_LISTVIEW_ROW_t* row;
            GUI_LISTVIEW_ITEM_t* item;
            uint16_t i = 0;
            
            /**
             * Remove all rows
             */
            while ((row = (GUI_LISTVIEW_ROW_t *)__GUI_LINKEDLIST_REMOVE_GEN(&o->Root, (GUI_LinkedList_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, 0))) != NULL) {
                /**
                 * Remove all items in row first
                 */
                while ((item = (GUI_LISTVIEW_ITEM_t *)__GUI_LINKEDLIST_REMOVE_GEN(&row->Root, (GUI_LinkedList_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&row->Root, 0))) != NULL) {
                    __GUI_MEMFREE(item);
                }
                __GUI_MEMFREE(row);                 /* Remove actual row entry */
                i++;
            }
            
            /**
             * Remove all columns
             */
            if (o->Cols) {
                for (i = 0; i < o->ColsCount; i++) {
                    __GUI_MEMFREE(o->Cols[i]);      /* Remove column object */
                }
                __GUI_MEMFREE(o->Cols);             /* Remove block of pointers to columns */
            }
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            tX = ts->RelX[0];                       /* Save X position */
            tY = ts->RelY[0];                       /* Save Y position */
            
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            if (__GH(h)->Font) {
                GUI_Dim_t height = __ItemHeight(h, NULL);   /* Get element height */
                GUI_iDim_t diff;
                
                diff = tY - ts->RelY[0];            /* Check Y difference */
                if (__GUI_ABS(diff) > height) {     /* Difference must be greater than 1 height entry */
                    __Slide(h, diff > 0 ? 1 : -1);  /* Slide widget */
                    tY = ts->RelY[0];               /* Save pointer */
                }
                
                if (tY < height) {                  /* Check if we are in top region part */
                    uint16_t i;
                    GUI_Dim_t sum = 0;
                    
                    diff = tX - ts->RelX[0];        /* Check X difference too */
                    for (i = 0; i < o->ColsCount; i++) {    /* Check X position for column first */
                        sum += o->Cols[i]->Width;   /* Check width */
                        if (__GUI_ABS(tX - sum) < 10) {
                            break;
                        }
                    }
                    if (i != o->ColsCount) {
                        if (o->Cols[i]->Width - diff >= 4) {
                            o->Cols[i]->Width -= diff;  /* Set new width for difference */
                        }
                        tX = ts->RelX[0];           /* Set new start X position for relative calculation */
                        __GUI_WIDGET_Invalidate(h); /* Invalidate widget */
                    }
                }
            }
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            uint8_t handled = 0;
            uint16_t itemHeight = __ItemHeight(h, NULL);    /* Get element height */
            
            GUI_Dim_t width = __GUI_WIDGET_GetWidth(h); /* Get widget widget */
            GUI_Dim_t height = __GUI_WIDGET_GetHeight(h);   /* Get widget height */
            
            if (o->Flags & GUI_FLAG_LISTVIEW_SLIDER_ON) {
                if (ts->RelX[0] > (width - o->SliderWidth)) {   /* Touch is inside slider */
                    if (ts->RelY[0] < o->SliderWidth) {
                        __Slide(h, -1);             /* Slide one value up */
                    } else if (ts->RelY[0] > (height - o->SliderWidth)) {
                        __Slide(h, 1);              /* Slide one value down */
                    }
                    handled = 1;
                }
            }
            if (!handled) {
                uint16_t tmpSelected;
                
                if (ts->RelY[0] > itemHeight) {     /* Check item height */
                    tmpSelected = (ts->RelY[0] - itemHeight) / itemHeight;  /* Get temporary selected index */
                    if ((o->VisibleStartIndex + tmpSelected) <= o->Count) {
                        __SetSelection(h, o->VisibleStartIndex + tmpSelected);
                        __GUI_WIDGET_Invalidate(h); /* Choose new selection */
                    }
                    handled = 1;
                }
            }
            
            if (!handled && ts->RelY[0] < itemHeight) { /* We are in top region */
                uint16_t i;
                GUI_Dim_t sum = 0;
                for (i = 0; i < o->ColsCount; i++) {    /* Process all columns */
                    if (ts->RelX[0] > sum && ts->RelX[0] < (sum + o->Cols[i]->Width)) {
                        break;
                    }
                        
                    sum += o->Cols[i]->Width;       /* Increase sum value */
                }
                if (i != o->ColsCount) {            /* We have value */
                    //TODO: Sort rows on press
                }
                handled = 1;
            }
            return 1;
        }
#if GUI_USE_KEYBOARD
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (kb->KB.Keys[0] == GUI_KEY_DOWN) {   /* On pressed down */
                __IncSelection(h, 1);               /* Increase selection */
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            } else if (kb->KB.Keys[0] == GUI_KEY_UP) {
                __IncSelection(h, -1);              /* Decrease selection */
                *(__GUI_KeyboardStatus_t *)result = keyHANDLED;
            }
            return 1;
        }
#endif /* GUI_USE_KEYBOARD */
        case GUI_WC_IncSelection: {
            __IncSelection(h, *(int16_t *)param);   /* Increase selection */
            *(uint8_t *)result = 1;                 /* Set operation result */
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_LISTVIEW_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_LISTVIEW_t* ptr;
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_LISTVIEW_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);    /* Allocate memory for basic widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_LISTVIEW_SetColor(GUI_HANDLE_p h, GUI_LISTVIEW_COLOR_t index, GUI_Color_t color) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set new color */

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTVIEW_AddColumn(GUI_HANDLE_p h, const GUI_Char* text, GUI_Dim_t width) {
    uint8_t ret = 0;
    GUI_LISTVIEW_COL_t* col;
    GUI_LISTVIEW_COL_t** cols;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    cols = (GUI_LISTVIEW_COL_t **)__GUI_MEMREALLOC(__GL(h)->Cols, sizeof(*__GL(h)->Cols) * (__GL(h)->ColsCount + 2));   /* Allocate new memory block for new pointers of columns */
    if (cols) {
        __GL(h)->Cols = cols;                       /* Save new pointer of memory */
        col = (GUI_LISTVIEW_COL_t *)__GUI_MEMALLOC(sizeof(*col));   /* Allocate memory for new column structure */
        if (col) {
            __GL(h)->Cols[__GL(h)->ColsCount++] = col;  /* Add column to array list */
            __GL(h)->Cols[__GL(h)->ColsCount] = 0;  /* Add zero to the end of array */
            
            col->Text = (GUI_Char *)text;
            col->Width = width > 0 ? width : 100;
            ret = 1;
        }
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTVIEW_SetColumnWidth(GUI_HANDLE_p h, uint16_t index, GUI_Dim_t width) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (index < __GL(h)->ColsCount) {
        __GL(h)->Cols[index]->Width = width > 4 ? width : 4;    /* Set new width */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
        ret = 1;
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

GUI_LISTVIEW_ROW_p GUI_LISTVIEW_AddRow(GUI_HANDLE_p h) {
    GUI_LISTVIEW_ROW_t* row;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);   /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    row = (GUI_LISTVIEW_ROW_t *)__GUI_MEMALLOC(sizeof(*row));   /* Allocate memory for new row(s) */
    if (row) {
        __GUI_LINKEDLIST_ADD_GEN(&__GL(h)->Root, (GUI_LinkedList_t *)row);  /* Add new row to linked list */
        __GL(h)->Count++;                           /* Increase number of rows */
        __CheckValues(h);                           /* Check values situation */
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return (GUI_LISTVIEW_ROW_p)row;
}

uint8_t GUI_LISTVIEW_SetItemString(GUI_HANDLE_p h, GUI_LISTVIEW_ROW_p row, uint16_t col, const GUI_Char* text) {
    uint8_t ret = 0;
    GUI_LISTVIEW_ITEM_t* item = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget && row); /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    item = (GUI_LISTVIEW_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&__GLR(row)->Root, 0);   /* Get first in linked list */
    col++;
    while (col--) {                                 /* Find proper column */
        if (!item) {
            item = __GUI_MEMALLOC(sizeof(*item));   /* Allocate for item */
            if (!item) {
                break;
            }
            __GUI_LINKEDLIST_ADD_GEN(&__GLR(row)->Root, (GUI_LinkedList_t *)item);  /* Add element to linked list */
        }
        if (col) {
            item = (GUI_LISTVIEW_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(0, (GUI_LinkedList_t *)item);    /* Get next in linked list */
        }
    }
    if (item) {
        item->Text = (GUI_Char *)text;              /* Set text to item */
        ret = 1;
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTVIEW_SetSliderAuto(GUI_HANDLE_p h, uint8_t autoMode) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (autoMode && !(__GL(h)->Flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO)) {
        __GL(h)->Flags |= GUI_FLAG_LISTVIEW_SLIDER_AUTO;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    } else if (!autoMode && (__GL(h)->Flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO)) {
        __GL(h)->Flags &= ~GUI_FLAG_LISTVIEW_SLIDER_AUTO;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_LISTVIEW_SetSliderVisibility(GUI_HANDLE_p h, uint8_t visible) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!(__GL(h)->Flags & GUI_FLAG_LISTVIEW_SLIDER_AUTO)) {
        if (visible && !(__GL(h)->Flags & GUI_FLAG_LISTVIEW_SLIDER_ON)) {
            __GL(h)->Flags |= GUI_FLAG_LISTVIEW_SLIDER_ON;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GL(h)->Flags & GUI_FLAG_LISTVIEW_SLIDER_ON)) {
            __GL(h)->Flags &= ~GUI_FLAG_LISTVIEW_SLIDER_ON;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            ret = 1;
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTVIEW_Scroll(GUI_HANDLE_p h, int16_t step) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    start = __GL(h)->VisibleStartIndex;
    __GL(h)->VisibleStartIndex += step;
        
    __CheckValues(h);                               /* Check widget values */
    
    start = start != __GL(h)->VisibleStartIndex;    /* Check if there was valid change */
    
    if (start) {
        __GUI_WIDGET_Invalidate(h);
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return (uint8_t)start;
}

uint8_t GUI_LISTVIEW_SetSelection(GUI_HANDLE_p h, int16_t selection) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __SetSelection(h, selection);                   /* Set selection */
    __CheckValues(h);                               /* Check values */
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

int16_t GUI_LISTVIEW_GetSelection(GUI_HANDLE_p h) {
    int16_t selection;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    selection = __GL(h)->Selected;                  /* Read selection */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return selection;
}

uint8_t GUI_LISTVIEW_GetItemValue(GUI_HANDLE_p h, uint16_t rindex, uint16_t cindex, GUI_Char* dst, size_t length) {
    int16_t ret = 0;
    GUI_LISTVIEW_ROW_t* row;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget && dst && length > 1);   /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    *dst = 0;
    row = (GUI_LISTVIEW_ROW_t *)__GetRow(h, rindex);    /* Get row pointer */
    if (row) {
        GUI_LISTVIEW_ITEM_t* item = __GetItemFromRow(h, row, cindex);   /* Get item from column */
        if (item) {                                 /* In case of valid index */
            GUI_STRING_CopyN(dst, item->Text, length - 1);  /* Copy text to destination */
            ret = 1;
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

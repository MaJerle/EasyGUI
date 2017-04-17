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
#define GUI_INTERNAL
#include "gui_dropdown.h"

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
#define __GD(x)             ((GUI_DROPDOWN_t *)(x))

static
uint8_t GUI_DROPDOWN_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

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
    GUI_COLOR_WIN_SEL_NOFOC_BG
};

const static GUI_WIDGET_t Widget = {
    .Name = _T("Dropdown"),                         /*!< Widget name */
    .Size = sizeof(GUI_DROPDOWN_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_DROPDOWN_Callback,              /*!< Callback function */
    .Colors = Colors,
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Define number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o                   ((GUI_DROPDOWN_t *)(h))

/* Check if status is opened */
#define __IsOpened(h)       (__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPENED)

/* Check if open direction is up */
#define __IsDirUp(h)        (__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPEN_UP)

/* Height increase when opened */
#define __HeightConst(h)    4

/* Get Y position for main and expandend mode and height for both */
static
void __GetOpenedPositions(GUI_HANDLE_p h, GUI_iDim_t* y, GUI_iDim_t* height, GUI_iDim_t* y1, GUI_iDim_t* height1) {
    *height1 = *height / __HeightConst(h);          /* Height of main part */
    if (__IsDirUp(h)) {
        *y1 = *y + *height - *height1;              /* Position of opened part, add difference in height values */
    } else {
        *y1 = *y;                                   /* Position of main part */
        *y += *height1;                             /* Position of opened part */
    }
    *height -= *height1;                            /* Height of opened part */
}

/* Get item height in dropdown list */
static
uint16_t __ItemHeight(GUI_HANDLE_p h, uint16_t* offset) {
    uint16_t size = (float)__GH(h)->Font->Size * 1.3f;
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
        GUI_iDim_t height = __GUI_WIDGET_GetHeight(h);  /* Get item height */
        if (!__IsOpened(h)) {
            height *= __HeightConst(h) - 1;         /* Get height of opened area part */
        } else {
            height -= height / __HeightConst(h);    /* Subtract height for default size */
        }
        res = height / __ItemHeight(h, NULL);       /* Calculate entries */
    }
    return res;
}

/* Open or close dropdown widget */
static
uint8_t __OpenClose(GUI_HANDLE_p h, uint8_t state) {
    if (state && !__IsOpened(h)) {
        o->Flags |= GUI_FLAG_DROPDOWN_OPENED;
        o->OldHeight = o->C.Height;                 /* Save height for restore */
        o->OldY = o->C.Y;                           /* Save Y position for restore */
        if (__IsDirUp(h)) {                         /* On up direction */
            o->C.Y = o->C.Y - (__HeightConst(h) - 1) * o->C.Height; /* Go up for 3 height values */
        }
        o->C.Height = __HeightConst(h) * o->C.Height;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
        return 1;
    } else if (!state && (o->Flags & GUI_FLAG_DROPDOWN_OPENED)) {
        o->Flags &= ~GUI_FLAG_DROPDOWN_OPENED;      /* Clear flag */
        o->C.Height = o->OldHeight;                 /* Restore height value */
        o->C.Y = o->OldY;                           /* Restore position */
        if (o->Selected == -1) {                    /* Go to top selection */
            o->VisibleStartIndex = 0;               /* Start from top again */
        } else {                                    /* We have one selection */
            int16_t perPage = __NumberOfEntriesPerPage(h);
            o->VisibleStartIndex = o->Selected;
            if ((o->VisibleStartIndex + perPage) >= o->Count) {
                o->VisibleStartIndex = o->Count - perPage;
            }
        }
        __GUI_WIDGET_InvalidateWithParent(h);       /* Invalidate widget */
        return 1;
    }
    return 0;
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

/* Get item from listbox entry */
static
GUI_DROPDOWN_ITEM_t* __GetItem(GUI_HANDLE_p h, uint16_t index) {
    uint16_t i = 0;
    GUI_DROPDOWN_ITEM_t* item = 0;
    
    if (index >= o->Count) {                        /* Check if valid index */
        return 0;
    }
    
    if (index == 0) {                               /* Check for first element */
        return (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, NULL); /* Return first element */
    } else if (index == o->Count - 1) {
        return (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETPREV_GEN(&o->Root, NULL); /* Return last element */
    }
    
    item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, NULL);
    while (i++ < index) {
        item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, &item->List);
    }
    return item;
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
    
    if (o->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO) { /* Check slider mode */
        if (o->Count > mPP) {
            o->Flags |= GUI_FLAG_DROPDOWN_SLIDER_ON;
        } else {
            o->Flags &= ~GUI_FLAG_DROPDOWN_SLIDER_ON;
        }
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

/* Delete list item box by index */
static
uint8_t __DeleteItem(GUI_HANDLE_p h, uint16_t index) {
    GUI_DROPDOWN_ITEM_t* item;
    
    item = __GetItem(h, index);                     /* Get list item from handle */
    if (item) {
        __GUI_LINKEDLIST_REMOVE_GEN(&__GD(h)->Root, &item->List);
        __GD(h)->Count--;                           /* Decrease count */
        
        if (o->Selected == index) {
            __SetSelection(h, -1);
        }
        
        __CheckValues(h);                           /* Check widget values */
        __GUI_WIDGET_Invalidate(h);
        return 1;
    }
    return 0;
}

/* Process widget click event */
static
void __ProcessClick(GUI_HANDLE_p h, __GUI_TouchData_t* ts) {
    GUI_iDim_t y, y1, height, height1;
    
    if (!__IsOpened(h)) {                           /* Check if opened */
        __OpenClose(h, 1);                          /* Open widget on click */
        return;
    }
    
    y = 0;                                          /* Relative Y position for touch events */
    height = __GUI_WIDGET_GetHeight(h);             /* Get widget height */
    __GetOpenedPositions(h, &y, &height, &y1, &height1);    /* Calculate values */
    
    /* Check if press was on normal area when widget is closed */
    if (ts->RelY[0] >= y1 && ts->RelY[0] <= (y1 + height1)) {   /* Check first part */
        __OpenClose(h, 0);                          /* Close widget on click */
    } else {
        uint16_t tmpSelected;
        
        if (__IsDirUp(h)) {
            tmpSelected = ts->RelY[0] / __ItemHeight(h, NULL);  /* Get temporary selected index */
        } else {
            tmpSelected = (ts->RelY[0] - height1) / __ItemHeight(h, NULL);  /* Get temporary selected index */
        }
        if ((o->VisibleStartIndex + tmpSelected) <= o->Count) {
            __SetSelection(h, o->VisibleStartIndex + tmpSelected);
            __GUI_WIDGET_Invalidate(h);             /* Choose new selection */
        }
    }
    __OpenClose(h, 0);                              /* Close widget on click */
}

static
uint8_t GUI_DROPDOWN_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
    static GUI_iDim_t tY;
#endif /* GUI_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_iDim_t x, y, width, height;
            GUI_iDim_t y1, height1;                 /* Position of main widget part, when widget is opened */
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            if (__IsOpened(h)) {
                __GetOpenedPositions(h, &y, &height, &y1, &height1);
                
                GUI_DRAW_Rectangle3D(disp, x, y, width, height1, GUI_DRAW_3D_State_Lowered);
                GUI_DRAW_FilledRectangle(disp, x + 2, y1 + 2, width - 4, height1 - 4, __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_BG));
                
                GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, width - 4, height - 4, __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_BG));
                GUI_DRAW_Rectangle3D(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            } else {
                y1 = y;
                height1 = height;
                
                GUI_DRAW_Rectangle3D(disp, x, y, width, height, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, width - 4, height - 4, __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_BG));
            }
                
            if (__GD(h)->Selected >= 0 && __GH(h)->Font) {
                int16_t i;
                GUI_DRAW_FONT_t f;
                GUI_DROPDOWN_ITEM_t* item;
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&__GD(h)->Root, NULL);
                for (i = 0; i < __GD(h)->Selected; i++) {
                    item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, (GUI_LinkedList_t *)item);
                }
                
                f.X = x + 3;
                f.Y = y1 + 3;
                f.Width = width - 6;
                f.Height = height1 - 6;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                f.Color1 = __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_TEXT);
                GUI_DRAW_WriteText(disp, __GH(h)->Font, item->Text, &f);
            }
            
            if (__IsOpened(h) && __GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_ON) {
                GUI_DRAW_SB_t sb;
                
                width -= __GD(h)->SliderWidth;       /* Available width is decreased */
                
                GUI_DRAW_ScrollBar_init(&sb);
                
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
                width--;                            /* Go down for one for alignment on non-slider */
            }
            
            if (__IsOpened(h) && __GH(h)->Font && __GUI_LINKEDLIST_HasEntries(&__GD(h)->Root)) {
                GUI_DRAW_FONT_t f;
                GUI_DROPDOWN_ITEM_t* item;
                uint16_t yOffset;
                uint16_t itemHeight;                /* Get item height */
                uint16_t index = 0;                 /* Start index */
                GUI_iDim_t tmp;
                
                itemHeight = __ItemHeight(h, &yOffset); /* Get item height and Y offset */
                
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.X = x + 4;
                f.Y = y + 2;
                f.Width = width - 6;
                f.Height = itemHeight;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                
                for (item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, NULL); 
                    item && index < o->VisibleStartIndex; 
                    item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, &item->List), index++);
                
                tmp = disp->Y2;
                if (disp->Y2 > (y + height)) {      /* Set cut-off Y position for drawing operations */
                    disp->Y2 = y + height;
                }
                while (height && item) {            /* Try to process all strings */                    
                    if (index == __GD(h)->Selected) {
                        GUI_DRAW_FilledRectangle(disp, x + 2, f.Y, width - 3, __GUI_MIN(f.Height, itemHeight), __GUI_WIDGET_IsFocused(h) ? __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_SEL_FOC_BG) : __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_SEL_NOFOC_BG));
                        f.Color1 = __GUI_WIDGET_IsFocused(h) ? __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_SEL_FOC) : __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_SEL_NOFOC);
                    } else {
                        f.Color1 = __GUI_WIDGET_GetColor(h, GUI_DROPDOWN_COLOR_TEXT);
                    }
                    GUI_DRAW_WriteText(disp, __GH(h)->Font, item->Text, &f);
                    f.Y += itemHeight;
                    if (f.Y > disp->Y2) {
                        break;
                    }
                    item = (GUI_DROPDOWN_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, &item->List);
                    index++;
                }
                disp->Y2 = tmp;                     /* Set temporary value back */
            }
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            tY = ts->RelY[0];
            
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            if (__GH(h)->Font) {
                GUI_Dim_t height = __ItemHeight(h, NULL);   /* Get element height */
                GUI_iDim_t diff = tY - ts->RelY[0];
                
                if (__GUI_ABS(diff) > height) {
                    __Slide(h, diff > 0 ? 1 : -1);  /* Slide widget */
                    tY = ts->RelY[0];               /* Save pointer */
                }
            }
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param;
            if (!__IsOpened(h)) {                   /* Check widget status */
                __OpenClose(h, 1);                  /* Open widget */
            } else {                                /* Widget opened, process data */
                __ProcessClick(h, ts);              /* Process click event */
            }
            
            return 1;
        }
        case GUI_WC_FocusOut: {
            __OpenClose(h, 0);                      /* Close widget */
        }
        case GUI_WC_KeyPress: {
            __GUI_KeyboardData_t* kb = (__GUI_KeyboardData_t *)param;
            if (kb->KB.Keys[0] == GUI_KEY_DOWN) {   /* On pressed down */
                __IncSelection(h, 1);               /* Increase selection */
            } else if (kb->KB.Keys[0] == GUI_KEY_UP) {
                __IncSelection(h, -1);              /* Decrease selection */
            }
            return 1;
        }
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
GUI_HANDLE_p GUI_DROPDOWN_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_DROPDOWN_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_DROPDOWN_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);   /* Allocate memory for basic widget */
    if (ptr) {        
        ptr->Selected = -1;                         /*!< Invalidate selection */
        ptr->SliderWidth = 30;                      /*!< Set slider width */
        ptr->Flags |= GUI_FLAG_DROPDOWN_SLIDER_AUTO;
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_DROPDOWN_SetColor(GUI_HANDLE_p h, GUI_DROPDOWN_COLOR_t index, GUI_Color_t color) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set new color */

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_AddString(GUI_HANDLE_p h, const GUI_Char* text) {
    GUI_DROPDOWN_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    item = (GUI_DROPDOWN_ITEM_t *)__GUI_MEMALLOC(sizeof(*item));    /* Allocate memory for entry */
    if (item) {
        item->Text = (GUI_Char *)text;              /* Add text to entry */
        __GUI_LINKEDLIST_ADD_GEN(&__GD(h)->Root, &item->List);  /* Add to linked list */
        __GD(h)->Count++;                           /* Increase number of strings */
        
        __CheckValues(h);                           /* Check values */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
         
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_SetOpenDirection(GUI_HANDLE_p h, GUI_DROPDOWN_OPENDIR_t dir) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!__IsOpened(h)) {                           /* Must be closed */
        if ((__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPEN_UP) && dir == GUI_DROPDOWN_OPENDIR_DOWN) {
            __GD(h)->Flags &= GUI_FLAG_DROPDOWN_OPEN_UP;
            ret = 1;
        } else if (!(__GD(h)->Flags & GUI_FLAG_DROPDOWN_OPEN_UP) && dir == GUI_DROPDOWN_OPENDIR_UP) {
            __GD(h)->Flags |= GUI_FLAG_DROPDOWN_OPEN_UP;
            ret = 1;
        }
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_SetString(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text) {
    GUI_DROPDOWN_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    item = __GetItem(h, index);                     /* Get list item from handle */
    if (item) {
        item->Text = (GUI_Char *)text;              /* Set new text */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_DeleteFirstString(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __DeleteItem(h, 0);                       /* Delete first item */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_DeleteLastString(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __DeleteItem(h, __GD(h)->Count - 1);      /* Delete last item */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_DeleteString(GUI_HANDLE_p h, uint16_t index) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __DeleteItem(h, index);                   /* Delete item */

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_SetSliderAuto(GUI_HANDLE_p h, uint8_t autoMode) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (autoMode && !(__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO)) {
        __GD(h)->Flags |= GUI_FLAG_DROPDOWN_SLIDER_AUTO;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    } else if (!autoMode && (__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO)) {
        __GD(h)->Flags &= ~GUI_FLAG_DROPDOWN_SLIDER_AUTO;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_DROPDOWN_SetSliderVisibility(GUI_HANDLE_p h, uint8_t visible) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!(__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_AUTO)) {
        if (visible && !(__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_ON)) {
            __GD(h)->Flags |= GUI_FLAG_DROPDOWN_SLIDER_ON;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GD(h)->Flags & GUI_FLAG_DROPDOWN_SLIDER_ON)) {
            __GD(h)->Flags &= ~GUI_FLAG_DROPDOWN_SLIDER_ON;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            ret = 1;
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_DROPDOWN_Scroll(GUI_HANDLE_p h, int16_t step) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    start = __GD(h)->VisibleStartIndex;
    __GD(h)->VisibleStartIndex += step;
        
    __CheckValues(h);                               /* Check widget values */
    
    start = start != __GD(h)->VisibleStartIndex;    /* Check if there was valid change */
    
    if (start) {
        __GUI_WIDGET_Invalidate(h);
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return start;
}

uint8_t GUI_DROPDOWN_SetSelection(GUI_HANDLE_p h, int16_t selection) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __SetSelection(h, selection);                   /* Set selection */
    __CheckValues(h);                               /* Check values */
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

int16_t GUI_DROPDOWN_GetSelection(GUI_HANDLE_p h) {
    int16_t selection;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    selection = __GD(h)->Selected;                  /* Read selection */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return selection;
}

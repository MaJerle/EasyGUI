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
#include "gui_listbox.h"

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
#define __GL(x)             ((GUI_LISTBOX_t *)(x))

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    .Name = _T("LISTBOX"),                          /*!< Widget name */
    .Size = sizeof(GUI_LISTBOX_t),                  /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_LISTBOX_Callback                /*!< Callback function */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o                   ((GUI_LISTBOX_t *)(h))

static
GUI_LISTBOX_ITEM_t* __GetListboxItem(GUI_HANDLE_p h, uint16_t index) {
    uint16_t i = 0;
    GUI_LISTBOX_ITEM_t* item = 0;
    
    if (index >= o->Count) {                        /* Check if valid index */
        return 0;
    }
    
    if (index == 0) {                               /* Check for first element */
        return (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, NULL);  /* Return first element */
    } else if (index == o->Count - 1) {
        return (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETPREV_GEN(&o->Root, NULL);  /* Return last element */
    }
    
    item = (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, NULL);
    while (i++ < index) {
        item = (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, &item->List);
    }
    return item;
}

/* Get number of entries maximal on one page */
static
int16_t __NumberOfEntriesPerPage(GUI_HANDLE_p h) {
    int16_t res = 0;
    if (h->Font) {                                  /* Font is responsible for this setup */
        res = __GUI_WIDGET_GetHeight(h) / h->Font->Size;
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
    
    if (o->Flags & GUI_FLAG_LISTBOX_SLIDER_AUTO) {  /* Check slider mode */
        if (o->Count > mPP) {
            o->Flags |= GUI_FLAG_LISTBOX_SLIDER_ON;
        } else {
            o->Flags &= ~GUI_FLAG_LISTBOX_SLIDER_ON;
        }
    }
}

/* Delete list item box by index */
static
uint8_t __DeleteListboxItem(GUI_HANDLE_p h, uint16_t index) {
    GUI_LISTBOX_ITEM_t* item;
    
    item = __GetListboxItem(h, index);              /* Get list item from handle */
    if (item) {
        __GUI_LINKEDLIST_REMOVE_GEN(&__GL(h)->Root, &item->List);
        __GL(h)->Count--;                           /* Decrease count */
        
        if (o->Selected == index) {
            __SetSelection(h, -1);
        }
        
        __CheckValues(h);                           /* Check widget values */
        __GUI_WIDGET_Invalidate(h);
        return 1;
    }
    return 0;
}

uint8_t GUI_LISTBOX_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
    static GUI_iDim_t tY;
#endif /* GUI_USE_TOUCH */
    
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, width, height, sliderW = 0;
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, width - 2, height - 2, GUI_COLOR_WHITE);
            GUI_DRAW_Rectangle(disp, x, y, width, height, GUI_COLOR_GRAY);
            
            /* Draw side scrollbar */
            if (o->Flags & GUI_FLAG_LISTBOX_SLIDER_ON) {
                GUI_Dim_t midHeight, rectHeight, yOffset = 0;
                int16_t mPP;
                
                sliderW = o->SliderWidth;
                GUI_DRAW_VLine(disp, x + width - sliderW, y, height, GUI_COLOR_GRAY);
                GUI_DRAW_Rectangle3D(disp, x + width - sliderW + 1, y + 1, sliderW - 2, sliderW - 2, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledTriangle(disp, x + width - sliderW + 5, y + sliderW - 7, x + width - sliderW / 2 - 1, y + 6, x + width - 7, y + sliderW - 7, GUI_COLOR_BLACK);
                
                GUI_DRAW_Rectangle3D(disp, x + width - sliderW + 1, y + height - sliderW + 1, sliderW - 2, sliderW - 2, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledTriangle(disp, x + width - sliderW + 5, y + height - sliderW + 7, x + width - sliderW / 2 - 1, y + height - 7, x + width - 7, y + height - sliderW + 7, 
                GUI_COLOR_BLACK);
                
                midHeight = (height - 2U * sliderW + 2);    /* Calculate middle rectangle part */
                mPP = __NumberOfEntriesPerPage(h);  /* Get number of entries visible at a time */
                
                if (mPP < o->Count) {
                    /* Draw rectangle on the middle */
                    rectHeight = midHeight * mPP / o->Count;    /* Entire area for drawing middle part */
                    if (rectHeight < 5) {
                        rectHeight = 5;
                    }
                    yOffset = (midHeight - rectHeight) * o->VisibleStartIndex / (o->Count - mPP);
                } else {
                    rectHeight = midHeight;
                }
                GUI_DRAW_FilledRectangle(disp, x + width - sliderW + 2, y + sliderW + yOffset, sliderW - 4, rectHeight - 2, GUI_COLOR_BLACK);
            }
            
            /* Draw text if possible */
            if (h->Font && __GUI_LINKEDLIST_HasEntries(&__GL(h)->Root)) {   /* Is first set? */
                GUI_DRAW_FONT_t f;
                GUI_LISTBOX_ITEM_t* item;
                uint16_t index = 0;                 /* Start index */
                
                GUI_DRAW_FONT_Init(&f);             /* Init structure */
                
                f.X = x + 2;
                f.Y = y + 1;
                f.Width = width - sliderW - 1;
                f.Height = height - 2;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_TOP;
                f.Color1Width = f.Width;
                
                for (item = (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(&o->Root, NULL); 
                    item && index < o->VisibleStartIndex; 
                    item = (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, &item->List), index++);
                
                while (height && item) {            /* Try to process all strings */                    
                    if (index == __GL(h)->Selected) {
                        GUI_DRAW_FilledRectangle(disp, x + 1, f.Y, f.Width, __GUI_MIN(f.Height, h->Font->Size), GUI_WIDGET_IsFocused(h) ? 0xFF97EEFF : 0xFFDDDDDD);
                        f.Color1 = GUI_COLOR_BLACK;
                    } else {
                        f.Color1 = GUI_COLOR_BLACK;
                    }
                    GUI_DRAW_WriteText(disp, h->Font, item->Text, &f);
                    if (f.Height < h->Font->Size) {
                        break;
                    } else {
                        f.Height -= h->Font->Size;
                        f.Y += h->Font->Size;
                    }
                    item = (GUI_LISTBOX_ITEM_t *)__GUI_LINKEDLIST_GETNEXT_GEN(NULL, &item->List);
                    index++;
                }
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
            if (h->Font) {
                GUI_Dim_t height = h->Font->Size;    /* Get element height */
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
            uint8_t handled = 0;
            GUI_Dim_t width = __GUI_WIDGET_GetWidth(h); /* Get widget widget */
            GUI_Dim_t height = __GUI_WIDGET_GetHeight(h);   /* Get widget height */
            
            if (o->Flags & GUI_FLAG_LISTBOX_SLIDER_ON) {
                if (ts->RelX[0] > (width - o->SliderWidth)) {   /* Touch is inside slider */
                    if (ts->RelY[0] < o->SliderWidth) {
                        __Slide(h, -1);             /* Slide one value up */
                    } else if (ts->RelY[0] > (height - o->SliderWidth)) {
                        __Slide(h, 1);              /* Slide one value down */
                    }
                    handled = 1;
                }
            }
            if (!handled && h->Font) {
                uint16_t height = h->Font->Size;    /* Get element height */
                uint16_t tmpSelected;
                
                tmpSelected = ts->RelY[0] / height; /* Get temporary selected index */
                if ((o->VisibleStartIndex + tmpSelected) <= o->Count) {
                    __SetSelection(h, o->VisibleStartIndex + tmpSelected);
                    __GUI_WIDGET_Invalidate(h);     /* Choose new selection */
                }
            }
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
GUI_HANDLE_p GUI_LISTBOX_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags) {
    GUI_LISTBOX_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_LISTBOX_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, flags);   /* Allocate memory for basic widget */
    if (ptr) {        
        ptr->Selected = -1;                         /*!< Invalidate selection */
        ptr->SliderWidth = 30;                      /*!< Set slider width */
        ptr->Flags |= GUI_FLAG_LISTBOX_SLIDER_AUTO;
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    
    return (GUI_HANDLE_p)ptr;
}

uint8_t GUI_LISTBOX_AddString(GUI_HANDLE_p h, const GUI_Char* text) {
    GUI_LISTBOX_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    item = (GUI_LISTBOX_ITEM_t *)__GUI_MEMALLOC(sizeof(*item)); /* Allocate memory for entry */
    if (item) {
        item->Text = (GUI_Char *)text;              /* Add text to entry */
        __GUI_LINKEDLIST_ADD_GEN(&__GL(h)->Root, &item->List);  /* Add to linked list */
        __GL(h)->Count++;                           /* Increase number of strings */
        
        __CheckValues(h);                           /* Check values */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
        
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTBOX_SetString(GUI_HANDLE_p h, uint16_t index, const GUI_Char* text) {
    GUI_LISTBOX_ITEM_t* item;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    item = __GetListboxItem(h, index);              /* Get list item from handle */
    if (item) {
        item->Text = (GUI_Char *)text;              /* Set new text */
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTBOX_DeleteFirstString(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __DeleteListboxItem(h, 0);                /* Delete first item */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTBOX_DeleteLastString(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __DeleteListboxItem(h, __GL(h)->Count - 1);   /* Delete last item */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTBOX_DeleteString(GUI_HANDLE_p h, uint16_t index) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __DeleteListboxItem(h, index);            /* Delete item */

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTBOX_SetSliderAuto(GUI_HANDLE_p h, uint8_t autoMode) {
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (autoMode && !(__GL(h)->Flags & GUI_FLAG_LISTBOX_SLIDER_AUTO)) {
        __GL(h)->Flags |= GUI_FLAG_LISTBOX_SLIDER_AUTO;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    } else if (!autoMode && (__GL(h)->Flags & GUI_FLAG_LISTBOX_SLIDER_AUTO)) {
        __GL(h)->Flags &= ~GUI_FLAG_LISTBOX_SLIDER_AUTO;
        __GUI_WIDGET_Invalidate(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_LISTBOX_SetSliderVisibility(GUI_HANDLE_p h, uint8_t visible) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!(__GL(h)->Flags & GUI_FLAG_LISTBOX_SLIDER_AUTO)) {
        if (visible && !(__GL(h)->Flags & GUI_FLAG_LISTBOX_SLIDER_ON)) {
            __GL(h)->Flags |= GUI_FLAG_LISTBOX_SLIDER_ON;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            ret = 1;
        } else if (!visible && (__GL(h)->Flags & GUI_FLAG_LISTBOX_SLIDER_ON)) {
            __GL(h)->Flags &= ~GUI_FLAG_LISTBOX_SLIDER_ON;
            __GUI_WIDGET_Invalidate(h);             /* Invalidate widget */
            ret = 1;
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_LISTBOX_Scroll(GUI_HANDLE_p h, int16_t step) {
    volatile int16_t start;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    start = __GL(h)->VisibleStartIndex;
    __GL(h)->VisibleStartIndex += step;
        
    __CheckValues(h);                               /* Check widget values */
    
    start = start != __GL(h)->VisibleStartIndex;    /* Check if there was valid change */
    
    if (start) {
        __GUI_WIDGET_Invalidate(h);
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return start;
}

uint8_t GUI_LISTBOX_SetSelection(GUI_HANDLE_p h, int16_t selection) {
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __SetSelection(h, selection);                   /* Set selection */
    __CheckValues(h);                               /* Check values */
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

int16_t GUI_LISTBOX_GetSelection(GUI_HANDLE_p h) {
    int16_t selection;
    
    __GUI_ASSERTPARAMS(h && h->Widget == &Widget);  /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    selection = __GL(h)->Selected;                  /* Read selection */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return selection;
}

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
#include "gui_checkbox.h"

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
#define __GC(x)             ((GUI_CHECKBOX_t *)(x))

#define CFG_CHECK           0x01
#define CFG_DISABLE         0x02

static uint8_t gui_checkbox_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WHITE,                                /*!< Default background color */
    GUI_COLOR_DARKGRAY,                             /*!< Default freground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
    GUI_COLOR_GRAY,                                 /*!< Default background color when disabled */
    GUI_COLOR_BLACK,                                /*!< Default text color for widget */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("CHECKBOX"),                        /*!< Widget name */
    .Size = sizeof(GUI_CHECKBOX_t),                 /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_checkbox_callback,              /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define c                   ((GUI_CHECKBOX_t *)(h))

/* Set checked status */
static uint8_t
set_checked(GUI_HANDLE_p h, uint8_t state) {
    if (state && !(c->Flags & GUI_FLAG_CHECKBOX_CHECKED)) {
        c->Flags |= GUI_FLAG_CHECKBOX_CHECKED;      /* Set flag */
        gui_widget_callback__(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        gui_widget_invalidate__(h);
        return 1;
    } else if (!state && (c->Flags & GUI_FLAG_CHECKBOX_CHECKED)) {
        c->Flags &= ~GUI_FLAG_CHECKBOX_CHECKED;     /* Clear flag */
        gui_widget_callback__(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
        gui_widget_invalidate__(h);
        return 1;
    }
    return 0;
}

/* Set disabled status */
static uint8_t
set_disabled(GUI_HANDLE_p h, uint8_t state) {
    if (state && !(c->Flags & GUI_FLAG_CHECKBOX_DISABLED)) {
        c->Flags |= GUI_FLAG_CHECKBOX_DISABLED;     /* Set flag */
        return 1;
    } else if (!state && (c->Flags & GUI_FLAG_CHECKBOX_DISABLED)) {
        c->Flags &= ~GUI_FLAG_CHECKBOX_DISABLED;    /* Clear flag */
        return 1;
    }
    return 0;
}

static uint8_t
gui_checkbox_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* p = (GUI_WIDGET_Param_t *)param;
            switch (p->Type) {
                case CFG_CHECK:                     /* Set current progress value */
                    set_checked(h, *(uint8_t *)p->Data);
                    break;
                case CFG_DISABLE:                   /* Set disabled status */
                    set_disabled(h, *(uint8_t *)p->Data);
                    break;
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Color_t c1;
            GUI_Dim_t x, y, width, height, size, sx, sy;
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            size = 20;
            
            sx = x;
            sy = y + (height - size) / 2;
            
            if (__GC(h)->Flags & GUI_FLAG_CHECKBOX_DISABLED) {
                c1 = gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_DISABLED_BG);
            } else {
                c1 = gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_BG);
            }
            
            gui_draw_filledrectangle(disp, sx + 1, sy + 1, size - 2, size - 2, c1);
            gui_draw_rectangle3d(disp, sx, sy, size, size, GUI_DRAW_3D_State_Lowered);
            
            if (gui_widget_isfocused__(h)) {        /* When in focus */
                gui_draw_rectangle(disp, sx + 2, sy + 2, size - 4, size - 4, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
            }
            
            if (__GC(h)->Flags & GUI_FLAG_CHECKBOX_CHECKED) {
                gui_draw_line(disp, sx + 4, sy + 5, sx + size - 4 - 2, sy + size - 4 - 1, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 4, sy + 4, sx + size - 4 - 1, sy + size - 4 - 1, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 5, sy + 4, sx + size - 4 - 1, sy + size - 4 - 2, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
                
                gui_draw_line(disp, sx + 4, sy + size - 4 - 2, sx + size - 4 - 2, sy + 4, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 4, sy + size - 4 - 1, sx + size - 4 - 1, sy + 4, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
                gui_draw_line(disp, sx + 5, sy + size - 4 - 1, sx + size - 4 - 1, sy + 5, gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_FG));
            }
            
            /* Draw text if possible */
            if (gui_widget_isfontandtextset__(h)) {
                GUI_DRAW_FONT_t f;
                gui_draw_font_init(&f);             /* Init structure */
                
                f.X = sx + size + 5;
                f.Y = y + 1;
                f.Width = width - size - 5;
                f.Height = height - 2;
                f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                f.Color1Width = f.Width;
                f.Color1 = gui_widget_getcolor__(h, GUI_CHECKBOX_COLOR_TEXT);
                gui_draw_writetext(disp, gui_widget_getfont__(h), gui_widget_gettext__(h), &f);
            }
            
            return 1;
        }
#if GUI_USE_TOUCH 
        case GUI_WC_TouchStart: {
            if (c->Flags & GUI_FLAG_CHECKBOX_DISABLED) {    /* Check if widget is disabled */
                *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;   /* Handle touch without focus */
            } else {
                *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Touch is handled and in focus */
            }
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        case GUI_WC_Click: {
            set_checked(h, (c->Flags & GUI_FLAG_CHECKBOX_CHECKED) ? 0 : 1);    /* Toggle checked state */
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef c


/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p
gui_checkbox_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

uint8_t
gui_checkbox_setcolor(GUI_HANDLE_p h, GUI_CHECKBOX_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

uint8_t
gui_checkbox_setchecked(GUI_HANDLE_p h, uint8_t checked) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_CHECK, &checked, 0, 0); /* Set parameter */
}

uint8_t
gui_checkbox_setdisabled(GUI_HANDLE_p h, uint8_t disabled) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setparam__(h, CFG_DISABLE, &disabled, 0, 0);  /* Set parameter */
}

uint8_t
gui_checkbox_ischecked(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = !!(__GC(h)->Flags & GUI_FLAG_CHECKBOX_CHECKED);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t
gui_checkbox_isdisabled(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = !!(__GC(h)->Flags & GUI_FLAG_CHECKBOX_DISABLED);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**	
 * \file            gui_radio.c
 * \brief           Radio box widget
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
#include "widget/gui_radio.h"

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
#define __GR(x)             ((GUI_RADIO_t *)(x))

static uint8_t gui_radio_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);

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
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("RADIOBOX"),                        /*!< Widget name */
    .Size = sizeof(GUI_RADIO_t),                    /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = gui_radio_callback,                 /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define c                   ((GUI_RADIO_t *)(h))
static uint8_t
set_active(GUI_HANDLE_p h) {
    GUI_HANDLE_p handle;
    
    if (__GR(h)->Flags & GUI_FLAG_RADIO_DISABLED) { /* Check if it can be enabled */
        return 0;
    }
    
    /**
     * Find radio widgets on the same page
     * and with the same group ID as widget to be set as active
     */
    for (handle = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)__GH(h)->Parent, NULL); handle; 
            handle = gui_linkedlist_widgetgetnext(NULL, handle)) {
        if (handle != h && __GH(handle)->Widget == &Widget && __GR(handle)->GroupId == __GR(h)->GroupId) {  /* Check if widget is radio box and group is the same as clicked widget */
            __GR(handle)->SelectedValue = __GR(h)->Value;   /* Set selected value for widget */
            if (__GR(handle)->Flags & GUI_FLAG_RADIO_CHECKED) { /* Check if widget active */
                __GR(handle)->Flags &= ~GUI_FLAG_RADIO_CHECKED; /* Clear flag */
                gui_widget_invalidate__(handle);    /* Invalidate widget */
            }
        }
    }
    
    if (!(__GR(h)->Flags & GUI_FLAG_RADIO_CHECKED)) {   /* Invalidate only if not checked already */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    }
    __GR(h)->Flags |= GUI_FLAG_RADIO_CHECKED;       /* Set active flag */
    __GR(h)->SelectedValue = __GR(h)->Value;        /* Set selected value of this radio */
    gui_widget_callback__(h, GUI_WC_SelectionChanged, NULL, NULL);  /* Call user function */
    
    return 1;
}

/* Set disabled status */
static uint8_t
set_disabled(GUI_HANDLE_p h, uint8_t state) {
    if (state && !(c->Flags & GUI_FLAG_RADIO_DISABLED)) {
        c->Flags |= GUI_FLAG_RADIO_DISABLED;        /* Set flag */
        gui_widget_invalidate__(h);
        return 1;
    } else if (!state && (c->Flags & GUI_FLAG_RADIO_DISABLED)) {
        c->Flags &= ~GUI_FLAG_RADIO_DISABLED;       /* Clear flag */
        gui_widget_invalidate__(h);
        return 1;
    }
    return 0;
}

static uint8_t
gui_radio_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = GUI_WIDGET_PARAMTYPE_DISP(param);
            GUI_Color_t c1;
            GUI_iDim_t x, y, width, height, size, sx, sy;
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute X coordinate */
            y = gui_widget_getabsolutey__(h);       /* Get absolute Y coordinate */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            size = 20;                              /* Circle size in pixels */
            
            sx = x;
            sy = y + (height - size) / 2;
            
            if (__GR(h)->Flags & GUI_FLAG_RADIO_DISABLED) {
                c1 = gui_widget_getcolor__(h, GUI_RADIO_COLOR_DISABLED_BG);
            } else {
                c1 = gui_widget_getcolor__(h, GUI_RADIO_COLOR_BG);
            }
            
            gui_draw_filledcircle(disp, sx + size / 2, sy + size / 2, size / 2, c1);
            gui_draw_circle(disp, sx + size / 2, sy + size / 2, size / 2, gui_widget_getcolor__(h, GUI_RADIO_COLOR_BORDER));
            
            if (gui_widget_isfocused__(h) && !(c->Flags & GUI_FLAG_RADIO_DISABLED)) {   /* When in focus */
                gui_draw_circle(disp, sx + size / 2, sy + size / 2, size / 2 - 2, gui_widget_getcolor__(h, GUI_RADIO_COLOR_FG));
            }

            if (__GR(h)->Flags & GUI_FLAG_RADIO_CHECKED) {
                gui_draw_filledcircle(disp, sx + size / 2, sy + size / 2, size / 2 - 5, gui_widget_getcolor__(h, GUI_RADIO_COLOR_FG));
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
                f.Color1 = gui_widget_getcolor__(h, GUI_RADIO_COLOR_FG);
                gui_draw_writetext(disp, gui_widget_getfont__(h), gui_widget_gettext__(h), &f);
            }
            
            return 1;
        }
        case GUI_WC_Click: {
            if (!(c->Flags & GUI_FLAG_RADIO_DISABLED)) {
                set_active(h);                      /* Set widget as active */
                gui_widget_invalidate__(h);         /* Invalidate widget */
            }
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef c


/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new radio widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p
gui_radio_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_RADIO_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color vas not set
 */
uint8_t
gui_radio_setcolor(GUI_HANDLE_p h, GUI_RADIO_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return gui_widget_setcolor__(h, (uint8_t)index, color); /* Set color */
}

/**
 * \brief           Set radio group for widget
 * \note            Radio widgets with the same group must be on the same parent widget
 * \param[in,out]   h: Widget handle
 * \param[in]       groupId: Group ID for widget
 * \retval          1: Group set ok
 * \retval          0: Group was not set
 * \sa              gui_radio_getgroup
 */
uint8_t
gui_radio_setgroup(GUI_HANDLE_p h, uint8_t groupId) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GR(h)->GroupId != groupId) {
        GUI_HANDLE_p handle;
        
        /**
         * Find radio widgets on the same parent 
         * and with the group the same as desired group ID
         *
         * This is to set selected value to new radio with new group ID
         */
        for (handle = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)__GH(h)->Parent, NULL); handle;
                handle = gui_linkedlist_widgetgetnext(NULL, handle)) {
            if (handle != h && __GH(handle)->Widget == &Widget && __GR(handle)->GroupId == groupId) {   /* Check if widget is radio box and group is the same as input group */
                __GR(h)->SelectedValue = __GR(handle)->SelectedValue;   /* Set selected value for widget */
                break;          
            }
        }
        
        __GR(h)->GroupId = groupId;                 /* Set new group id */
        gui_widget_invalidate__(h);                 /* Invalidate widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get radio group for widget
 * \param[in,out]   h: Widget handle
 * \retval          Widget group
 * \sa              gui_radio_setgroup
 */
uint8_t
gui_radio_getgroup(GUI_HANDLE_p h) {
    uint8_t group = 0;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    group = __GR(h)->GroupId;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return group;
}

/**
 * \brief           Set value for widget when pressed
 * \param[in,out]   h: Widget handle
 * \param[in]       value: Value of widget group when specific widget is selected
 * \retval          1: Value set ok
 * \retval          0: Value was not set
 * \sa              gui_radio_getvalue
 */
uint8_t
gui_radio_setvalue(GUI_HANDLE_p h, uint32_t value) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (__GR(h)->Value != value) {
        __GR(h)->Value = value;                     /* Set new value */
        if (__GR(h)->Flags & GUI_FLAG_RADIO_CHECKED) {
            set_active(h);                          /* Check active radio widgets */
        }
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

/**
 * \brief           Get value for specific widget
 * \param[in,out]   h: Widget handle
 * \retval          Widget value
 * \sa              gui_radio_setvalue
 */
uint32_t
gui_radio_getvalue(GUI_HANDLE_p h) {
    uint32_t val;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    val = __GR(h)->Value;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Set radio widget selected in radio group
 * \param[in,out]   h: Widget handle
 * \retval          1: Widget selected ok
 * \retval          0: Widget was not selected
 */
uint8_t gui_radio_setselected(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = set_active(h);                            /* Set radio active */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Get value of selected widget from widget group
 * \note            If 3 widgets share the same group ID, no matter which widget is used in this function, result will be always the same
 * \param[in,out]   h: Widget handle
 * \retval          Widget selected value
 */
uint32_t
gui_radio_getselectedvalue(GUI_HANDLE_p h) {
    uint32_t val;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    val = __GR(h)->SelectedValue;
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

/**
 * \brief           Set disabled status to widget
 * \param[in,out]   h: Widget handle
 * \param[in]       disabled: Disabled status
 * \retval          1: Radio was set to desired disabled value
 * \retval          0: Radio was not set to desired disabled value
 */
uint8_t
gui_radio_setdisabled(GUI_HANDLE_p h, uint8_t disabled) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = set_disabled(h, disabled);                /* Set checked status */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

/**
 * \brief           Check if Radio is disabled
 * \param[in,out]   h: Widget handle
 * \retval          1: Radio is disabled
 * \retval          0: Radio is not disabled
 */
uint8_t
gui_radio_isdisabled(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    ret = !!(__GR(h)->Flags & GUI_FLAG_RADIO_DISABLED);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

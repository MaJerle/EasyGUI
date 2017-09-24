/**	
 * \file            gui_list_container.c
 * \brief           List container widget
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
#include "gui_list_container.h"

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
#define __GL(x)             ((GUI_LED_t *)(x))

#define CFG_TOGGLE          0x01
#define CFG_SET             0x02
#define CFG_TYPE            0x03

static uint8_t gui_listcontainer_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_LIGHTBLUE,                            /*!< Default color when led is on */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("LED"),                             /*!< Widget name */ 
    .Size = sizeof(GUI_LIST_CONTAINER_t),           /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN,        /*!< List of widget flags */
    .Callback = gui_listcontainer_callback,         /*!< Control function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define l           ((GUI_LIST_CONTAINER_t *)(h))

/* Calculate scroll limits according to children widgets */
static void
calculate_limits(GUI_HANDLE_p h) {
    GUI_HANDLE_p w;
    GUI_Dim_t x, y, width, height;
    GUI_Dim_t cmx = 0, cmy = 0;
    
    for (w = gui_linkedlist_widgetgetnext((GUI_HANDLE_ROOT_t *)h, NULL); w;
            w = gui_linkedlist_widgetgetnext(NULL, w)) {

        x = gui_widget_getrelativex__(w);           /* Get absolute position on screen */
        y = gui_widget_getrelativey__(w);           /* Get absolute position on screen */
        width = gui_widget_getwidth__(w);           /* Get widget width */
        height = gui_widget_getheight__(w);         /* Get widget height */
        
        if (x + width > cmx) {
            cmx = x + width;
        }
        if (y + height > cmy) {
            cmy = y + height;
        }
    }

    width = gui_widget_getwidth__(h);               /* Get widget width */
    height = gui_widget_getheight__(h);             /* Get widget height */
    
    l->MaxScrollX = 0;
    l->MaxScrollY = 0;
    if (width < cmx) {
        l->MaxScrollX = cmx - width;
    }
    if (height < cmy) {
        l->MaxScrollY = cmy - height;
    }
    
    if (gui_widget_getscrollx(h) > l->MaxScrollX) {
        gui_widget_setscrollx(h, l->MaxScrollX);
    }
    if (gui_widget_getscrolly(h) > l->MaxScrollY) {
        gui_widget_setscrolly(h, l->MaxScrollY);
    }
}

static uint8_t
gui_listcontainer_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            gui_widget_setpadding__(h, 3);          /* Set padding */
            return 1;
        }
        case GUI_WC_ChildWidgetCreated: {           /* Child widget has been created */
//            GUI_Dim_t x, y, width, height;
//            
//            x = gui_widget_getabsolutex__(param);   /* Get absolute position on screen */
//            y = gui_widget_getabsolutey__(param);   /* Get absolute position on screen */
//            width = gui_widget_getwidth__(param);   /* Get widget width */
//            height = gui_widget_getheight__(param); /* Get widget height */
            
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y, width, height;
            
            calculate_limits(h);                    /* Calculate new limits for scrolling */
            
            x = gui_widget_getabsolutex__(h);       /* Get absolute position on screen */
            y = gui_widget_getabsolutey__(h);       /* Get absolute position on screen */
            width = gui_widget_getwidth__(h);       /* Get widget width */
            height = gui_widget_getheight__(h);     /* Get widget height */
            
            if (width != l->LastWidth && height != l->LastHeight) {
                l->LastWidth = width;
                l->LastHeight = height;
            }
            
            gui_draw_filledrectangle(disp, x, y, width, height, gui_widget_getcolor__(h, GUI_LIST_CONTAINER_COLOR_BG));
            return 1;                               /* */
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */
            *(__GUI_TouchStatus_t *)result = touchHANDLED;
            gui_widget_incscrolly(h, ts->RelOldY[0] - ts->RelY[0]);
            if (gui_widget_getscrolly(h) < 0) {
                gui_widget_setscrolly(h, 0);
            }
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef l

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/

/**
 * \brief           Create new list container widget
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
gui_listcontainer_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)gui_widget_create__(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

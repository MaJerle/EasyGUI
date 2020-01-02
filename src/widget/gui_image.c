/**	
 * \file            gui_image.c
 * \brief           Image widget
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "widget/gui_image.h"

/**
 * \ingroup         GUI_IMAGE
 * \brief           Image object structure
 */
typedef struct {
    gui_handle C;                                   /*!< GUI handle object, must always be first on list */
    
    const gui_image_desc_t* image;                  /*!< Pointer to image object to draw */
} gui_image_t;

static uint8_t gui_image_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \brief           Widget initialization structure
 */
static const
gui_widget_t widget = {
    .name = _GT("IMAGE"),                           /*!< Widget name */
    .size = sizeof(gui_image_t),                    /*!< Size of widget for memory allocation */
    .flags = 0,                                     /*!< List of widget flags */
    .callback = gui_image_callback,                 /*!< Callback function */
    .colors = 0,                                    /*!< List of default colors */
    .color_count = 0,                               /*!< Define number of colors */
};

/**
 * \brief           Default widget callback function
 * \param[in]       h: Widget handle
 * \param[in]       evt: Event type
 * \param[in]       param: Input parameters for callback type
 * \param[out]      result: Result for callback type
 * \return          `1` if command processed, `0` otherwise
 */
static uint8_t
gui_image_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    gui_image_t* o = GUI_VP(h);
    
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget);
    switch (evt) {
        case GUI_EVT_DRAW: {
            gui_display_t* disp = GUI_EVT_PARAMTYPE_DISP(param);
            gui_dim_t x, y;

            x = gui_widget_getabsolutex(h);
            y = gui_widget_getabsolutey(h);
            
            gui_draw_image(disp, x, y, o->image);   /* Draw actual image on screen */
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/**
 * \brief           Create new image widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget `X` position relative to parent widget
 * \param[in]       y: Widget `Y` position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in units of pixels
 * \param[in]       parent: Parent widget handle. Set to `NULL` to use current active parent widget
 * \param[in]       evt_fn: Custom widget callback function. Set to `NULL` to use default callback
 * \param[in]       flags: flags for widget creation
 * \return          Widget handle on success, `NULL` otherwise
 */
gui_handle_p
gui_image_create(gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags) {
    return (gui_handle_p)gui_widget_create(&widget, id, x, y, width, height, parent, evt_fn, flags);
}

/**
 * \brief           Set image source to draw
 * \param[in]       h: Widget handle
 * \param[in]       img: Image object. Set to `NULL` to clear image
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_image_setsource(gui_handle_p h, const gui_image_desc_t* img) {
    gui_image_t* o = GUI_VP(h);
    GUI_ASSERTPARAMS(h != NULL && h->widget == &widget && img != NULL);

    o->image = img;                                 /* Set image */
    
    /* When alpha image is applied, invalidate parent */
    gui_widget_setinvalidatewithparent(h, img != NULL && img->bpp == 32);
    gui_widget_invalidatewithparent(h);

    return 1;
}

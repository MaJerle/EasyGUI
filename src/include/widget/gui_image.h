/**	
 * \file            gui_image.h
 * \brief           Image widget
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
#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_IMAGE Image
 * \brief           Image display widget
 * \{
 *
 * Image widget can display images of different color depth (ARGB8888, RGB888 or RGB565)
 *
 * \image html image_widget_image.png Image widget of 3 images. On the left is ARGB8888 (TrueColor with alpha), middle one is RGB888 and right one format is RGB565
 *
 * \note            When image has transparent alpha channel then on each redraw, parent widget must be redrawn. 
 *                  In other words, each image with alpha channel which is not maximum (fully visible) must use more resources to redraw image widget by redrawing parent first.
 *                  On above image is clearly visible how left image has transparency (source data) over button.
 *
 * Example code for example above
 * \code{c}
handle = gui_button_create(0, 10, 10, 300, 50, 0, 0, 0);
gui_widget_settext(handle, _GT("Button"));
handle = gui_image_create(0, 2, 2, bmimage_brand.xSize, bmimage_brand.ySize, 0, 0, 0);
gui_image_setsource(handle, &bmimage_brand);
handle = gui_image_create(0, 200, 40, bmimage_voyo.xSize, bmimage_voyo.ySize, 0, 0, 0);
gui_image_setsource(handle, &bmimage_voyo);
handle = gui_image_create(0, 350, 10, bmimage_voyo565.xSize, bmimage_voyo565.ySize, 0, 0, 0);
gui_image_setsource(handle, &bmimage_voyo565);
\endcode
 */

#if defined(GUI_INTERNAL) || __DOXYGEN__
/**
 * \brief           Image widget structure
 */
typedef struct GUI_IMAGE_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    const GUI_IMAGE_DESC_t* Image;          /*!< Pointer to image object to draw */
} GUI_IMAGE_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */
    
GUI_HANDLE_p gui_image_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_image_setsource(GUI_HANDLE_p h, const GUI_IMAGE_DESC_t* img);

/**
 * \}
 */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

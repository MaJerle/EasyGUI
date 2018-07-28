/**	
 * \file            gui_dialog.h
 * \brief           Dialog widget
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#ifndef __GUI_DIALOG_H
#define __GUI_DIALOG_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_widget.h"
    
/**
 * \ingroup         GUI_WIDGETS
 * \defgroup        GUI_DIALOG Dialog base element
 * \brief           Base element for dialog, usually parent of dialog window
 * \{
 */

#if defined(GUI_INTERNAL) || __DOXYGEN__
/**
 * \brief           Dialog object structure
 */
typedef struct {
    gui_handle C;                           /*!< GUI handle object, must always be first on list */
} GUI_DIALOG_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

gui_handle_p    gui_dialog_create(gui_id_t id, float x, float y, float width, float height, gui_widget_createfunc_t func, gui_widget_callback_t cb, uint16_t flags, const uint8_t protect);
int             gui_dialog_createblocking(gui_id_t id, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_widget_createfunc_t func, gui_widget_callback_t cb, uint16_t flags);
uint8_t         gui_dialog_dismiss(gui_handle_p h, int status, const uint8_t protect);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __GUI_DIALOG_H */

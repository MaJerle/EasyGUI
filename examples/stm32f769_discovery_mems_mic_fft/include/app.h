/**
 * \file            app.h
 * \brief           Application header
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef __APP_H
#define __APP_H

#include "cmsis_os.h"

/* GUI library */
#include "gui/gui.h"
#include "widget/gui_window.h"
#include "widget/gui_button.h"
#include "widget/gui_led.h"
#include "widget/gui_progbar.h"
#include "widget/gui_graph.h"
#include "widget/gui_edittext.h"
#include "widget/gui_checkbox.h"
#include "widget/gui_radio.h"
#include "widget/gui_listbox.h"
#include "widget/gui_listview.h"
#include "widget/gui_textview.h"
#include "widget/gui_dropdown.h"
#include "widget/gui_dialog.h"
#include "widget/gui_image.h"
#include "widget/gui_slider.h"
#include "widget/gui_container.h"
#include "widget/gui_debugbox.h"
#include "widget/gui_list_container.h"
#include "gui/gui_keyboard.h"
#include "gui/gui_lcd.h"
#include "netconn_server.h"

#include "tm_stm32_touch.h"

#include "gui_app.h"

extern TM_TOUCH_t TS;

/* Thread definitions */
void user_thread(void const * arg);
void init_thread(void const * arg);

/* Read touch function prototype */
void read_touch(void);

/* Widget event functions */
uint8_t gui_graph_callback(gui_handle_p h, gui_wc_t ctrl, gui_widget_param_t* param, gui_widget_result_t* result);

/* List of GUI widget IDs */
#define GUI_ID_GRAPH_FFT            (GUI_ID_USER + 0x0001)

/* List of fonts */
extern gui_const gui_font_t GUI_Font_Arial_Bold_18;

#endif /* __APP_H */

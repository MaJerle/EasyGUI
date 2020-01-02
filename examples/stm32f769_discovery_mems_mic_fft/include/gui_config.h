/**
 * \file            gui_config_template.h
 * \brief           Template config file
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
#ifndef __GUI_CONFIG_H
#define __GUI_CONFIG_H

/*
 * Rename this file to "gui_config.h" for your application
 */

/*
 * Open "include/gui/gui_config_default.h" and 
 * copy & replace here settings you want to change values
 */
 
#define GUI_CFG_USE_KEYBOARD                        1
#define GUI_CFG_USE_TRANSPARENCY                    1
#define GUI_CFG_USE_UNICODE                         1

#define GUI_CFG_WIDGET_GRAPH_DATA_AUTO_INVALIDATE   0

/* After user configuration, call default config to merge config together */
#include "gui/gui_config_default.h"

#endif /* __GUI_CONFIG_H */

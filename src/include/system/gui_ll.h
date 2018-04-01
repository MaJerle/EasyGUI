/**	
 * \file            gui_ll.h
 * \brief           Low-level LCD part
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
#ifndef __GUI_LL_H
#define __GUI_LL_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_PORT
 * \{
 */

/**
 * \defgroup        GUI_LL Low-level layer
 * \brief       
 * \{
 */

#include "gui/gui.h"
#include "gui/gui_lcd.h"

/**
 * \brief           Initialize LCD low-level display driver
 *
 *                  - Set up width and height
 *                  - Set up number of layers for drawing
 *                  - Set up drawing functions
 *
 * \param[in,out]   LCD: Pointer to \ref gui_lcd_t structure for LCD properties
 * \param[in,out]   LL: Pointer to \ref gui_ll_t structure with drawing functions
 * \return          0 on success, non-zero otherwise
 */
uint8_t gui_ll_init(gui_lcd_t* LCD, gui_ll_t* LL);

/**
 * \brief           Send command to for LCD from GUI
 * \param[in,out]   LCD: Pointer to \ref gui_lcd_t structure with LCD properties
 * \param[in]       cmd: Command to be executed. This parameter can be a value of \ref GUI_LL_Command_t enumeration
 * \param[in]       param: Optional data included in command. Check \ref GUI_LL_Command_t enumeration what is used for each command
 * \param[out]      result: Result from command
 * \return          0 on success, non-zero otherwise
 */
uint8_t gui_ll_control(gui_lcd_t* LCD, GUI_LL_Command_t cmd, void* param, void* result);
 
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

#endif /* __GUI_LL_H */

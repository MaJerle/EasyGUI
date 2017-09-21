/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI low-level implementation
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2017 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_LL_H
#define GUI_LL_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_LL Low-level layer
 * \brief       
 * \{
 */
#include "gui.h"
#include "gui_lcd.h"

/**
 * \brief           Initialize LCD low-level display driver
 *
 *                  - Set up width and height
 *                  - Set up number of layers for drawing
 *                  - Set up drawing functions
 * \param[in,out]   *LCD: Pointer to \ref GUI_LCD_t structure for LCD properties
 * \param[in,out]   *LL: Pointer to \ref GUI_LL_t structure with drawing functions
 * \retval          0: Initialization successful
 * \retval          1: Initialization failed
 */
uint8_t gui_ll_init(GUI_LCD_t* LCD, GUI_LL_t* LL);

/**
 * \brief           Send command to for LCD from GUI
 * \param[in,out]   *LCD: Pointer to \ref GUI_LCD_t structure with LCD properties
 * \param[in]       cmd: Command to be executed. This parameter can be a value of \ref GUI_LL_Command_t enumeration
 * \param[in]       *param: Optional data included in command. Check \ref GUI_LL_Command_t enumeration what is used for each command
 * \param[out]      *result: Result from command
 * \retval          1: Command has been processed
 * \retval          0: Command has not been processed
 */
uint8_t gui_ll_control(GUI_LCD_t* LCD, GUI_LL_Command_t cmd, void* param, void* result);
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

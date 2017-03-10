/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \brief   GUI WIDGET object manager
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

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
#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI
 * \brief       
 * \{
 */
#include "gui.h"
#include "gui_draw.h"

/**
 * \defgroup        GUI_WIDGET_Macros
 * \brief           Library defines
 * \{
 */

#define GUI_WIDGET_GetParentWidth(ptr)              (__GH(ptr)->Parent ? __GH(ptr)->Parent->Width : GUI.LCD.Width)
#define GUI_WIDGET_GetParentHeight(ptr)             (__GH(ptr)->Parent ? __GH(ptr)->Parent->Height : GUI.LCD.Height)

 /**
 * \defgroup        GUI_WIDGET_ID_Values
 * \brief           Macros for fast ID setup
 * \{
 */   
#define GUI_ID_WINDOW_BASE          ((uint16_t)0x0000)
#define GUI_ID_WINDOW_1             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0001))
#define GUI_ID_WINDOW_2             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0002))
#define GUI_ID_WINDOW_3             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0003))
#define GUI_ID_WINDOW_4             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0004))
#define GUI_ID_WINDOW_5             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0005))
#define GUI_ID_WINDOW_6             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0006))
#define GUI_ID_WINDOW_7             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0007))
#define GUI_ID_WINDOW_8             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0008))
#define GUI_ID_WINDOW_9             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0009))
#define GUI_ID_WINDOW_10            ((uint16_t)(GUI_ID_WINDOW_BASE + 0x000A))
    
#define GUI_ID_BUTTON_BASE          ((uint16_t)0x0100)
#define GUI_ID_BUTTON_1             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0001))
#define GUI_ID_BUTTON_2             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0002))
#define GUI_ID_BUTTON_3             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0003))
#define GUI_ID_BUTTON_4             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0004))
#define GUI_ID_BUTTON_5             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0005))
#define GUI_ID_BUTTON_6             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0006))
#define GUI_ID_BUTTON_7             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0007))
#define GUI_ID_BUTTON_8             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0008))
#define GUI_ID_BUTTON_9             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0009))
#define GUI_ID_BUTTON_10            ((uint16_t)(GUI_ID_BUTTON_BASE + 0x000A)
    
/**
 * \} GUI_WIDGET_ID_Values
 */

/**
 * \} GUI_WIDGET_Macros
 */
 
/**
 * \defgroup        GUI_WIDGET_Typedefs
 * \brief           Library Typedefs
 * \{
 */

/**
 * \} GUI_WIDGET_Typedefs
 */

/**
 * \defgroup        GUI_WIDGET_Functions
 * \brief           Library Functions
 * \{
 */

void __GUI_WIDGET_Init(void);
GUI_Dim_t __GUI_WIDGET_GetAbsoluteX(void* ptr);
GUI_Dim_t __GUI_WIDGET_GetAbsoluteY(void* ptr);
uint8_t __GUI_WIDGET_Invalidate(void* ptr);
uint8_t __GUI_WIDGET_InvalidateWithParent(void* ptr);

uint8_t __GUI_WIDGET_SetXY(void* ptr, GUI_iDim_t x, GUI_iDim_t y);
uint8_t __GUI_WIDGET_SetSize(void* ptr, GUI_Dim_t width, GUI_Dim_t height);

uint8_t __GUI_WIDGET_Enable3DStyle(void* ptr);
uint8_t __GUI_WIDGET_Disable3DStyle(void* ptr);

uint8_t __GUI_WIDGET_SetFont(void* ptr, GUI_Const GUI_FONT_t* font);
uint8_t __GUI_WIDGET_SetText(void* ptr, const char* text);
uint8_t __GUI_WIDGET_AllocateTextMemory(void* ptr, uint16_t size);
uint8_t __GUI_WIDGET_FreeTextMemory(void* ptr);

GUI_HANDLE_t __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height);
uint8_t __GUI_WIDGET_Remove(GUI_HANDLE_t* h);

void __GUI_WIDGET_SetClippingRegion(void* ptr);
uint8_t __GUI_WIDGET_IsInsideClippingRegion(void* ptr);

/**
 * \} GUI_WIDGET_Functions
 */
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

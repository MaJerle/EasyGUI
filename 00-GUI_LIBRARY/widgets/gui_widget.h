/**
 * \author  Tilen Majerle
 * \brief   GUI Widget handle manager
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
#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_WIDGETS Widgets
 * \brief           Group for visible widgets on screen
 * \{
 */
    
/**
 * \defgroup        GUI_WIDGETS_CORE Core widget functions
 * \brief           Core functions for all widgets
 *
 * Use can use all function which do not start with <b>__GUI</b> which indicate private functions.
 *
 * \{
 */
#include "gui.h"
#include "gui_draw.h"

/**
 * \defgroup        GUI_WIDGET_ID_Values Predefined ID values
 * \brief           Macros for fast ID setup
 * \{
 */  
    
#define GUI_ID_WINDOW_BASE          ((uint16_t)0x0000)                          /*!< Window base ID */
#define GUI_ID_WINDOW_1             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0001))   /*!< Window object ID 1 */
#define GUI_ID_WINDOW_2             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0002))   /*!< Window object ID 2 */
#define GUI_ID_WINDOW_3             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0003))   /*!< Window object ID 3 */
#define GUI_ID_WINDOW_4             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0004))   /*!< Window object ID 4 */
#define GUI_ID_WINDOW_5             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0005))   /*!< Window object ID 5 */
#define GUI_ID_WINDOW_6             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0006))   /*!< Window object ID 6 */
#define GUI_ID_WINDOW_7             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0007))   /*!< Window object ID 7 */
#define GUI_ID_WINDOW_8             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0008))   /*!< Window object ID 8 */
#define GUI_ID_WINDOW_9             ((uint16_t)(GUI_ID_WINDOW_BASE + 0x0009))   /*!< Window object ID 9 */
#define GUI_ID_WINDOW_10            ((uint16_t)(GUI_ID_WINDOW_BASE + 0x000A))   /*!< Window object ID 10 */
    
#define GUI_ID_BUTTON_BASE          ((uint16_t)0x0100)                          /*!< Button base ID */
#define GUI_ID_BUTTON_1             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0001))   /*!< Button object ID 1 */
#define GUI_ID_BUTTON_2             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0002))   /*!< Button object ID 2 */
#define GUI_ID_BUTTON_3             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0003))   /*!< Button object ID 3 */
#define GUI_ID_BUTTON_4             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0004))   /*!< Button object ID 4 */
#define GUI_ID_BUTTON_5             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0005))   /*!< Button object ID 5 */
#define GUI_ID_BUTTON_6             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0006))   /*!< Button object ID 6 */
#define GUI_ID_BUTTON_7             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0007))   /*!< Button object ID 7 */
#define GUI_ID_BUTTON_8             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0008))   /*!< Button object ID 8 */
#define GUI_ID_BUTTON_9             ((uint16_t)(GUI_ID_BUTTON_BASE + 0x0009))   /*!< Button object ID 9 */
#define GUI_ID_BUTTON_10            ((uint16_t)(GUI_ID_BUTTON_BASE + 0x000A))   /*!< Button object ID 10 */
    
/**
 * \}
 */

/**
 * \brief           Returns width of parent element. If parent does not exists, it returns LCD width
 * \param[in]       h: Pointer to \ref GUI_HANDLE_t structure
 * \retval          Parent width in units of pixels
 * \hideinitializer
 */
#define GUI_WIDGET_GetParentWidth(h)                (__GH(h)->Parent ? __GH(h)->Parent->Width : GUI.LCD.Width)

/**
 * \brief           Returns height of parent element. If parent does not exists, it returns LCD height
 * \param[in,out]   h: Widget handle
 * \retval          Parent height in units of pixels
 * \hideinitializer
 */
#define GUI_WIDGET_GetParentHeight(h)               (__GH(h)->Parent ? __GH(h)->Parent->Height : GUI.LCD.Height)

/**
 * \brief           Checks if Widget handle is currently in focus
 * \param[in,out]   h: \ref GUI_HANDLE_t widget
 * \retval          Status whether widget is in focus or not
 * \hideinitializer
 */
#define GUI_WIDGET_IsFocused(h)                     (__GH(h) == GUI.FocusedWidget)

/**
 * \brief           Remove widget from memory
 * \note            If widget has child widgets, they will be removed too
 * \param[in,out]   *h: Pointer to widget handle. If removed, pointer will be invalidated and set to NULL
 * \retval          1: Widget deleted
 * \retval          0: Delete failed
 */
uint8_t GUI_WIDGET_Remove(GUI_HANDLE_t* h);

/**
 * \brief           Allocate memory for text operations if text will be dynamic
 * \note            When unicode feature is enabled, memory should be 4x required characters because unicode can store up to 4 bytes for single character
 * \param[in,out]   h: Widget handle
 * \param[in]       size: Number of bytes to allocate
 * \retval          Number of bytes allocated
 * \sa              GUI_WIDGET_FreeTextMemory()
 */
uint32_t GUI_WIDGET_AllocTextMemory(GUI_HANDLE_t h, uint32_t size);

/**
 * \brief           Frees memory previously allocated for text
 * \param[in,out]   h: Widget handle to free memory on
 * \retval          Widget handle
 * \sa              GUI_WIDGET_AllocTextMemory()
 */
GUI_HANDLE_t GUI_WIDGET_FreeTextMemory(GUI_HANDLE_t h);

/**
 * \brief           Set text to widget
 * \note            If dynamic memory allocation was used then content will be copied to allocated memory
 *                     otherwise only pointer to input text will be used 
 *                     and each further change of input pointer text will affect to output
 * \param[in,out]   h: Widget handle
 * \retval          Widget handle
 * \sa              GUI_WIDGET_AllocTextMemory()
 * \sa              GUI_WIDGET_SetFont()
 */
GUI_HANDLE_t GUI_WIDGET_SetText(GUI_HANDLE_t h, const GUI_Char* text);

/**
 * \brief           Set widget font for drawing operations
 * \param[in,out]   h: Widget handle
 * \param[in]       *font: Pointer to \ref GUI_FONT_t object for font
 * \retval          Widget handle
 * \sa              GUI_WIDGET_SetText()
 */
GUI_HANDLE_t GUI_WIDGET_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font);

/**
 * \brief           Set widget size in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       width: Height in units of pixels
 * \param[in]       height: Width in units of pixels
 * \retval          Widget handle
 * \sa              GUI_WIDGET_SetXY()
 */
GUI_HANDLE_t GUI_WIDGET_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height);

/**
 * \brief           Set widget position relative to parent object in units of pixels
 * \param[in,out]   h: Widget handle
 * \param[in]       x: X position relative to parent object
 * \param[in]       y: Y position relative to parent object
 * \retval          Widget handle
 * \sa              GUI_WIDGET_SetSize()
 */
GUI_HANDLE_t GUI_WIDGET_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y);

/**
 * \brief           Invalidate widget object and prepare to new redraw
 * \param[in,out]   h: Widget handle
 * \retval          Widget handle
 */
GUI_HANDLE_t GUI_WIDGET_Invalidate(GUI_HANDLE_t h);

#if !defined(DOXYGEN)
void __GUI_WIDGET_Init(void);
GUI_Dim_t __GUI_WIDGET_GetAbsoluteX(GUI_HANDLE_t h);
GUI_Dim_t __GUI_WIDGET_GetAbsoluteY(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_Invalidate(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_InvalidateWithParent(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_SetXY(GUI_HANDLE_t h, GUI_iDim_t x, GUI_iDim_t y);
uint8_t __GUI_WIDGET_SetSize(GUI_HANDLE_t h, GUI_Dim_t width, GUI_Dim_t height);
uint8_t __GUI_WIDGET_Enable3DStyle(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_Disable3DStyle(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_SetFont(GUI_HANDLE_t h, GUI_Const GUI_FONT_t* font);
uint8_t __GUI_WIDGET_SetText(GUI_HANDLE_t h, const GUI_Char* text);
uint8_t __GUI_WIDGET_AllocateTextMemory(GUI_HANDLE_t h, uint32_t size);
uint8_t __GUI_WIDGET_FreeTextMemory(GUI_HANDLE_t h);
GUI_HANDLE_t __GUI_WIDGET_Create(const GUI_WIDGET_t* widget, GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height);
uint8_t __GUI_WIDGET_Remove(GUI_HANDLE_t* h);
void __GUI_WIDGET_SetClippingRegion(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_IsInsideClippingRegion(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_IsFontAndTextSet(GUI_HANDLE_t h);
uint8_t __GUI_WIDGET_ProcessTextKey(GUI_HANDLE_t h, GUI_KeyboardData_t* key);
#endif /* !defined(DOXYGEN) */

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

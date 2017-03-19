/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website 
 * \license MIT
 * \version 0.1.0
 * \brief   GUI Library
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
#ifndef GUI_DRAW_H
#define GUI_DRAW_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_DRAW
 * \brief           LCD Drawing operations
 * \{
 */

/*!< Important GUI definitions */
#include "gui.h"

/**
 * \defgroup        GUI_DRAW_Macros
 * \brief           Macros for drawing operations
 * \{
 */

#define GUI_DRAW_CIRCLE_TL              0x01/*!< Draw top left part of circle */
#define GUI_DRAW_CIRCLE_TR              0x02/*!< Draw top right part of circle */
#define GUI_DRAW_CIRCLE_BR              0x04/*!< Draw bottom left part of circle */
#define GUI_DRAW_CIRCLE_BL              0x08/*!< Draw bottom right part of circle */
    
#define GUI_HALIGN_LEFT                 0x01/*!< Horizontal align is left */ 
#define GUI_HALIGN_CENTER               0x02/*!< Horizontal align is center */
#define GUI_HALIGN_RIGHT                0x04/*!< Horizontal align is right */
#define GUI_VALIGN_TOP                  0x08/*!< Vertical align is top */
#define GUI_VALIGN_CENTER               0x10/*!< Vertical align is center */
#define GUI_VALIGN_BOTTOM               0x20/*!< Vertical align is bottom */

/**
 * \} GUI_DRAW_Macros
 */

/**
 * \defgroup        GUI_DRAW_Typedefs
 * \brief           
 * \{
 */           

/**
 * \brief           Structure for drawing strings on widgets
 */
typedef struct GUI_DRAW_FONT_t {
    GUI_Dim_t X;                            /*!< Top left X position for rectangle */
    GUI_Dim_t Y;                            /*!< Top left Y position for rectangle */
    GUI_Dim_t Width;                        /*!< Rectangle width for string draw */
    GUI_Dim_t Height;                       /*!< Rectangle height for string draw */
    GUI_Byte Align;                         /*!< Alignment parameters */
    GUI_Byte Flags;                         /*!< Flags for future use */
    GUI_Dim_t Color1Width;                  /*!< Width for color 1 */
    GUI_Color_t Color1;                     /*!< Color 1 */
    GUI_Color_t Color2;                     /*!< Color 2 */
} GUI_DRAW_FONT_t;

typedef enum GUI_DRAW_3D_State_t {
    GUI_DRAW_3D_State_Raised = 0x00,        /*!< Raised 3D style */
    GUI_DRAW_3D_State_Lowered = 0x01        /*!< Lowered 3D style */
} GUI_DRAW_3D_State_t;

/**
 * \}
 */

/**
 * \defgroup        GUI_DRAW_Functions
 * \brief           Library Functions
 * \{
 */

void GUI_DRAW_FillScreen(GUI_Display_t* disp, GUI_Color_t color);

void GUI_DRAW_SetPixel(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Color_t color);
GUI_Color_t GUI_DRAW_GetPixel(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y);
void GUI_DRAW_VLine(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color);
void GUI_DRAW_HLine(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color);
void GUI_DRAW_Line(GUI_Display_t* disp, GUI_Dim_t x1, GUI_Dim_t y1, GUI_Dim_t x2, GUI_Dim_t y2, GUI_Color_t color);
void GUI_DRAW_Rectangle(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_Color_t color);
void GUI_DRAW_FilledRectangle(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_Color_t color);
void GUI_DRAW_RoundedRectangle(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_Dim_t r, GUI_Color_t color);
void GUI_DRAW_FilledRoundedRectangle(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_Dim_t r, GUI_Color_t color);
void GUI_DRAW_Circle(GUI_Display_t* disp, GUI_Dim_t x0, GUI_Dim_t y0, GUI_Dim_t r, GUI_Color_t color);
void GUI_DRAW_FilledCircle(GUI_Display_t* disp, GUI_Dim_t x0, GUI_Dim_t y0, GUI_Dim_t r, GUI_Color_t color);
void GUI_DRAW_Triangle(GUI_Display_t* disp, GUI_Dim_t x1, GUI_Dim_t y1,  GUI_Dim_t x2, GUI_Dim_t y2, GUI_Dim_t x3, GUI_Dim_t y3, GUI_Color_t color);
void GUI_DRAW_FilledTriangle(GUI_Display_t* disp, GUI_Dim_t x1, GUI_Dim_t y1, GUI_Dim_t x2, GUI_Dim_t y2, GUI_Dim_t x3, GUI_Dim_t y3, GUI_Color_t color);
void GUI_DRAW_CircleCorner(GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Byte_t c, GUI_Color_t color);
void GUI_DRAW_FilledCircleCorner(GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Byte_t c, uint32_t color);

//3D shapes
void GUI_DRAW_Rectangle3D(GUI_Display_t* disp, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_DRAW_3D_State_t state);

//Draw text in box with custom alignment
void GUI_DRAW_WriteText(GUI_Display_t* disp, GUI_Const GUI_FONT_t* font, const char* str, GUI_DRAW_FONT_t* draw);

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

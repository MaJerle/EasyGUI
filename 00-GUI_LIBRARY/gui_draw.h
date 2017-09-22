/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI DRAW
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
 * \defgroup        GUI_DRAW Drawing process
 * \brief           LCD Drawing operations
 * \{
 */

#include "gui.h"

/**
 * \defgroup        GUI_COLORS Colors
 * \brief           List of default GUI colors for fast development
 *
 * There are only predefined GUI colors which are most used.
 * For other colors, user can simply use any other RGB combination for any colors of 16M possible values
 * \{
 */

#define GUI_COLOR_BLUE                  0xFF0000FF  /*!< ARGB8888 red color with 100% alpha channel */
#define GUI_COLOR_GREEN                 0xFF00FF00  /*!< ARGB8888 green color with 100% alpha channel */
#define GUI_COLOR_RED                   0xFFFF0000  /*!< ARGB8888 red color with 100% alpha channel */
#define GUI_COLOR_CYAN                  0xFF00FFFF  /*!< ARGB8888 cyan color with 100% alpha channel */
#define GUI_COLOR_MAGENTA               0xFFFF00FF  /*!< ARGB8888 magenta color with 100% alpha channel */
#define GUI_COLOR_YELLOW                0xFFFFFF00  /*!< ARGB8888 yellow color with 100% alpha channel */
#define GUI_COLOR_LIGHTBLUE             0xFF8080FF  /*!< ARGB8888 light blue color with 100% alpha channel */
#define GUI_COLOR_LIGHTGREEN            0xFF80FF80  /*!< ARGB8888 light green color with 100% alpha channel */
#define GUI_COLOR_LIGHTRED              0xFFFF8080  /*!< ARGB8888 light red color with 100% alpha channel */
#define GUI_COLOR_LIGHTCYAN             0xFF80FFFF  /*!< ARGB8888 light cyan color with 100% alpha channel */
#define GUI_COLOR_LIGHTMAGENTA          0xFFFF80FF  /*!< ARGB8888 light magenta color with 100% alpha channel */
#define GUI_COLOR_LIGHTYELLOW           0xFFFFFF80  /*!< ARGB8888 light yellow color with 100% alpha channel */
#define GUI_COLOR_DARKBLUE              0xFF000080  /*!< ARGB8888 dark blue color with 100% alpha channel */
#define GUI_COLOR_DARKGREEN             0xFF008000  /*!< ARGB8888 dark green color with 100% alpha channel */
#define GUI_COLOR_DARKRED               0xFF800000  /*!< ARGB8888 dark red color with 100% alpha channel */
#define GUI_COLOR_DARKCYAN              0xFF008080  /*!< ARGB8888 dark cyan color with 100% alpha channel */
#define GUI_COLOR_DARKMAGENTA           0xFF800080  /*!< ARGB8888 dark magenta color with 100% alpha channel */
#define GUI_COLOR_DARKYELLOW            0xFF808000  /*!< ARGB8888 dark yellow color with 100% alpha channel */
#define GUI_COLOR_WHITE                 0xFFFFFFFF  /*!< ARGB8888 white color with 100% alpha channel */
#define GUI_COLOR_LIGHTGRAY             0xFFD3D3D3  /*!< ARGB8888 light gray color with 100% alpha channel */
#define GUI_COLOR_GRAY                  0xFF808080  /*!< ARGB8888 gray color with 100% alpha channel */
#define GUI_COLOR_DARKGRAY              0xFF404040  /*!< ARGB8888 dark gray color with 100% alpha channel */
#define GUI_COLOR_BLACK                 0xFF000000  /*!< ARGB8888 black color with 100% alpha channel */
#define GUI_COLOR_BROWN                 0xFFA52A2A  /*!< ARGB8888 brown color with 100% alpha channel */
#define GUI_COLOR_ORANGE                0xFFFFA500  /*!< ARGB8888 orange color with 100% alpha channel */
#define GUI_COLOR_TRANS                 0x00000000  /*!< Transparent color */

#define GUI_COLOR_ALPHA_0               0x00000000  /*!< 0% alpha. Color is transparent */
#define GUI_COLOR_ALPHA_5               0x0C000000  /*!< 5% alpha of selected color */
#define GUI_COLOR_ALPHA_10              0x19000000  /*!< 10% alpha of selected color */
#define GUI_COLOR_ALPHA_15              0x26000000  /*!< 15% alpha of selected color */
#define GUI_COLOR_ALPHA_20              0x33000000  /*!< 20% alpha of selected color */
#define GUI_COLOR_ALPHA_25              0x3F000000  /*!< 25% alpha of selected color */
#define GUI_COLOR_ALPHA_30              0x4C000000  /*!< 30% alpha of selected color */
#define GUI_COLOR_ALPHA_35              0x59000000  /*!< 35% alpha of selected color */
#define GUI_COLOR_ALPHA_40              0x66000000  /*!< 40% alpha of selected color */
#define GUI_COLOR_ALPHA_45              0x72000000  /*!< 45% alpha of selected color */
#define GUI_COLOR_ALPHA_50              0x7F000000  /*!< 50% alpha of selected color */
#define GUI_COLOR_ALPHA_55              0x7C000000  /*!< 55% alpha of selected color */
#define GUI_COLOR_ALPHA_60              0x99000000  /*!< 60% alpha of selected color */
#define GUI_COLOR_ALPHA_65              0xA5000000  /*!< 65% alpha of selected color */
#define GUI_COLOR_ALPHA_70              0xB2000000  /*!< 70% alpha of selected color */
#define GUI_COLOR_ALPHA_75              0xBF000000  /*!< 75% alpha of selected color */
#define GUI_COLOR_ALPHA_80              0xCC000000  /*!< 80% alpha of selected color */
#define GUI_COLOR_ALPHA_85              0xD8000000  /*!< 85% alpha of selected color */
#define GUI_COLOR_ALPHA_90              0xE5000000  /*!< 90% alpha of selected color */
#define GUI_COLOR_ALPHA_95              0xF2000000  /*!< 95% alpha of selected color */
#define GUI_COLOR_ALPHA_100             0xFF000000  /*!< 100% alpha of selected color */

#define GUI_COLOR_WIN_DARKGRAY          0xFF555555  /*!< Windows dark gray color */
#define GUI_COLOR_WIN_MIDDLEGRAY        0xFF848484  /*!< Windows middle gray color */
#define GUI_COLOR_WIN_LIGHTGRAY         0xFFC6C6C6  /*!< Windows light gray color */

#define GUI_COLOR_WIN_BLUE              0xFF5590B7  /*!< Windows blue color */
#define GUI_COLOR_WIN_RED               0xFFD14752  /*!< Windows red color */

#define GUI_COLOR_WIN_BG                GUI_COLOR_WHITE /*!< Default widget background color */
#define GUI_COLOR_WIN_TEXT              GUI_COLOR_BLACK /*!< Default text color */
#define GUI_COLOR_WIN_TEXT_TITLE        GUI_COLOR_BLACK /*!< Default text color for titles/labels */
#define GUI_COLOR_WIN_SEL_FOC           GUI_COLOR_WHITE /*!< Text color of selected item when widget in focus */
#define GUI_COLOR_WIN_SEL_NOFOC         GUI_COLOR_WHITE /*!< Text color of selected item when widget not in focus */
#define GUI_COLOR_WIN_SEL_FOC_BG        GUI_COLOR_BLUE  /*!< Background color of selected item when widget in focus */
#define GUI_COLOR_WIN_SEL_NOFOC_BG      GUI_COLOR_WIN_MIDDLEGRAY    /*!< Background color of selected item when widget not in focus */

/**
 * \brief           Macro for combining default color with transparent value
 * \param[in]       c: Colors in ARGB 8888 format
 * \param[in]       a: Alpha between 0 and 255. Use prefedined alpha macros
 * \retval          ARGB8888 color with mixed alpha channel
 * \hideinitializer
 */
#define GUI_COLOR_ALPHA(c, a)           (((c) & 0x00FFFFFFUL) | (a))
    
/**
 * \}
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
 * \brief           Structure for drawing strings on widgets
 * \sa              gui_draw_font_init
 */
typedef struct GUI_DRAW_FONT_t {
    GUI_iDim_t X;                           /*!< Top left X position for rectangle */
    GUI_iDim_t Y;                           /*!< Top left Y position for rectangle */
    GUI_iDim_t Width;                       /*!< Rectangle width for string draw */
    GUI_iDim_t Height;                      /*!< Rectangle height for string draw */
    GUI_Byte Align;                         /*!< Alignment parameters */
    GUI_Dim_t LineHeight;                   /*!< Line height, only when multiline mode is enabled */
    GUI_Byte Flags;                         /*!< Flags for future use */
    GUI_iDim_t Color1Width;                 /*!< Width for color 1 */
    GUI_Color_t Color1;                     /*!< Color 1 */
    GUI_Color_t Color2;                     /*!< Color 2 */
    uint32_t ScrollY;                       /*!< Scroll in vertical direction */
} GUI_DRAW_FONT_t;

/**
 * \brief           Scroll bar direction enumeration
 */
typedef enum GUI_DRAW_SB_DIR_t {
    GUI_DRAW_SB_DIR_VERTICAL = 0x00,        /*!< Vertical slider */
    GUI_DRAW_SB_DIR_HORIZONTAL = 0x00,      /*!< Horizontal slider */
} GUI_DRAW_SB_DIR_t;

/**
 * \brief           Scroll bar structure
 */
typedef struct GUI_DRAW_SB_t {
    GUI_iDim_t X;                           /*!< X position on screen */
    GUI_iDim_t Y;                           /*!< Y position on screen */
    GUI_iDim_t Width;                       /*!< Total width */
    GUI_iDim_t Height;                      /*!< Total height */
    GUI_DRAW_SB_DIR_t Dir;                  /*!< Scroll bar direction (horizontal or vertical */
    uint32_t EntriesTotal;                  /*!< Total number of entries */
    uint32_t EntriesVisible;                /*!< Number of entries visible at a time */
    uint32_t EntriesTop;                    /*!< Top visible entry number */
} GUI_DRAW_SB_t;

/**
 * \brief           3D states enumeration
 * \sa              gui_draw_rectangle3d
 */
typedef enum GUI_DRAW_3D_State_t {
    GUI_DRAW_3D_State_Raised = 0x00,        /*!< Raised 3D style */
    GUI_DRAW_3D_State_Lowered = 0x01        /*!< Lowered 3D style */
} GUI_DRAW_3D_State_t;

/**
 * \brief           Poly line object coordinates
 * \sa              gui_draw_poly
 */
typedef struct GUI_DRAW_Poly_t {
    GUI_iDim_t X;                           /*!< Poly point X location */
    GUI_iDim_t Y;                           /*!< Poly point Y location */
} GUI_DRAW_Poly_t;

void gui_draw_font_init(GUI_DRAW_FONT_t* f);
void gui_draw_fillscreen(const GUI_Display_t* disp, GUI_Color_t color);
void gui_draw_setpixel(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_Color_t color);
GUI_Color_t gui_draw_getpixel(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y);
void gui_draw_vline(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t length, GUI_Color_t color);
void gui_draw_hline(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t length, GUI_Color_t color);
void gui_draw_line(const GUI_Display_t* disp, GUI_iDim_t x1, GUI_iDim_t y1, GUI_iDim_t x2, GUI_iDim_t y2, GUI_Color_t color);
void gui_draw_rectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_Color_t color);
void gui_draw_filledrectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_Color_t color);
void gui_draw_roundedrectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_iDim_t r, GUI_Color_t color);
void gui_draw_filledroundedrectangle(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_iDim_t r, GUI_Color_t color);
void gui_draw_circle(const GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Color_t color);
void gui_draw_filledcircle(const GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Color_t color);
void gui_draw_circlecorner(const GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Byte_t c, GUI_Color_t color);
void gui_draw_filledcirclecorner(const GUI_Display_t* disp, GUI_iDim_t x0, GUI_iDim_t y0, GUI_iDim_t r, GUI_Byte_t c, uint32_t color);
void gui_draw_triangle(const GUI_Display_t* disp, GUI_iDim_t x1, GUI_iDim_t y1,  GUI_iDim_t x2, GUI_iDim_t y2, GUI_iDim_t x3, GUI_iDim_t y3, GUI_Color_t color);
void gui_draw_filledtriangle(const GUI_Display_t* disp, GUI_iDim_t x1, GUI_iDim_t y1, GUI_iDim_t x2, GUI_iDim_t y2, GUI_iDim_t x3, GUI_iDim_t y3, GUI_Color_t color);
void gui_draw_image(GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, const GUI_IMAGE_DESC_t* img);
void gui_draw_writetext(const GUI_Display_t* disp, const GUI_FONT_t* font, const GUI_Char* str, GUI_DRAW_FONT_t* draw);
void gui_draw_rectangle3d(const GUI_Display_t* disp, GUI_iDim_t x, GUI_iDim_t y, GUI_iDim_t width, GUI_iDim_t height, GUI_DRAW_3D_State_t state);
void gui_draw_poly(const GUI_Display_t* disp, const GUI_DRAW_Poly_t* points, size_t len, GUI_Color_t color);
void gui_draw_scrollbar_init(GUI_DRAW_SB_t* sb);
void gui_draw_scrollbar(const GUI_Display_t* disp, GUI_DRAW_SB_t* sb);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

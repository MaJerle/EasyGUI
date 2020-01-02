/**	
 * \file            gui_draw.h
 * \brief           Drawing operations
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef GUI_HDR_DRAW_H
#define GUI_HDR_DRAW_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup        GUI_DRAW Drawing process
 * \brief           LCD Drawing operations
 * \{
 */

#include "gui/gui.h"

/**
 * \defgroup        GUI_COLORS Colors
 * \brief           List of default GUI colors for fast development
 * \{
 *
 * There are only predefined GUI colors which are most used.
 * For other colors, user can simply use any other RGB combination for any colors of 16M possible values
 *
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

/**
 * \brief           Structure for drawing strings on widgets
 * \sa              gui_draw_text_init
 */
typedef struct {
    gui_dim_t x;                            /*!< Top left X position for rectangle */
    gui_dim_t y;                            /*!< Top left Y position for rectangle */
    gui_dim_t width;                        /*!< Rectangle width for string draw */
    gui_dim_t height;                       /*!< Rectangle height for string draw */
    uint8_t align;                          /*!< Text alignment. Check \ref GUI_FLAG_ALIGN */
    gui_dim_t lineheight;                   /*!< Line height, only when multiline mode is enabled */
    uint8_t flags;                          /*!< Flags for future use */
    gui_dim_t color1width;                  /*!< Width for color 1 */
    gui_color_t color1;                     /*!< Color 1 */
    gui_color_t color2;                     /*!< Color 2 */
    uint32_t scrolly;                       /*!< Scroll in vertical direction */
} gui_draw_text_t;

#define GUI_FLAG_DRAW_GRAD_VER              0x01
#define GUI_FLAG_DRAW_GRAD_HOR              0x02
#define GUI_FLAG_DRAW_FILLED                0x04

/**
 * \brief           Extended rectangle structure
 */
typedef struct {
    gui_dim_t x;                            /*!< Rectangle top-left X position */
    gui_dim_t y;                            /*!< Rectangle top-left Y position */
    gui_dim_t width;                        /*!< Rectangle width */
    gui_dim_t height;                       /*!< Rectangle height */
    uint8_t flags;
    gui_color_t color;                      /*!< Solid color or start color when gradient is used */
    gui_color_t color_end;                  /*!< End color in case of gradient */
    gui_dim_t radius;                       /*!< Radius in units of pixels */
} gui_draw_rect_ex_t;

/**
 * \brief           Scroll bar direction enumeration
 */
typedef enum {
    GUI_DRAW_SB_DIR_VERTICAL = 0x00,        /*!< Vertical slider */
    GUI_DRAW_SB_DIR_HORIZONTAL = 0x00,      /*!< Horizontal slider */
} gui_draw_sb_dir_t;

/**
 * \brief           Scroll bar structure
 */
typedef struct {
    gui_dim_t x;                            /*!< X position on screen */
    gui_dim_t y;                            /*!< Y position on screen */
    gui_dim_t width;                        /*!< Total width */
    gui_dim_t height;                       /*!< Total height */
    gui_draw_sb_dir_t dir;                  /*!< Scroll bar direction (horizontal or vertical */
    uint32_t entriestotal;                  /*!< Total number of entries */
    uint32_t entriesvisible;                /*!< Number of entries visible at a time */
    uint32_t entriestop;                    /*!< Top visible entry number */
} gui_draw_sb_t;

/**
 * \brief           3D states enumeration
 * \sa              gui_draw_rectangle3d
 */
typedef enum {
    GUI_DRAW_3D_State_Raised = 0x00,        /*!< Raised 3D style */
    GUI_DRAW_3D_State_Lowered = 0x01        /*!< Lowered 3D style */
} gui_draw_3d_state_t;

/**
 * \brief           Poly line object coordinates
 * \sa              gui_draw_poly
 */
typedef struct {
    gui_dim_t x;                           /*!< Poly point X location */
    gui_dim_t y;                           /*!< Poly point Y location */
} gui_draw_poly_t;

void        gui_draw_text_init(gui_draw_text_t* f);
void        gui_draw_fillscreen(const gui_display_t* disp, gui_color_t color);
void        gui_draw_setpixel(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_color_t color);
gui_color_t gui_draw_getpixel(const gui_display_t* disp, gui_dim_t x, gui_dim_t y);
void        gui_draw_vline(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t length, gui_color_t color);
void        gui_draw_hline(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t length, gui_color_t color);
void        gui_draw_line(const gui_display_t* disp, gui_dim_t x1, gui_dim_t y1, gui_dim_t x2, gui_dim_t y2, gui_color_t color);
void        gui_draw_rectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_color_t color);
void        gui_draw_rectangle_ex(const gui_display_t* disp, gui_draw_rect_ex_t* rect);
void        gui_draw_filledrectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_color_t color);
void        gui_draw_roundedrectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_dim_t r, gui_color_t color);
void        gui_draw_filledroundedrectangle(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_dim_t r, gui_color_t color);
void        gui_draw_circle(const gui_display_t* disp, gui_dim_t x0, gui_dim_t y0, gui_dim_t r, gui_color_t color);
void        gui_draw_filledcircle(const gui_display_t* disp, gui_dim_t x0, gui_dim_t y0, gui_dim_t r, gui_color_t color);
void        gui_draw_circlecorner(const gui_display_t* disp, gui_dim_t x0, gui_dim_t y0, gui_dim_t r, uint8_t c, gui_color_t color);
void        gui_draw_filledcirclecorner(const gui_display_t* disp, gui_dim_t x0, gui_dim_t y0, gui_dim_t r, uint8_t c, uint32_t color);
void        gui_draw_triangle(const gui_display_t* disp, gui_dim_t x1, gui_dim_t y1,  gui_dim_t x2, gui_dim_t y2, gui_dim_t x3, gui_dim_t y3, gui_color_t color);
void        gui_draw_filledtriangle(const gui_display_t* disp, gui_dim_t x1, gui_dim_t y1, gui_dim_t x2, gui_dim_t y2, gui_dim_t x3, gui_dim_t y3, gui_color_t color);
void        gui_draw_image(gui_display_t* disp, gui_dim_t x, gui_dim_t y, const gui_image_desc_t* img);
void        gui_draw_writetext(const gui_display_t* disp, const gui_font_t* font, const gui_char* str, gui_draw_text_t* draw);
void        gui_draw_rectangle3d(const gui_display_t* disp, gui_dim_t x, gui_dim_t y, gui_dim_t width, gui_dim_t height, gui_draw_3d_state_t state);
void        gui_draw_poly(const gui_display_t* disp, const gui_draw_poly_t* points, size_t len, gui_color_t color);
void        gui_draw_scrollbar_init(gui_draw_sb_t* sb);
void        gui_draw_scrollbar(const gui_display_t* disp, gui_draw_sb_t* sb);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_DRAW_H */

/**	
 * \file            gui_defs.h
 * \brief           Core definitions
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
#ifndef GUI_HDR_DEFS_H
#define GUI_HDR_DEFS_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
    
/* List of includes */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "gui_config.h"
#include "pt/pt.h"

/**
 * \addtogroup      GUI
 * \{
 */

/**
 * \addtogroup      GUI_FLAGS Flags
 * \brief           List of all flags in GUI library
 * \{
 */

/**
 * \anchor          GUI_FLAGS_WIDGET
 * \name            Widget flags
 * \brief           Widget flags
 * \{
 */

#define GUI_FLAG_REDRAW                     ((uint32_t)0x00000001)  /*!< Indicates widget should be redrawn */
#define GUI_FLAG_CHILD                      ((uint32_t)0x00000002)  /*!< Indicates widget is child (window) */
#define GUI_FLAG_DYNAMICTEXTALLOC           ((uint32_t)0x00000004)  /*!< Indicates memory for text has been dynamically allocated */
#define GUI_FLAG_ACTIVE                     ((uint32_t)0x00000008)  /*!< Indicates widget is active by mouser or touch */
#define GUI_FLAG_FOCUS                      ((uint32_t)0x00000010)  /*!< Indicates widget is currently in focus */
#define GUI_FLAG_HIDDEN                     ((uint32_t)0x00000020)  /*!< Indicates widget is hidden */
#define GUI_FLAG_DISABLED                   ((uint32_t)0x00000040)  /*!< Indicates widget is disabled */
#define GUI_FLAG_3D                         ((uint32_t)0x00000080)  /*!< Indicates widget has enabled 3D style */
#define GUI_FLAG_WIDTH_PERCENT              ((uint32_t)0x00000100)  /*!< Indicates widget width is in units of percentage according to parent widget width */
#define GUI_FLAG_HEIGHT_PERCENT             ((uint32_t)0x00000200)  /*!< Indicates widget height is in units of percentage according to parent widget height */
#define GUI_FLAG_XPOS_PERCENT               ((uint32_t)0x00000400)  /*!< Indicates widget X position is in percent relative to parent width */
#define GUI_FLAG_YPOS_PERCENT               ((uint32_t)0x00000800)  /*!< Indicates widget Y position is in percent relative to parent height */
#define GUI_FLAG_EXPANDED                   ((uint32_t)0x00001000)  /*!< Indicates children widget is set to (temporary) XY = 0,0 and width/height = parent width / parent height (maximize windows function) */
#define GUI_FLAG_REMOVE                     ((uint32_t)0x00002000)  /*!< Indicates widget should be deleted */
#define GUI_FLAG_IGNORE_INVALIDATE          ((uint32_t)0x00004000)  /*!< Indicates widget invalidation is ignored completely when invalidating it directly */
#define GUI_FLAG_FIRST_INVALIDATE           ((uint32_t)0x00008000)  /*!< Indicates widget is invalidated for "first" time, thus ignore check if parent is hidden or not */
#define GUI_FLAG_TOUCH_MOVE                 ((uint32_t)0x00010000)  /*!< Indicates widget callback has processed touch move event. This parameter works in conjunction with \ref GUI_FLAG_ACTIVE flag */

/**
 * \}
 */

/**
 * \anchor          GUI_FLAGS_WIDGET_CORE
 * \name            Widget base flags
 * \brief           Widget description flags
 * \{
 */

#define GUI_FLAG_WIDGET_ALLOW_CHILDREN      ((uint32_t)0x00040000)  /*!< Widget allows children widgets */
#define GUI_FLAG_WIDGET_DIALOG_BASE         ((uint32_t)0x00080000)  /*!< Widget is dialog base. When it is active, no other widget around dialog can be pressed */
#define GUI_FLAG_WIDGET_INVALIDATE_PARENT   ((uint32_t)0x00100000)  /*!< Anytime widget is invalidated, parent should be invalidated too */

/**
 * \}
 */

#define GUI_FLAG_LCD_WAIT_LAYER_CONFIRM     ((uint32_t)0x00000001)  /*!< Indicates waiting for layer change confirmation */

/**
 * \anchor          GUI_FLAG_ALIGN
 * \name            Alignment flags
 * \brief           Alignment flags used for drawing
 * \{
 */

#define GUI_HALIGN_MASK                 0x03
#define GUI_HALIGN_LEFT                 0x00/*!< Horizontal align is left */ 
#define GUI_HALIGN_CENTER               0x01/*!< Horizontal align is center */
#define GUI_HALIGN_RIGHT                0x02/*!< Horizontal align is right */

#define GUI_VALIGN_MASK                 (0x03 << 2)
#define GUI_VALIGN_TOP                  (0x00 << 2) /*!< Vertical align is top */
#define GUI_VALIGN_CENTER               (0x01 << 2) /*!< Vertical align is center */
#define GUI_VALIGN_BOTTOM               (0x02 << 2) /*!< Vertical align is bottom */         

/**
 * \}
 */

/**
 * \}
 */

/**
 * \defgroup        GUI_KEYS List of special keys
 * \brief           List of used special keys for keyboard emulation
 * \{
 *
 * Most of them have virtual assigned number, except some which are the same as in ASCII number.
 */

#define GUI_KEY_UP                      ((uint32_t)0x0001)  /*!< Keyboard up button */
#define GUI_KEY_DOWN                    ((uint32_t)0x0002)  /*!< Keyboard down button */
#define GUI_KEY_LEFT                    ((uint32_t)0x0003)  /*!< Keyboard left button */
#define GUI_KEY_RIGHT                   ((uint32_t)0x0004)  /*!< Keyboard right button */
#define GUI_KEY_ESC                     ((uint32_t)0x0005)  /*!< Keyboard ESC (Escape) button */
#define GUI_KEY_PG_UP                   ((uint32_t)0x0006)  /*!< Keyboard page up button */
#define GUI_KEY_PG_DOWN                 ((uint32_t)0x0007)  /*!< Keyboard page down button */
#define GUI_KEY_BACKSPACE               ((uint32_t)0x0008)  /*!< Keyboard backspace button = ASCII 0x08 */
#define GUI_KEY_TAB                     ((uint32_t)0x0009)  /*!< Keyboard TAB button */
#define GUI_KEY_LF                      ((uint32_t)'\n')    /*!< Keyboard line feed = ASCII 0x0A */
#define GUI_KEY_CR                      ((uint32_t)'\r')    /*!< Keyboard carriage return = ASCII 0x0D */
#define GUI_KEY_WS                      ((uint32_t)' ')     /*!< Keyboard white space */
#define GUI_KEY_SHIFT                   ((uint32_t)0x0015)  /*!< Keyboard shift key */

/**
 * \}
 */

/**
 * \defgroup        GUI_Typedefs Structures and enumerations
 * \brief           List of core structures and enumerations
 * \{
 */

/**
 * \brief           Results enumeration
 */
typedef enum guir {
    guiOK = 0x00,                           /*!< GUI status is OK */
    guiERROR = 0x01                         /*!< There was an error in processing */
} guir_t;

/**
 * \brief           Linked list structure
 * \note            When used with widgets, it must be first element in \ref gui_handle structure
 * \sa              gui_linkedlistroot_t
 */
typedef struct gui_linkedlist {
    void* prev;                             /*!< Previous object in linked list */
    void* next;                             /*!< Next object in linked list */
} gui_linkedlist_t;

/**
 * \brief           Structure used for dynamic struct allocation
 * \sa              gui_linkedlistroot_t
 */
typedef struct gui_linkedlistmulti {
    gui_linkedlist_t list;                  /*!< Linked list structure, must be first on structure for casting */
    void* element;                          /*!< Pointer to element in linked list */
} gui_linkedlistmulti_t;

/**
 * \brief           Linked list root structure for start and end widget in structure
 * \sa              gui_linkedlist_t
 * \sa              gui_linkedlistmulti_t
 */
typedef struct gui_linkedlistroot {
    void* first;                            /*!< First element in linked list */
    void* last;                             /*!< Last element in linked list */
} gui_linkedlistroot_t;
 
/**
 * \brief           Core timer structure for GUI timers
 */
typedef struct gui_timer_core {
    uint32_t time;                          /*!< Last processed time */
    gui_linkedlistroot_t list;              /*!< Root linked list object */
} gui_timer_core_t;

typedef uint32_t    gui_id_t;               /*!< GUI object ID */
typedef uint32_t    gui_color_t;            /*!< Color definition */
typedef int16_t     gui_dim_t;              /*!< GUI dimensions in units of pixels */
typedef uint8_t     gui_char;               /*!< GUI char data type for all string operations */
#define _GT(x)      (gui_char *)(x)         /*!< Macro to force strings to right format for processing */
#define gui_const   const                   /*!< Macro for constant keyword */
    
#define GUI_DIM_MIN                         ((gui_dim_t)0x8000)
#define GUI_DIM_MAX                         ((gui_dim_t)0x7FFF)

/**
 * \brief           Get size of statically declared array
 * \param[in]       x: Input array
 * \return          Number of array elements
 * \hideinitializer
 */
#define GUI_ARRAYSIZE(x)                    (sizeof(x) / sizeof((x)[0]))

#define GUI_COUNT_OF(x)                     GUI_ARRAYSIZE(x)
    
/**
 * \brief           Get container object from member
 * 
 * \include         _example_defs_containerof.c
 */
#define             gui_containerof(ptr, type, memb)      (type *)((char *)(ptr) - (char *)offsetof(type, memb))

/**
 * \anchor          GUI_CAST
 * \name            Casting macros
 * \brief           Helpers to cast result to different type
 * \{
 */

#define GUI_U8(x)                           ((uint8_t)(x))      /*!< Result casted to `uint8_t` */
#define GUI_I8(x)                           ((int8_t)(x))       /*!< Result casted to `int8_t` */
#define GUI_U16(x)                          ((uint16_t)(x))     /*!< Result casted to `uint16_t` */
#define GUI_I16(x)                          ((int16_t)(x))      /*!< Result casted to `int16_t` */
#define GUI_U32(x)                          ((uint32_t)(x))     /*!< Result casted to `uint32_t` */
#define GUI_I32(x)                          ((int32_t)(x        /*!< Result casted to `int32_t` */
#define GUI_VP(x)                           ((void *)(x))       /*!< Result casted to `void *` */
#define GUI_FLOAT(x)                        ((float)(x))        /*!< Result casted to `float` */
#define GUI_DIM(x)                          ((gui_dim_t)(x))    /*!< Result casted to `gui_dim_t` */

/**
 * \}
 */

/**
 * \brief           Basic widget structure
 */
struct gui_handle;

/**
 * \brief           Handle object for GUI widget
 */
typedef struct gui_handle* gui_handle_p;

/**
 * \brief           Global event callback function declaration
 */
typedef void (*gui_eventcallback_t)(void);

/**
 * \brief           Color gradient definition
 */
typedef struct {
    gui_color_t start;                      /*!< Gradient start color */
    gui_color_t stop;                       /*!< Gradient end color */
} gui_gradient_t;

/**
 * \brief           Touch state on widget
 */
typedef enum {
    GUI_TOUCH_STATE_RELEASED = 0x00,        /*!< Touch released */
    GUI_TOUCH_STATE_PRESSED = 0x01,         /*!< Touch detected */
} gui_touch_state_t;

/**
 * \brief           Touch internal processing enumeration
 */
typedef enum {
    touchHANDLED = 0x00,                    /*!< Touch has been handled */
    touchHANDLEDNOFOCUS,                    /*!< Touch has been handled but widget is not in focus state (used for widget who don't have touch events) */
    touchCONTINUE                           /*!< Touch has not been handled and further checking can be done */
} guii_touch_status_t;

/**
 * \brief           Single touch data structure
 */
typedef struct {
    uint8_t count;                          /*!< Number of touches detected */
    gui_dim_t x[GUI_CFG_TOUCH_MAX_PRESSES]; /*!< Touch X coordinate */
    gui_dim_t y[GUI_CFG_TOUCH_MAX_PRESSES]; /*!< Touch Y coordinate */
    gui_touch_state_t status;               /*!< Touch status, pressed or released */
    uint32_t time;                          /*!< Time when touch was recorded */
} gui_touch_data_t;

/**
 * \brief           Internal touch structure used for widget callbacks
 */
typedef struct {
    gui_touch_data_t ts;                    /*!< Touch data structure, used to read from buffer */

    gui_dim_t x_old[GUI_CFG_TOUCH_MAX_PRESSES]; /*!< Old absolute `X` position */
    gui_dim_t y_old[GUI_CFG_TOUCH_MAX_PRESSES]; /*!< Old absolute `Y` position */
    gui_dim_t x_rel[GUI_CFG_TOUCH_MAX_PRESSES]; /*!< Relative `X` position to current widget */
    gui_dim_t y_rel[GUI_CFG_TOUCH_MAX_PRESSES]; /*!< Relative `Y` position to current widget */
    gui_dim_t x_diff[GUI_CFG_TOUCH_MAX_PRESSES];/*!< `X` difference between old and new touch position */
    gui_dim_t y_diff[GUI_CFG_TOUCH_MAX_PRESSES];/*!< `Y` difference between old and new touch position */

    gui_dim_t widget_x;                     /*!< Widget absolute `X` position */
    gui_dim_t widget_y;                     /*!< Widget absolute `Y` position */
    gui_dim_t widget_width;                 /*!< Widget width */
    gui_dim_t widget_height;                /*!< Widget height */
#if GUI_CFG_TOUCH_MAX_PRESSES > 1 || __DOXYGEN__
    float distance_old;                     /*!< Old distance between `2` points */
    float distance;                         /*!< Distance between `2` points when `2` touch elements are detected */
#endif /* GUI_CFG_TOUCH_MAX_PRESSES > 1 || __DOXYGEN__ */
    struct pt pt;                           /*!< Protothread structure */
} guii_touch_data_t;

/**
 * \brief           Single key data structure
 */
typedef struct {
#if GUI_CFG_USE_UNICODE || __DOXYGEN__
    gui_char keys[4];                       /*!< Key pressed, plain unicode bytes, up to 4 bytes */
#else
    gui_char keys[1];                       /*!< Key pressed, no unicode support */
#endif
    uint8_t flags;                          /*!< flags for special keys */
    uint32_t time;                          /*!< Event time */
} gui_keyboard_data_t;

/**
 * \brief           Internal single key data structure
 * \note            Used for private purpose between input and widget
 */
typedef struct {
    gui_keyboard_data_t kb;                 /*!< Keyboard structure */
} guii_keyboard_data_t;

/**
 * \brief           Keyboard internal processing enumeration
 */
typedef enum {
    keyHANDLED = 0x00,                      /*!< Key has been handled */
    keyCONTINUE                             /*!< Key has not been handled and further checking can be done */
} guii_keyboard_status_t;

/**
 * \brief           GUI clipping management
 */
typedef struct {
    gui_dim_t x1;                           /*!< Clipping area start X */
    gui_dim_t y1;                           /*!< Clipping area start Y */
    gui_dim_t x2;                           /*!< Clipping area end X */
    gui_dim_t y2;                           /*!< Clipping area end Y */
} gui_display_t;

/**
 * \brief           LCD layer structure
 */
typedef struct {
    uint8_t num;                            /*!< Layer number */
    void* start_address;                    /*!< Start address in memory if it exists */
    volatile uint8_t pending;               /*!< Layer pending for redrawing operation */
    gui_display_t display;                  /*!< Display setup for clipping regions for main layers (no virtual) */
    
    gui_dim_t width;                        /*!< Layer width, used for virtual layers mainly */
    gui_dim_t height;                       /*!< Layer height, used for virtual layers mainly */
    gui_dim_t x_pos;                        /*!< Absolute X position on screen, used for virtual layers */
    gui_dim_t y_pos;                        /*!< Absolute Y position on screen, used for virtual layers */
} gui_layer_t;

/**
 * \brief           GUI LCD structure
 */
typedef struct {
    gui_dim_t width;                        /*!< LCD width in units of pixels */
    gui_dim_t height;                       /*!< LCD height in units of pixels */
    uint8_t pixel_size;                     /*!< Number of bytes per pixel */
    gui_layer_t* active_layer;              /*!< Active layer number currently shown to LCD */
    gui_layer_t* drawing_layer;             /*!< Currently active drawing layer */
    size_t layer_count;                     /*!< Number of layers used for LCD and drawings */
    gui_layer_t* layers;                    /*!< Pointer to layers */
    uint32_t flags;                         /*!< List of flags */
} gui_lcd_t;

/**
 * \ingroup         GUI_IMAGE
 * \brief           Image descriptor structure
 */
typedef struct {
    gui_dim_t x_size;                       /*!< Image X size */
    gui_dim_t y_size;                       /*!< Image Y size */
    uint8_t bpp;                            /*!< Bits per pixel */
    const uint8_t* image;                   /*!< Pointer to image byte array */
} gui_image_desc_t;

/**
 * \brief           Low-level LCD command enumeration
 */
typedef enum {
    /**
     * \brief       Initialize low-level part of GUI
     *
     * \param[in]   *param: Pointer to \ref gui_ll_t structure to fill data to
     * \param[out]  *result: Pointer to `uint8_t` variable to save result: 0 = OK otherwise ERROR
     */
    GUI_LL_Command_Init = 0x00,             /*!< Set new layer as active layer */
    
    /**
     * \brief       Initialize low-level part of GUI
     *
     * \param[in]   *param: Pointer to \ref gui_ll_t structure to fill data to
     * \param[out]  *result: Pointer to `uint8_t` variable to save result: 0 = OK otherwise ERROR
     */
    GUI_LL_Command_SetActiveLayer,          /*!< Set new layer as active layer */
} GUI_LL_Command_t;

/**
 * \brief           GUI Low-Level structure for drawing operations
 */
typedef struct gui_ll_t {
    void            (*Init)         (gui_lcd_t *);                                                                      /*!< Pointer to LCD initialization function */
    uint8_t         (*IsReady)      (gui_lcd_t *);                                                                      /*!< Pointer to LCD is ready function */
    void            (*SetPixel)     (gui_lcd_t *, gui_layer_t *, gui_dim_t, gui_dim_t, gui_color_t);                    /*!< Pointer to LCD set pixel function */
    gui_color_t     (*GetPixel)     (gui_lcd_t *, gui_layer_t *, gui_dim_t, gui_dim_t);                                 /*!< Pointer to read pixel from LCD */
    void            (*Fill)         (gui_lcd_t *, gui_layer_t *, void *, gui_dim_t, gui_dim_t, gui_dim_t, gui_color_t); /*!< Pointer to LCD fill screen or rectangle function */
    void            (*Copy)         (gui_lcd_t *, gui_layer_t *, void *, const void *, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t);   /*!< Pointer to LCD copy data from source to destination */
    void            (*CopyBlend)    (gui_lcd_t *, gui_layer_t *, void *, const void *, uint8_t, uint8_t, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t);   /*!< Pointer to function to copy layers together (blending) with support to set overall layer transparency */
    void            (*DrawHLine)    (gui_lcd_t *, gui_layer_t *, gui_dim_t, gui_dim_t, gui_dim_t, gui_color_t);         /*!< Pointer to horizontal line drawing. Set to 0 if you do not have optimized version */
    void            (*DrawVLine)    (gui_lcd_t *, gui_layer_t *, gui_dim_t, gui_dim_t, gui_dim_t, gui_color_t);         /*!< Pointer to vertical line drawing. Set to 0 if you do not have optimized version */
    void            (*FillRect)     (gui_lcd_t *, gui_layer_t *, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t, gui_color_t);  /*!< Pointer to function for filling rectangle on LCD */
    void            (*DrawImage16)  (gui_lcd_t *, gui_layer_t *, const gui_image_desc_t *, void *, const void *, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t);   /*!< Pointer to function for drawing 16BPP (RGB565) images */
    void            (*DrawImage24)  (gui_lcd_t *, gui_layer_t *, const gui_image_desc_t *, void *, const void *, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t);   /*!< Pointer to function for drawing 24BPP (RGB888) images */
    void            (*DrawImage32)  (gui_lcd_t *, gui_layer_t *, const gui_image_desc_t *, void *, const void *, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t);   /*!< Pointer to function for drawing 32BPP (ARGB8888) images */
    void            (*CopyChar)     (gui_lcd_t *, gui_layer_t *, void *, const void *, gui_dim_t, gui_dim_t, gui_dim_t, gui_dim_t, gui_color_t);                /*!< Pointer to copy char function with alpha only as source */
} gui_ll_t;

/**
 * \defgroup        GUI_FONT Fonts
 * \brief           Font description structures and flags
 * \{
 */
 
/**
 * \brief           GUI FONT character information
 */
typedef struct {
    uint8_t x_size;                         /*!< Character x size in units of pixels */
    uint8_t y_size;                         /*!< Character y size in units of pixels */
    uint8_t x_pos;                          /*!< Character relative x offset in units of pixels */
    uint8_t y_pos;                          /*!< Character relative y offset in units of pixels */
    uint8_t x_margin;                       /*!< Right margin after character in units of pixels. Should be set to 0 if unknown */
    const uint8_t* data;                    /*!< Pointer to actual data for font */
} gui_font_char_t;

/**
 * \brief           FONT structure for writing usage
 */
typedef struct {
    const gui_char* name;                   /*!< Pointer to font name */
    uint8_t size;                           /*!< Font size in units of pixels */
    uint16_t startchar;                     /*!< Start character number in list */
    uint16_t endchar;                       /*!< End character number in list */
    uint8_t flags;                          /*!< List of flags for font */
    const gui_font_char_t* data;            /*!< Pointer to first character */
} gui_font_t;

#define GUI_FLAG_FONT_AA                ((uint8_t)0x01) /*!< Indicates anti-alliasing on font */
#define GUI_FLAG_TEXT_RIGHTALIGN        ((uint8_t)0x02) /*!< Indicates right align text if string length is too wide for rectangle */
#define GUI_FLAG_TEXT_MULTILINE         ((uint8_t)0x04) /*!< Indicates multi line support on widget */
#define GUI_FLAG_TEXT_EDITMODE          ((uint8_t)0x08) /*!< Edit mode is enabled on text */

#if defined(GUI_INTERNAL) || __DOXYGEN__
/**
 * \brief           Char temporary entry stored in RAM for faster copy with blending operations
 */
typedef struct {
    gui_linkedlist_t list;                  /*!< Linked list entry. Must always be first on the list */
    const gui_font_char_t* ch;              /*!< Character value */
    const gui_font_t* font;                 /*!< Pointer to font structure */
} gui_font_charentry_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

#if !__DOXYGEN__
#define ________                        0x00
#define _______X                        0x01
#define ______X_                        0x02
#define ______XX                        0x03
#define _____X__                        0x04
#define _____X_X                        0x05
#define _____XX_                        0x06
#define _____XXX                        0x07
#define ____X___                        0x08
#define ____X__X                        0x09
#define ____X_X_                        0x0A
#define ____X_XX                        0x0B
#define ____XX__                        0x0C
#define ____XX_X                        0x0D
#define ____XXX_                        0x0E
#define ____XXXX                        0x0F
#define ___X____                        0x10
#define ___X___X                        0x11
#define ___X__X_                        0x12
#define ___X__XX                        0x13
#define ___X_X__                        0x14
#define ___X_X_X                        0x15
#define ___X_XX_                        0x16
#define ___X_XXX                        0x17
#define ___XX___                        0x18
#define ___XX__X                        0x19
#define ___XX_X_                        0x1A
#define ___XX_XX                        0x1B
#define ___XXX__                        0x1C
#define ___XXX_X                        0x1D
#define ___XXXX_                        0x1E
#define ___XXXXX                        0x1F
#define __X_____                        0x20
#define __X____X                        0x21
#define __X___X_                        0x22
#define __X___XX                        0x23
#define __X__X__                        0x24
#define __X__X_X                        0x25
#define __X__XX_                        0x26
#define __X__XXX                        0x27
#define __X_X___                        0x28
#define __X_X__X                        0x29
#define __X_X_X_                        0x2A
#define __X_X_XX                        0x2B
#define __X_XX__                        0x2C
#define __X_XX_X                        0x2D
#define __X_XXX_                        0x2E
#define __X_XXXX                        0x2F
#define __XX____                        0x30
#define __XX___X                        0x31
#define __XX__X_                        0x32
#define __XX__XX                        0x33
#define __XX_X__                        0x34
#define __XX_X_X                        0x35
#define __XX_XX_                        0x36
#define __XX_XXX                        0x37
#define __XXX___                        0x38
#define __XXX__X                        0x39
#define __XXX_X_                        0x3A
#define __XXX_XX                        0x3B
#define __XXXX__                        0x3C
#define __XXXX_X                        0x3D
#define __XXXXX_                        0x3E
#define __XXXXXX                        0x3F
#define _X______                        0x40
#define _X_____X                        0x41
#define _X____X_                        0x42
#define _X____XX                        0x43
#define _X___X__                        0x44
#define _X___X_X                        0x45
#define _X___XX_                        0x46
#define _X___XXX                        0x47
#define _X__X___                        0x48
#define _X__X__X                        0x49
#define _X__X_X_                        0x4A
#define _X__X_XX                        0x4B
#define _X__XX__                        0x4C
#define _X__XX_X                        0x4D
#define _X__XXX_                        0x4E
#define _X__XXXX                        0x4F
#define _X_X____                        0x50
#define _X_X___X                        0x51
#define _X_X__X_                        0x52
#define _X_X__XX                        0x53
#define _X_X_X__                        0x54
#define _X_X_X_X                        0x55
#define _X_X_XX_                        0x56
#define _X_X_XXX                        0x57
#define _X_XX___                        0x58
#define _X_XX__X                        0x59
#define _X_XX_X_                        0x5A
#define _X_XX_XX                        0x5B
#define _X_XXX__                        0x5C
#define _X_XXX_X                        0x5D
#define _X_XXXX_                        0x5E
#define _X_XXXXX                        0x5F
#define _XX_____                        0x60
#define _XX____X                        0x61
#define _XX___X_                        0x62
#define _XX___XX                        0x63
#define _XX__X__                        0x64
#define _XX__X_X                        0x65
#define _XX__XX_                        0x66
#define _XX__XXX                        0x67
#define _XX_X___                        0x68
#define _XX_X__X                        0x69
#define _XX_X_X_                        0x6A
#define _XX_X_XX                        0x6B
#define _XX_XX__                        0x6C
#define _XX_XX_X                        0x6D
#define _XX_XXX_                        0x6E
#define _XX_XXXX                        0x6F
#define _XXX____                        0x70
#define _XXX___X                        0x71
#define _XXX__X_                        0x72
#define _XXX__XX                        0x73
#define _XXX_X__                        0x74
#define _XXX_X_X                        0x75
#define _XXX_XX_                        0x76
#define _XXX_XXX                        0x77
#define _XXXX___                        0x78
#define _XXXX__X                        0x79
#define _XXXX_X_                        0x7A
#define _XXXX_XX                        0x7B
#define _XXXXX__                        0x7C
#define _XXXXX_X                        0x7D
#define _XXXXXX_                        0x7E
#define _XXXXXXX                        0x7F
#define X_______                        0x80
#define X______X                        0x81
#define X_____X_                        0x82
#define X_____XX                        0x83
#define X____X__                        0x84
#define X____X_X                        0x85
#define X____XX_                        0x86
#define X____XXX                        0x87
#define X___X___                        0x88
#define X___X__X                        0x89
#define X___X_X_                        0x8A
#define X___X_XX                        0x8B
#define X___XX__                        0x8C
#define X___XX_X                        0x8D
#define X___XXX_                        0x8E
#define X___XXXX                        0x8F
#define X__X____                        0x90
#define X__X___X                        0x91
#define X__X__X_                        0x92
#define X__X__XX                        0x93
#define X__X_X__                        0x94
#define X__X_X_X                        0x95
#define X__X_XX_                        0x96
#define X__X_XXX                        0x97
#define X__XX___                        0x98
#define X__XX__X                        0x99
#define X__XX_X_                        0x9A
#define X__XX_XX                        0x9B
#define X__XXX__                        0x9C
#define X__XXX_X                        0x9D
#define X__XXXX_                        0x9E
#define X__XXXXX                        0x9F
#define X_X_____                        0xA0
#define X_X____X                        0xA1
#define X_X___X_                        0xA2
#define X_X___XX                        0xA3
#define X_X__X__                        0xA4
#define X_X__X_X                        0xA5
#define X_X__XX_                        0xA6
#define X_X__XXX                        0xA7
#define X_X_X___                        0xA8
#define X_X_X__X                        0xA9
#define X_X_X_X_                        0xAA
#define X_X_X_XX                        0xAB
#define X_X_XX__                        0xAC
#define X_X_XX_X                        0xAD
#define X_X_XXX_                        0xAE
#define X_X_XXXX                        0xAF
#define X_XX____                        0xB0
#define X_XX___X                        0xB1
#define X_XX__X_                        0xB2
#define X_XX__XX                        0xB3
#define X_XX_X__                        0xB4
#define X_XX_X_X                        0xB5
#define X_XX_XX_                        0xB6
#define X_XX_XXX                        0xB7
#define X_XXX___                        0xB8
#define X_XXX__X                        0xB9
#define X_XXX_X_                        0xBA
#define X_XXX_XX                        0xBB
#define X_XXXX__                        0xBC
#define X_XXXX_X                        0xBD
#define X_XXXXX_                        0xBE
#define X_XXXXXX                        0xBF
#define XX______                        0xC0
#define XX_____X                        0xC1
#define XX____X_                        0xC2
#define XX____XX                        0xC3
#define XX___X__                        0xC4
#define XX___X_X                        0xC5
#define XX___XX_                        0xC6
#define XX___XXX                        0xC7
#define XX__X___                        0xC8
#define XX__X__X                        0xC9
#define XX__X_X_                        0xCA
#define XX__X_XX                        0xCB
#define XX__XX__                        0xCC
#define XX__XX_X                        0xCD
#define XX__XXX_                        0xCE
#define XX__XXXX                        0xCF
#define XX_X____                        0xD0
#define XX_X___X                        0xD1
#define XX_X__X_                        0xD2
#define XX_X__XX                        0xD3
#define XX_X_X__                        0xD4
#define XX_X_X_X                        0xD5
#define XX_X_XX_                        0xD6
#define XX_X_XXX                        0xD7
#define XX_XX___                        0xD8
#define XX_XX__X                        0xD9
#define XX_XX_X_                        0xDA
#define XX_XX_XX                        0xDB
#define XX_XXX__                        0xDC
#define XX_XXX_X                        0xDD
#define XX_XXXX_                        0xDE
#define XX_XXXXX                        0xDF
#define XXX_____                        0xE0
#define XXX____X                        0xE1
#define XXX___X_                        0xE2
#define XXX___XX                        0xE3
#define XXX__X__                        0xE4
#define XXX__X_X                        0xE5
#define XXX__XX_                        0xE6
#define XXX__XXX                        0xE7
#define XXX_X___                        0xE8
#define XXX_X__X                        0xE9
#define XXX_X_X_                        0xEA
#define XXX_X_XX                        0xEB
#define XXX_XX__                        0xEC
#define XXX_XX_X                        0xED
#define XXX_XXX_                        0xEE
#define XXX_XXXX                        0xEF
#define XXXX____                        0xF0
#define XXXX___X                        0xF1
#define XXXX__X_                        0xF2
#define XXXX__XX                        0xF3
#define XXXX_X__                        0xF4
#define XXXX_X_X                        0xF5
#define XXXX_XX_                        0xF6
#define XXXX_XXX                        0xF7
#define XXXXX___                        0xF8
#define XXXXX__X                        0xF9
#define XXXXX_X_                        0xFA
#define XXXXX_XX                        0xFB
#define XXXXXX__                        0xFC
#define XXXXXX_X                        0xFD
#define XXXXXXX_                        0xFE
#define XXXXXXXX                        0xFF
#endif /* !__DOXYGEN__ */

/**
 * \}
 */
 
/**
 * \}
 */
 
/**
 * \ingroup         GUI_TIMER
 * \brief           Timer structure
 */
typedef struct gui_timer {
    gui_linkedlist_t list;                  /*!< Linked list entry, must be first on the list */
    uint16_t period;                        /*!< Timer period value */
    uint16_t counter;                       /*!< Timer current counter */
    uint8_t flags;                          /*!< Timer flags */
    void* params;                           /*!< Custom parameters passed to callback function */
    void (*callback)(struct gui_timer *);   /*!< Timer callback function */
} gui_timer_t;

/**
 * \ingroup         GUI_TIMER
 * \brief           Pointer to \ref gui_timer_t
 */
typedef gui_timer_t* gui_timer_p;

/**
 * \addtogroup      GUI_WIDGETS_CORE
 * \{
 */

/**
 * \brief           Control parameters for widget
 * \note            Must always start with number 1
 */
typedef enum {
    GUI_EVT_NONE = 0x00,
#if defined(GUI_INTERNAL) || __DOXYGEN__
    /**
     * \brief       Called just after widget has been created. Used for internal widget purpose only
     *
     * \note        This field is only visible inside GUI library and can't be used in custom widget callback
     *
     * \param[in]   param: None
     * \param[out]  result: Pointer to `uint8_t` variable with result. If set to 0 by user, widget will be deleted
     */
    GUI_EVT_PRE_INIT = 0x01,
    
    /**
     * \brief       Check if widget should not be added to linked list after creation
     *
     * \param[in]   param: None
     * \param[out]  result: Pointer to `uint8_t` variable type to store result to `1 = exclude, 0 = do not exclude`
     */
    GUI_EVT_EXCLUDELINKEDLIST = 0x02,
    
    /**
     * \brief       Set user parameter for widget
     *
     * \param[in]   param: Pointer to \ref gui_evt_param_t structure with custom user parameter for widget
     * \param[out]  result: None
     */
    GUI_EVT_SETPARAM = 0x03,
    
    /**
     * \brief       Get parameter for widget
     *
     * \param[in]   param: Pointer to \ref gui_evt_param_t structure with identification and memory where to save parameter value
     * \param[out]  result: None
     */
    GUI_EVT_GETPARAM = 0x04,
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */
    
    /**
     * \brief       Widget has been created and ready to init for future setup
     *
     * \note        Default values for widget can be set here.
     *              Example: Checkbox check can be made and default checkbox value should be set if required
     *
     * \param[in]   param: None
     * \param[out]  result: None
     */
    GUI_EVT_INIT = 0x05,
    
    /**
     * \brief       A new child widget has been added to parent's linked list. It is called on parent widget
     *
     * \param[in]   param: Child widget handle
     * \param[out]  result: None
     */
    GUI_EVT_CHILDWIDGETCREATED,
    
    /**
     * \brief       Draw widget on screen
     *
     * \param[in]   param: Pointer to \ref gui_display_t structure
     * \param[out]  result: None
     */
    GUI_EVT_DRAW,
    
    /**
     * \brief       Draw widget on screen after its children widgets were drawn
     *
     * \param[in]   param: Pointer to \ref gui_display_t structure
     * \param[out]  result: None
     */
    GUI_EVT_DRAWAFTER,
    
    /**
     * \brief       Check if widget can be removed. User can perform check if for example widget needs save or similar operation
     *
     * \param[in]   param: None
     * \param[out]  result: Pointer to `uint8_t` variable type to store result to `1 = remove, 0 = do not remove`
     * \sa          GUI_EVT_REMOVE
     */
    GUI_EVT_CANREMOVE,
    
    /**
     * \brief       Notification before widget delete will actually happen.
     *              In case of specific user allocations in memory, they can be restored here
     * 
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_CANREMOVE
     */
    GUI_EVT_REMOVE,
    
    /**
     * \brief       Notification called when widget becomes focused
     *
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_FOCUSOUT
     */
    GUI_EVT_FOCUSIN,
    
    /**
     * \brief       Notification called when widget clears widget state
     *
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_FOCUSIN
     */
    GUI_EVT_FOCUSOUT,
    
    /**
     * \brief       Notification for active status on widget
     *
     * \note        Called when widget just become active
     *
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_ACTIVEOUT
     */
    GUI_EVT_ACTIVEIN,
    
    /**
     * \brief       Notification for cleared active status on widget
     *
     * \note        Called when widget just clears active
     *
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_ACTIVEIN
     */
    GUI_EVT_ACTIVEOUT,
    
    /**
     * \brief       Notification when touch down event occurs on widget
     *
     * \param[in]   param: Pointer to \ref guii_touch_data_t structure
     * \param[out]  result: Value of \ref guii_touch_status_t enumeration
     * \sa          GUI_EVT_TOUCHMOVE, GUI_EVT_TOUCHEND
     */
    GUI_EVT_TOUCHSTART,
    
    /**
     * \brief       Notification when touch move event occurs on widget
     *
     * \note        If return value from function on this parameter is 1, then \ref GUI_EVT_CLICK event won't be called after released status
     *              to prevent collisions between widget events (sliding on widget and click at the same time can cause unknown behaviour on user)
     *
     * \param[in]   param: Pointer to \ref guii_touch_data_t structure
     * \param[out]  result: Value of \ref guii_touch_status_t enumeration
     * \sa          GUI_EVT_TOUCHSTART, GUI_EVT_TOUCHEND
     */
    GUI_EVT_TOUCHMOVE,
    
    /**
     * \brief       Notification when touch up event occurs on widget
     *
     * \param[in]   param: Pointer to \ref guii_touch_data_t structure
     * \param[out]  result: Value of \ref guii_touch_status_t enumeration
     * \sa          GUI_EVT_TOUCHSTART, GUI_EVT_TOUCHMOVE
     */
    GUI_EVT_TOUCHEND,
    
    /**
     * \brief       Notification when click event has been detected
     *
     * \param[in]   param: Pointer to \ref guii_touch_data_t structure with valid touch press location
     * \param[out]  result: None
     */
    GUI_EVT_CLICK,
    
    /**
     * \brief       Notification when long press has been detected
     *
     * \param[in]   param: Pointer to \ref guii_touch_data_t structure with valid touch press location
     * \param[out]  result: None
     */
    GUI_EVT_LONGCLICK,
    
    /**
     * \brief       Notification when double click has been detected
     *
     * \param[in]   param: Pointer to \ref guii_touch_data_t structure with valid touch press location
     * \param[out]  result: None
     */
    GUI_EVT_DBLCLICK,
    
    /**
     * \brief       Notification when key has been pushed to this widget
     *
     * \param[in]   param: Pointer to \ref guii_keyboard_data_t structure
     * \param[out]  result: Value of \ref guii_keyboard_status_t enumeration
     */
    GUI_EVT_KEYPRESS,
    
    /**
     * \brief       Notification when widget selection has changed
     *
     * \note        Called from widget by user when necessary. Not all widget reports this value by itself
     *
     * \note        Used by widgets where selection can be changed (listbox, selectbox, radio, etc)
     *
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_VALUECHANGED
     */
    GUI_EVT_SELECTIONCHANGED,
    
    /**
     * \brief       Value of widget has been changed
     *
     * \note        Called from widget by user when necessary. Not all widget reports this value by itself
     *
     * \note        Used by widgets where value or status can change (checkbox)
     *
     * \param[in]   param: None
     * \param[out]  result: None
     * \sa          GUI_EVT_SELECTIONCHANGED
     */
    GUI_EVT_VALUECHANGED,
    
    /**     
     * \brief       Widget text value has been changed
     *
     * \note        Called from widget by user when necessary. Not all widget reports this value by itself
     *
     * \param[in]   param: None
     * \param[out]  result: None
     */
    GUI_EVT_TEXTCHANGED,
    
    /**     
     * \brief       Widget should increase/decrease selection
     *
     * \note        Called from \ref gui_widget_incselection function to increase/decrease selection for widgets
     *                 such as listbox or dropdown, where active selection can be changed
     *
     * \param[in]   param: Pointer to `int16_t` variable for amount of increase/decrease value
     * \param[out]  result: Pointer to output `uint8_t` variable to save status of increase/decrease operation
     */
    GUI_EVT_INCSELECTION,
    
    /**     
     * \brief       Called when dialog is dismissed
     *
     * \note        Callback is activated when dismiss function is called on dialog widget.
     *              Notification can be used to proceed with function call
     *
     * \param[in]   param: Pointer to `int` variable passed to dismiss function
     * \param[out]  result: None
     */
    GUI_EVT_ONDISMISS,
} gui_widget_evt_t;

/**
 * \brief           Style information
 */
typedef struct {
    void* arg;
} gui_style_t;

/**
 * \brief           Style handle object
 */
typedef gui_style_t* gui_style_p;

/**
 * \brief           Structure used in setting and getting parameter values from widgets using callbacks
 */
typedef struct {
    uint16_t type;                          /*!< Type of command to set or get */
    void* data;                             /*!< Pointer to actual data to set or get to for specific widget */
} gui_widget_param;

/**
 * \brief           Structure of input parameters for widget callback
 */
typedef struct {
    union {
        int i;                              /*!< Integer value */
        int16_t i16;                        /*!< Signed 16-bit value */
        gui_display_t* disp;                /*!< Pointer to input display data */
        guii_touch_data_t* td;              /*!< Pointer to input touch data */
        guii_keyboard_data_t* kd;           /*!< Pointer to input keyboard data */
        gui_widget_param* wp;               /*!< Widget parameter */
        gui_handle_p h;                     /*!< Widget handle */
    } u;                                    /*!< Union of possible parameters */
} gui_evt_param_t;

/**
 * \brief           Structure of output results for widget callback
 */
typedef struct {
    union {
        uint8_t u8;                         /*!< Unsigned char result */
        guii_touch_status_t ts;             /*!< Touch status result */
        guii_keyboard_status_t ks;          /*!< Keyboard status result */
    } u;                                    /*!< Union of possible results */
} gui_evt_result_t;

/**
 * \brief           Callback function for widget
 * 
 *                  Handles everything related to widget. 
 *                      - Initialization
 *                      - Drawing
 *                      - Touch handling
 *                      - Key presses
 *                      - Value change, selection change
 *                      - and more
 */
typedef uint8_t (*gui_widget_evt_fn) (gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

#define GUI_EVT_PARAMTYPE_DISP(x)               (x)->u.disp
#define GUI_EVT_PARAMTYPE_TOUCH(x)              (x)->u.td
#define GUI_EVT_PARAMTYPE_KEYBOARD(x)           (x)->u.kd
#define GUI_EVT_PARAMTYPE_WIDGETPARAM(x)        (x)->u.wp
#define GUI_EVT_PARAMTYPE_I16(x)                (x)->u.i16
#define GUI_EVT_PARAMTYPE_HANDLE(x)             (x)->u.h
#define GUI_EVT_PARAMTYPE_INT(x)                (x)->u.i

#define GUI_EVT_RESULTTYPE_TOUCH(x)             (x)->u.ts
#define GUI_EVT_RESULTTYPE_KEYBOARD(x)          (x)->u.ks
#define GUI_EVT_RESULTTYPE_U8(x)                (x)->u.u8

/**
 * \brief           Structure for each widget type
 */
typedef struct {
    const gui_char* name;                   /*!< Widget name for display purpose */
    uint16_t size;                          /*!< Bytes required for widget memory allocation */
    uint32_t flags;                         /*!< List of flags for widget setup. This field can use \ref GUI_WIDGETS_CORE_FLAGS flags */
    gui_widget_evt_fn callback;         /*!< Pointer to control function, returns 1 if command handled or 0 if not */
    const gui_color_t* colors;              /*!< Pointer to list of colors as default values for widget */
    uint8_t color_count;                    /*!< Number of colors used in widget */
} gui_widget_t;

#if defined(GUI_INTERNAL) || __DOXYGEN__

/**
 * \brief           Common GUI values for widgets
 */
typedef struct gui_handle {
    gui_linkedlist_t list;                  /*!< Linked list entry, must always be on top for casting */
    gui_linkedlistroot_t root_list;         /*!< Linked list root of children widgets */

    gui_id_t id;                            /*!< Widget ID number */
    uint32_t footprint;                     /*!< Footprint indicates widget is valid */
    const gui_widget_t* widget;             /*!< Widget parameters with callback functions */
    gui_widget_evt_fn callback;             /*!< Callback function prototype */
    struct gui_handle* parent;              /*!< Pointer to parent widget */

    float x;                                /*!< Object X position relative to parent window in units of pixel/percent */
    float y;                                /*!< Object Y position relative to parent window in units of pixel/percent */
    float width;                            /*!< Object width in units of pixel/percent */
    float height;                           /*!< Object height in units of pixel/percent */

#if GUI_CFG_USE_POS_SIZE_CACHE || __DOXYGEN__
    /* Absolute values for position and size, changed each time position/size is modified */
    gui_dim_t abs_x;                        /*!< Absolute X position of top-left corner on screen */
    gui_dim_t abs_y;                        /*!< Absolute Y position of top-left corner on screen */
    gui_dim_t abs_width;                    /*!< Absolute width on screen in units of pixels */
    gui_dim_t abs_height;                   /*!< Absolute height on screen in units of pixels */
    
    /* Visible parts on screen, used on clipping calculations */
    gui_dim_t abs_visible_x1;               /*!< Absolute visible left X position on screen for widget */
    gui_dim_t abs_visible_y1;               /*!< Absolute visible top Y positon on screen for widget */
    gui_dim_t abs_visible_x2;               /*!< Absolute visible right X position on screen for widget */
    gui_dim_t abs_visible_y2;               /*!< Absolute visible bottom Y positon on screen for widget */
#endif /* GUI_CFG_USE_POS_SIZE_CACHE */

    uint32_t padding;                       /*!< 4-bytes long padding, each byte of one side, MSB = top padding, LSB = left padding.
                                                    Used for children widgets if virtual padding should be used */
    int32_t zindex;                         /*!< Z-Index value of widget, which can be set by user.
                                                    All widgets with same z-index are changeable when active on visible area */
#if GUI_CFG_USE_ALPHA || __DOXYGEN__
    uint8_t alpha;                          /*!< Widget alpha relative to parent widget */
#endif /* GUI_CFG_USE_ALPHA */
    uint32_t flags;                         /*!< All possible flags for specific widget */
    gui_const gui_font_t* font;             /*!< Font used for widget drawings */
    gui_char* text;                         /*!< Pointer to widget text if exists */
    size_t textmemsize;                     /*!< Number of bytes for text when dynamically allocated */
    size_t textcursor;                      /*!< Text cursor position */
    gui_timer_t* timer;                     /*!< Software timer pointer */
    gui_color_t* colors;                    /*!< Pointer to allocated color memory when custom colors are used */
    
    /* Scroll feature, available only for widgets with children support */
    gui_dim_t x_scroll;                     /*!< Scroll of widgets in horizontal direction in units of pixels */
    gui_dim_t y_scroll;                     /*!< Scroll of widgets in vertical direction in units of pixels */
    
    void* arg;                              /*!< Pointer to optional user data */
} gui_handle;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

/**
 * \brief           Widget create function footprint for structures as callbacks
 */
typedef gui_handle_p (*gui_widget_createfunc_fn)(gui_id_t, float, float, float, float, gui_handle_p, gui_widget_evt_fn, uint16_t);

/**
 * \}
 */

/**
 * \ingroup         GUI_TRANSLATE
 * \brief           Translation language structure
 */
typedef struct {
    const gui_char* lang;                   /*!< Language code used to identify it later when setting active language */
    const gui_char** entries;               /*!< Pointer to list containing pointers to translated entries */
    size_t count;                           /*!< Number of entries in translated array */
} gui_translate_language_t;

/**
 * \ingroup         GUI_TRANSLATE
 * \brief           Basic translation structure for internal use
 */
typedef struct gui_translate {
    const gui_translate_language_t* source; /*!< Pointer to source language table */
    const gui_translate_language_t* active; /*!< Pointer to current language table */
} gui_translate_t;

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* GUI_HDR_DEFS_H */

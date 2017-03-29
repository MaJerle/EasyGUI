/**
 * \author  Tilen Majerle
 * \brief   GUI structures and enumerations
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
#ifndef GUI_DEFS_H
#define GUI_DEFS_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
    
/* List of includes */
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include "gui_config.h"

/**
 * \addtogroup      GUI
 * \{
 */

/**
 * \addtogroup      GUI_Flags Flags
 * \brief           List of all flags in GUI library
 * \{
 */

#define GUI_FLAG_REDRAW                 ((uint32_t)0x00000001)  /*!< Indicates widget should be redrawn */
#define GUI_FLAG_CHILD                  ((uint32_t)0x00000002)  /*!< Indicates widget is child (window) */
#define GUI_FLAG_DYNAMICTEXTALLOC       ((uint32_t)0x00000004)  /*!< Indicates memory for text has been dynamically allocated */
#define GUI_FLAG_ACTIVE                 ((uint32_t)0x00000008)  /*!< Indicates widget is active by mouser or touch */
#define GUI_FLAG_FOCUS                  ((uint32_t)0x00000010)  /*!< Indicates widget is currently in focus */
#define GUI_FLAG_HIDDEN                 ((uint32_t)0x00000020)  /*!< Indicates widget is hidden */
#define GUI_FLAG_DISABLED               ((uint32_t)0x00000040)  /*!< Indicates widget is disabled */
#define GUI_FLAG_3D                     ((uint32_t)0x00000080)  /*!< Indicates widget has enabled 3D style */
#define GUI_FLAG_WIDTH_PERCENT          ((uint32_t)0x00000100)  /*!< Indicates widget width is in units of percentage according to parent widget width */
#define GUI_FLAG_HEIGHT_PERCENT         ((uint32_t)0x00000200)  /*!< Indicates widget height is in units of percentage according to parent widget height */
#define GUI_FLAG_WIDTH_FILL             ((uint32_t)0x00000400)  /*!< Indicates widget width fills to the end of widget */
#define GUI_FLAG_HEIGHT_FILL            ((uint32_t)0x00000800)  /*!< Indicates widget height fills to the end of widget */

#define GUI_FLAG_LCD_WAIT_LAYER_CONFIRM ((uint32_t)0x00000001)  /*!< Indicates waiting for layer change confirmation */

/**
 * \}
 */

/**
 * \defgroup        GUI_Keys List of special keys
 * \brief           List of used special keys for keyboard emulation
 * \{
 *
 * Most of them have virtual assigned number, except some which are the same as in ASCII number.
 */

#define GUI_KEY_UP                      ((uint32_t)0x0001)  /*!< Keyboard up button */
#define GUI_KEY_DOWN                    ((uint32_t)0x0002)  /*!< Keyboard down button */
#define GUI_KEY_LEFT                    ((uint32_t)0x0003)  /*!< Keyboard left button */
#define GUI_KEY_RIGHT                   ((uint32_t)0x0004)  /*!< Keyboard right button */
#define GUI_KEY_ESC                     ((uint16_t)0x0005)  /*!< Keyboard ESC (Escape) button */
#define GUI_KEY_PG_UP                   ((uint16_t)0x0006)  /*!< Keyboard page up button */
#define GUI_KEY_PG_DOWN                 ((uint16_t)0x0007)  /*!< Keyboard page down button */
#define GUI_KEY_BACKSPACE               ((uint16_t)0x0008)  /*!< Keyboard backspace button = ASCII 0x08 */
#define GUI_KEY_TAB                     ((uint16_t)0x0009)  /*!< Keyboard TAB button */
#define GUI_KEY_LF                      ((uint16_t)0x000A)  /*!< Keyboard line feed = ASCII 0x0A */
#define GUI_KEY_CR                      ((uint16_t)0x000D)  /*!< Keyboard carriage return = ASCII 0x0D */

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
typedef enum GUI_Result_t {
    guiOK = 0x00,                           /*!< GUI status is OK */
    guiERROR = 0x01                         /*!< There was an error in processing */
} GUI_Result_t;

/**
 * \brief           Linked list structure
 * \note            This structure must be first element in \ref GUI_HANDLE_p structure
 * \sa              GUI_LinkedListRoot_t
 */
typedef struct GUI_LinkedList_t {
    void* Prev;                             /*!< Previous object in linked list */
    void* Next;                             /*!< Next object in linked list */
} GUI_LinkedList_t;

/**
 * \brief           Structure used for dynamic struct allocation
 * \sa              GUI_LinkedListRoot_t
 */
typedef struct GUI_LinkedListMulti_t {
    GUI_LinkedList_t List;                  /*!< Linked list structure, must be first on structure for casting */
    void* Element;                          /*!< Pointer to element in linked list */
} GUI_LinkedListMulti_t;

/**
 * \brief           Linked list root structure for start and end widget in structure
 * \sa              GUI_LinkedList_t
 * \sa              GUI_LinkedListMulti_t
 */
typedef struct GUI_LinkedListRoot_t {
    void* First;                            /*!< First element in linked list */
    void* Last;                             /*!< Last element in linked list */
} GUI_LinkedListRoot_t;
 
/**
 * \brief           Core timer structure for GUI timers
 */
typedef struct GUI_TIMER_CORE_t {
    uint32_t Time;                          /*!< Last processed time */
    GUI_LinkedListRoot_t List;              /*!< Root linked list object */
} GUI_TIMER_CORE_t;

typedef uint32_t    GUI_ID_t;               /*!< GUI object ID */
typedef uint32_t    GUI_Color_t;            /*!< Color definition */
typedef uint16_t    GUI_Dim_t;              /*!< GUI dimensions in units of pixels */
typedef int16_t     GUI_iDim_t;             /*!< GUI signed dimensions in units of pixels  */
typedef uint8_t     GUI_Byte;               /*!< GUI byte data type */
typedef GUI_Byte    GUI_Byte_t;             /*!< GUI byte data type */
typedef int8_t      GUI_iByte;              /*!< GUI signed byte data type */
typedef GUI_iByte   GUI_iByte_t;            /*!< GUI signed byte data type */
typedef uint8_t     GUI_Char;               /*!< GUI char data type for all string operations */
#define _T(x)       (GUI_Char *)(x)         /*!< Macro to force strings to right format for processing */
#define GUI_Const   const                   /*!< Macro for constant keyword */
    
/**
 * \brief           Get container object from member
 * 
 * Consider below structure
 * \code{c}
//Declare driver structure
typedef struct {
    int x;
    int y;
} drv_t

//Declare custom structure
//Use in custom software
typedef struct {
    int a;
    int b;
    int c;
    drv_t driver;
} my_structure;

//Create structure object
my_structure my;

//Create pointer to structure object
drv_t* driver_ptr = &my->driver;

//Pass pointer to our working function somewhere
//Let's assume this function is callback called from our software
void myFunc(drv_t* drv) {
    //We know drv pointer is child of my_structure
    //Let's get that my_structure pointer from drv_t structure
    my_structure* my_struct = GUI_ContainerOf(drv, my_structure, driver);
    
    //Check if they match
    if (my_struct == &my) {
        //This should be true
    }
}
\endcode
 */
#define             GUI_ContainerOf(ptr, type, memb)      (type *)((char *)(ptr) - (char *)offsetof(type, memb))

/**
 * \brief           Color gradient definition
 */
typedef struct GUI_Gradient_t {
    GUI_Color_t Start;                      /*!< Gradient start color */
    GUI_Color_t Stop;                       /*!< Gradient end color */
} GUI_Gradient_t;

/**
 * \brief           Touch state on widget
 */
typedef enum GUI_TouchState_t {
    GUI_TouchState_PRESSED = 0x01,          /*!< Touch detected */
    GUI_TouchState_RELEASED = 0x00          /*!< Touch released */
} GUI_TouchState_t;

/**
 * \brief           Touch internal processing enumeration
 */
typedef enum __GUI_TouchStatus_t {
    touchHANDLED = 0x00,                    /*!< Touch has been handled */
    touchHANDLEDNOFOCUS,                    /*!< Touch has been handled but widget is not in focus state (used for widget who don't have touch events) */
    touchCONTINUE                           /*!< Touch has not been handled and further checking can be done */
} __GUI_TouchStatus_t;

/**
 * \brief           Single touch data structure
 */
typedef struct GUI_TouchData_t {
    uint8_t Count;                          /*!< Number of touches detected */
    GUI_iDim_t X[GUI_TOUCH_MAX_PRESSES];    /*!< Touch X coordinate */
    GUI_iDim_t Y[GUI_TOUCH_MAX_PRESSES];    /*!< Touch Y coordinate */
    GUI_TouchState_t Status;                /*!< Touch status, pressed or released */
} GUI_TouchData_t;

/**
 * \brief           Internal touch structure used for widget callbacks
 */
typedef struct __GUI_TouchData_t {
    GUI_TouchData_t TS;                     /*!< Touch structure from outside */
    GUI_iDim_t RelX[GUI_TOUCH_MAX_PRESSES]; /*!< Relative X position to current widget */
    GUI_iDim_t RelY[GUI_TOUCH_MAX_PRESSES]; /*!< Relative Y position to current widget */
} __GUI_TouchData_t;

/**
 * \brief           Single key data structure
 */
typedef struct GUI_KeyboardData_t {
#if GUI_USE_UNICODE
    GUI_Char Keys[4];                       /*!< Key pressed, plain UTF-8 bytes, up to 4 bytes */
#else
    GUI_Char Keys[1];                       /*!< Key pressed */
#endif
    uint8_t Flags;                          /*!< Flags for special keys */
} GUI_KeyboardData_t;

/**
 * \brief           Internal single key data structure
 * \note            Used for private purpose between input and widget
 */
typedef struct __GUI_KeyboardData_t {
    GUI_KeyboardData_t KB;                  /*!< Keyboard structure */
} __GUI_KeyboardData_t;

/**
 * \brief           Keyboard internal processing enumeration
 */
typedef enum __GUI_KeyboardStatus_t {
    keyHANDLED = 0x00,                      /*!< Key has been handled */
    keyCONTINUE                             /*!< Key has not been handled and further checking can be done */
} __GUI_KeyboardStatus_t;

/**
 * \brief           LCD layer structure
 */
typedef struct GUI_Layer_t {
    uint8_t Num;                            /*!< Layer number */
    uint32_t StartAddress;                  /*!< Start address in memory if it exists */
    volatile uint8_t Pending;               /*!< Layer pending for redrawing operation */
} GUI_Layer_t;

/**
 * \brief           GUI LCD structure
 */
typedef struct GUI_LCD_t {
    GUI_Dim_t Width;                        /*!< LCD width in units of pixels */
    GUI_Dim_t Height;                       /*!< LCD height in units of pixels */
    GUI_Byte ActiveLayer;                   /*!< Active layer number currently shown to LCD */
    GUI_Byte DrawingLayer;                  /*!< Currently active drawing layer */
    GUI_Byte LayersCount;                   /*!< Number of layers used for LCD and drawings */
    GUI_Layer_t* Layers;                    /*!< Pointer to layers */
    uint32_t Flags;                         /*!< List of flags */
} GUI_LCD_t;

/**
 * \brief           GUI clipping management
 */
typedef struct GUI_Display_t {
    GUI_Dim_t X1;                           /*!< Clipping area start X */
    GUI_Dim_t Y1;                           /*!< Clipping area start Y */
    GUI_Dim_t X2;                           /*!< Clipping area end X */
    GUI_Dim_t Y2;                           /*!< Clipping area end Y */
} GUI_Display_t;

/**
 * \brief           Low-level LCD command enumeration
 */
typedef enum GUI_LL_Command_t {
    GUI_LL_Command_SetActiveLayer = 0x00,   /*!< Set new layer as active layer */
} GUI_LL_Command_t;

/**
 * \brief           GUI Low-Level structure for drawing operations
 */
typedef struct GUI_LL_t {
    void            (*Init)         (GUI_LCD_t* LCD);                                                                           /*!< Pointer to LCD initialization function */
    void            (*SetPixel)     (GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);                         /*!< Pointer to LCD set pixel function */
    GUI_Color_t     (*GetPixel)     (GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t, GUI_Dim_t);                                      /*!< Pointer to read pixel from LCD */
    void            (*Fill)         (GUI_LCD_t* LCD, uint8_t layer, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);      /*!< Pointer to LCD fill screen or rectangle function */
    void            (*Copy)         (GUI_LCD_t* LCD, uint8_t layer, void *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t);/*!< Pointer to LCD copy data from source to destination */
    void            (*DrawHLine)    (GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);              /*!< Pointer to horizontal line drawing. Set to 0 if you do not have optimized version */
    void            (*DrawVLine)    (GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);              /*!< Pointer to vertical line drawing. Set to 0 if you do not have optimized version */
    void            (*FillRect)     (GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);   /*!< Pointer to function for filling rectangle on LCD */
} GUI_LL_t;

/**
 * \defgroup        GUI_FONT Fonts
 * \brief           Font description structures and flags
 * \{
 */
 
/**
 * \brief           GUI FONT character information
 */
typedef struct GUI_FONT_CharInfo_t {
    GUI_Byte xSize;                         /*!< Character x size in units of pixels */
    GUI_Byte ySize;                         /*!< Character y size in units of pixels */
    GUI_Byte xPos;                          /*!< Character relative x offset in units of pixels */
    GUI_Byte yPos;                          /*!< Character relative y offset in units of pixels */
    GUI_Byte xMargin;                       /*!< Right margin after character in units of pixels. Should be set to 0 if unknown */
    GUI_Const GUI_Byte* Data;               /*!< Pointer to actual data for font */
} GUI_FONT_CharInfo_t;

/**
 * \brief           FONT structure for writing usage
 */
typedef struct {
    GUI_Const GUI_Char* Name;               /*!< Pointer to font name */
    GUI_Byte Size;                          /*!< Font size in units of pixels */
    uint16_t StartChar;                     /*!< Start character number in list */
    uint16_t EndChar;                       /*!< End character number in list */
    GUI_Byte Flags;                         /*!< List of flags for font */
    GUI_Const GUI_FONT_CharInfo_t* Data;    /*!< Pointer to first character */
} GUI_FONT_t;

#define GUI_FLAG_FONT_AA                0x01/*!< Indicates anti-alliasing on font */
#define GUI_FLAG_FONT_RIGHTALIGN        0x02/*!< Indicates right align text if string length is too wide for rectangle */

#if !defined(DOXYGEN)
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
#define ____X_X_                        0x0a
#define ____X_XX                        0x0b
#define ____XX__                        0x0c
#define ____XX_X                        0x0d
#define ____XXX_                        0x0e
#define ____XXXX                        0x0f
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
#define ___XX_X_                        0x1a
#define ___XX_XX                        0x1b
#define ___XXX__                        0x1c
#define ___XXX_X                        0x1d
#define ___XXXX_                        0x1e
#define ___XXXXX                        0x1f
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
#define __X_X_X_                        0x2a
#define __X_X_XX                        0x2b
#define __X_XX__                        0x2c
#define __X_XX_X                        0x2d
#define __X_XXX_                        0x2e
#define __X_XXXX                        0x2f
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
#define __XXX_X_                        0x3a
#define __XXX_XX                        0x3b
#define __XXXX__                        0x3c
#define __XXXX_X                        0x3d
#define __XXXXX_                        0x3e
#define __XXXXXX                        0x3f
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
#define _X__X_X_                        0x4a
#define _X__X_XX                        0x4b
#define _X__XX__                        0x4c
#define _X__XX_X                        0x4d
#define _X__XXX_                        0x4e
#define _X__XXXX                        0x4f
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
#define _X_XX_X_                        0x5a
#define _X_XX_XX                        0x5b
#define _X_XXX__                        0x5c
#define _X_XXX_X                        0x5d
#define _X_XXXX_                        0x5e
#define _X_XXXXX                        0x5f
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
#define _XX_X_X_                        0x6a
#define _XX_X_XX                        0x6b
#define _XX_XX__                        0x6c
#define _XX_XX_X                        0x6d
#define _XX_XXX_                        0x6e
#define _XX_XXXX                        0x6f
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
#define _XXXX_X_                        0x7a
#define _XXXX_XX                        0x7b
#define _XXXXX__                        0x7c
#define _XXXXX_X                        0x7d
#define _XXXXXX_                        0x7e
#define _XXXXXXX                        0x7f
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
#define X___X_X_                        0x8a
#define X___X_XX                        0x8b
#define X___XX__                        0x8c
#define X___XX_X                        0x8d
#define X___XXX_                        0x8e
#define X___XXXX                        0x8f
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
#define X__XX_X_                        0x9a
#define X__XX_XX                        0x9b
#define X__XXX__                        0x9c
#define X__XXX_X                        0x9d
#define X__XXXX_                        0x9e
#define X__XXXXX                        0x9f
#define X_X_____                        0xa0
#define X_X____X                        0xa1
#define X_X___X_                        0xa2
#define X_X___XX                        0xa3
#define X_X__X__                        0xa4
#define X_X__X_X                        0xa5
#define X_X__XX_                        0xa6
#define X_X__XXX                        0xa7
#define X_X_X___                        0xa8
#define X_X_X__X                        0xa9
#define X_X_X_X_                        0xaa
#define X_X_X_XX                        0xab
#define X_X_XX__                        0xac
#define X_X_XX_X                        0xad
#define X_X_XXX_                        0xae
#define X_X_XXXX                        0xaf
#define X_XX____                        0xb0
#define X_XX___X                        0xb1
#define X_XX__X_                        0xb2
#define X_XX__XX                        0xb3
#define X_XX_X__                        0xb4
#define X_XX_X_X                        0xb5
#define X_XX_XX_                        0xb6
#define X_XX_XXX                        0xb7
#define X_XXX___                        0xb8
#define X_XXX__X                        0xb9
#define X_XXX_X_                        0xba
#define X_XXX_XX                        0xbb
#define X_XXXX__                        0xbc
#define X_XXXX_X                        0xbd
#define X_XXXXX_                        0xbe
#define X_XXXXXX                        0xbf
#define XX______                        0xc0
#define XX_____X                        0xc1
#define XX____X_                        0xc2
#define XX____XX                        0xc3
#define XX___X__                        0xc4
#define XX___X_X                        0xc5
#define XX___XX_                        0xc6
#define XX___XXX                        0xc7
#define XX__X___                        0xc8
#define XX__X__X                        0xc9
#define XX__X_X_                        0xca
#define XX__X_XX                        0xcb
#define XX__XX__                        0xcc
#define XX__XX_X                        0xcd
#define XX__XXX_                        0xce
#define XX__XXXX                        0xcf
#define XX_X____                        0xd0
#define XX_X___X                        0xd1
#define XX_X__X_                        0xd2
#define XX_X__XX                        0xd3
#define XX_X_X__                        0xd4
#define XX_X_X_X                        0xd5
#define XX_X_XX_                        0xd6
#define XX_X_XXX                        0xd7
#define XX_XX___                        0xd8
#define XX_XX__X                        0xd9
#define XX_XX_X_                        0xda
#define XX_XX_XX                        0xdb
#define XX_XXX__                        0xdc
#define XX_XXX_X                        0xdd
#define XX_XXXX_                        0xde
#define XX_XXXXX                        0xdf
#define XXX_____                        0xe0
#define XXX____X                        0xe1
#define XXX___X_                        0xe2
#define XXX___XX                        0xe3
#define XXX__X__                        0xe4
#define XXX__X_X                        0xe5
#define XXX__XX_                        0xe6
#define XXX__XXX                        0xe7
#define XXX_X___                        0xe8
#define XXX_X__X                        0xe9
#define XXX_X_X_                        0xea
#define XXX_X_XX                        0xeb
#define XXX_XX__                        0xec
#define XXX_XX_X                        0xed
#define XXX_XXX_                        0xee
#define XXX_XXXX                        0xef
#define XXXX____                        0xf0
#define XXXX___X                        0xf1
#define XXXX__X_                        0xf2
#define XXXX__XX                        0xf3
#define XXXX_X__                        0xf4
#define XXXX_X_X                        0xf5
#define XXXX_XX_                        0xf6
#define XXXX_XXX                        0xf7
#define XXXXX___                        0xf8
#define XXXXX__X                        0xf9
#define XXXXX_X_                        0xfa
#define XXXXX_XX                        0xfb
#define XXXXXX__                        0xfc
#define XXXXXX_X                        0xfd
#define XXXXXXX_                        0xfe
#define XXXXXXXX                        0xff
#endif /* !defined(DOXYGEN) */

/**
 * \}
 */

/**
 * \}
 */
  
/**
 * \addtogroup      GUI_TIMER
 * \{
 */
 
/**
 * \brief           Timer structure
 */
typedef struct GUI_TIMER_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must be first on the list */
    uint16_t Period;                        /*!< Timer period value */
    uint16_t Counter;                       /*!< Timer current counter */
    uint8_t Flags;                          /*!< Timer flags */
    void* Params;                           /*!< Custom parameters passed to callback function */
    void (*Callback)    (struct GUI_TIMER_t *); /*!< Timer callback function */
} GUI_TIMER_t;

/**
 * \}
 */

/**
 * \addtogroup      GUI_WIDGETS_CORE
 * \{
 */

/**
 * \brief           Control parameters for widget
 * \note            Must always start with number 1
 */
typedef enum GUI_WC_t {
    /**
     * \brief       Draw widget on screen
     *
     * \param[in]   *param: Pointer to \ref GUI_Display_t structure
     * \param[out]  *result: None
     */
    GUI_WC_Draw = 0x01,
    
    /**
     * \brief       Check if widget should not be added to linked list after creation
     *
     * \param[in]   *param: None
     * \param[out]  *result: Pointer to uint8_t variable type to store result to [1 = exclude, 0 = do not exclude]
     */
    GUI_WC_ExcludeLinkedList,
    
    /**
     * \brief       Check if widget can be removed. User can perform check if for example widget needs save or similar operation
     *
     * \param[in]   *param: None
     * \param[out]  *result: Pointer to uint8_t variable type to store result to [1 = remove, 0 = do not remove]
     * \sa          GUI_WC_Remove
     */
    GUI_WC_CanRemove,
    
    /**
     * \brief       Notification before widget delete will actually happen.
     *              In case of specific user allocations in memory, they can be restored here
     * 
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_CanRemove
     */
    GUI_WC_Remove,
    
    /**
     * \brief       Notification called when widget becomes focused
     *
     * \param[in]   *param None
     * \param[out]  *result: None
     * \sa          GUI_WC_FocusOut
     */
    GUI_WC_FocusIn,
    
    /**
     * \brief       Notification called when widget clears widget state
     *
     * \param[in]   *param None
     * \param[out]  *result: None
     * \sa          GUI_WC_FocusIn
     */
    GUI_WC_FocusOut,
    
    /**
     * \brief       Notification for active status on widget
     *
     * \note        Called when widget just become active
     *
     * \param[in]   *param None
     * \param[out]  *result: None
     * \sa          GUI_WC_ActiveOut
     */
    GUI_WC_ActiveIn,
    
    /**
     * \brief       Notification for cleared active status on widget
     *
     * \note        Called when widget just clears active
     *
     * \param[in]   *param None
     * \param[out]  *result: None
     * \sa          GUI_WC_ActiveIn
     */
    GUI_WC_ActiveOut,
    
    /**
     * \brief       Notification when touch down event occurs on widget
     *
     * \param[in]   *param Pointer to \ref __GUI_TouchData_t structure
     * \param[out]  *result: Value of \ref __GUI_TouchStatus_t enumeration
     * \sa          GUI_WC_TouchMove
     * \sa          GUI_WC_TouchEnd
     */
    GUI_WC_TouchStart,
    
    /**
     * \brief       Notification when touch move event occurs on widget
     *
     * \param[in]   *param Pointer to \ref __GUI_TouchData_t structure
     * \param[out]  *result: Value of \ref __GUI_TouchStatus_t enumeration
     * \sa          GUI_WC_TouchStart
     * \sa          GUI_WC_TouchEnd
     */
    GUI_WC_TouchMove,
    
    /**
     * \brief       Notification when touch up event occurs on widget
     *
     * \param[in]   *param Pointer to \ref __GUI_TouchData_t structure
     * \param[out]  *result: Value of \ref __GUI_TouchStatus_t enumeration
     * \sa          GUI_WC_TouchStart
     * \sa          GUI_WC_TouchMove
     */
    GUI_WC_TouchEnd,
    
    /**
     * \brief       Notification when key has been pushed to this widget
     *
     * \param[in]   *param Pointer to \ref __GUI_KeyboardData_t structure
     * \param[out]  *result: Value of \ref __GUI_KeyboardStatus_t enumeration
     */
    GUI_WC_KeyPress,
} GUI_WC_t;

/**
 * \defgroup        GUI_WIDGETS_CORE_FLAGS Widget type flags
 * \brief           Flags used for widget type description
 * \{
 */

#define GUI_FLAG_WIDGET_ALLOW_CHILDREN      ((uint16_t)0x0001)  /*!< Widget allows children widgets */

/**
 * \}
 */

/**
 * \brief           Structure declaration
 */
struct GUI_HANDLE;

/**
 * \brief           Structure for each widget type
 */
typedef struct GUI_WIDGET_t {
    struct {
        const GUI_Char* Name;               /*!< Widget name for display purpose */
        uint16_t WidgetSize;                /*!< Bytes required for widget memory allocation */
        uint8_t Flags;                      /*!< List of flags for widget setup. This field can use \ref GUI_WIDGETS_CORE_FLAGS flags */
    } MetaData;                             /*!< Meta data structure for widget */
    
    /**
     * \brief       callback function for widget
     * 
     *              Handles everything related to widget. 
     *                  - Draw widget
     *                  - Touch handling
     *                  - Key presses
     */
    GUI_Byte (*Callback)    (struct GUI_HANDLE* h, GUI_WC_t cmd, void* param, void* result);    /*!< Pointer to control function, returns 1 if command handled or 0 if not */
} GUI_WIDGET_t;

/**
 * \brief           Common GUI values for widgets
 */
typedef struct GUI_HANDLE {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must always be first on list */
    GUI_ID_t Id;                            /*!< Widget ID number */
    const GUI_WIDGET_t* Widget;             /*!< Widget parameters with callback functions */
    uint8_t (*Callback) (struct GUI_HANDLE* h, GUI_WC_t cmd, void* param, void* result);    /*!< Callback function for widget specific */
    struct GUI_HANDLE* Parent;              /*!< Pointer to parent widget */
    GUI_Dim_t X;                            /*!< Object X position relative to parent window in units of pixels */
    GUI_Dim_t Y;                            /*!< Object Y position relative to parent window in units of pixels */
    GUI_Dim_t Width;                        /*!< Object width in units of pixels or percentages */
    GUI_Dim_t Height;                       /*!< Object height in units of pixels or percentages */
    uint32_t Padding;                       /*!< 4-byte long padding, each byte of one side, MSB = top padding, LSB = left padding.
                                                    Used for children widgets if virtual padding should be used */
    uint32_t Flags;                         /*!< All possible flags for specific widget */
    GUI_Const GUI_FONT_t* Font;             /*!< Font used for widget drawings */
    GUI_Char* Text;                         /*!< Pointer to widget text if exists */
    uint32_t TextMemSize;                   /*!< Number of bytes for text when dynamically allocated */
    uint32_t TextCursor;                    /*!< Text cursor position */
    GUI_TIMER_t* Timer;                     /*!< Software timer pointer */
} GUI_HANDLE;

/**
 * \brief           Common GUI values for widgets who can have children widgets (windows, panels)
 */
typedef struct GUI_HANDLE_ROOT {
    GUI_HANDLE Handle;                      /*!< Root widget structure, must be first in structure */
    GUI_LinkedListRoot_t RootList;          /*!< Linked list root structure */
} GUI_HANDLE_ROOT_t;

/**
 * \brief           Handle object for GUI widget
 */
typedef GUI_HANDLE* GUI_HANDLE_p;

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

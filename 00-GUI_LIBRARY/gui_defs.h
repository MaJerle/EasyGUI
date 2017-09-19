/**
 * \author  Tilen Majerle <tilen@majerle.eu>
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
#include "pt/pt.h"

/**
 * \addtogroup      GUI
 * \{
 */

/**
 * \addtogroup      GUI_Flags Flags
 * \brief           List of all flags in GUI library
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
#define GUI_FLAG_WIDTH_FILL                 ((uint32_t)0x00000400)  /*!< Indicates widget width fills to the end of widget */
#define GUI_FLAG_HEIGHT_FILL                ((uint32_t)0x00000800)  /*!< Indicates widget height fills to the end of widget */
#define GUI_FLAG_EXPANDED                   ((uint32_t)0x00001000)  /*!< Indicates children widget is set to (temporary) XY = 0,0 and width/height = parent width / parent height (maximize windows function) */
#define GUI_FLAG_REMOVE                     ((uint32_t)0x00002000)  /*!< Indicates widget should be deleted */
#define GUI_FLAG_IGNORE_INVALIDATE          ((uint32_t)0x00004000)  /*!< Indicates widget invalidation is ignored completely when invalidating it directly */
#define GUI_FLAG_TOUCH_MOVE                 ((uint32_t)0x00008000)  /*!< Indicates widget callback has processed touch move event. This parameter works in conjunction with \ref GUI_FLAG_ACTIVE flag */
#define GUI_FLAG_XPOS_PERCENT               ((uint32_t)0x00010000)  /*!< Indicates widget X position is in percent relative to parent width */
#define GUI_FLAG_YPOS_PERCENT               ((uint32_t)0x00020000)  /*!< Indicates widget Y position is in percent relative to parent height */
/**
 * \defgroup        GUI_WIDGETS_CORE_FLAGS Widget type flags
 * \brief           Flags used for widget type description
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
typedef enum GUI_Result_t {
    guiOK = 0x00,                           /*!< GUI status is OK */
    guiERROR = 0x01                         /*!< There was an error in processing */
} GUI_Result_t;

/**
 * \brief           Linked list structure
 * \note            This structure must be first element in \ref GUI_HANDLE_p structure
 * \sa              gui_linkedlistroot_t
 */
typedef struct GUI_LinkedList_t {
    void* Prev;                             /*!< Previous object in linked list */
    void* Next;                             /*!< Next object in linked list */
} GUI_LinkedList_t;

/**
 * \brief           Structure used for dynamic struct allocation
 * \sa              gui_linkedlistroot_t
 */
typedef struct GUI_LinkedListMulti_t {
    GUI_LinkedList_t List;                  /*!< Linked list structure, must be first on structure for casting */
    void* Element;                          /*!< Pointer to element in linked list */
} GUI_LinkedListMulti_t;

/**
 * \brief           Linked list root structure for start and end widget in structure
 * \sa              gui_linkedlist_t
 * \sa              gui_linkedlistmulti_t
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
typedef int16_t     GUI_Dim_t;              /*!< GUI dimensions in units of pixels */
typedef int16_t     GUI_iDim_t;             /*!< GUI signed dimensions in units of pixels  */
typedef uint8_t     GUI_Byte;               /*!< GUI byte data type */
typedef GUI_Byte    GUI_Byte_t;             /*!< GUI byte data type */
typedef int8_t      GUI_iByte;              /*!< GUI signed byte data type */
typedef GUI_iByte   GUI_iByte_t;            /*!< GUI signed byte data type */
typedef uint8_t     GUI_Char;               /*!< GUI char data type for all string operations */
#define _GT(x)      (GUI_Char *)(x)         /*!< Macro to force strings to right format for processing */
#define GUI_Const   const                   /*!< Macro for constant keyword */

/**
 * \brief           Count number of elements in array
 * \warning         This does not work with pointers
 * \hideinitializer
 */
#define GUI_COUNT_OF(x)         (sizeof(x) / sizeof((x)[0]))
    
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
    my_structure* my_struct = gui_containerof(drv, my_structure, driver);
    
    //Check if they match
    if (my_struct == &my) {
        //This should be true
    }
}
\endcode
 */
#define             gui_containerof(ptr, type, memb)      (type *)((char *)(ptr) - (char *)offsetof(type, memb))

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
    GUI_TouchState_RELEASED = 0x00,         /*!< Touch released */
    GUI_TouchState_PRESSED = 0x01,          /*!< Touch detected */
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
    uint32_t Time;                          /*!< Time when touch was recorded */
} GUI_TouchData_t;

/**
 * \brief           Internal touch structure used for widget callbacks
 */
typedef struct __GUI_TouchData_t {
    GUI_TouchData_t TS;                     /*!< Touch structure from outside */
    GUI_iDim_t RelX[GUI_TOUCH_MAX_PRESSES]; /*!< Relative X position to current widget */
    GUI_iDim_t RelY[GUI_TOUCH_MAX_PRESSES]; /*!< Relative Y position to current widget */
    GUI_Dim_t WidgetWidth;                  /*!< Save widget width value */
    GUI_Dim_t WidgetHeight;                 /*!< Save widget height value */
#if GUI_TOUCH_MAX_PRESSES > 1 || defined(DOXYGEN)
    float Distance;                         /*!< Distance between 2 points when 2 touch elements are detected */
    float DistanceOld;                      /*!< Old distance between 2 points */
#endif /* GUI_TOUCH_MAX_PRESSES > 1 || defined(DOXYGEN) */
    struct pt pt;                           /*!< Protothreads structure */
} __GUI_TouchData_t;

/**
 * \brief           Single key data structure
 */
typedef struct GUI_KeyboardData_t {
#if GUI_USE_UNICODE || defined(DOXYGEN)
    GUI_Char Keys[4];                       /*!< Key pressed, plain unicode bytes, up to 4 bytes */
#else
    GUI_Char Keys[1];                       /*!< Key pressed, no unicode support */
#endif
    uint8_t Flags;                          /*!< Flags for special keys */
    uint32_t Time;                          /*!< Event time */
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
 * \brief           GUI clipping management
 */
typedef struct GUI_Display_t {
    GUI_iDim_t X1;                          /*!< Clipping area start X */
    GUI_iDim_t Y1;                          /*!< Clipping area start Y */
    GUI_iDim_t X2;                          /*!< Clipping area end X */
    GUI_iDim_t Y2;                          /*!< Clipping area end Y */
} GUI_Display_t;

/**
 * \brief           LCD layer structure
 */
typedef struct GUI_Layer_t {
    uint8_t Num;                            /*!< Layer number */
    uint32_t StartAddress;                  /*!< Start address in memory if it exists */
    volatile uint8_t Pending;               /*!< Layer pending for redrawing operation */
    GUI_Display_t Display;                  /*!< Display setup for clipping regions for main layers (no virtual) */
    
    GUI_iDim_t Width;                       /*!< Layer width, used for virtual layers mainly */
    GUI_iDim_t Height;                      /*!< Layer height, used for virtual layers mainly */
    GUI_iDim_t OffsetX;                     /*!< Offset value for pixel position calculation in X direction, used for virtual layers */
    GUI_iDim_t OffsetY;                     /*!< Offset value for pixel position calculation in Y direction, used for virtual layers */
} GUI_Layer_t;

/**
 * \brief           GUI LCD structure
 */
typedef struct GUI_LCD_t {
    GUI_Dim_t Width;                        /*!< LCD width in units of pixels */
    GUI_Dim_t Height;                       /*!< LCD height in units of pixels */
    uint8_t PixelSize;                      /*!< Number of bytes per pixel */
    GUI_Layer_t* ActiveLayer;               /*!< Active layer number currently shown to LCD */
    GUI_Layer_t* DrawingLayer;              /*!< Currently active drawing layer */
    size_t LayersCount;                     /*!< Number of layers used for LCD and drawings */
    GUI_Layer_t* Layers;                    /*!< Pointer to layers */
    uint32_t Flags;                         /*!< List of flags */
} GUI_LCD_t;

/**
 * \addtogroup      GUI_IMAGE
 * \{
 */

/**
 * \brief           Image descriptor structure
 */
typedef struct GUI_IMAGE_DESC_t {
    GUI_Dim_t xSize;                        /*!< X size */
    GUI_Dim_t ySize;                        /*!< Y size */
    uint8_t BPP;                            /*!< Bits per pixel */
    const uint8_t* Image;                   /*!< Pointer to image byte array */
} GUI_IMAGE_DESC_t;

/**
 * \}
 */

/**
 * \brief           Low-level LCD command enumeration
 */
typedef enum GUI_LL_Command_t {
    /**
     * \brief       Initialize low-level part of GUI
     *
     * \param[in]   *param: Pointer to \ref GUI_LL_t structure to fill data to
     * \param[out]  *result: Pointer to \ref uint8_t variable to save result: 0 = OK otherwise ERROR
     */
    GUI_LL_Command_Init = 0x00,             /*!< Set new layer as active layer */
    
    /**
     * \brief       Initialize low-level part of GUI
     *
     * \param[in]   *param: Pointer to \ref GUI_LL_t structure to fill data to
     * \param[out]  *result: Pointer to \ref uint8_t variable to save result: 0 = OK otherwise ERROR
     */
    GUI_LL_Command_SetActiveLayer,          /*!< Set new layer as active layer */
} GUI_LL_Command_t;

/**
 * \brief           GUI Low-Level structure for drawing operations
 */
typedef struct GUI_LL_t {
    void            (*Init)         (GUI_LCD_t *);                                                                      /*!< Pointer to LCD initialization function */
    uint8_t         (*IsReady)      (GUI_LCD_t *);                                                                      /*!< Pointer to LCD is ready function */
    void            (*SetPixel)     (GUI_LCD_t *, GUI_Layer_t *, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);                    /*!< Pointer to LCD set pixel function */
    GUI_Color_t     (*GetPixel)     (GUI_LCD_t *, GUI_Layer_t *, GUI_Dim_t, GUI_Dim_t);                                 /*!< Pointer to read pixel from LCD */
    void            (*Fill)         (GUI_LCD_t *, GUI_Layer_t *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t); /*!< Pointer to LCD fill screen or rectangle function */
    void            (*Copy)         (GUI_LCD_t *, GUI_Layer_t *, const void *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t);   /*!< Pointer to LCD copy data from source to destination */
    void            (*CopyBlend)    (GUI_LCD_t *, GUI_Layer_t *, const void *, void *, uint8_t, uint8_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t);   /*!< Pointer to function to copy layers together (blending) with support to set overall layer transparency */
    void            (*DrawHLine)    (GUI_LCD_t *, GUI_Layer_t *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);         /*!< Pointer to horizontal line drawing. Set to 0 if you do not have optimized version */
    void            (*DrawVLine)    (GUI_LCD_t *, GUI_Layer_t *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);         /*!< Pointer to vertical line drawing. Set to 0 if you do not have optimized version */
    void            (*FillRect)     (GUI_LCD_t *, GUI_Layer_t *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);  /*!< Pointer to function for filling rectangle on LCD */
    void            (*DrawImage16)  (GUI_LCD_t *, GUI_Layer_t *, const GUI_IMAGE_DESC_t *, const void *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t);   /*!< Pointer to function for drawing 16BPP (RGB565) images */
    void            (*DrawImage24)  (GUI_LCD_t *, GUI_Layer_t *, const GUI_IMAGE_DESC_t *, const void *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t);   /*!< Pointer to function for drawing 24BPP (RGB888) images */
    void            (*DrawImage32)  (GUI_LCD_t *, GUI_Layer_t *, const GUI_IMAGE_DESC_t *, const void *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t);   /*!< Pointer to function for drawing 32BPP (ARGB8888) images */
    void            (*CopyChar)     (GUI_LCD_t *, GUI_Layer_t *, const void *, void *, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Dim_t, GUI_Color_t);                /*!< Pointer to copy char function with alpha only as source */
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
    uint8_t Flags;                          /*!< List of flags for font */
    GUI_Const GUI_FONT_CharInfo_t* Data;    /*!< Pointer to first character */
} GUI_FONT_t;

#define GUI_FLAG_FONT_AA                ((uint8_t)0x01) /*!< Indicates anti-alliasing on font */
#define GUI_FLAG_FONT_RIGHTALIGN        ((uint8_t)0x02) /*!< Indicates right align text if string length is too wide for rectangle */
#define GUI_FLAG_FONT_MULTILINE         ((uint8_t)0x04) /*!< Indicates multi line support on widget */
#define GUI_FLAG_FONT_EDITMODE          ((uint8_t)0x08) /*!< Edit mode is enabled on text */

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Char temporary entry stored in RAM for faster copy with blending operations
 */
typedef struct GUI_FONT_CharEntry_t {
    GUI_LinkedList_t List;                  /*!< Linked list entry. Must always be first on the list */
    const GUI_FONT_CharInfo_t* Ch;          /*!< Character value */
    const GUI_FONT_t* Font;                 /*!< Pointer to font structure */
} GUI_FONT_CharEntry_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

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
 * \brief           Pointer to \ref GUI_TIMER_t
 */
typedef GUI_TIMER_t* GUI_TIMER_p;

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
    GUI_WC_None = 0x00,
#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    /**
     * \brief       Called just after widget has been created. Used for internal widget purpose only
     *
     * \note        This field is only visible inside GUI library and can't be used in custom widget callback
     *
     * \param[in]   *param: None
     * \param[out]  *result: Pointer to uint8_t variable with result. If set to 0 by user, widget will be deleted
     */
    GUI_WC_PreInit = 0x01,
    
    /**
     * \brief       Check if widget should not be added to linked list after creation
     *
     * \param[in]   *param: None
     * \param[out]  *result: Pointer to uint8_t variable type to store result to [1 = exclude, 0 = do not exclude]
     */
    GUI_WC_ExcludeLinkedList = 0x02,
    
    /**
     * \brief       Set user parameter for widget
     *
     * \param[in]   *param: Pointer to \ref GUI_WIDGET_Param_t structure with custom user parameter for widget
     * \param[out]  *result: None
     */
    GUI_WC_SetParam = 0x03,
    
    /**
     * \brief       Get parameter fror widget
     *
     * \param[in]   *param: Pointer to \ref GUI_WIDGET_Param_t structure with identification and memory where to save parameter value
     * \param[out]  *result: None
     */
    GUI_WC_GetParam = 0x04,
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */
    
    /**
     * \brief       Widget has been created and ready to init for future setup
     *
     * \note        Default values for widget can be set here.
     *              Example: CHeckbox check can be made and default checkbox value should be set if required
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     */
    GUI_WC_Init = 0x05,
    
    /**
     * \brief       Draw widget on screen
     *
     * \param[in]   *param: Pointer to \ref GUI_Display_t structure
     * \param[out]  *result: None
     */
    GUI_WC_Draw,
    
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
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_FocusOut
     */
    GUI_WC_FocusIn,
    
    /**
     * \brief       Notification called when widget clears widget state
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_FocusIn
     */
    GUI_WC_FocusOut,
    
    /**
     * \brief       Notification for active status on widget
     *
     * \note        Called when widget just become active
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_ActiveOut
     */
    GUI_WC_ActiveIn,
    
    /**
     * \brief       Notification for cleared active status on widget
     *
     * \note        Called when widget just clears active
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_ActiveIn
     */
    GUI_WC_ActiveOut,
    
    /**
     * \brief       Notification when touch down event occurs on widget
     *
     * \param[in]   *param: Pointer to \ref __GUI_TouchData_t structure
     * \param[out]  *result: Value of \ref __GUI_TouchStatus_t enumeration
     * \sa          GUI_WC_TouchMove, GUI_WC_TouchEnd
     */
    GUI_WC_TouchStart,
    
    /**
     * \brief       Notification when touch move event occurs on widget
     *
     * \note        If return value from function on this parameter is 1, then \ref GUI_WC_Click event won't be called after released status
     *              to prevent collisions between widget events (sliding on widget and click at the same time can cause unknown behaviour on user)
     *
     * \param[in]   *param: Pointer to \ref __GUI_TouchData_t structure
     * \param[out]  *result: Value of \ref __GUI_TouchStatus_t enumeration
     * \sa          GUI_WC_TouchStart, GUI_WC_TouchEnd
     */
    GUI_WC_TouchMove,
    
    /**
     * \brief       Notification when touch up event occurs on widget
     *
     * \param[in]   *param: Pointer to \ref __GUI_TouchData_t structure
     * \param[out]  *result: Value of \ref __GUI_TouchStatus_t enumeration
     * \sa          GUI_WC_TouchStart, GUI_WC_TouchMove
     */
    GUI_WC_TouchEnd,
    
    /**
     * \brief       Notification when click event has been detected
     *
     * \param[in]   *param: Pointer to \ref __GUI_TouchData_t structure with valid touch press location
     * \param[out]  *result: None
     */
    GUI_WC_Click,
    
    /**
     * \brief       Notification when long press has been detected
     *
     * \param[in]   *param: Pointer to \ref __GUI_TouchData_t structure with valid touch press location
     * \param[out]  *result: None
     */
    GUI_WC_LongClick,
    
    /**
     * \brief       Notification when double click has been detected
     *
     * \param[in]   *param: Pointer to \ref __GUI_TouchData_t structure with valid touch press location
     * \param[out]  *result: None
     */
    GUI_WC_DblClick,
    
    /**
     * \brief       Notification when key has been pushed to this widget
     *
     * \param[in]   *param: Pointer to \ref __GUI_KeyboardData_t structure
     * \param[out]  *result: Value of \ref __GUI_KeyboardStatus_t enumeration
     */
    GUI_WC_KeyPress,
    
    /**     
     * \brief       Notification when widget selection has changed
     *
     * \note        Called from widget by user when necessary. Not all widget reports this value by itself
     *
     * \note        Used by widgets where selection can be changed (listbox, selectbox, radio, etc)
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_ValueChanged
     */
    GUI_WC_SelectionChanged,
    
    /**     
     * \brief       Value of widget has been changed
     *
     * \note        Called from widget by user when necessary. Not all widget reports this value by itself
     *
     * \note        Used by widgets where value or status can change (checkbox)
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     * \sa          GUI_WC_SelectionChanged
     */
    GUI_WC_ValueChanged,
    
    /**     
     * \brief       Widget text value has been changed
     *
     * \note        Called from widget by user when necessary. Not all widget reports this value by itself
     *
     * \param[in]   *param: None
     * \param[out]  *result: None
     */
    GUI_WC_TextChanged,
    
    /**     
     * \brief       Widget should increase/decrease selection
     *
     * \note        Called from \ref gui_widget_incselection function to increase/decrease selection for widgets
     *                 such as listbox or dropdown, where active selection can be changed
     *
     * \param[in]   *param: Pointer to \ref int16_t variable for amount of increase/decrease value
     * \param[out]  *result: Pointer to output \ref uint8_t variable to save status of increase/decrease operation
     */
    GUI_WC_IncSelection,
    
    /**     
     * \brief       Called when dialog is dismissed
     *
     * \note        Callback is activated when dismiss function is called on dialog widget.
     *              Notification can be used to proceed with function call
     *
     * \param[in]   *param: Pointer to int variable passed to dismiss function
     * \param[out]  *result: None
     */
    GUI_WC_OnDismiss,
} GUI_WC_t;

/**
 * \brief           Structure declaration
 */
struct GUI_HANDLE;

/**
 * \brief           Handle object for GUI widget
 */
typedef struct GUI_HANDLE* GUI_HANDLE_p;

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
typedef uint8_t (*GUI_WIDGET_CALLBACK_t) (GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);

/**
 * \brief           Structure for each widget type
 */
typedef struct GUI_WIDGET_t {
    const GUI_Char* Name;                   /*!< Widget name for display purpose */
    uint16_t Size;                          /*!< Bytes required for widget memory allocation */
    uint32_t Flags;                         /*!< List of flags for widget setup. This field can use \ref GUI_WIDGETS_CORE_FLAGS flags */
    GUI_WIDGET_CALLBACK_t Callback;         /*!< Pointer to control function, returns 1 if command handled or 0 if not */
    const GUI_Color_t* Colors;              /*!< Pointer to list of colors as default values for widget */
    uint8_t ColorsCount;                    /*!< Number of colors used in widget */
} GUI_WIDGET_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)

/**
 * \brief           Common GUI values for widgets
 */
typedef struct GUI_HANDLE {
    GUI_LinkedList_t List;                  /*!< Linked list entry, must always be on top for casting */
    GUI_ID_t Id;                            /*!< Widget ID number */
    uint32_t Footprint;                     /*!< Footprint indicates widget is valid */
    const GUI_WIDGET_t* Widget;             /*!< Widget parameters with callback functions */
    GUI_WIDGET_CALLBACK_t Callback;         /*!< Callback function prototype */
    struct GUI_HANDLE* Parent;              /*!< Pointer to parent widget */
    float X;                                /*!< Object X position relative to parent window in units of pixels */
    float Y;                                /*!< Object Y position relative to parent window in units of pixels */
    float Width;                            /*!< Object width in units of pixels or percentages */
    float Height;                           /*!< Object height in units of pixels or percentages */
    uint32_t Padding;                       /*!< 4-bytes long padding, each byte of one side, MSB = top padding, LSB = left padding.
                                                    Used for children widgets if virtual padding should be used */
    int32_t ZIndex;                         /*!< Z-Index value of widget, which can be set by user. All widgets with same z-index are changeable when active on visible area */
    uint8_t Transparency;                   /*!< Transparency of widget relative to parent widget */
    uint32_t Flags;                         /*!< All possible flags for specific widget */
    GUI_Const GUI_FONT_t* Font;             /*!< Font used for widget drawings */
    GUI_Char* Text;                         /*!< Pointer to widget text if exists */
    size_t TextMemSize;                     /*!< Number of bytes for text when dynamically allocated */
    size_t TextCursor;                      /*!< Text cursor position */
    GUI_TIMER_t* Timer;                     /*!< Software timer pointer */
    GUI_Color_t* Colors;                    /*!< Pointer to allocated color memory when custom colors are used */
    void* UserData;                         /*!< Pointer to optional user data */
} GUI_HANDLE;

/**
 * \brief           Common GUI values for widgets who can have children widgets (windows, panels)
 */
typedef struct GUI_HANDLE_ROOT {
    GUI_HANDLE Handle;                      /*!< Root widget structure, must be first in structure */
    
    GUI_LinkedListRoot_t RootList;          /*!< Linked list root of children widgets */
    GUI_iDim_t ScrollX;                     /*!< Scroll of widgets in horizontal direction in units of pixels */
    GUI_iDim_t ScrollY;                     /*!< Scroll of widgets in vertical direction in units of pixels */
} GUI_HANDLE_ROOT_t;

/**
 * \brief           Structure used in setting and getting parameter values from widgets using callbacks
 */
typedef struct GUI_WIDGET_Param_t {
    uint16_t Type;                          /*!< Type of command to set or get */
    void* Data;                             /*!< Pointer to actual data to set or get to for specific widget */
} GUI_WIDGET_Param_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Widget create function footprint for structures as callbacks
 */
typedef GUI_HANDLE_p (*GUI_WIDGET_CreateFunc_t)(GUI_ID_t, GUI_iDim_t, GUI_iDim_t, GUI_iDim_t, GUI_iDim_t, GUI_HANDLE_p, GUI_WIDGET_CALLBACK_t, uint16_t);

/**
 * \}
 */

/**
 * \addtogroup      GUI_TRANSLATE
 */

/**
 * \brief           Translation language structure
 */
typedef struct GUI_TRANSLATE_Language_t {
    const GUI_Char* Lang;                   /*!< Language code used to identify it later when setting active language */
    const GUI_Char** Entries;               /*!< Pointer to list containing pointers to translated entries */
    size_t Count;                           /*!< Number of entries in translated array */
} GUI_TRANSLATE_Language_t;

//#if defined(GUI_INTERNAL) || defined(DOXYGEN)
/**
 * \brief           Basic translation structure for internal use
 */
typedef struct GUI_TRANSLATE_t {
    const GUI_TRANSLATE_Language_t* Source; /*!< Pointer to source language table */
    const GUI_TRANSLATE_Language_t* Active; /*!< Pointer to current language table */
} GUI_TRANSLATE_t;

//#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

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

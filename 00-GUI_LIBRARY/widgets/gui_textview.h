/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI dropdown widget
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
#ifndef GUI_TEXTVIEW_H
#define GUI_TEXTVIEW_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_WIDGETS
 * \{
 */
#include "gui_widget.h"

/**
 * \defgroup        GUI_TEXTVIEW Text view
 * \brief           Text view widget
 * \{
 *
 * Text view widget can only display text assigned to its memory
 *
 * \image html image_widget_textview.png Text view widget in different alignments
 *
 * Example code of image above:
 * \code{c}
size_t i = 0;
static const GUI_Char* texts[] = {
    _GT("Text view with top left alignment on screen"),
    _GT("Text view with top center alignment on screen"),
    _GT("Text view with top right alignment on screen"),
    _GT("Text view with middle left alignment on screen"),
    _GT("Text view with middle center alignment on screen"),
    _GT("Text view with middle right alignment on screen"),
    _GT("Text view with bottom left alignment on screen"),
    _GT("Text view with bottom center alignment on screen"),
    _GT("Text view with bottom right alignment on screen"),
};
for (i = 0; i < 9; i++) {
    handle = gui_textview_create(0, 1, 1, 1, 1, 0, 0, 0);
    gui_widget_setsizepercent(handle, 30, 30);
    gui_widget_setpositionpercent(handle, 3 + (i % 3) * 33, 3 + (i / 3) * 33);
    gui_widget_settext(handle, texts[i]);
    gui_widget_setfont(handle, &GUI_Font_Roboto_Italic_14);
    switch (i % 4) {
        case 0: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_WHITE); break;
        case 1: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_YELLOW); break;
        case 2: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_GRAY); break;
        case 3: gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_LIGHTGREEN); break;
    }
    switch (i % 3) {
        case 0: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_LEFT); break;
        case 1: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_CENTER); break;
        case 2: gui_textview_sethalign(handle, GUI_TEXTVIEW_HALIGN_RIGHT); break;
    }
    switch (i / 3) {
        case 0: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_TOP); break;
        case 1: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_CENTER); break;
        case 2: gui_textview_setvalign(handle, GUI_TEXTVIEW_VALIGN_BOTTOM); break;
    }
}
\endcode
 */
    
/**
 * \brief           List of text view colors
 */
typedef enum GUI_TEXTVIEW_COLOR_t {
    GUI_TEXTVIEW_COLOR_BG = 0x00,           /*!< Background color */
    GUI_TEXTVIEW_COLOR_TEXT,                /*!< Default text color */
} GUI_TEXTVIEW_COLOR_t;

/**
 * \brief           Vertical align for text view inside box
 */
typedef enum GUI_TEXTVIEW_VALIGN_t {        
    GUI_TEXTVIEW_VALIGN_TOP = GUI_VALIGN_TOP,       /*!< Vertical align top, default value */
    GUI_TEXTVIEW_VALIGN_CENTER = GUI_VALIGN_CENTER, /*!< Vertical align center */
    GUI_TEXTVIEW_VALIGN_BOTTOM = GUI_VALIGN_BOTTOM  /*!< Vertical align bottom */
} GUI_TEXTVIEW_VALIGN_t;

/**
 * \brief           Horizontal align for text view inside box
 */
typedef enum GUI_TEXTVIEW_HALIGN_t {        
    GUI_TEXTVIEW_HALIGN_LEFT = GUI_HALIGN_LEFT,     /*!< Horizontal align left, default value */
    GUI_TEXTVIEW_HALIGN_CENTER = GUI_HALIGN_CENTER, /*!< Horizontal align center */
    GUI_TEXTVIEW_HALIGN_RIGHT = GUI_HALIGN_RIGHT    /*!< Horizontal align right */
} GUI_TEXTVIEW_HALIGN_t;

#if defined(GUI_INTERNAL) || defined(DOXYGEN)
    
/**
 * \brief           Dropdown object structure
 */
typedef struct GUI_TEXTVIEW_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */

    GUI_TEXTVIEW_VALIGN_t VAlign;           /*!< Vertical text align */
    GUI_TEXTVIEW_HALIGN_t HAlign;           /*!< Horizontal text align */
    uint8_t Flags;                          /*!< Widget flags */
} GUI_TEXTVIEW_t;
#endif /* defined(GUI_INTERNAL) || defined(DOXYGEN) */

/**
 * \brief           Create new text view widget
 * \param[in]       id: Widget unique ID to use for identity for callback processing
 * \param[in]       x: Widget X position relative to parent widget
 * \param[in]       y: Widget Y position relative to parent widget
 * \param[in]       width: Widget width in units of pixels
 * \param[in]       height: Widget height in uints of pixels
 * \param[in]       parent: Parent widget handle. Set to NULL to use current active parent widget
 * \param[in]       cb: Pointer to \ref GUI_WIDGET_CALLBACK_t callback function. Set to NULL to use default widget callback
 * \param[in]       flags: Flags for widget creation
 * \retval          > 0: \ref GUI_HANDLE_p object of created widget
 * \retval          0: Widget creation failed
 */
GUI_HANDLE_p gui_textview_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);

/**
 * \brief           Set color to specific part of widget
 * \param[in,out]   h: Widget handle
 * \param[in]       index: Color index. This parameter can be a value of \ref GUI_TEXTVIEW_COLOR_t enumeration
 * \param[in]       color: Color value
 * \retval          1: Color was set ok
 * \retval          0: Color was not set
 */
uint8_t gui_textview_setcolor(GUI_HANDLE_p h, GUI_TEXTVIEW_COLOR_t index, GUI_Color_t color);

/**
 * \brief           Set vertical align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align. This parameter can be a value of \ref GUI_TEXTVIEW_VALIGN_t enumeration
 * \retval          1: Align was set ok
 * \retval          0: Align was not set
 */
uint8_t gui_textview_setvalign(GUI_HANDLE_p h, GUI_TEXTVIEW_VALIGN_t align);

/**
 * \brief           Set horizontal align for text inside text box
 * \param[in,out]   h: Widget handle
 * \param[in]       align: Vertical align. This parameter can be a value of \ref GUI_TEXTVIEW_HALIGN_t enumeration
 * \retval          1: Align was set ok
 * \retval          0: Align was not set
 */
uint8_t gui_textview_sethalign(GUI_HANDLE_p h, GUI_TEXTVIEW_HALIGN_t align);
    
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

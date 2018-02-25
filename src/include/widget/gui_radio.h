/**	
 * \file            gui_radio.h
 * \brief           Radio box widget
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
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
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#ifndef __GUI_RADIO_H
#define __GUI_RADIO_H

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
 * \defgroup        GUI_RADIO Radio box
 * \brief           Radio box widget
 * \{
 */
    
/**
 * \brief           List of radio colors
 */
typedef enum GUI_RADIO_COLOR_t {
    GUI_RADIO_COLOR_BG = 0x00,              /*!< Background color index */
    GUI_RADIO_COLOR_FG,                     /*!< Foreground color index */
    GUI_RADIO_COLOR_BORDER,                 /*!< Border color index */
    GUI_RADIO_COLOR_DISABLED_BG,            /*!< Background color index when disabled */
} GUI_RADIO_COLOR_t;
    
#if defined(GUI_INTERNAL) || __DOXYGEN__
    
#define GUI_FLAG_RADIO_CHECKED              0x01    /*!< Indicates radio is currently checked */
#define GUI_FLAG_RADIO_DISABLED             0x02    /*!< Indicates radio is currently disabled */

/**
 * \brief           Radio object structure
 */
typedef struct GUI_RADIO_t {
    GUI_HANDLE C;                           /*!< GUI handle object, must always be first on list */
    
    uint8_t GroupId;                        /*!< Group ID for radio box */
    uint32_t Value;                         /*!< Single radio value when selected */
    uint32_t SelectedValue;                 /*!< Currently selected value in radio group. 
                                                    All Widgets in the same group share the same value in this field all the time */
    uint8_t Flags;                          /*!< Flags for checkbox */
} GUI_RADIO_t;
#endif /* defined(GUI_INTERNAL) || __DOXYGEN__ */

GUI_HANDLE_p gui_radio_create(GUI_ID_t id, float x, float y, float width, float height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags);
uint8_t gui_radio_setcolor(GUI_HANDLE_p h, GUI_RADIO_COLOR_t index, GUI_Color_t color);
uint8_t gui_radio_setgroup(GUI_HANDLE_p h, uint8_t groupId);
uint8_t gui_radio_getgroup(GUI_HANDLE_p h);
uint8_t gui_radio_setvalue(GUI_HANDLE_p h, uint32_t value);
uint32_t gui_radio_getvalue(GUI_HANDLE_p h);
uint32_t gui_radio_getselectedvalue(GUI_HANDLE_p h);
uint8_t gui_radio_setdisabled(GUI_HANDLE_p h, uint8_t disabled);
uint8_t gui_radio_isdisabled(GUI_HANDLE_p h);
uint8_t gui_radio_setselected(GUI_HANDLE_p h);
    
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

#endif /* __GUI_RADIO_H */

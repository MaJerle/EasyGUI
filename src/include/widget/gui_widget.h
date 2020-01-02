/**	
 * \file            gui_widget.h
 * \brief           Widget specific shared functions
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
#ifndef GUI_HDR_WIDGET_H
#define GUI_HDR_WIDGET_H

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
 * \{
 */
#include "gui/gui.h"
#include "gui/gui_draw.h"

/**
 * \defgroup        GUI_WIDGET_ID_Values Predefined ID values
 * \brief           Macros for fast ID setup
 * \{
 */  
    
#define GUI_ID_WINDOW_BASE          ((gui_id_t)0x0000)                          /*!< Window base ID */
#define GUI_ID_KEYBOARD_BASE        ((gui_id_t)(GUI_ID_WINDOW_BASE + 0x1000))   /*!< Keyboard based ID */

#define GUI_ID_USER                 ((gui_id_t)(0x10000))                       /*!< Start number of user based ID values for widgets */

#define GUI_WIDGET_ZINDEX_MAX       ((int32_t)(0x7FFFFFFF))                     /*!< Maximal allowed z-index value */
#define GUI_WIDGET_ZINDEX_MIN       ((int32_t)(0x80000000))                     /*!< Maximal allowed z-index value */
    
/**
 * \}
 */
 
/**
 * \defgroup        GUI_WIDGET_CREATE_FLAGS Widget create flags
 * \brief           A list of flags supported for widget creation
 *
 * List of flags used on \ref gui_widget_create function when creating new widget
 * 
 * \{
 */
#define GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP       0x01    /*!< Indicates parent object of widget should be desktop window */
#define GUI_FLAG_WIDGET_CREATE_NO_PARENT            0x02    /*!< No parent widget */
#define GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE    0x04    /*!< Ignore invalidate on widget creation */

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_PRIVATE Private functions
 * \brief           Private widget functions and enumerations.
 *
 * \note            Since they are not thread safe, they can only be used when inside GUI library.
 *                  It means they can be used in custom widget implementations where thread safety is guaranteed
 *
 * \{
 */
 
/**
 * \brief           Widget footprint value
 * \hideinitializer
 */
#define GUI_WIDGET_FOOTPRINT                        0x00ACCE55
 
/**
 * \brief           Checks if inserted pointer is valid widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Input pointer to test for widget
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_iswidget(h)                     (__GH(h) != NULL && __GH(h)->footprint == GUI_WIDGET_FOOTPRINT)

/**
 * \brief           Get widget relative X position according to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Relative X according to parent widget
 * \hideinitializer
 */
#define guii_widget_getrelativex(h)                 GUI_DIM((gui_widget_isexpanded(h) ? 0 : \
                                                        (guii_widget_getflag(__GH(h), GUI_FLAG_XPOS_PERCENT) ? (gui_dim_t)((float)GUI_ROUND(__GH(h)->x * guii_widget_getparentinnerwidth(__GH(h))) / 100.0f) : __GH(h)->x) \
                                                    ))

/**
 * \brief           Get widget relative Y position according to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Relative Y according to parent widget
 * \hideinitializer
 */
#define guii_widget_getrelativey(h)                 GUI_DIM(gui_widget_isexpanded(__GH(h)) ? 0 : \
                                                        (guii_widget_getflag(__GH(h), GUI_FLAG_YPOS_PERCENT) ? (gui_dim_t)((float)GUI_ROUND(__GH(h)->y * guii_widget_getparentinnerheight(__GH(h))) / 100.0f) : __GH(h)->y) \
                                                    )

/**
 * \brief           Get widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \return          `non-zero` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_getflag(h, flag)                (__GH(h)->flags & (flag))

/**
 * \brief           Get widget core flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \param[in]       flag: Flag(s) to check
 * \return          `non-zero` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_getcoreflag(h, flag)            (__GH(h)->widget->flags & (flag))

/**
 * \brief           Set widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \param[in]       flag: Flag(s) to set
 * \hideinitializer
 */
#define guii_widget_setflag(h, flag)                (__GH(h)->flags |= (flag))

/**
 * \brief           Clear widget flag(s)
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \param[in]       flag: Flag(s) to clear
 * \hideinitializer
 */
#define guii_widget_clrflag(h, flag)                (__GH(h)->flags &= ~(flag))

/**
 * \brief           Get pointer to parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Pointer to parent widget
 * \hideinitializer
 */
#define guii_widget_getparent(h)                    (__GH(h) != NULL ? __GH(h)->parent : NULL)

/**
 * \brief           Check if widget has parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_hasparent(h)                    (__GH(h) != NULL && __GH(h)->parent != NULL)

/**
 * \brief           Process widget callback with command, parameters and result pointers
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \param[in]       cmd: Callback command. This parameter can be a value of \ref gui_widget_evt_t enumeration
 * \param[in]       param: Pointer to parameters if any for this command
 * \param[out]      result: Pointer to result pointer where calback can store result
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_callback(h, cmd, param, result) (__GH(h)->callback != NULL ? __GH(h)->callback(h, cmd, param, result) : __GH(h)->widget->callback(h, cmd, param, result))

/**
 * \brief           Get widget colors from list of colors
 *                  It takes colors from allocated memory if exists or from default widget setup for default
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \param[in]       index: Color index from color array for specific widget
 * \retval          Color value
 * \hideinitializer
 */
#define guii_widget_getcolor(h, index)              (__GH(h)->colors != NULL ? __GH(h)->colors[(uint8_t)(index)] : (__GH(h)->widget->colors != NULL ? __GH(h)->widget->colors[(uint8_t)(index)] : GUI_COLOR_BLACK))

/**
 * \brief           Returns width of parent element. If parent does not exists, it returns LCD width
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Parent width in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentwidth(h)               GUI_DIM((guii_widget_hasparent(h) ? gui_widget_getwidth(guii_widget_getparent(h)) : GUI.lcd.width))

/**
 * \brief           Returns height of parent element. If parent does not exists, it returns LCD height
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Parent height in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentheight(h)              GUI_DIM((guii_widget_hasparent(h) ? gui_widget_getheight(guii_widget_getparent(h)) : GUI.lcd.height))

/**
 * \brief           Returns inner width of parent element. If parent does not exists, it returns LCD width
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            It returns inner width = total width - padding left - padding right
 * \param[in]       h: Widget handle
 * \retval          Parent width in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentinnerwidth(h)          GUI_DIM((guii_widget_hasparent(h) ? gui_widget_getinnerwidth(guii_widget_getparent(h)) : GUI.lcd.width))

/**
 * \brief           Returns inner height of parent element. If parent does not exists, it returns LCD height
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \note            It returns inner height = total height - padding top - padding bottom
 * \param[in]       h: Widget handle
 * \retval          Parent height in units of pixels
 * \hideinitializer
 */
#define guii_widget_getparentinnerheight(h)         GUI_DIM((guii_widget_hasparent(h) ? gui_widget_getinnerheight(guii_widget_getparent(h)) : GUI.lcd.height))

/**
 * \brief           Check if widget is visible in any way, either with transparency or hidden flag
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#if GUI_CFG_USE_ALPHA
#define guii_widget_isvisible(h)                    (!guii_widget_getflag(__GH(h), GUI_FLAG_HIDDEN) && __GH(h)->alpha)
#else /* GUI_CFG_USE_ALPHA */
#define guii_widget_isvisible(h)                    (!guii_widget_getflag(h, GUI_FLAG_HIDDEN))
#endif /* !GUI_CFG_USE_ALPHA */

/**
 * \brief           Check if widget is hidden
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_ishidden(h)                     (!guii_widget_isvisible(__GH(h)))

/**
 * \brief           Check if widget allows children widgets
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_allowchildren(h)                (!!guii_widget_getcoreflag(h, GUI_FLAG_WIDGET_ALLOW_CHILDREN))

/**
 * \brief           Check if widget has any children widgets
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_haschildren(h)                  (guii_widget_allowchildren(h) && gui_linkedlist_hasentries(&h->root_list))

/**
 * \brief           Check if widget is base for dialog
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 * \hideinitializer
 */
#define guii_widget_isdialogbase(h)                 (!!(guii_widget_getcoreflag(h, GUI_FLAG_WIDGET_DIALOG_BASE) || guii_widget_getflag(h, GUI_FLAG_WIDGET_DIALOG_BASE)))

/**
 * \brief           Checks if widget handle is currently in focus
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Status whether widget is in focus or not
 * \hideinitializer
 */
#define guii_widget_isfocused(h)                    (!!guii_widget_getflag(h, GUI_FLAG_FOCUS))

/**
 * \brief           Checks if widget handle is currently active
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \retval          Status whether widget is active or not
 * \hideinitializer
 */
#define guii_widget_isactive(h)                     (!!guii_widget_getflag(h, GUI_FLAG_ACTIVE))

/**
 * \brief           Check is widget has alpha less than maximum
 * \note            Check if widget is visible and alpha is not set to 1 (full view)
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle
 * \return          `1` on success, `0` otherwise
 */
#define guii_widget_hasalpha(h)                     (guii_widget_isvisible(h) && gui_widget_getalpha(h) < 0xFF)

uint8_t         guii_widget_processtextkey(gui_handle_p h, guii_keyboard_data_t* key);

uint8_t         guii_widget_setparam(gui_handle_p h, uint16_t cfg, const void* data, uint8_t invalidate, uint8_t invalidateparent);
uint8_t         guii_widget_getparam(gui_handle_p h, uint16_t cfg, void* data);
gui_dim_t       guii_widget_getparentabsolutex(gui_handle_p h);
gui_dim_t       guii_widget_getparentabsolutey(gui_handle_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_SHARED Shared functions
 * \brief           Shared public function which should not be directly accessed by user in API
 * \{
 */

void*           gui_widget_create(const gui_widget_t* widget, gui_id_t id, float x, float y, float width, float height, gui_handle_p parent, gui_widget_evt_fn evt_fn, uint16_t flags);
uint8_t         gui_widget_setcolor(gui_handle_p h, uint8_t index, gui_color_t color);
uint8_t         gui_widget_setstyle(gui_handle_p h, const gui_style_p style);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_TEXT Text management
 * \brief           Text and font management functions
 * \{
 */

uint32_t            gui_widget_alloctextmemory(gui_handle_p h, uint32_t size);
uint8_t             gui_widget_freetextmemory(gui_handle_p h);
uint8_t             gui_widget_settext(gui_handle_p h, const gui_char* text);
const gui_char *    gui_widget_gettext(gui_handle_p h);
const gui_char *    gui_widget_gettextcopy(gui_handle_p h, gui_char* dst, uint32_t len);
uint8_t             gui_widget_setfont(gui_handle_p h, const gui_font_t* font);
const gui_font_t *  gui_widget_getfont(gui_handle_p h);
uint8_t             gui_widget_setfontdefault(const gui_font_t* font);
uint8_t             gui_widget_isfontandtextset(gui_handle_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_SIZE Size management
 * \brief           Size management functions
 * \{
 */

uint8_t         gui_widget_setsize(gui_handle_p h, gui_dim_t width, gui_dim_t height);
uint8_t         gui_widget_setsizepercent(gui_handle_p h, float width, float height);
uint8_t         gui_widget_setsizeoriginal(gui_handle_p h, float width, float height);
uint8_t         gui_widget_setwidth(gui_handle_p h, gui_dim_t width);
uint8_t         gui_widget_setwidthpercent(gui_handle_p h, float width);
uint8_t         gui_widget_setwidthoriginal(gui_handle_p h, float width);
uint8_t         gui_widget_setheight(gui_handle_p h, gui_dim_t height);
uint8_t         gui_widget_setheightpercent(gui_handle_p h, float height);
uint8_t         gui_widget_setheightoriginal(gui_handle_p h, float height);

gui_dim_t       gui_widget_getwidth(gui_handle_p h);
gui_dim_t       gui_widget_getheight(gui_handle_p h);
gui_dim_t       gui_widget_getinnerwidth(gui_handle_p h);
gui_dim_t       gui_widget_getinnerheight(gui_handle_p h);
float           gui_widget_getwidthpercent(gui_handle_p h);
float           gui_widget_getheightpercent(gui_handle_p h);
float           gui_widget_getwidthoriginal(gui_handle_p h, uint8_t* is_percent);
float           gui_widget_getheightoriginal(gui_handle_p h, uint8_t* is_percent);

static inline float
gui_widget_getwidthex(gui_handle_p h, uint8_t in_percent) {
    return GUI_FLOAT(in_percent ? gui_widget_getwidthpercent(h) : gui_widget_getwidth(h));
}

static inline float
gui_widget_getheightex(gui_handle_p h, uint8_t in_percent) {
    return GUI_FLOAT(in_percent ? gui_widget_getheightpercent(h) : gui_widget_getheight(h));
}

uint8_t         gui_widget_setexpanded(gui_handle_p h, uint8_t state);
uint8_t         gui_widget_toggleexpanded(gui_handle_p h);
uint8_t         gui_widget_isexpanded(gui_handle_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_POSITION Position management
 * \brief           Position management functions
 * \{
 */

gui_dim_t       gui_widget_getabsolutex(gui_handle_p h);
gui_dim_t       gui_widget_getabsolutey(gui_handle_p h);

float           gui_widget_getxpositionoriginal(gui_handle_p h, uint8_t* is_percent);
float           gui_widget_getypositionoriginal(gui_handle_p h, uint8_t* is_percent);

uint8_t         gui_widget_setposition(gui_handle_p h, gui_dim_t x, gui_dim_t y);
uint8_t         gui_widget_setpositionpercent(gui_handle_p h, float x, float y);
uint8_t         gui_widget_setpositionoriginal(gui_handle_p h, float x, float y);
uint8_t         gui_widget_setxposition(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setxpositionpercent(gui_handle_p h, float x);
uint8_t         gui_widget_setxpositionoriginal(gui_handle_p h, float x);
uint8_t         gui_widget_setyposition(gui_handle_p h, gui_dim_t y);
uint8_t         gui_widget_setypositionpercent(gui_handle_p h, float y);
uint8_t         gui_widget_setypositionoriginal(gui_handle_p h, float y);

gui_dim_t       gui_widget_getxposition(gui_handle_p h);
gui_dim_t       gui_widget_getyposition(gui_handle_p h);

uint8_t         gui_widget_setscrollx(gui_handle_p h, gui_dim_t scroll);
uint8_t         gui_widget_setscrolly(gui_handle_p h, gui_dim_t scroll);
uint8_t         gui_widget_incscrollx(gui_handle_p h, gui_dim_t scroll);
uint8_t         gui_widget_incscrolly(gui_handle_p h, gui_dim_t scroll);
gui_dim_t       gui_widget_getscrollx(gui_handle_p h);
gui_dim_t       gui_widget_getscrolly(gui_handle_p h);

/**
 * \}
 */
 
/**
 * \defgroup        GUI_WIDGET_VISIBILITY Visibility management
 * \brief           Visibility management functions
 * \{
 */

uint8_t         gui_widget_show(gui_handle_p h);
uint8_t         gui_widget_hide(gui_handle_p h);
uint8_t         gui_widget_hidechildren(gui_handle_p h);
uint8_t         gui_widget_putonfront(gui_handle_p h, uint8_t focus);
uint8_t         gui_widget_getalpha(gui_handle_p h);
uint8_t         gui_widget_setalpha(gui_handle_p h, uint8_t alpha);
 
/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_MISC Miscellaneous
 * \brief           Miscellaneous functions
 * \{
 */

uint8_t         gui_widget_invalidate(gui_handle_p h);
uint8_t         gui_widget_force_invalidate(gui_handle_p h);
uint8_t         gui_widget_invalidatewithparent(gui_handle_p h);
uint8_t         gui_widget_setignoreinvalidate(gui_handle_p h, uint8_t en, uint8_t invalidate);
uint8_t         gui_widget_setinvalidatewithparent(gui_handle_p h, uint8_t value);
uint8_t         gui_widget_setuserdata(gui_handle_p h, void* const data);
void *          gui_widget_getuserdata(gui_handle_p h);
uint8_t         gui_widget_ischildof(gui_handle_p h, gui_handle_p parent);
uint8_t         gui_widget_incselection(gui_handle_p h, int16_t dir);
uint8_t         gui_widget_setfocus(gui_handle_p h);
uint8_t         gui_widget_setzindex(gui_handle_p h, int32_t zindex);
int32_t         gui_widget_getzindex(gui_handle_p h);
gui_handle_p    gui_widget_getparent(gui_handle_p h);
gui_id_t        gui_widget_getid(gui_handle_p h);
gui_handle_p    gui_widget_getbyid(gui_id_t id);
gui_handle_p    gui_widget_getbyid_ex(gui_id_t id, gui_handle_p parent, uint8_t deep);
uint8_t         gui_widget_remove(gui_handle_p* h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_PADDING Padding
 * \brief           Padding related functions
 * \{
 */
 
uint8_t         gui_widget_setpaddingtop(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingright(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingbottom(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingleft(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingtopbottom(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpaddingleftright(gui_handle_p h, gui_dim_t x);
uint8_t         gui_widget_setpadding(gui_handle_p h, gui_dim_t x);

gui_dim_t       gui_widget_getpaddingtop(gui_handle_p h);
gui_dim_t       gui_widget_getpaddingright(gui_handle_p h);
gui_dim_t       gui_widget_getpaddingbottom(gui_handle_p h);
gui_dim_t       gui_widget_getpaddingleft(gui_handle_p h);

/**
 * \}
 */

/**
 * \defgroup        GUI_WIDGET_CALLBACK Callback processing
 * \brief           Callback based functions
 * \{
 */

uint8_t         gui_widget_processdefaultcallback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);
uint8_t         gui_widget_setcallback(gui_handle_p h, gui_widget_evt_fn callback);
uint8_t         gui_widget_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);

/**
 * \}
 */
 
#if defined(GUI_INTERNAL) && !__DOXYGEN__
//Strictly private functions by GUI
//Widget initialization purpose
void guii_widget_init(void);

//Clipping regions
uint8_t guii_widget_isinsideclippingregion(gui_handle_p h, uint8_t check_sib_cover);

//Move widget down and all its parents with it
void guii_widget_movedowntree(gui_handle_p h);

void guii_widget_focus_clear(void);
void guii_widget_focus_set(gui_handle_p h);
void guii_widget_active_clear(void);
void guii_widget_active_set(gui_handle_p h);

//Execute actual widget remove process
uint8_t guii_widget_executeremove(void);
#endif /* !__DOXYGEN__ */

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

#endif /* GUI_HDR_WIDGET_H */

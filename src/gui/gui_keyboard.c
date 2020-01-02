/**	
 * \file            gui_keyboard.c
 * \brief           Software keyboard widget
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
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui_keyboard.h"
#include "widget/gui_container.h"
#include "widget/gui_button.h"

#if !__DOXYGEN__

typedef struct {
    uint32_t c;                                     /*!< Character to print */
    uint32_t cs;                                    /*!< Character on shift */
    float x;                                        /*!< Start X position */
    float w;                                        /*!< Character width */
    uint8_t s;                                      /*!< Special function */
} key_btn_t;

typedef struct {
    gui_dim_t x_offset;                             /*!< Starting X offset */
    const key_btn_t* btns;                          /*!< Pointer to buttons structure */
    size_t btns_count;                              /*!< Number of buttons in array */
} key_row_t;

typedef struct {
    const key_row_t* rows;                          /*!< Pointer to rows objects */
    size_t rows_count;                              /*!< Number of rows */
    gui_id_t id;                                    /*!< Layout ID */
} key_layout_t;

typedef struct {
    uint8_t is_shift;                               /*!< Status indicating shift mode is enabled */
    gui_handle_p handle;                            /*!< Pointer to keyboard handle */
    gui_handle_p main_layout_handle;                /*!< Pointer to main keyboard layout */
    gui_handle_p target;                            /*!< Target key */
    
    const gui_font_t* font;                         /*!< Pointer to used font */
    const gui_font_t* default_font;                 /*!< Pointer to default font */
    
    uint8_t action;                                 /*!< Kbd show/hide action */
    uint8_t action_value;                           /*!< Action custom value */
} key_info_t;

#define SPECIAL_123                     ((uint32_t)0x01)
#define SPECIAL_ABC                     ((uint32_t)0x02)
#define SPECIAL_CALC                    ((uint32_t)0x03)
#define SPECIAL_SHIFT                   ((uint32_t)0x04)
#define SPECIAL_BACKSPACE               ((uint32_t)0x05)
#define SPECIAL_ENTER                   ((uint32_t)0x06)
#define SPECIAL_HIDE                    ((uint32_t)0x07)

#define GUI_ID_KEYBOARD_LAYOUT_ABC      (GUI_ID_KEYBOARD_BASE + 0x01UL)
#define GUI_ID_KEYBOARD_LAYOUT_123      (GUI_ID_KEYBOARD_BASE + 0x02UL)
#define GUI_ID_KEYBOARD_LAYOUT_CALC     (GUI_ID_KEYBOARD_BASE + 0x03UL)

#define SHIFT_CLEARED                   0x00
#define SHIFT_NORMAL                    0x01
#define SHIFT_UPPERCASE                 0x02

#define ACTION_HIDE                     0x01
#define ACTION_SHOW                     0x02

#define KEY_ROW(c_v, cs_v, x_v, w_v, s_v)         { .c = ((uint32_t)(c_v)), .cs = ((uint32_t)(cs_v)), .x = x_v, .w = w_v, .s = (s_v) }

/***************************/
/*   Layout 1 descriptors  */
/***************************/
static const key_btn_t
buttons_l1_r1[] = {
    KEY_ROW('q', 'Q', 0.5f,  9.0f, 0),
    KEY_ROW('w', 'W', 10.5f, 9.0f, 0),
    KEY_ROW('e', 'E', 20.5f, 9.0f, 0),
    KEY_ROW('r', 'R', 30.5f, 9.0f, 0),
    KEY_ROW('t', 'T', 40.5f, 9.0f, 0),
    KEY_ROW('z', 'Z', 50.5f, 9.0f, 0),
    KEY_ROW('u', 'U', 60.5f, 9.0f, 0),
    KEY_ROW('i', 'I', 70.5f, 9.0f, 0),
    KEY_ROW('o', 'O', 80.5f, 9.0f, 0),
    KEY_ROW('p', 'P', 90.5f, 9.0f, 0),
};

static const key_btn_t
buttons_l1_r2[] = {
    KEY_ROW('a', 'A', 5.5f, 9.0f, 0),
    KEY_ROW('s', 'S', 15.5f, 9.0f, 0),
    KEY_ROW('d', 'D', 25.5f, 9.0f, 0),
    KEY_ROW('f', 'F', 35.5f, 9.0f, 0),
    KEY_ROW('g', 'G', 45.5f, 9.0f, 0),
    KEY_ROW('h', 'H', 55.5f, 9.0f, 0),
    KEY_ROW('j', 'J', 65.5f, 9.0f, 0),
    KEY_ROW('k', 'K', 75.5f, 9.0f, 0),
    KEY_ROW('l', 'L', 85.5f, 9.0f, 0),
};

static const key_btn_t
buttons_l1_r3[] = {
    KEY_ROW(0, 0, 0.5f, 14.0f, SPECIAL_SHIFT),
    KEY_ROW('y', 'Y', 15.5f, 9.0f, 0),
    KEY_ROW('x', 'X', 25.5f, 9.0f, 0),
    KEY_ROW('c', 'C', 35.5f, 9.0f, 0),
    KEY_ROW('v', 'V', 45.5f, 9.0f, 0),
    KEY_ROW('b', 'B', 55.5f, 9.0f, 0),
    KEY_ROW('n', 'N', 65.5f, 9.0f, 0),
    KEY_ROW('m', 'M', 75.5f, 9.0f, 0),
    KEY_ROW(0, 0, 85.5f, 14.0f, SPECIAL_BACKSPACE),
};

static const key_btn_t
buttons_l1_r4[] = {
    KEY_ROW(0, 0, 0.5f, 9.0f, SPECIAL_123),
    KEY_ROW(' ', 0, 10.5f, 59.0f, 0),
    KEY_ROW('.', 0, 70.5f, 9.0f, 0),
    KEY_ROW(0, 0, 80.5f, 9.0f, SPECIAL_ENTER),
    KEY_ROW(0, 0, 90.5f, 9.0f, SPECIAL_HIDE),
};

static const key_row_t
keyboard_rows_l1[] = {
    {.x_offset = 1, .btns = buttons_l1_r1, .btns_count = GUI_COUNT_OF(buttons_l1_r1)},
    {.x_offset = 1, .btns = buttons_l1_r2, .btns_count = GUI_COUNT_OF(buttons_l1_r2)},
    {.x_offset = 1, .btns = buttons_l1_r3, .btns_count = GUI_COUNT_OF(buttons_l1_r3)},
    {.x_offset = 1, .btns = buttons_l1_r4, .btns_count = GUI_COUNT_OF(buttons_l1_r4)},
};
                
/***************************/
/*   Layout 2 descriptors  */
/***************************/
static const key_btn_t
buttons_l2_r1[] = {
    KEY_ROW('1', 0, 0.5f, 9.0f, 0),
    KEY_ROW('2', 0, 10.5f, 9.0f, 0),
    KEY_ROW('3', 0, 20.5f, 9.0f, 0),
    KEY_ROW('4', 0, 30.5f, 9.0f, 0),
    KEY_ROW('5', 0, 40.5f, 9.0f, 0),
    KEY_ROW('6', 0, 50.5f, 9.0f, 0),
    KEY_ROW('7', 0, 60.5f, 9.0f, 0),
    KEY_ROW('8', 0, 70.5f, 9.0f, 0),
    KEY_ROW('9', 0, 80.5f, 9.0f, 0),
    KEY_ROW('0', 0, 90.5f, 9.0f, 0),
};

static const key_btn_t
buttons_l2_r2[] = {
    KEY_ROW('-', 0, 0.5f, 9.0f, 0),
    KEY_ROW('/', 0, 10.5f, 9.0f, 0),
    KEY_ROW(':', 0, 20.5f, 9.0f, 0),
    KEY_ROW(';', 0, 30.5f, 9.0f, 0),
    KEY_ROW('(', 0, 40.5f, 9.0f, 0),
    KEY_ROW(')', 0, 50.5f, 9.0f, 0),
    KEY_ROW('$', 0, 60.5f, 9.0f, 0),
    KEY_ROW('&', 0, 70.5f, 9.0f, 0),
    KEY_ROW('@', 0, 80.5f, 9.0f, 0),
    KEY_ROW('"', 0, 90.5f, 9.0f, 0),
};

static const key_btn_t
buttons_l2_r3[] = {
    KEY_ROW(0, 0, 0.5f, 14.0f, SPECIAL_CALC),
    KEY_ROW('.', 0, 15.5f, 13.0f, 0),
    KEY_ROW(',', 0, 29.5f, 13.0f, 0),
    KEY_ROW('?', 0, 43.5f, 13.0f, 0),
    KEY_ROW('!', 0, 57.5f, 13.0f, 0),
    KEY_ROW('\'', 0, 71.5f, 13.0f, 0),
    KEY_ROW(0, 0, 85.5f, 14.0f, SPECIAL_BACKSPACE),
};

static const key_btn_t
buttons_l2_r4[] = {
    {.c = 0, .x = 0.5, .w = 9, .s = ((uint32_t)SPECIAL_ABC)},
    {.c = ((uint32_t)' '), .x = 10.5, .w = 59},
    {.c = ((uint32_t)'.'), .x = 70.5, .w = 9},
    {.c = 0, .x = 80.5, .w = 9, .s = ((uint32_t)SPECIAL_ENTER)},
    {.c = 0, .x = 90.5, .w = 9, .s = ((uint32_t)SPECIAL_HIDE)},
};

static const key_row_t
keyboard_rows_l2[] = {
    {.x_offset = 1, .btns = buttons_l2_r1, .btns_count = GUI_COUNT_OF(buttons_l2_r1)},
    {.x_offset = 1, .btns = buttons_l2_r2, .btns_count = GUI_COUNT_OF(buttons_l2_r2)},
    {.x_offset = 1, .btns = buttons_l2_r3, .btns_count = GUI_COUNT_OF(buttons_l2_r3)},
    {.x_offset = 1, .btns = buttons_l2_r4, .btns_count = GUI_COUNT_OF(buttons_l2_r4)},
};

/***************************/
/*   Layout 2 descriptors  */
/***************************/
static const key_btn_t
buttons_l3_r1[] = {
    {.c = ((uint32_t)'['), .x = 0.5, .w = 9},
    {.c = ((uint32_t)']'), .x = 10.5, .w = 9},
    {.c = ((uint32_t)'{'), .x = 20.5, .w = 9},
    {.c = ((uint32_t)'}'), .x = 30.5, .w = 9},
    {.c = ((uint32_t)'#'), .x = 40.5, .w = 9},
    {.c = ((uint32_t)'%'), .x = 50.5, .w = 9},
    {.c = ((uint32_t)'^'), .x = 60.5, .w = 9},
    {.c = ((uint32_t)'*'), .x = 70.5, .w = 9},
    {.c = ((uint32_t)'+'), .x = 80.5, .w = 9},
    {.c = ((uint32_t)'='), .x = 90.5, .w = 9}
};

static const key_btn_t
buttons_l3_r2[] = {
    {.c = ((uint32_t)'_'), .x = 0.5, .w = 9},
    {.c = ((uint32_t)'\\'), .x = 10.5, .w = 9},
    {.c = ((uint32_t)'|'), .x = 20.5, .w = 9},
    {.c = ((uint32_t)'~'), .x = 30.5, .w = 9},
    {.c = ((uint32_t)'<'), .x = 40.5, .w = 9},
    {.c = ((uint32_t)'>'), .x = 50.5, .w = 9},
    {.c = ((uint32_t)'$'), .x = 60.5, .w = 9},
    {.c = ((uint32_t)':'), .x = 70.5, .w = 9},
    {.c = ((uint32_t)';'), .x = 80.5, .w = 9},
    {.c = ((uint32_t)'-'), .x = 90.5, .w = 9}
};

static const key_btn_t
buttons_l3_r3[] = {
    {.c = 0, .x = 0.5, .w = 14, .s = SPECIAL_123},
    {.c = ((uint32_t)'.'), .x = 15.5, .w = 13},
    {.c = ((uint32_t)','), .x = 29.5, .w = 13},
    {.c = ((uint32_t)'?'), .x = 43.5, .w = 13},
    {.c = ((uint32_t)'!'), .x = 57.5, .w = 13},
    {.c = ((uint32_t)'\''), .x = 71.5, .w = 13},
    {.c = 0, .x = 85.5, .w = 14, .s = SPECIAL_BACKSPACE},
};

static const key_btn_t
buttons_l3_r4[] = {
    {.c = 0, .x = 0.5, .w = 9, .s = ((uint32_t)SPECIAL_ABC)},
    {.c = ((uint32_t)' '), .x = 10.5, .w = 59},
    {.c = ((uint32_t)'.'), .x = 70.5, .w = 9},
    {.c = 0, .x = 80.5, .w = 9, .s = ((uint32_t)SPECIAL_ENTER)},
    {.c = 0, .x = 90.5, .w = 9, .s = ((uint32_t)SPECIAL_HIDE)},
};

static const key_row_t
keyboard_rows_l3[] = {
    {.x_offset = 1, .btns = buttons_l3_r1, .btns_count = GUI_COUNT_OF(buttons_l3_r1)},
    {.x_offset = 1, .btns = buttons_l3_r2, .btns_count = GUI_COUNT_OF(buttons_l3_r2)},
    {.x_offset = 1, .btns = buttons_l3_r3, .btns_count = GUI_COUNT_OF(buttons_l3_r3)},
    {.x_offset = 1, .btns = buttons_l3_r4, .btns_count = GUI_COUNT_OF(buttons_l3_r4)},
};

/************************/
/*  Layouts descriptor  */
/************************/
static const key_layout_t
layouts[] = {
    {.id = GUI_ID_KEYBOARD_LAYOUT_ABC, .rows = keyboard_rows_l1, .rows_count = GUI_COUNT_OF(keyboard_rows_l1)},
    {.id = GUI_ID_KEYBOARD_LAYOUT_123, .rows = keyboard_rows_l2, .rows_count = GUI_COUNT_OF(keyboard_rows_l2)},
    {.id = GUI_ID_KEYBOARD_LAYOUT_CALC, .rows = keyboard_rows_l3, .rows_count = GUI_COUNT_OF(keyboard_rows_l3)},
};

static key_info_t
keyboard = {
    .action_value = 10              /* Set action value to max */
};   

#define SHIFT_DISABLE()     if (keyboard.is_shift) {        \
    keyboard.is_shift = 0;                                  \
    gui_widget_invalidate(keyboard.main_layout_handle);     \
}

#define SHIFT_ENABLE(mode)  do {                            \
    if (!keyboard.is_shift && (mode)) {                     \
        gui_widget_invalidate(keyboard.main_layout_handle); \
    }                                                       \
    keyboard.is_shift = (mode);                             \
} while (0)

#define SHIFT_TOGGLE()      do {                            \
    keyboard.is_shift = !keyboard.is_shift;                 \
    gui_widget_invalidate(keyboard.main_layout_handle);     \
} while (0)

#endif /* !__DOXYGEN__ */

/**
 * \brief           Keyboard button callback function
 */
static uint8_t
keyboard_btn_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    switch (evt) {
        case GUI_EVT_DRAW: {                        /* Draw button */
            gui_char str[10] = {0};
            const key_btn_t* kbtn;
            
            kbtn = (const key_btn_t *)gui_widget_getuserdata(h);/* Get user data */
            
            switch (kbtn->s) {                      /* Check if there is specal key */
                case SPECIAL_123: 
                    strcpy((char *)str, (const char *)_GT("123"));
                    break;
                case SPECIAL_ABC: 
                    strcpy((char *)str, (const char *)_GT("abc"));
                    break;
                case SPECIAL_CALC:
                    strcpy((char *)str, (const char *)_GT("#+="));
                    break;
                case SPECIAL_BACKSPACE: 
                    strcpy((char *)str, (const char *)_GT("Back"));
                    break;
                case SPECIAL_ENTER: 
                    strcpy((char *)str, (const char *)_GT("Ent"));
                    break;
                case SPECIAL_SHIFT: 
                    strcpy((char *)str, (const char *)_GT("Shift"));
                    break;
                case SPECIAL_HIDE: 
                    strcpy((char *)str, (const char *)_GT("Hide"));
                    break;
                default:
                    if (keyboard.is_shift && kbtn->cs) {    /* Character when shift is ON */
                        gui_string_unicode_encode(kbtn->cs, str);   /* Encode character to unicode */
                    } else {                        /* Character when shift is OFF */
                        gui_string_unicode_encode(kbtn->c, str);    /* Encode character to unicode */
                    }
                    break;
            }
                                                
            if (keyboard.font != NULL) {            /* Check if widget font is set */
                gui_widget_setfont(h, keyboard.font);   /* Set drawing font */
            } else {
                gui_widget_setfont(h, keyboard.default_font);   /* Set drawing font */
            }
            
            gui_widget_settext(h, str);             /* Temporary set text */
            gui_widget_processdefaultcallback(h, evt, param, result);  /* Process default callback with drawing */
            return 1;
        }
        case GUI_EVT_CLICK: {                        /* Handle pressed button */
            gui_handle_p tmp1, tmp2, tmp3;
            const key_btn_t* kbtn;
            uint32_t ch = 0;
            /* gui_keyboard_data_t kbd = {0}; */
            
            kbtn = gui_widget_getuserdata(h);       /* Get data from widget */
            if (kbtn->s) {                          /* Has button special function? */
                switch (kbtn->s) {                  /* Check special function */
                    case SPECIAL_123:
                    case SPECIAL_ABC: 
                    case SPECIAL_CALC: {            /* Special functions 123 or ABC */
                        tmp1 = gui_widget_getbyid(GUI_ID_KEYBOARD_LAYOUT_123);
                        tmp2 = gui_widget_getbyid(GUI_ID_KEYBOARD_LAYOUT_ABC);
                        tmp3 = gui_widget_getbyid(GUI_ID_KEYBOARD_LAYOUT_CALC);
                        
                        if (kbtn->s == SPECIAL_ABC) {
                            gui_widget_hide(tmp1);
                            gui_widget_show(tmp2);
                            gui_widget_hide(tmp3);
                        } else if (kbtn->s == SPECIAL_123) {
                            gui_widget_show(tmp1);
                            gui_widget_hide(tmp2);
                            gui_widget_hide(tmp3);
                        } else if (kbtn->s == SPECIAL_CALC) {
                            gui_widget_hide(tmp1);
                            gui_widget_hide(tmp2);
                            gui_widget_show(tmp3);
                        }
                        SHIFT_DISABLE();
                        break;
                    }
                    case SPECIAL_SHIFT: {
                        SHIFT_TOGGLE();
                        break;
                    }
                    case SPECIAL_BACKSPACE: {
                        ch = GUI_KEY_BACKSPACE;
                        break;
                    }
                    case SPECIAL_HIDE: {
                        gui_keyboard_hide();
                        break;
                    }
                }
            }
                                                    
            /* Check if we have to add key to input buffer */
            if (ch || !kbtn->s) {                   /* If character from special is set or normal key pressed */
                if (!ch) {                          /* Only if char not yet set */
                    if (keyboard.is_shift && kbtn->cs) {/* If shift mode enabled and character has shift mode character */
                        ch = kbtn->cs;              /* Use shift mode character */
                    } else {
                        ch = kbtn->c;               /* Use normal character */
                    }
                }
                
                /* Clear shift mode if necessary */
                if (keyboard.is_shift != SHIFT_UPPERCASE) {   /* When not in uppercase shift mode */
                    SHIFT_DISABLE();                /* Clear shift mode */
                }
                
                /************************************/
                /* Send character to focused widget */
                /************************************/
                if (keyboard.target != NULL) {
                    gui_evt_param_t param;
                    gui_evt_result_t result;
                    guii_keyboard_data_t key;

                    gui_string_unicode_encode(ch, key.kb.keys);/* Decode key */
                    GUI_EVT_PARAMTYPE_KEYBOARD(&param) = &key;
                    GUI_EVT_RESULTTYPE_KEYBOARD(&result) = keyCONTINUE;
                    guii_widget_callback(keyboard.target, GUI_EVT_KEYPRESS, &param, &result);

                    key.kb.keys[0] = 0;             /* Set key to 0 */
                    GUI_EVT_PARAMTYPE_KEYBOARD(&param) = &key;
                    GUI_EVT_RESULTTYPE_KEYBOARD(&result) = keyCONTINUE;
                    guii_widget_callback(keyboard.target, GUI_EVT_KEYPRESS, &param, &result);
                }
                //kbd.keys[0] = 0;                    /* Set key to 0 */
                //gui_input_keyadd(&kbd);             /* Add end key */
            }
            return 1;
        }
        case GUI_EVT_DBLCLICK: {
            const key_btn_t* kbtn;
            
            kbtn = gui_widget_getuserdata(h);
            switch (kbtn->s) {
                case SPECIAL_SHIFT: {
                    SHIFT_ENABLE(SHIFT_UPPERCASE);  /* Enable shift upper case mode */
                    return 1;
                }
            }
            return 0;
        }
        default:
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return gui_widget_processdefaultcallback(h, evt, param, result);   /* Process default callback */
    }
}

/* Callback for keyboard base widget */
static uint8_t
keyboard_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    switch (evt) {
        case GUI_EVT_INIT: {
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return gui_widget_processdefaultcallback(h, evt, param, result);/* Process default callback */
    }
}

/* Callback function for base element of keyboard */
static uint8_t
keyboard_base_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    switch (evt) {
        case GUI_EVT_PRE_INIT: {
            return 1;
        }
        case GUI_EVT_INIT: {                         /* When base element is initialized */
            gui_handle_p handle, handleLayout;
            size_t i, k, z;
            const key_layout_t* layout;
            const key_row_t* row;
            const key_btn_t* btn;
            
            /***************************/
            /*   Configure keyboard    */
            /***************************/
            gui_widget_setignoreinvalidate(h, 1, 0);
            gui_widget_setsizepercent(h, 100, 100);
            gui_widget_setpositionpercent(h, 0, 0);
            gui_widget_setzindex(h, GUI_WIDGET_ZINDEX_MAX);
            gui_widget_hide(h);
            gui_widget_setignoreinvalidate(h, 0, 1);
            
            keyboard.default_font = h->font;
            
            /***************************/
            /* Create keyboard layouts */
            /***************************/
            for (i = 0; i < GUI_COUNT_OF(layouts); i++) {
                layout = &layouts[i];               /* Get layout data */
                
                /***************************/
                /* Create keyboard layout  */
                /***************************/
                handleLayout = gui_container_create(layout->id, 0, 0, 100, 100, h, keyboard_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
                gui_widget_setsizepercent(handleLayout, 100, 50);
                gui_widget_setpositionpercent(handleLayout, 0, 50);
                gui_widget_setuserdata(handleLayout, (void *)layout);
                if (i) {                            /* Show only first layout */
                    gui_widget_hide(handleLayout);
                } else {                            /* Save main layout handle */
                    keyboard.main_layout_handle = handleLayout;
                }
                gui_widget_setignoreinvalidate(handleLayout, 0, 1);
                
                /***************************/
                /* Draw buttons on layout  */
                /***************************/
                for (k = 0; k < layout->rows_count; k++) {
                    row = &layout->rows[k];
                    for (z = 0; z < row->btns_count; z++) {
                        btn = &row->btns[z];
                        
                        handle = gui_button_create(0, 0, 0, 1, 1, handleLayout, keyboard_btn_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);   /* Create button object */
                        gui_button_set3dstyle(handle, 0);
                        gui_widget_setuserdata(handle, (void *)btn);
                        gui_widget_setsizepercent(handle, btn->w, 23);
                        gui_widget_setpositionpercent(handle, btn->x, GUI_DIM(1 + (100.0f / layout->rows_count) * k));
                        gui_widget_setzindex(handle, z);
                        gui_widget_setignoreinvalidate(handle, 0, 1);
                    }
                }
            }
            return 1;
        }
        default:                                    /* Handle default option */
            GUI_UNUSED3(h, param, result);          /* Unused elements to prevent compiler warnings */
            return gui_widget_processdefaultcallback(h, evt, param, result);/* Process default callback */
    }
}

/**
 * \brief           Creates virtual keyboard for user interaction.
 * \return          Keyboard handle on success, NULL otherwise
 */
gui_handle_p
gui_keyboard_create(void) {
    if (keyboard.handle == NULL) {
        /* Create keyboard base element with desktop as parent */
        keyboard.handle = gui_container_create(GUI_ID_KEYBOARD_BASE, 0, 0, 1, 1, 0, keyboard_base_callback, GUI_FLAG_WIDGET_CREATE_NO_PARENT);  
    }

    return keyboard.handle;
}

/**
 * \brief           Hide virtual keyboard
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_keyboard_hide(void) {
    uint8_t ret = 0;

    if (keyboard.handle != NULL) {
        gui_widget_hide(keyboard.handle);           /* Hide keyboard widget */
        gui_widget_show(GUI.root.first);            /* Show first on linked list */
        ret = 1;
    }

    return ret;
}

/**
 * \brief           Show virtual keyboard
 * \param[in]       h: Widget handle for which keyboard will be opened
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_keyboard_show(gui_handle_p h) {
    uint8_t ret = 0;

    if (keyboard.handle != NULL) {
        keyboard.target = h;                        /* Save target widget */
        if (h != NULL && h->font != NULL) {         /* Check widget and font for it */
            keyboard.font = h->font;                /* Save font as display font */
            gui_widget_invalidate(keyboard.handle); /* Force invalidation */
        }
        gui_widget_hide(GUI.root.first);            /* Hide first on linked list */
        gui_widget_show(keyboard.handle);           /* Show keyboard widget */
        ret = 1;
    }

    return ret;
}

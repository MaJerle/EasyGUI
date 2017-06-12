/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_keyboard.h"
#include "gui_container.h"
#include "gui_button.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct {
    uint32_t C;                                     /*!< Character to print */
    uint32_t CS;                                    /*!< Character on shift */
    float X;                                        /*!< Start X position */
    float W;                                        /*!< Character width */
    uint8_t S;                                      /*!< Special function */
} KeyboardBtn_t;

typedef struct {
    GUI_Dim_t xOffset;                              /*!< Starting X offset */
    const KeyboardBtn_t* Btns;                      /*!< Pointer to buttons structure */
    size_t BtnsCount;                               /*!< Number of buttons in array */
} KeyboardRow_t;

typedef struct {
    const KeyboardRow_t* Rows;                      /*!< Pointer to rows objects */
    size_t RowsCount;                               /*!< Number of rows */
    GUI_ID_t ID;                                    /*!< Layout ID */
} KeyboardLayout_t;

typedef struct {
    uint8_t IsShift;                                /*!< Status indicating shift mode is enabled */
    GUI_HANDLE_p Handle;                            /*!< Pointer to keyboard handle */
    GUI_HANDLE_p MainLayoutHandle;                  /*!< Pointer to main keyboard layout */
    
    uint8_t Action;                                 /*!< Kbd show/hide action */
    uint8_t ActionValue;                            /*!< Action custom value */
} KeyboardInfo_t;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
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

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
/***************************/
/*   Layout 1 descriptors  */
/***************************/
static 
const KeyboardBtn_t ButtonsL1R1[] = {
    {.C = ((uint32_t)'q'), .X = 0.5, .W = 9, .CS = ((uint32_t)'Q')},
    {.C = ((uint32_t)'w'), .X = 10.5, .W = 9, .CS = ((uint32_t)'W')},
    {.C = ((uint32_t)'e'), .X = 20.5, .W = 9, .CS = ((uint32_t)'E')},
    {.C = ((uint32_t)'r'), .X = 30.5, .W = 9, .CS = ((uint32_t)'R')},
    {.C = ((uint32_t)'t'), .X = 40.5, .W = 9, .CS = ((uint32_t)'T')},
    {.C = ((uint32_t)'z'), .X = 50.5, .W = 9, .CS = ((uint32_t)'Z')},
    {.C = ((uint32_t)'u'), .X = 60.5, .W = 9, .CS = ((uint32_t)'U')},
    {.C = ((uint32_t)'i'), .X = 70.5, .W = 9, .CS = ((uint32_t)'I')},
    {.C = ((uint32_t)'o'), .X = 80.5, .W = 9, .CS = ((uint32_t)'O')},
    {.C = ((uint32_t)'p'), .X = 90.5, .W = 9, .CS = ((uint32_t)'P')}
};

static 
const KeyboardBtn_t ButtonsL1R2[] = {
    {.C = ((uint32_t)'a'), .X = 5.5, .W = 9, .CS = ((uint32_t)'A')},
    {.C = ((uint32_t)'s'), .X = 15.5, .W = 9, .CS = ((uint32_t)'S')},
    {.C = ((uint32_t)'d'), .X = 25.5, .W = 9, .CS = ((uint32_t)'D')},
    {.C = ((uint32_t)'f'), .X = 35.5, .W = 9, .CS = ((uint32_t)'F')},
    {.C = ((uint32_t)'g'), .X = 45.5, .W = 9, .CS = ((uint32_t)'G')},
    {.C = ((uint32_t)'h'), .X = 55.5, .W = 9, .CS = ((uint32_t)'H')},
    {.C = ((uint32_t)'j'), .X = 65.5, .W = 9, .CS = ((uint32_t)'J')},
    {.C = ((uint32_t)'k'), .X = 75.5, .W = 9, .CS = ((uint32_t)'K')},
    {.C = ((uint32_t)'l'), .X = 85.5, .W = 9, .CS = ((uint32_t)'L')}
};

static 
const KeyboardBtn_t ButtonsL1R3[] = {
    {.C = 0, .X = 0.5, .W = 14, .S = SPECIAL_SHIFT},
    {.C = ((uint32_t)'y'), .X = 15.5, .W = 9, .CS = ((uint32_t)'Y')},
    {.C = ((uint32_t)'x'), .X = 25.5, .W = 9, .CS = ((uint32_t)'X')},
    {.C = ((uint32_t)'c'), .X = 35.5, .W = 9, .CS = ((uint32_t)'C')},
    {.C = ((uint32_t)'v'), .X = 45.5, .W = 9, .CS = ((uint32_t)'V')},
    {.C = ((uint32_t)'b'), .X = 55.5, .W = 9, .CS = ((uint32_t)'B')},
    {.C = ((uint32_t)'n'), .X = 65.5, .W = 9, .CS = ((uint32_t)'N')},
    {.C = ((uint32_t)'m'), .X = 75.5, .W = 9, .CS = ((uint32_t)'M')},
    {.C = 0, .X = 85.5, .W = 14, .S = SPECIAL_BACKSPACE},
};

static 
const KeyboardBtn_t ButtonsL1R4[] = {
    {.C = 0, .X = 0.5, .W = 9, .S = ((uint32_t)SPECIAL_ABC)},
    {.C = ((uint32_t)' '), .X = 10.5, .W = 59},
    {.C = ((uint32_t)'.'), .X = 70.5, .W = 9},
    {.C = 0, .X = 80.5, .W = 9, .S = ((uint32_t)SPECIAL_ENTER)},
    {.C = 0, .X = 90.5, .W = 9, .S = ((uint32_t)SPECIAL_HIDE)},
};

static
const KeyboardRow_t KeyboardL1Rows[] = {
    {.xOffset = 1, .Btns = ButtonsL1R1, .BtnsCount = GUI_COUNT_OF(ButtonsL1R1)},
    {.xOffset = 1, .Btns = ButtonsL1R2, .BtnsCount = GUI_COUNT_OF(ButtonsL1R2)},
    {.xOffset = 1, .Btns = ButtonsL1R3, .BtnsCount = GUI_COUNT_OF(ButtonsL1R3)},
    {.xOffset = 1, .Btns = ButtonsL1R4, .BtnsCount = GUI_COUNT_OF(ButtonsL1R4)},
};


                
/***************************/
/*   Layout 2 descriptors  */
/***************************/
static 
const KeyboardBtn_t ButtonsL2R1[] = {
    {.C = ((uint32_t)'1'), .X = 0.5, .W = 9},
    {.C = ((uint32_t)'2'), .X = 10.5, .W = 9},
    {.C = ((uint32_t)'3'), .X = 20.5, .W = 9},
    {.C = ((uint32_t)'4'), .X = 30.5, .W = 9},
    {.C = ((uint32_t)'5'), .X = 40.5, .W = 9},
    {.C = ((uint32_t)'6'), .X = 50.5, .W = 9},
    {.C = ((uint32_t)'7'), .X = 60.5, .W = 9},
    {.C = ((uint32_t)'8'), .X = 70.5, .W = 9},
    {.C = ((uint32_t)'9'), .X = 80.5, .W = 9},
    {.C = ((uint32_t)'0'), .X = 90.5, .W = 9}
};

static 
const KeyboardBtn_t ButtonsL2R2[] = {
    {.C = ((uint32_t)'-'), .X = 0.5, .W = 9},
    {.C = ((uint32_t)'/'), .X = 10.5, .W = 9},
    {.C = ((uint32_t)':'), .X = 20.5, .W = 9},
    {.C = ((uint32_t)';'), .X = 30.5, .W = 9},
    {.C = ((uint32_t)'('), .X = 40.5, .W = 9},
    {.C = ((uint32_t)')'), .X = 50.5, .W = 9},
    {.C = ((uint32_t)'$'), .X = 60.5, .W = 9},
    {.C = ((uint32_t)'&'), .X = 70.5, .W = 9},
    {.C = ((uint32_t)'@'), .X = 80.5, .W = 9},
    {.C = ((uint32_t)'"'), .X = 90.5, .W = 9}
};

static 
const KeyboardBtn_t ButtonsL2R3[] = {
    {.C = 0, .X = 0.5, .W = 14, .S = SPECIAL_CALC},
    {.C = ((uint32_t)'.'), .X = 15.5, .W = 13},
    {.C = ((uint32_t)','), .X = 29.5, .W = 13},
    {.C = ((uint32_t)'?'), .X = 43.5, .W = 13},
    {.C = ((uint32_t)'!'), .X = 57.5, .W = 13},
    {.C = ((uint32_t)'\''), .X = 71.5, .W = 13},
    {.C = 0, .X = 85.5, .W = 14, .S = SPECIAL_BACKSPACE},
};

static 
const KeyboardBtn_t ButtonsL2R4[] = {
    {.C = 0, .X = 0.5, .W = 9, .S = ((uint32_t)SPECIAL_ABC)},
    {.C = ((uint32_t)' '), .X = 10.5, .W = 59},
    {.C = ((uint32_t)'.'), .X = 70.5, .W = 9},
    {.C = 0, .X = 80.5, .W = 9, .S = ((uint32_t)SPECIAL_ENTER)},
    {.C = 0, .X = 90.5, .W = 9, .S = ((uint32_t)SPECIAL_HIDE)},
};

static
const KeyboardRow_t KeyboardL2Rows[] = {
    {.xOffset = 1, .Btns = ButtonsL2R1, .BtnsCount = GUI_COUNT_OF(ButtonsL2R1)},
    {.xOffset = 1, .Btns = ButtonsL2R2, .BtnsCount = GUI_COUNT_OF(ButtonsL2R2)},
    {.xOffset = 1, .Btns = ButtonsL2R3, .BtnsCount = GUI_COUNT_OF(ButtonsL2R3)},
    {.xOffset = 1, .Btns = ButtonsL2R4, .BtnsCount = GUI_COUNT_OF(ButtonsL2R4)},
};

/***************************/
/*   Layout 2 descriptors  */
/***************************/
static 
const KeyboardBtn_t ButtonsL3R1[] = {
    {.C = ((uint32_t)'['), .X = 0.5, .W = 9},
    {.C = ((uint32_t)']'), .X = 10.5, .W = 9},
    {.C = ((uint32_t)'{'), .X = 20.5, .W = 9},
    {.C = ((uint32_t)'}'), .X = 30.5, .W = 9},
    {.C = ((uint32_t)'#'), .X = 40.5, .W = 9},
    {.C = ((uint32_t)'%'), .X = 50.5, .W = 9},
    {.C = ((uint32_t)'^'), .X = 60.5, .W = 9},
    {.C = ((uint32_t)'*'), .X = 70.5, .W = 9},
    {.C = ((uint32_t)'+'), .X = 80.5, .W = 9},
    {.C = ((uint32_t)'='), .X = 90.5, .W = 9}
};

static 
const KeyboardBtn_t ButtonsL3R2[] = {
    {.C = ((uint32_t)'_'), .X = 0.5, .W = 9},
    {.C = ((uint32_t)'\\'), .X = 10.5, .W = 9},
    {.C = ((uint32_t)'|'), .X = 20.5, .W = 9},
    {.C = ((uint32_t)'~'), .X = 30.5, .W = 9},
    {.C = ((uint32_t)'<'), .X = 40.5, .W = 9},
    {.C = ((uint32_t)'>'), .X = 50.5, .W = 9},
    {.C = ((uint32_t)'$'), .X = 60.5, .W = 9},
    {.C = ((uint32_t)':'), .X = 70.5, .W = 9},
    {.C = ((uint32_t)';'), .X = 80.5, .W = 9},
    {.C = ((uint32_t)'-'), .X = 90.5, .W = 9}
};

static 
const KeyboardBtn_t ButtonsL3R3[] = {
    {.C = 0, .X = 0.5, .W = 14, .S = SPECIAL_123},
    {.C = ((uint32_t)'.'), .X = 15.5, .W = 13},
    {.C = ((uint32_t)','), .X = 29.5, .W = 13},
    {.C = ((uint32_t)'?'), .X = 43.5, .W = 13},
    {.C = ((uint32_t)'!'), .X = 57.5, .W = 13},
    {.C = ((uint32_t)'\''), .X = 71.5, .W = 13},
    {.C = 0, .X = 85.5, .W = 14, .S = SPECIAL_BACKSPACE},
};

static 
const KeyboardBtn_t ButtonsL3R4[] = {
    {.C = 0, .X = 0.5, .W = 9, .S = ((uint32_t)SPECIAL_ABC)},
    {.C = ((uint32_t)' '), .X = 10.5, .W = 59},
    {.C = ((uint32_t)'.'), .X = 70.5, .W = 9},
    {.C = 0, .X = 80.5, .W = 9, .S = ((uint32_t)SPECIAL_ENTER)},
    {.C = 0, .X = 90.5, .W = 9, .S = ((uint32_t)SPECIAL_HIDE)},
};

static
const KeyboardRow_t KeyboardL3Rows[] = {
    {.xOffset = 1, .Btns = ButtonsL3R1, .BtnsCount = GUI_COUNT_OF(ButtonsL3R1)},
    {.xOffset = 1, .Btns = ButtonsL3R2, .BtnsCount = GUI_COUNT_OF(ButtonsL3R2)},
    {.xOffset = 1, .Btns = ButtonsL3R3, .BtnsCount = GUI_COUNT_OF(ButtonsL3R3)},
    {.xOffset = 1, .Btns = ButtonsL3R4, .BtnsCount = GUI_COUNT_OF(ButtonsL3R4)},
};

/************************/
/*  Layouts descriptor  */
/************************/
static const KeyboardLayout_t KeyboardLayouts[] = {
    {.ID = GUI_ID_KEYBOARD_LAYOUT_ABC, .Rows = KeyboardL1Rows, .RowsCount = GUI_COUNT_OF(KeyboardL1Rows)},
    {.ID = GUI_ID_KEYBOARD_LAYOUT_123, .Rows = KeyboardL2Rows, .RowsCount = GUI_COUNT_OF(KeyboardL2Rows)},
    {.ID = GUI_ID_KEYBOARD_LAYOUT_CALC, .Rows = KeyboardL3Rows, .RowsCount = GUI_COUNT_OF(KeyboardL3Rows)},
};

static
KeyboardInfo_t Kbd = {.ActionValue = 10};   /* Set action value to max */

#define SHIFT_DISABLE()     if (Kbd.IsShift) { Kbd.IsShift = 0; __GUI_WIDGET_Invalidate(Kbd.MainLayoutHandle); }
#define SHIFT_ENABLE(mode)  do {                        \
    if (!Kbd.IsShift && (mode)) {                       \
        __GUI_WIDGET_Invalidate(Kbd.MainLayoutHandle);  \
    }                                                   \
    Kbd.IsShift = (mode);                               \
} while (0)

#define SHIFT_TOGGLE()      do {        \
    Kbd.IsShift = !Kbd.IsShift;         \
    __GUI_WIDGET_Invalidate(Kbd.MainLayoutHandle);  \
} while (0)

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
/* Callback for keyboard button */
static
uint8_t keyboard_btn_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    switch (cmd) {                                  /* Process commands */
        case GUI_WC_Draw: {
            GUI_Char str[10] = {0};
            const KeyboardBtn_t* kbtn;
            
            kbtn = (const KeyboardBtn_t *)__GUI_WIDGET_GetUserData(h);  /* Get user data */
            
            switch (kbtn->S) {
                case SPECIAL_123: 
                    strcpy((char *)str, (char *)_GT("123"));
                    break;
                case SPECIAL_ABC: 
                    strcpy((char *)str, (char *)_GT("abc"));
                    break;
                case SPECIAL_CALC:
                    strcpy((char *)str, (char *)_GT("#+="));
                    break;
                case SPECIAL_BACKSPACE: 
                    strcpy((char *)str, (char *)_GT("Back"));
                    break;
                case SPECIAL_ENTER: 
                    strcpy((char *)str, (char *)_GT("Ent"));
                    break;
                case SPECIAL_SHIFT: 
                    strcpy((char *)str, (char *)_GT("Shift"));
                    break;
                case SPECIAL_HIDE: 
                    strcpy((char *)str, (char *)_GT("Hide"));
                    break;
                default:
                    if (Kbd.IsShift && kbtn->CS) {  /* Character when shift is ON */
                        GUI_STRING_UNICODE_Encode(kbtn->CS, str);   /* Encode character to unicode */
                    } else {                        /* Character when shift is OFF */
                        GUI_STRING_UNICODE_Encode(kbtn->C, str);    /* Encode character to unicode */
                    }
            }
            
            __GUI_WIDGET_SetText(h, str);           /* Temporary set text */
            GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);   /* Process default callback with drawing */
            return 1;
        }
        case GUI_WC_Click: {                        /* Handle pressed button */
            GUI_HANDLE_p tmp1, tmp2, tmp3;
            const KeyboardBtn_t* kbtn;
            uint32_t ch = 0;
            GUI_KeyboardData_t kbd = {0};
            
            kbtn = __GUI_WIDGET_GetUserData(h);     /* Get data from widget */
            if (kbtn->S) {                          /* Is button special function? */
                switch (kbtn->S) {                  /* Check special function */
                    case SPECIAL_123:
                    case SPECIAL_ABC: 
                    case SPECIAL_CALC: {            /* Special functions 123 or ABC */
                        tmp1 = __GUI_WIDGET_GetById(GUI_ID_KEYBOARD_LAYOUT_123);
                        tmp2 = __GUI_WIDGET_GetById(GUI_ID_KEYBOARD_LAYOUT_ABC);
                        tmp3 = __GUI_WIDGET_GetById(GUI_ID_KEYBOARD_LAYOUT_CALC);
                        
                        /* Hide layouts */
                        if (kbtn->S == SPECIAL_ABC) {
                            __GUI_WIDGET_Hide(tmp1);
                            __GUI_WIDGET_Hide(tmp3);
                        } else if (kbtn->S == SPECIAL_123) {
                            __GUI_WIDGET_Hide(tmp2);
                            __GUI_WIDGET_Hide(tmp3);
                        } else if (kbtn->S == SPECIAL_CALC) {
                            __GUI_WIDGET_Hide(tmp1);
                            __GUI_WIDGET_Hide(tmp2);
                        }
                        
                        /* Show layout */
                        if (kbtn->S == SPECIAL_ABC) {
                            __GUI_WIDGET_Show(tmp2);
                        } else if (kbtn->S == SPECIAL_123) {
                            __GUI_WIDGET_Show(tmp1);
                        } else if (kbtn->S == SPECIAL_CALC) {
                            __GUI_WIDGET_Show(tmp3);
                        }
                        SHIFT_DISABLE();            /* Clear shift mode */
                        break;
                    }
                    case SPECIAL_SHIFT: {
                        SHIFT_TOGGLE();             /* Toggle shift mode */
                        break;
                    }
                    case SPECIAL_BACKSPACE: {
                        ch = GUI_KEY_BACKSPACE;     /* Fake backspace key */
                        break;
                    }
                    case SPECIAL_HIDE: {            /* Hide button pressed */
                        __GUI_KEYBOARD_Hide();      /* Hide keyboard */
                        break;
                    }
                }
            }
            
            /**
             * Check if we have to add key to input buffer
             */
            if (ch || !kbtn->S) {                   /* If character from special is set or normal key pressed */
                if (!ch) {                          /* Only if char not yet set */
                    if (Kbd.IsShift && kbtn->CS) {  /* If shift mode enabled and character has shift mode character */
                        ch = kbtn->CS;              /* Use shift mode character */
                    } else {
                        ch = kbtn->C;               /* Use normal character */
                    }
                }
                
                /* Clear shift mode if necessary */
                if (Kbd.IsShift != SHIFT_UPPERCASE) {   /* When not in uppercase shift mode */
                    SHIFT_DISABLE();                /* Clear shift mode */
                }
                
                /************************************/
                /* Send character to focused widget */
                /************************************/
                GUI_STRING_UNICODE_Encode(ch, kbd.Keys);    /* Decode key */
                GUI_INPUT_KeyAdd(&kbd);             /* Add actual key */
                kbd.Keys[0] = 0;                    /* Set key to 0 */
                GUI_INPUT_KeyAdd(&kbd);             /* Add end key */
            }
            return 1;
        }
        case GUI_WC_DblClick: {
            const KeyboardBtn_t* kbtn;
            
            kbtn = __GUI_WIDGET_GetUserData(h);     /* Get data from widget */
            switch (kbtn->S) {
                case SPECIAL_SHIFT: {
                    SHIFT_ENABLE(SHIFT_UPPERCASE);  /* Enable shift upper case mode */
                    break;
                }
            }
            return 1;
        }
        default:
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);    /* Process default callback */
    }
}

/* Callback for keyboard base widget */
static
uint8_t keyboard_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    switch (cmd) {
        case GUI_WC_Init: {
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);    /* Process default callback */
    }
}

/* Timer callback for keyboard */
static
void keyboard_timer_callback(GUI_TIMER_t* timer) {
    if (Kbd.Action == ACTION_HIDE) {                /* We should hide the keyboard */
        if (Kbd.ActionValue < 10) {
            Kbd.ActionValue++;
            __GUI_WIDGET_SetPositionPercent(Kbd.Handle, 0, 50 + Kbd.ActionValue * 5);
        } else {
            __GUI_WIDGET_Hide(Kbd.Handle);          /* Hide keyboard */
            __GUI_TIMER_Stop(timer);                /* Stop timer */
        }
    } else if (Kbd.Action == ACTION_SHOW) {         /* We should show the keyboard */
        if (Kbd.ActionValue) {
            if (Kbd.ActionValue == 10) {            /* At the bottom? */
                __GUI_WIDGET_Show(Kbd.Handle);      /* First set keyboard as visible */
            }
            Kbd.ActionValue--;                      /* Decrease value */
            __GUI_WIDGET_SetPositionPercent(Kbd.Handle, 0, 50 + Kbd.ActionValue * 5);
        }
    }
}

/* Callback function for base element of keyboard */
static
uint8_t keyboard_base_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    switch (cmd) {
        case GUI_WC_PreInit: {
            __GH(h)->Timer = __GUI_TIMER_Create(60, keyboard_timer_callback, 0);    /* Create timer */
            if (!__GH(h)->Timer) {
                *(uint8_t *)result = 0;             /* Failed, stop and clear memory */
            }
            return 1;
        }
        case GUI_WC_Init: {                         /* When base element is initialized */
            GUI_HANDLE_p handle, handleLayout;
            size_t i, k, z;
            const KeyboardLayout_t* Layout;
            const KeyboardRow_t* Row;
            const KeyboardBtn_t* Btn;
            
            /***************************/
            /*   Configure keyboard    */
            /***************************/
            __GUI_WIDGET_SetSizePercent(h, 100, 50);    /* Set keyboard size */
            __GUI_WIDGET_SetPositionPercent(h, 0, 100); /* Set position of keyboard outside visible area */
            __GUI_WIDGET_SetZIndex(h, GUI_WIDGET_ZINDEX_MAX);   /* Set to maximal z-index */
            __GUI_WIDGET_Hide(h);                       /* Hide keyboard by default */
            
            /***************************/
            /* Create keyboard layouts */
            /***************************/
            for (i = 0; i < GUI_COUNT_OF(KeyboardLayouts); i++) {
                Layout = &KeyboardLayouts[i];       /* Get layout data */
                
                /***************************/
                /* Create keyboard layout  */
                /***************************/
                handleLayout = GUI_CONTAINER_Create(Layout->ID, 0, 0, 100, 100, h, keyboard_callback, 0);
                __GUI_WIDGET_SetSizePercent(handleLayout, 100, 100);
                __GUI_WIDGET_SetPositionPercent(handleLayout, 0, 0);
                __GUI_WIDGET_SetUserData(handleLayout, (void *)Layout);
                if (i) {                            /* Show only first layout */
                    __GUI_WIDGET_Hide(handleLayout);
                } else {                            /* Save main layout handle */
                    Kbd.MainLayoutHandle = handleLayout;
                }
                
                /***************************/
                /* Draw buttons on layout  */
                /***************************/
                for (k = 0; k < Layout->RowsCount; k++) {
                    Row = &Layout->Rows[k];         /* Get row pointer */
                    for (z = 0; z < Row->BtnsCount; z++) {
                        Btn = &Row->Btns[z];        /* Get button pointer */
                        
                        handle = GUI_BUTTON_Create(0, 0, 0, 1, 1, handleLayout, keyboard_btn_callback, 0);    /* Create button object */
                        __GUI_WIDGET_SetUserData(handle, (void *)Btn);  /* Set pointer to button */
                        __GUI_WIDGET_SetSizePercent(handle, Btn->W, 23);    /* Set button percent */
                        __GUI_WIDGET_SetPositionPercent(handle, Btn->X, 1 + 25 * k);
                        __GUI_WIDGET_Set3DStyle(handle, 0); /* Make buttons flat */
                    }
                }
            }
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);    /* Process default callback */
    }
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
uint8_t __GUI_KEYBOARD_Hide(void) {
    __GUI_ASSERTPARAMS(Kbd.Handle);
    Kbd.Action = ACTION_HIDE;                       /* Set action to hide */
    __GUI_TIMER_StartPeriodic(__GH(Kbd.Handle)->Timer); /* Start periodic timer */
    
    return 1;
}

uint8_t __GUI_KEYBOARD_Show(void) {
    __GUI_ASSERTPARAMS(Kbd.Handle);
    Kbd.Action = ACTION_SHOW;                       /* Set action to show */
    __GUI_TIMER_StartPeriodic(__GH(Kbd.Handle)->Timer); /* Start periodic timer */
    
    return 1;
}

/******************************************************************************/
/******************************************************************************/
/***                         Thread safe public API                          **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_KEYBOARD_Create(void) {    
    __GUI_ENTER();                                  /* Enter GUI */
    
    if (!Kbd.Handle) {
        Kbd.Handle = GUI_CONTAINER_Create(GUI_ID_KEYBOARD_BASE, 0, 0, 1, 1, 0, keyboard_base_callback, GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP);  /* Create keyboard base element with desktop as parent */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return Kbd.Handle;
}

uint8_t GUI_KEYBOARD_Hide(void) {
    uint8_t ret;
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_KEYBOARD_Hide();                    /* Hide keyboard */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

uint8_t GUI_KEYBOARD_Show(void) {
    uint8_t ret;
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_KEYBOARD_Show();                    /* Show keyboard */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

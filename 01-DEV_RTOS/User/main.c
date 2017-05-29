/**
 * Keil project example for GSM SIM800/900 for SMS and RTOS support
 *
 * @note      Check defines.h file for configuration settings!
 * @note      When using Nucleo F411 board, example has set 8MHz external HSE clock!
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 *
 * \par Description
 *
 * This examples shows how you can receive SMS and read it.
 * If you send SMS with specific content, it will do actions with LED on board:
 *
 * - LED ON: led will turn on,
 * - LED OFF: Led will turn off,
 * - LED TOGGLE: Led will toggle.
 *
 * After that, SMS with "OK" or "ERROR" should be returned to received number to confirm action
 *
 * \par Pinout for example (Nucleo STM32F411)
 *
\verbatim
GSM         STM32F4xx           DESCRIPTION
 
RX          PA9                 TX from STM to RX from GSM
TX          PA10                RX from STM
VCC         3.3V                Use external 3.3V regulator
GND         GND
RST         PA0
CTS         PA3                 RTS from ST to CTS from GSM
            BUTTON(PA0, PC13)   Discovery/Nucleo button, depends on configuration
            
            PA2                 TX for debug purpose (connect to PC) with 921600 bauds
\endverbatim
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_touch.h"
#include "tm_stm32_exti.h"
#include "cmsis_os.h"
#include "tm_stm32_general.h"
#include "main.h"
#include "tm_stm32_sdram.h"
#define GUI_INTERNAL
#include "gui.h"
#include "gui_window.h"
#include "gui_button.h"
#include "gui_led.h"
#include "gui_progbar.h"
#include "gui_graph.h"
#include "gui_edittext.h"
#include "gui_checkbox.h"
#include "gui_radio.h"
#include "gui_listbox.h"
#include "gui_listview.h"
#include "gui_textview.h"
#include "gui_dropdown.h"
#include "gui_dialog.h"
#include "gui_image.h"
#include "gui_slider.h"

#include "math.h"

#define COUNT_OF(x)     (sizeof(x) / sizeof((x)[0]))

TM_TOUCH_t TS;

GUI_HANDLE_p win1, handle;
GUI_GRAPH_DATA_p graphdata1, graphdata2, graphdata3, graphdata4;
GUI_HANDLE_p dialog1;

#define ID_BASE                 (GUI_ID_USER)
#define ID_BASE_WIN             (ID_BASE + 0x0100)
#define ID_BASE_BTN             (ID_BASE_WIN + 0x0100)
#define ID_BASE_TEXTWIEW        (ID_BASE_BTN + 0x0100)
#define ID_BASE_CHECKBOX        (ID_BASE_TEXTWIEW + 0x0100)
#define ID_BASE_LED             (ID_BASE_CHECKBOX + 0x0100)
#define ID_BASE_GRAPH           (ID_BASE_LED + 0x0100)

/* List of window widget IDs */
#define ID_WIN_BTN              (ID_BASE_WIN + 0x01)
#define ID_WIN_EDIT             (ID_BASE_WIN + 0x02)
#define ID_WIN_RADIO            (ID_BASE_WIN + 0x03)
#define ID_WIN_CHECKBOX         (ID_BASE_WIN + 0x04)
#define ID_WIN_PROGBAR          (ID_BASE_WIN + 0x05)
#define ID_WIN_GRAPH            (ID_BASE_WIN + 0x06)
#define ID_WIN_LISTBOX          (ID_BASE_WIN + 0x07)
#define ID_WIN_LED              (ID_BASE_WIN + 0x08)
#define ID_WIN_TEXTVIEW         (ID_BASE_WIN + 0x09)
#define ID_WIN_DROPDOWN         (ID_BASE_WIN + 0x0A)
#define ID_WIN_DIALOG           (ID_BASE_WIN + 0x0B)
#define ID_WIN_LISTVIEW         (ID_BASE_WIN + 0x0C)
#define ID_WIN_IMAGE            (ID_BASE_WIN + 0x0D)
#define ID_WIN_SLIDER           (ID_BASE_WIN + 0x0E)

/* List of base buttons IDs */
#define ID_BTN_WIN_BTN          (ID_BASE_BTN + 0x01)
#define ID_BTN_WIN_EDIT         (ID_BASE_BTN + 0x02)
#define ID_BTN_WIN_RADIO        (ID_BASE_BTN + 0x03)
#define ID_BTN_WIN_CHECKBOX     (ID_BASE_BTN + 0x04)
#define ID_BTN_WIN_PROGBAR      (ID_BASE_BTN + 0x05)
#define ID_BTN_WIN_GRAPH        (ID_BASE_BTN + 0x06)
#define ID_BTN_WIN_LISTBOX      (ID_BASE_BTN + 0x07)
#define ID_BTN_WIN_LED          (ID_BASE_BTN + 0x08)
#define ID_BTN_WIN_TEXTVIEW     (ID_BASE_BTN + 0x09)
#define ID_BTN_WIN_DROPDOWN     (ID_BASE_BTN + 0x0A)
#define ID_BTN_WIN_DIALOG       (ID_BASE_BTN + 0x0B)
#define ID_BTN_WIN_LISTVIEW     (ID_BASE_BTN + 0x0C)
#define ID_BTN_WIN_IMAGE        (ID_BASE_BTN + 0x0D)
#define ID_BTN_WIN_SLIDER       (ID_BASE_BTN + 0x0E)

#define ID_BTN_DIALOG_CONFIRM   (ID_BASE_BTN + 0x20)
#define ID_BTN_DIALOG_CANCEL    (ID_BASE_BTN + 0x21)

#define ID_TEXTVIEW_1           (ID_BASE_TEXTWIEW + 0x01)

#define ID_CHECKBOX_LED         (ID_BASE_CHECKBOX + 0x01)
#define ID_CHECKBOX_GRAPH       (ID_BASE_CHECKBOX + 0x02)

#define ID_LED_1                (ID_BASE_LED + 0x01)
#define ID_LED_2                (ID_BASE_LED + 0x02)
#define ID_LED_3                (ID_BASE_LED + 0x03)
#define ID_LED_4                (ID_BASE_LED + 0x04)

#define ID_GRAPH_MAIN           (ID_BASE_GRAPH + 0x01)

typedef struct {
    GUI_ID_t win_id;
    const GUI_Char* win_text;
} btn_user_data_t;

typedef struct {
    GUI_ID_t id;
    const GUI_Char* text;
    btn_user_data_t data;
} bulk_init_t;

#define RADIO_GROUP_HALIGN  0x01
#define RADIO_GROUP_VALIGN  0x02

bulk_init_t buttons[] = {
    {ID_BTN_WIN_BTN,        _T("Buttons"),      {ID_WIN_BTN, _T("Buttons")}},
    {ID_BTN_WIN_EDIT,       _T("Edit text"),    {ID_WIN_EDIT, _T("Edit text")}},
    {ID_BTN_WIN_RADIO,      _T("Radio box"),    {ID_WIN_RADIO, _T("Radio box")}},
    {ID_BTN_WIN_CHECKBOX,   _T("Check box"),    {ID_WIN_CHECKBOX, _T("Check box")}},
    {ID_BTN_WIN_PROGBAR,    _T("Progress bar"), {ID_WIN_PROGBAR, _T("Progress bar")}},
    {ID_BTN_WIN_GRAPH,      _T("Graph"),        {ID_WIN_GRAPH, _T("Graph")}},
    {ID_BTN_WIN_LISTBOX,    _T("List box"),     {ID_WIN_LISTBOX, _T("List box")}},
    {ID_BTN_WIN_LED,        _T("Led"),          {ID_WIN_LED, _T("Led")}},
    {ID_BTN_WIN_TEXTVIEW,   _T("Text view"),    {ID_WIN_TEXTVIEW, _T("Text view")}},
    {ID_BTN_WIN_DROPDOWN,   _T("Dropdown"),     {ID_WIN_DROPDOWN, _T("Dropdown")}},
    {ID_BTN_WIN_DIALOG,     _T("Dialog"),       {ID_WIN_DIALOG, _T("Dialog")}},
    {ID_BTN_WIN_LISTVIEW,   _T("Listview"),     {ID_WIN_LISTVIEW, _T("Listview")}},
    {ID_BTN_WIN_IMAGE,      _T("Image"),        {ID_WIN_IMAGE, _T("Image")}},
    {ID_BTN_WIN_SLIDER,     _T("Slider"),       {ID_WIN_SLIDER, _T("Slider")}},
};

char str[100];

extern GUI_Const GUI_FONT_t GUI_Font_Comic_Sans_MS_Regular_22;
extern GUI_Const GUI_FONT_t GUI_Font_Calibri_Bold_8;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Bold_18;
extern GUI_Const GUI_FONT_t GUI_Font_FontAwesome_Regular_30;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Narrow_Italic_22;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Narrow_Italic_21_AA;

uint32_t time;

GUI_Char* listboxtexts[] = {
    _T("Item 0"),
    _T("Item 1"),
    _T("Item 2"),
    _T("Item 3"),
    _T("Item 4"),
    _T("Item 5"),
    _T("Item 6"),
    _T("Item 7"),
    _T("Item 8"),
    _T("Item 9"),
    _T("Item 10"),
    _T("Item 11"),
    _T("Item 12"),
};

uint8_t window_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t radio_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t led_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t dialog_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t listview_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t slider_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);

#define PI      3.14159265359f

static float len = 72, radius = 90;
float x, y;

int pressed = 0;

extern GUI_t GUI;

#define IMG_WIDTH       64
#define IMG_HEIGHT      64

uint8_t Mask[IMG_WIDTH * IMG_HEIGHT];

#define __ABS(x)        ((uint32_t)((int32_t)(x) < 0 ? -(x) : (x)))
void CreateMask(void) {
    uint32_t x, y;
    uint32_t midX = IMG_WIDTH >> 1;
    uint32_t midY = IMG_HEIGHT >> 1;
    uint32_t alphaX, alphaY;
    uint8_t* ptr = Mask;
    uint32_t multX = 256 / midX;
    uint32_t multY = 256 / midY;
    
    for (y = 0; y < IMG_HEIGHT; y++) {
        alphaY = (__ABS((int16_t)y - (int16_t)midY)) * multY;
        for (x = 0; x < IMG_WIDTH; x++) {
            alphaX = (__ABS((int16_t)x - (int16_t)midX) << 2) * multX;
            
            *ptr = ((alphaX * alphaY) >> 8) & 0xFF;
            ptr++;
        }
    }
}

void DMA2D_Transfer(void) {
    uint32_t dst;
    
    dst = GUI.LCD.Layers[GUI.LCD.ActiveLayer].StartAddress;
    
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    
    DMA2D->CR = DMA2D_M2M_BLEND;                    /* Memory to memory with blending */
    DMA2D->FGMAR = (uint32_t)Mask;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = 0;
    DMA2D->BGOR = GUI.LCD.Width - IMG_WIDTH * 2;
    DMA2D->FGCOLR = 0x80FF0000;
    DMA2D->OOR = GUI.LCD.Width - IMG_WIDTH * 2;  
    DMA2D->FGPFCCR = DMA2D_INPUT_A4;                   /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = DMA2D_OUTPUT_RGB565;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = DMA2D_OUTPUT_RGB565;                   /* Output     PFC Control Register (Defines the output pixel format) */
    DMA2D->NLR = (uint32_t)(IMG_WIDTH << 17) | (uint16_t)IMG_HEIGHT; 
    DMA2D->CR |= DMA2D_CR_START;
}

int main(void) {
    GUI_STRING_UNICODE_t s;
    
    GUI_KeyboardData_t key;
    uint32_t state;
    
    TM_RCC_InitSystem();                                    /* Init system */
    HAL_Init();                                             /* Init HAL layer */
    TM_DISCO_LedInit();                                     /* Init leds */
    TM_DISCO_ButtonInit();                                  /* Init button */
    TM_DELAY_Init();                                        /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 115200*2);   /* Init USART for debug purpose */
    
    /* Print first screen message */
    printf("GUI; Compiled: %s %s\r\n", __DATE__, __TIME__);
    
    TM_GENERAL_DWTCounterEnable();
    
    GUI_Init();
    
    GUI_WIDGET_SetFontDefault(&GUI_Font_Arial_Narrow_Italic_21_AA); /* Set default font for widgets */
    GUI_WIDGET_SetFontDefault(&GUI_Font_Comic_Sans_MS_Regular_22); /* Set default font for widgets */
    
    win1 = GUI_WINDOW_GetDesktop();                         /* Get desktop window */
    
    for (state = 0; state < GUI_COUNT_OF(buttons); state++) {
        handle = GUI_BUTTON_Create(buttons[state].id, 5 + (state % 3) * 160, 5 + (state / 3) * 50, 150, 40, win1, button_callback, 0);
        GUI_WIDGET_SetText(handle, buttons[state].text);
        GUI_WIDGET_SetUserData(handle, &buttons[state].data);
    }
    
    handle = GUI_WIDGET_GetById(buttons[COUNT_OF(buttons) - 1].id);
    //GUI_WIDGET_Callback(handle, GUI_WC_Click, 0, 0);
    
    __GUI_LINKEDLIST_PrintList(NULL);

    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = TOUCH_ORIENT_DEFAULT;
#if defined(STM32F769_DISCOVERY)
    TS.Orientation = TOUCH_ORIENT_INVERT_Y;
#endif
    TM_TOUCH_Init(NULL, &TS);
    
    TM_EXTI_Attach(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, TM_EXTI_Trigger_Rising);
    
    GUI_STRING_UNICODE_Init(&s);
    
    /* Create mask software */
    CreateMask();
  
//    time = TM_DELAY_Time();
    state = 0;
	while (1) {
        GUI_Process();
        
        if ((TM_DELAY_Time() - time) >= 2000) {
            time = TM_DELAY_Time();
            
            handle = GUI_WIDGET_GetById(buttons[state].id);
            if (handle) {
                //GUI_WIDGET_Callback(handle, GUI_WC_Click, 0, 0);
            }
            if (++state >= COUNT_OF(buttons)) {
                state = 0;
            }
        }
        
        if (pressed) {
            DMA2D_Transfer();
//            pressed = GUI_DIALOG_CreateBlocking(ID_WIN_DIALOG, 50, 10, 300, 150, 0, dialog_callback, 0);
//            if (pressed == 1) {
//                __GUI_DEBUG("Confirmed\r\n");
//                TM_DISCO_LedToggle(LED_GREEN);
//            } else if (pressed == 0) {
//                __GUI_DEBUG("Canceled\r\n");
//            }
            pressed = 0;
        }
        
        while (!TM_USART_BufferEmpty(DISCO_USART)) {
            GUI_Char ch = TM_USART_Getc(DISCO_USART);
            switch (GUI_STRING_UNICODE_Decode(&s, ch)) {
                case UNICODE_OK:
                    if (ch == 'l') {
                        __GUI_LINKEDLIST_PrintList(NULL);
                        break;
                    }
                    if (ch == 'b') {
                        TM_USART_Send(DISCO_USART, (uint8_t *)Mask, sizeof(Mask));
                        break;
                    }
                    if (ch == 'w') {
                        ch = GUI_KEY_UP;
                    } else if (ch == 's') {
                        ch = GUI_KEY_DOWN;
                    } else if (ch == 'a') {
                        ch = GUI_KEY_LEFT;
                    } else if (ch == 'd') {
                        ch = GUI_KEY_RIGHT;
                    }
                    key.Keys[s.t - 1] = ch;
                    GUI_INPUT_KeyAdd(&key);
                    key.Keys[0] = 0;
                    GUI_INPUT_KeyAdd(&key);
                    break;
                case UNICODE_PROGRESS:
                    key.Keys[s.t - s.r - 1] = ch;
                default:
                    break;
            }
        }
	}
}

const GUI_IMAGE_DESC_t maskImg = {
    .xSize = IMG_WIDTH,
    .ySize = IMG_HEIGHT,
    .BPP = 8,
    .Image = (uint8_t *)Mask
};

uint8_t window_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t res = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_Init) {           /* Window has been just initialized */
        switch (GUI_WIDGET_GetId(h)) {  /* Button callbacks */
            case ID_WIN_BTN: {
                handle = GUI_BUTTON_Create(0, 10, 10, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _T("Button 1"));
                handle = GUI_BUTTON_Create(0, 10, 60, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _T("Button 2"));
                break;
            }
            case ID_WIN_CHECKBOX: {     /* Check box */
                handle = GUI_CHECKBOX_Create(ID_CHECKBOX_LED, 10, 10, 400, 40, h, checkbox_callback, 0);
                GUI_WIDGET_SetText(handle, _T("Check box 1"));
                handle = GUI_CHECKBOX_Create(1, 10, 60, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Check box 2"));
                break;
            }
            case ID_WIN_RADIO: {        /* Radio box */
                uint8_t i;
                for (i = 0; i < 4; i++) {
                    handle = GUI_RADIO_Create(10, 10, 10 + (i * 30), 200, 25, h, 0, 0);
                    GUI_WIDGET_SetText(handle, _T("Radio box"));
                    GUI_RADIO_SetGroup(handle, i % 1);
                    GUI_RADIO_SetValue(handle, i);
                    GUI_RADIO_SetDisabled(handle, i / 2);
                }
                break;
            }
            case ID_WIN_LISTBOX: {      /* List box */
                uint8_t i;
                handle = GUI_LISTBOX_Create(1, 10, 10, 190, 195, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_LISTBOX_AddString(handle, listboxtexts[i]);
                }
                GUI_LISTBOX_SetSliderAuto(handle, 0);
                GUI_LISTBOX_SetSliderVisibility(handle, 1);
                
                handle = GUI_LISTBOX_Create(1, 210, 10, 200, 195, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_LISTBOX_AddString(handle, listboxtexts[i]);
                }
                GUI_LISTBOX_SetSliderAuto(handle, 0);
                GUI_LISTBOX_SetSliderVisibility(handle, 0);
                break;
            }
            case ID_WIN_LISTVIEW: {     /* Listview */
                GUI_LISTVIEW_ROW_p row;
                handle = GUI_LISTVIEW_Create(0, 10, 10, 100, 40, h, listview_callback, 0);
                GUI_WIDGET_SetExpanded(handle, 1);
                
                GUI_LISTVIEW_AddColumn(handle, _T("Name"));
                GUI_LISTVIEW_AddColumn(handle, _T("LastName"));
                GUI_LISTVIEW_AddColumn(handle, _T("Email"));
                GUI_LISTVIEW_AddColumn(handle, _T("Phone"));
                
                GUI_LISTVIEW_SetColumnWidth(handle, 0, 120);
                GUI_LISTVIEW_SetColumnWidth(handle, 1, 150);
                GUI_LISTVIEW_SetColumnWidth(handle, 2, 220);
                GUI_LISTVIEW_SetColumnWidth(handle, 3, 160);
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Tilen"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Majerle"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@majerle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("040167724"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Nekdo"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Nekje"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("til@erle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("123456789"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Tilen"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Majerle"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@majerle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("040167724"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Nekdo"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Nekje"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("til@erle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("123456789"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Tilen"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Majerle"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@majerle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("040167724"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Nekdo"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Nekje"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("til@erle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("123456789"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Tilen"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Majerle"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@majerle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("040167724"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Nekdo"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Nekje"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("til@erle.eu"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("123456789"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                
                row = GUI_LISTVIEW_AddRow(handle);
                GUI_LISTVIEW_SetItemString(handle, row, 0, _T("Olala"));
                GUI_LISTVIEW_SetItemString(handle, row, 1, _T("Ulalala"));
                GUI_LISTVIEW_SetItemString(handle, row, 2, _T("tilen@gmail.com"));
                GUI_LISTVIEW_SetItemString(handle, row, 3, _T("987654321"));
                break;
            }
            case ID_WIN_GRAPH: {        /* Graph window */
                uint16_t i;
                handle = GUI_GRAPH_Create(ID_GRAPH_MAIN, 10, 10, 400, 220, h, 0, 0);
                GUI_WIDGET_SetExpanded(handle, 1);

                GUI_GRAPH_SetMinX(handle, -100);
                GUI_GRAPH_SetMaxX(handle, 100);
                GUI_GRAPH_SetMinY(handle, -100);
                GUI_GRAPH_SetMaxY(handle, 100);
                GUI_WIDGET_Invalidate(handle);
                GUI_GRAPH_ZoomReset(handle);
                GUI_GRAPH_Zoom(handle, 0.1, 0.5, 0.5);

                if (!graphdata1) {
                    graphdata1 = GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_XY, len);
                    graphdata1->Color = GUI_COLOR_RED;
                }
                if (!graphdata2) {
                    graphdata2 = GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_YT, len / 2);
                    graphdata2->Color = GUI_COLOR_GREEN;
                }

                for (i = 0; i <= 360; i += 360 / len) {
                    x = cos((float)i * (PI / 180.0f));
                    y = sin((float)i * (PI / 180.0f));
                    GUI_GRAPH_DATA_AddValue(graphdata1, x * radius, y * radius);
                    GUI_GRAPH_DATA_AddValue(graphdata2, x * radius / 3, y * radius / 4);
                }
                GUI_GRAPH_AttachData(handle, graphdata1);
                GUI_GRAPH_AttachData(handle, graphdata2);
                
                /* Create checkbox handle for graph */
                handle = GUI_CHECKBOX_Create(ID_CHECKBOX_GRAPH, 17, 15, 200, 30, h, checkbox_callback, 0);
                GUI_WIDGET_SetText(handle, _T("SetExpanded"));
                GUI_WIDGET_SetZIndex(handle, 1);
                GUI_CHECKBOX_SetColor(handle, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_WHITE);
                break;
            }
            case ID_WIN_EDIT: {         /* Edit text */
                handle = GUI_EDITTEXT_Create(1, 10, 10, 400, 40, h, 0, 0);
                GUI_WIDGET_AllocTextMemory(handle, 255);
                GUI_WIDGET_SetText(handle, _T("Edit text"));
                break;
            }
            case ID_WIN_PROGBAR: {      /* Progress bar */
                handle = GUI_PROGBAR_Create(2, 10, 10, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Progbar"));
                
                handle = GUI_PROGBAR_Create(2, 10, 100, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Progbar"));
                GUI_PROGBAR_EnablePercentages(handle);
                break;
            }
            case ID_WIN_LED: {          /* Leds */
                handle = GUI_LED_Create(ID_LED_1, 10, 10, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_CIRCLE);
                GUI_LED_Set(handle, 1);
                handle = GUI_LED_Create(ID_LED_2, 10, 40, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_CIRCLE);
                
                handle = GUI_LED_Create(ID_LED_3, 10, 70, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_RECT);
                GUI_LED_Set(handle, 1);
                handle = GUI_LED_Create(ID_LED_4, 10, 100, 20, 20, h, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_RECT);
                
                handle = GUI_TEXTVIEW_Create(0, 40, 10, 400, 1000, h, 0, 0);
                GUI_WIDGET_SetFont(handle, &GUI_Font_Arial_Bold_18);
                GUI_WIDGET_SetText(handle, _T("\"LED\" are widgets used to indicate some status or any other situation. Press blue button on discovery board to see LED in happen\r\n"));
                break;
            }
            case ID_WIN_TEXTVIEW: {     /* Text view */
                handle = GUI_TEXTVIEW_Create(ID_TEXTVIEW_1, 10, 10, 300, 180, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Text view with automatic new line detector and support for different aligns.\r\n\r\nHowever, I can also manually jump to new line! Just like Word works ;)"));
                GUI_WIDGET_SetExpanded(handle, 1);
                GUI_WIDGET_SetZIndex(handle, -1);
                
                handle = GUI_RADIO_Create(0, 10, 200, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_HALIGN);
                GUI_WIDGET_SetText(handle, _T("Align left"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_HALIGN_LEFT);
                
                handle = GUI_RADIO_Create(0, 120, 200, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_HALIGN);
                GUI_WIDGET_SetText(handle, _T("Align center"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_HALIGN_CENTER);
                
                handle = GUI_RADIO_Create(0, 260, 200, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_HALIGN);
                GUI_WIDGET_SetText(handle, _T("Align right"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_HALIGN_RIGHT);
                
                
                handle = GUI_RADIO_Create(0, 320, 10, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_VALIGN);
                GUI_WIDGET_SetText(handle, _T("Align top"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_VALIGN_TOP);
                
                handle = GUI_RADIO_Create(0, 320, 50, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_VALIGN);
                GUI_WIDGET_SetText(handle, _T("Align center"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_VALIGN_CENTER);
                
                handle = GUI_RADIO_Create(0, 320, 90, 150, 30, h, radio_callback, 0);
                GUI_RADIO_SetGroup(handle, RADIO_GROUP_VALIGN);
                GUI_WIDGET_SetText(handle, _T("Align bottom"));
                GUI_RADIO_SetValue(handle, GUI_TEXTVIEW_VALIGN_BOTTOM);
                break;
            }
            case ID_WIN_DROPDOWN: {     /* Dropdown */
                uint8_t i;
                handle = GUI_DROPDOWN_Create(0, 10, 10, 200, 40, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_DROPDOWN_AddString(handle, listboxtexts[i]);
                }
                GUI_DROPDOWN_SetSliderAuto(handle, 0);
                GUI_DROPDOWN_SetSliderVisibility(handle, 1);
                
                handle = GUI_DROPDOWN_Create(0, 220, 180, 200, 40, h, 0, 0);
                GUI_DROPDOWN_SetOpenDirection(handle, GUI_DROPDOWN_OPENDIR_UP);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    GUI_DROPDOWN_AddString(handle, listboxtexts[i]);
                }
                break;
            }
            case ID_WIN_IMAGE: {
                handle = GUI_BUTTON_Create(0, 10, 10, 300, 50, h, 0, 0);
                GUI_WIDGET_SetText(handle, _T("Button"));
//                handle = GUI_IMAGE_Create(0, 2, 2, bmimage_brand.xSize, bmimage_brand.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &bmimage_brand);
//                handle = GUI_IMAGE_Create(0, 200, 40, bmimage_voyo.xSize, bmimage_voyo.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &bmimage_voyo);
//                handle = GUI_IMAGE_Create(0, 350, 10, bmimage_voyo565.xSize, bmimage_voyo565.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &bmimage_voyo565);
//                handle = GUI_IMAGE_Create(0, 250, 10, maskImg.xSize, maskImg.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &maskImg);
                break;
            }
            case ID_WIN_SLIDER: {
                handle = GUI_SLIDER_Create(0, 5, 5, 400, 50, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_RIGHT_LEFT);
                handle = GUI_SLIDER_Create(0, 5, 70, 400, 15, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_LEFT_RIGHT);
                handle = GUI_SLIDER_Create(0, 430, 10, 10, 230, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_BOTTOM_TOP);
                handle = GUI_SLIDER_Create(0, 450, 10, 20, 230, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_TOP_BOTTOM);
                break;
            }
            default:
                break;  
        }
    }
    return res;
}

uint8_t radio_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_SelectionChanged) {
        uint32_t group = GUI_RADIO_GetGroup(h);
        if (group == RADIO_GROUP_VALIGN || group == RADIO_GROUP_HALIGN) {
            GUI_HANDLE_p textview = GUI_WIDGET_GetById(ID_TEXTVIEW_1);
            if (textview) {
                if (group == RADIO_GROUP_VALIGN) {
                    GUI_TEXTVIEW_SetVAlign(textview, (GUI_TEXTVIEW_VALIGN_t)GUI_RADIO_GetValue(h));
                } else if (group == RADIO_GROUP_HALIGN) {
                    GUI_TEXTVIEW_SetHAlign(textview, (GUI_TEXTVIEW_HALIGN_t)GUI_RADIO_GetValue(h));
                }
            }
        }
    }
    return ret;
}

uint8_t led_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    return ret;
}

uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_ValueChanged) {
        switch (GUI_WIDGET_GetId(h)) {
            case ID_CHECKBOX_LED: {
                if (GUI_CHECKBOX_IsChecked(h)) {
                    TM_DISCO_LedOn(LED_ALL);
                } else {
                    TM_DISCO_LedOff(LED_ALL);
                }
                break;
            }
            case ID_CHECKBOX_GRAPH: {
                GUI_HANDLE_p handle = GUI_WIDGET_GetById(ID_GRAPH_MAIN); /* Find graph widget if exists */
                if (handle) {
                    GUI_WIDGET_SetExpanded(handle, GUI_CHECKBOX_IsChecked(h));
                }
                break;
            }
        }
    }
    return ret;
}

uint8_t listview_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_SelectionChanged) {
        __GUI_DEBUG("Selection changed!\r\n");
    }
    return ret;
}

uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    GUI_ID_t id;
    uint8_t res = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            break;
        }
        case GUI_WC_Click: {
            id = GUI_WIDGET_GetId(h);
            switch (id) {
                case ID_BTN_WIN_BTN:
                case ID_BTN_WIN_CHECKBOX:
                case ID_BTN_WIN_DROPDOWN:
                case ID_BTN_WIN_EDIT:
                case ID_BTN_WIN_GRAPH:
                case ID_BTN_WIN_IMAGE:
                case ID_BTN_WIN_LED:
                case ID_BTN_WIN_LISTBOX:
                case ID_BTN_WIN_LISTVIEW:
                case ID_BTN_WIN_PROGBAR:
                case ID_BTN_WIN_RADIO:
                case ID_BTN_WIN_TEXTVIEW:
                case ID_BTN_WIN_SLIDER: {
                    btn_user_data_t* data = GUI_WIDGET_GetUserData(h);
                    uint32_t diff = id - ID_BTN_WIN_BTN;
                    if (data) {
                        GUI_HANDLE_p tmp;
                        if ((tmp = GUI_WIDGET_GetById(data->win_id)) != 0) {
                            GUI_WIDGET_Show(tmp);
                            GUI_WIDGET_PutOnFront(tmp);
                        } else {
                            //tmp = GUI_WINDOW_CreateChild(data->win_id, 5, 5, 470, 262, GUI_WINDOW_GetDesktop(), window_callback, NULL);
                            tmp = GUI_WINDOW_Create(data->win_id, 40, 20, 300, 200, GUI_WINDOW_GetDesktop(), window_callback, NULL);
                            GUI_WIDGET_SetExpanded(tmp, 1);
                            GUI_WIDGET_SetText(tmp, data->win_text);
                            GUI_WIDGET_SetZIndex(tmp, diff);
                            GUI_WIDGET_PutOnFront(tmp);
                        }
                    }
                    break;
                }
                case ID_BTN_WIN_DIALOG: {
                    GUI_DIALOG_Create(ID_WIN_DIALOG, 60, 20, 300, 150, 0, dialog_callback, 0);
                    break;
                }
                case ID_BTN_DIALOG_CONFIRM:
                case ID_BTN_DIALOG_CANCEL: {
                    GUI_DIALOG_Dismiss(GUI_WIDGET_GetById(ID_WIN_DIALOG), ID_BTN_DIALOG_CONFIRM == id);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return res;
}

uint8_t dialog_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t res = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            handle = GUI_TEXTVIEW_Create(0, 10, 10, GUI_WIDGET_GetWidth(h) - 20, GUI_WIDGET_GetHeight(h) - 52, h, 0, 0);
            if (TM_DISCO_LedIsOn(LED_GREEN)) {
                GUI_WIDGET_SetText(handle, _T("LED is ON. Do you want to turn it off?"));
            } else  {
                GUI_WIDGET_SetText(handle, _T("LED is OFF. Do you want to turn it on?"));
            }
            handle = GUI_BUTTON_Create(ID_BTN_DIALOG_CONFIRM, 0,   118, 150, 32, h, button_callback, 0);
            GUI_WIDGET_SetText(handle, _T("OK"));
            handle = GUI_BUTTON_Create(ID_BTN_DIALOG_CANCEL,  150, 118, 150, 32, h, button_callback, 0);
            GUI_WIDGET_SetText(handle, _T("CANCEL"));
            GUI_WIDGET_SetFocus(handle);
            break;
        }
        case GUI_WC_Draw: {
            break;
        }
        default:
            break;
    }
    return res;
}

uint8_t slider_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t res = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            GUI_SLIDER_SetMin(h, 0);
            GUI_SLIDER_SetMax(h, 1000);
            GUI_SLIDER_SetValue(h, 212);
            break;
        }
        case GUI_WC_ValueChanged: {
            int32_t value = GUI_SLIDER_GetValue(h); /* Get widget handle */
            __GUI_DEBUG("V: %d\r\n", value);
            //Set timer PWM
            break;
        }
        default:
            break;
    }
    return res;
}

/* 1ms handler */
void TM_DELAY_1msHandler() {
    //osSystickHandler();                             /* Kernel systick handler processing */
    
    GUI_UpdateTime(1);
}

/* printf handler */
int fputc(int ch, FILE* fil) {
    TM_USART_Putc(DISCO_USART, ch);
    return ch;
}

/**
 * Handle EXTI interrupt routine
 */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
    static GUI_TouchData_t p = {0}, t = {0};
    if (GPIO_PIN_13 == GPIO_Pin) {
        uint8_t i, update = 0, diffx, diffy;
        TM_TOUCH_Read(&TS);                         /* Read touch data */
        
        memset((void *)&t, 0x00, sizeof(t));
        t.Status = TS.NumPresses ? GUI_TouchState_PRESSED : GUI_TouchState_RELEASED;
        t.Count = TS.NumPresses > GUI_TOUCH_MAX_PRESSES ? GUI_TOUCH_MAX_PRESSES : TS.NumPresses;
        for (i = 0; i < t.Count; i++) {
            t.X[i] = TS.X[i];
            t.Y[i] = TS.Y[i];
        }
        if (t.Count != p.Count) {
            update = 1;
        } else {
            for (i = 0; i < t.Count; i++) {
                diffx = __GUI_ABS(p.X[i] - t.X[i]);
                diffy = __GUI_ABS(p.Y[i] - t.Y[i]);
                if (diffx > 2 || diffy > 2) {
                    update = 1;
                    break;
                }
            }
        }
        
        /* Check differences */
        if (update || t.Status == GUI_TouchState_RELEASED) {
            GUI_INPUT_TouchAdd(&t);
            memcpy(&p, &t, sizeof(p));
        }
    } else if (DISCO_BUTTON_PIN == GPIO_Pin) {
        pressed = 1;
    }
}

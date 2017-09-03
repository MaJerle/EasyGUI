/**
 * Keil project example for Graphical User Interface software
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

#include "tm_stm32_general.h"
#include "main.h"
#include "tm_stm32_sdram.h"
//#define GUI_INTERNAL
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
#include "gui_keyboard.h"
#include "gui_container.h"

#include "math.h"

#include "cmsis_os.h"
#include "cpu_utils.h"

#define COUNT_OF(x)     (sizeof(x) / sizeof((x)[0]))

TM_TOUCH_t TS;

GUI_HANDLE_p win1, win2, handle;
GUI_GRAPH_DATA_p graphdata1, graphdata2, graphdata3, graphdata4;
GUI_HANDLE_p dialog1;

#define ID_BASE                 (GUI_ID_USER)
#define ID_BASE_WIN             (ID_BASE + 0x0100)
#define ID_BASE_BTN             (ID_BASE_WIN + 0x0100)
#define ID_BASE_TEXTWIEW        (ID_BASE_BTN + 0x0100)
#define ID_BASE_EDITTEXT        (ID_BASE_TEXTWIEW + 0x0100)
#define ID_BASE_CHECKBOX        (ID_BASE_TEXTWIEW + 0x0100)
#define ID_BASE_LED             (ID_BASE_CHECKBOX + 0x0100)
#define ID_BASE_GRAPH           (ID_BASE_LED + 0x0100)
#define ID_BASE_SLIDER          (ID_BASE_GRAPH + 0x0100)

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
#define ID_WIN_ZINDEX           (ID_BASE_WIN + 0x0F)
#define ID_WIN_TRANSP           (ID_BASE_WIN + 0x10)
#define ID_WIN_WINDOW           (ID_BASE_WIN + 0x11)

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
#define ID_BTN_WIN_ZINDEX       (ID_BASE_BTN + 0x0F)
#define ID_BTN_WIN_TRANSP       (ID_BASE_BTN + 0x10)
#define ID_BTN_WIN_WINDOW       (ID_BASE_BTN + 0x11)

#define ID_BTN_DIALOG_CONFIRM   (ID_BASE_BTN + 0x20)
#define ID_BTN_DIALOG_CANCEL    (ID_BASE_BTN + 0x21)

#define ID_TEXTVIEW_1           (ID_BASE_TEXTWIEW + 0x01)
#define ID_TEXTVIEW_2           (ID_BASE_TEXTWIEW + 0x02)

#define ID_CHECKBOX_LED         (ID_BASE_CHECKBOX + 0x01)
#define ID_CHECKBOX_GRAPH       (ID_BASE_CHECKBOX + 0x02)

#define ID_LED_1                (ID_BASE_LED + 0x01)
#define ID_LED_2                (ID_BASE_LED + 0x02)
#define ID_LED_3                (ID_BASE_LED + 0x03)
#define ID_LED_4                (ID_BASE_LED + 0x04)

#define ID_GRAPH_MAIN           (ID_BASE_GRAPH + 0x01)

#define ID_SLIDER_0             (ID_BASE_SLIDER + 0x01)
#define ID_SLIDER_1             (ID_BASE_SLIDER + 0x02)

#define ID_BUTTON_0             (ID_BASE_BTN + 0x040)
#define ID_BUTTON_1             (ID_BASE_BTN + 0x041)
#define ID_BUTTON_2             (ID_BASE_BTN + 0x042)
#define ID_BUTTON_3             (ID_BASE_BTN + 0x043)

#define ID_EDITTEXT_1           (ID_BASE_EDITTEXT + 0x01)

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
    {ID_BTN_WIN_BTN,        _GT("Buttons"),      {ID_WIN_BTN, _GT("Buttons")}},
    {ID_BTN_WIN_EDIT,       _GT("Edit text"),    {ID_WIN_EDIT, _GT("Edit text")}},
    {ID_BTN_WIN_RADIO,      _GT("Radio box"),    {ID_WIN_RADIO, _GT("Radio box")}},
    {ID_BTN_WIN_CHECKBOX,   _GT("Check box"),    {ID_WIN_CHECKBOX, _GT("Check box")}},
    {ID_BTN_WIN_PROGBAR,    _GT("Progress bar"), {ID_WIN_PROGBAR, _GT("Progress bar")}},
    {ID_BTN_WIN_GRAPH,      _GT("Graph"),        {ID_WIN_GRAPH, _GT("Graph")}},
    {ID_BTN_WIN_LISTBOX,    _GT("List box"),     {ID_WIN_LISTBOX, _GT("List box")}},
    {ID_BTN_WIN_LED,        _GT("Led"),          {ID_WIN_LED, _GT("Led")}},
    {ID_BTN_WIN_TEXTVIEW,   _GT("Text view"),    {ID_WIN_TEXTVIEW, _GT("Text view")}},
    {ID_BTN_WIN_DROPDOWN,   _GT("Dropdown"),     {ID_WIN_DROPDOWN, _GT("Dropdown")}},
    {ID_BTN_WIN_DIALOG,     _GT("Dialog"),       {ID_WIN_DIALOG, _GT("Dialog")}},
    {ID_BTN_WIN_LISTVIEW,   _GT("Listview"),     {ID_WIN_LISTVIEW, _GT("Listview")}},
    {ID_BTN_WIN_IMAGE,      _GT("Image"),        {ID_WIN_IMAGE, _GT("Image")}},
    {ID_BTN_WIN_SLIDER,     _GT("Slider"),       {ID_WIN_SLIDER, _GT("Slider")}},
    {ID_BTN_WIN_ZINDEX,     _GT("z-index"),      {ID_WIN_ZINDEX, _GT("z-index test page")}},
    {ID_BTN_WIN_TRANSP,     _GT("Transparency"), {ID_WIN_TRANSP, _GT("Transparency presentation")}},
    {ID_BTN_WIN_WINDOW,     _GT("Window"),       {ID_WIN_WINDOW, _GT("Window")}},
};

char str[100];

extern const GUI_FONT_t GUI_Font_Comic_Sans_MS_Regular_22;
extern GUI_Const GUI_FONT_t GUI_Font_FontAwesome_Regular_30;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Narrow_Italic_21_AA;
extern const GUI_FONT_t GUI_Font_Roboto_Italic_14;

uint32_t time;

GUI_Char* listboxtexts[] = {
    _GT("Item 0"),
    _GT("Item 1"),
    _GT("Item 2"),
    _GT("Item 3"),
    _GT("Item 4"),
    _GT("Item 5"),
    _GT("Item 6"),
    _GT("Item 7"),
    _GT("Item 8"),
    _GT("Item 9"),
    _GT("Item 10"),
    _GT("Item 11"),
    _GT("Item 12"),
};

uint8_t window_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t radio_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t led_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t dialog_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t listview_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t slider_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);
uint8_t edittext_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);

#define PI      3.14159265359f

static float len = 72, radius = 90;
float x, y;

int pressed = 0;

extern GUI_t GUI;

#define IMG_WIDTH       64
#define IMG_HEIGHT      64

const GUI_Char myStr[30] = "Hello world!\r\n";
GUI_STRING_t ptr;

//Create table for English entries
const GUI_Char* languageEnglishEntries[] = {
    _GT("Button"),
    _GT("Dropdown"),
    _GT("Listview"),
    _GT("Your string here without translate."),
};

//Create table for German entries
const GUI_Char* languageGermanEntries[] = {
    _GT("Taste"),           //Translate for "Button"
    _GT("Dropdown-Liste"),  //Translate for "Dropdown"
    _GT("Listenansicht"),   //Translate for "Listview"
    //Missing translate for "Your string here without translate."
};

//Create English language table
GUI_TRANSLATE_Language_t languageEnglish = {
    .Entries = languageEnglishEntries,
    .Count = GUI_COUNT_OF(languageEnglishEntries)
};

//Create German language structure
GUI_TRANSLATE_Language_t languageGerman = {
    .Entries = languageGermanEntries,
    .Count = GUI_COUNT_OF(languageGermanEntries)
};

size_t i;
GUI_HANDLE_p h;

void read_touch(void);

void user_thread(void const * arg);
void touch_thread(void const * arg);

osThreadDef(user_thread, user_thread, osPriorityNormal, 0, 512);
osThreadId user_thread_id;
osThreadDef(touch_thread, touch_thread, osPriorityNormal, 0, 512);
osThreadId user_thread_id, touch_thread_id;

int main(void) {
    //hardfault debug code
    SCB->CCR |= 0x10;
    
    TM_RCC_InitSystem();                        /* Init system */
    HAL_Init();                                 /* Init HAL layer */
    TM_DISCO_LedInit();                         /* Init leds */
    TM_DISCO_ButtonInit();                      /* Init button */
    TM_DELAY_Init();                            /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 115200*4);   /* Init USART for debug purpose */
    
    /* Print first screen message */
    printf("GUI; Compiled: %s %s\r\n", __DATE__, __TIME__);
    TM_GENERAL_DWTCounterEnable();
    
    /* Main button as interrupt */
    TM_EXTI_Attach(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, TM_EXTI_Trigger_Rising);
    
    /* Create thread for user */
    user_thread_id = osThreadCreate(osThread(user_thread), NULL);
    
    osKernelStart();                            /* Start RTOS kernel */
    
	while (1) {        

	}
}

/**
 * \brief           User thread for GUI
 * \param[in]       *arg: Pointer to argument for thread
 */
static void
user_thread(void const * arg) {
    GUI_STRING_UNICODE_t s;
    
    GUI_KeyboardData_t key;
    uint32_t state;
    
    GUI_Init();                                 /* Initialize GUI as RTOS */
    
    GUI_WIDGET_SetFontDefault(&GUI_Font_Arial_Narrow_Italic_21_AA); /* Set default font for widgets */
    //GUI_WIDGET_SetFontDefault(&GUI_Font_Comic_Sans_MS_Regular_22); /* Set default font for widgets */
    //GUI_WIDGET_SetFontDefault(&GUI_Font_Roboto_Italic_14); /* Set default font for widgets */
    
    /* Create all widgets on screen */
    win1 = GUI_WINDOW_GetDesktop();             /* Get desktop window */
    for (state = 0; state < GUI_COUNT_OF(buttons); state++) {
        handle = GUI_BUTTON_Create(buttons[state].id, 5 + (state % 3) * 160, 5 + (state / 3) * 40, 150, 35, win1, button_callback, 0);
        GUI_WIDGET_SetText(handle, buttons[state].text);
        GUI_WIDGET_SetUserData(handle, &buttons[state].data);
        GUI_WIDGET_Set3DStyle(handle, 0);
    }
    
    GUI_KEYBOARD_Create();                      /* Create virtual keyboard */
    
    touch_thread_id = osThreadCreate(osThread(touch_thread), NULL); /* Create touch thread */
    
    while (1) {                                 /* Start thread execution */
        osDelay(1000);
        //printf("CPU usage: %d\r\n", osGetCPUUsage());
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
        
        /**
         * On pressed button, open dialog in blocking and wait for user press to continue
         */
        if (pressed) {
            pressed = GUI_DIALOG_CreateBlocking(ID_WIN_DIALOG, 50, 10, 300, 150, 0, dialog_callback, 0);
            if (pressed == 1) {
                __GUI_DEBUG("Confirmed\r\n");
                TM_DISCO_LedToggle(LED_GREEN);
            } else if (pressed == 0) {
                __GUI_DEBUG("Canceled\r\n");
            }
            pressed = 0;
        }
        
        while (!TM_USART_BufferEmpty(DISCO_USART)) {
            GUI_Char ch = TM_USART_Getc(DISCO_USART);
            switch (GUI_STRING_UNICODE_Decode(&s, ch)) {
                case UNICODE_OK:
                    if (ch == 'l') {
                        __GUI_LINKEDLIST_PrintList();
                        break;
                    }
                    if (ch == 'b') {
                        TM_USART_Send(DISCO_USART, (uint8_t *)GUI.LCD.ActiveLayer->StartAddress, (size_t)GUI.LCD.ActiveLayer->Width * (size_t)GUI.LCD.ActiveLayer->Height * (size_t)GUI.LCD.PixelSize);
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
                    } else if (ch == GUI_KEY_CR) {
                        ch = GUI_KEY_LF;
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

osSemaphoreId touch_semaphore;

/**
 * \brief           Read touch from controller
 * \param[in]       *arg: Pointer to user argument
 */
static void
touch_thread(void const * arg) {
#if !defined(STM32F4xx)
    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
#endif
    TS.Orientation = TOUCH_ORIENT_DEFAULT;
#if defined(STM32F769_DISCOVERY)
    TS.Orientation = TOUCH_ORIENT_INVERT_Y;
#endif
    TM_TOUCH_Init(NULL, &TS);
    
    /* Create semaphore */
    osSemaphoreDef(touch_semaphore);
    touch_semaphore = osSemaphoreCreate(osSemaphore(touch_semaphore), 1);
    
    /* Take it immediatelly */
    osSemaphoreWait(touch_semaphore, osWaitForever);    /* Wait for semaphore */
    while (1) {
        if (osSemaphoreWait(touch_semaphore, osWaitForever) == osOK) {
            read_touch();                       /* Read touch and send to GUI stack */
        }
    }
}

uint8_t window_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_Init) {           /* Window has been just initialized */
        switch (GUI_WIDGET_GetId(h)) {  /* Button callbacks */
            case ID_WIN_BTN: {
                handle = GUI_BUTTON_Create(0, 0, 0, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _GT("Button 1"));
                GUI_WIDGET_SetPositionPercent(handle, 0, 0);
                GUI_WIDGET_SetSizePercent(handle, 50, 50);
                
                handle = GUI_BUTTON_Create(0, 10, 60, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _GT("Button 2"));
                GUI_WIDGET_SetPositionPercent(handle, 50, 0);
                GUI_WIDGET_SetSizePercent(handle, 50, 50);
                
                handle = GUI_BUTTON_Create(0, 0, 0, 100, 40, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _GT("Button 3"));
                GUI_WIDGET_SetPositionPercent(handle, 0, 50);
                GUI_WIDGET_SetSizePercent(handle, 50, 50);
                
                handle = GUI_BUTTON_Create(0, 10, 60, 100, 30, h, button_callback, 0);
                GUI_WIDGET_SetText(handle, _GT("Button 4"));
                GUI_WIDGET_SetPositionPercent(handle, 50, 50);
                GUI_WIDGET_SetSizePercent(handle, 50, 50);
                break;
            }
            case ID_WIN_CHECKBOX: {     /* Check box */
                handle = GUI_CHECKBOX_Create(ID_CHECKBOX_LED, 10, 10, 400, 40, h, checkbox_callback, 0);
                GUI_WIDGET_SetText(handle, _GT("Check box default"));
                handle = GUI_CHECKBOX_Create(1, 10, 50, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Check box checked"));
                GUI_CHECKBOX_SetChecked(handle, 1);
                handle = GUI_CHECKBOX_Create(ID_CHECKBOX_LED, 10, 90, 400, 40, h, checkbox_callback, 0);
                GUI_WIDGET_SetText(handle, _GT("Check box disabled"));
                GUI_CHECKBOX_SetDisabled(handle, 1);
                handle = GUI_CHECKBOX_Create(1, 10, 130, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Check box checked disabled"));
                GUI_CHECKBOX_SetChecked(handle, 1);
                GUI_CHECKBOX_SetDisabled(handle, 1);
                break;
            }
            case ID_WIN_RADIO: {        /* Radio box */
                uint8_t i;
                for (i = 0; i < 4; i++) {
                    handle = GUI_RADIO_Create(10, 10, 10 + (i * 30), 300, 25, h, 0, 0);
                    switch (i) {
                        case 0: GUI_WIDGET_SetText(handle, _GT("Radio box default")); break;
                        case 1: GUI_WIDGET_SetText(handle, _GT("Radio box selected")); break;
                        case 2: GUI_WIDGET_SetText(handle, _GT("Radio box disabled")); break;
                        case 3: GUI_WIDGET_SetText(handle, _GT("Radio box disabled checked")); break;
                        default: break;
                    }
                    GUI_RADIO_SetGroup(handle, i < 2 ? 1 : 2);
                    GUI_RADIO_SetValue(handle, i);
                    if (i % 2) {
                        GUI_RADIO_SetSelected(handle);
                    }
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
                size_t len;
                GUI_LISTVIEW_ROW_p row;
                handle = GUI_LISTVIEW_Create(0, 10, 10, 100, 40, h, listview_callback, 0);
                GUI_WIDGET_SetExpanded(handle, 1);
                GUI_WIDGET_SetFont(handle, &GUI_Font_Roboto_Italic_14);
                
                GUI_LISTVIEW_AddColumn(handle, _GT("Name"), 120);
                GUI_LISTVIEW_AddColumn(handle, _GT("LastName"), 150);
                GUI_LISTVIEW_AddColumn(handle, _GT("Email"), 220);
                GUI_LISTVIEW_AddColumn(handle, _GT("Phone"), 160);
                
                for (len = 0; len < 10; len++) {
                    row = GUI_LISTVIEW_AddRow(handle);
                    GUI_LISTVIEW_SetItemString(handle, row, 0, _GT("Name 1"));
                    GUI_LISTVIEW_SetItemString(handle, row, 1, _GT("Last name 2"));
                    GUI_LISTVIEW_SetItemString(handle, row, 2, _GT("info@example.com"));
                    GUI_LISTVIEW_SetItemString(handle, row, 3, _GT("0123456789"));
                }
                
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

                //Create data objects
                if (!graphdata1) {
                    graphdata1 = GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_XY, len);
                    graphdata1->Color = GUI_COLOR_RED;
                }
                if (!graphdata2) {
                    graphdata2 = GUI_GRAPH_DATA_Create(GUI_GRAPH_TYPE_YT, len / 2);
                    graphdata2->Color = GUI_COLOR_GREEN;
                }

                //Fill data objects
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
                GUI_WIDGET_SetText(handle, _GT("SetExpanded"));
                GUI_WIDGET_SetZIndex(handle, 1);
                GUI_CHECKBOX_SetColor(handle, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_WHITE);
                GUI_CHECKBOX_SetChecked(handle, GUI_WIDGET_IsExpanded(GUI_WIDGET_GetById(ID_GRAPH_MAIN)));
                break;
            }
            case ID_WIN_EDIT: {         /* Edit text */
                handle = GUI_EDITTEXT_Create(1, 10, 10, 400, 40, h, 0, 0);
                GUI_WIDGET_AllocTextMemory(handle, 255);
                GUI_WIDGET_SetText(handle, _GT("Edit text very long text OLA CIKAS OLA CIKAS OLA CIKASS"));
                GUI_WIDGET_SetFont(handle, &GUI_Font_Arial_Narrow_Italic_21_AA);
                
                /* Create text field first to hold view of edit text*/
                handle = GUI_TEXTVIEW_Create(ID_TEXTVIEW_2, 10, 210, 400, 150, h, 0, 0);
                
                /* edittext_callback is responsible to set display text from edit text to text view */
                handle = GUI_EDITTEXT_Create(ID_EDITTEXT_1, 10, 60, 400, 150, h, edittext_callback, 0);
                GUI_WIDGET_AllocTextMemory(handle, 255);
                GUI_EDITTEXT_SetMultiline(handle, 1);
                GUI_EDITTEXT_SetVAlign(handle, GUI_EDITTEXT_VALIGN_TOP);
                GUI_WIDGET_SetText(handle, _GT("Edit\ntext\nmultiline\n\n\n\nTextText\nTexttttt\nOLAAA\nOLAAAAAAA\n\nEvenMoreText\nMoreee\n"));
                break;
            }
            case ID_WIN_PROGBAR: {      /* Progress bar */
                handle = GUI_PROGBAR_Create(2, 10, 10, 400, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Progress bar with custom text"));
                
                handle = GUI_PROGBAR_Create(2, 10, 60, 400, 40, h, 0, 0);
                GUI_PROGBAR_EnablePercentages(handle);
                GUI_PROGBAR_SetMin(handle, 20);
                GUI_PROGBAR_SetMax(handle, 30);
                GUI_PROGBAR_SetValue(handle, 22);
                break;
            }
            case ID_WIN_LED: {          /* Leds */                
                handle = GUI_LED_Create(ID_LED_1, 10, 80, 30, 30, 0, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_CIRCLE);
                GUI_LED_Set(handle, 1);
                
                handle = GUI_LED_Create(ID_LED_2, 50, 80, 30, 30, 0, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_CIRCLE);
                
                handle = GUI_LED_Create(ID_LED_3, 90, 80, 30, 30, 0, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_RECT);
                GUI_LED_Set(handle, 1);
                handle = GUI_LED_Create(ID_LED_4, 130, 80, 30, 30, 0, led_callback, 0);
                GUI_LED_SetType(handle, GUI_LED_TYPE_RECT);
                
                handle = GUI_TEXTVIEW_Create(0, 40, 10, 400, 1000, h, 0, 0);
                GUI_WIDGET_SetFont(handle, &GUI_Font_Roboto_Italic_14);
                GUI_WIDGET_SetText(handle, _GT("\"LEDs\" are widgets used to indicate some status or any other situation. Press blue button on discovery board to see LED in happen"));
                GUI_TEXTVIEW_SetColor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_TRANS);
                GUI_WIDGET_SetZIndex(handle, -1);
                break;
            }
            case ID_WIN_TEXTVIEW: {     /* Text view */
                size_t i = 0;
                static const GUI_Char* texts[] = {
                    _GT("Text majkemi moje\neeeeeon screen"),
                    _GT("Text\nview with\ntop center alignment on screen"),
                    _GT("Text view with top right alignment on screen"),
                    _GT("Text view                          with middle left alignment on screen"),
                    _GT("Text view with middle center alignment on screen"),
                    _GT("Text view with                         middle right alignment on screen"),
                    _GT("Text view                          with bottom left alignment on screen"),
                    _GT("Text view with bottom center alignment on screen"),
                    _GT("Text view with bottom right                          alignment on screen koji kurac se to dogaja?"),
                };
                for (i = 0; i < 9; i++) {
                    handle = GUI_TEXTVIEW_Create(0, 1, 1, 1, 1, 0, 0, 0);
                    GUI_WIDGET_SetSizePercent(handle, 30, 30);
                    GUI_WIDGET_SetPositionPercent(handle, 3 + (i % 3) * 33, 3 + (i / 3) * 33);
                    GUI_WIDGET_AllocTextMemory(handle, 255);
                    GUI_WIDGET_SetText(handle, texts[i]);
                    GUI_WIDGET_SetFont(handle, &GUI_Font_Roboto_Italic_14);
                    switch (i % 4) {
                        case 0: GUI_TEXTVIEW_SetColor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_WHITE); break;
                        case 1: GUI_TEXTVIEW_SetColor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_YELLOW); break;
                        case 2: GUI_TEXTVIEW_SetColor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_GRAY); break;
                        case 3: GUI_TEXTVIEW_SetColor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_LIGHTGREEN); break;
                    }
                    switch (i % 3) {
                        case 0: GUI_TEXTVIEW_SetHAlign(handle, GUI_TEXTVIEW_HALIGN_LEFT); break;
                        case 1: GUI_TEXTVIEW_SetHAlign(handle, GUI_TEXTVIEW_HALIGN_CENTER); break;
                        case 2: GUI_TEXTVIEW_SetHAlign(handle, GUI_TEXTVIEW_HALIGN_RIGHT); break;
                    }
                    switch (i / 3) {
                        case 0: GUI_TEXTVIEW_SetVAlign(handle, GUI_TEXTVIEW_VALIGN_TOP); break;
                        case 1: GUI_TEXTVIEW_SetVAlign(handle, GUI_TEXTVIEW_VALIGN_CENTER); break;
                        case 2: GUI_TEXTVIEW_SetVAlign(handle, GUI_TEXTVIEW_VALIGN_BOTTOM); break;
                    }
                }
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
//                handle = GUI_BUTTON_Create(0, 10, 10, 300, 50, h, 0, 0);
//                GUI_WIDGET_SetText(handle, _GT("Button"));
//                handle = GUI_IMAGE_Create(0, 2, 2, bmimage_brand.xSize, bmimage_brand.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &bmimage_brand);
//                handle = GUI_IMAGE_Create(0, 200, 40, bmimage_voyo.xSize, bmimage_voyo.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &bmimage_voyo);
//                handle = GUI_IMAGE_Create(0, 350, 10, bmimage_voyo565.xSize, bmimage_voyo565.ySize, h, 0, 0);
//                GUI_IMAGE_SetSource(handle, &bmimage_voyo565);
                break;
            }
            case ID_WIN_SLIDER: {
                GUI_BUTTON_Create(ID_BUTTON_0, 5, 100, 100, 30, h, 0, 0);
               
                handle = GUI_SLIDER_Create(ID_SLIDER_0, 5, 5, 400, 50, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_LEFT_RIGHT);
                GUI_SLIDER_SetMin(handle, 100);
                GUI_SLIDER_SetMax(handle, 300);
                GUI_SLIDER_SetValue(handle, 200);
                
                handle = GUI_SLIDER_Create(0, 5, 70, 400, 15, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_RIGHT_LEFT);
                handle = GUI_SLIDER_Create(0, 430, 10, 10, 230, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_BOTTOM_TOP);
                handle = GUI_SLIDER_Create(0, 450, 10, 20, 230, h, slider_callback, 0);
                GUI_SLIDER_SetMode(handle, GUI_SLIDER_MODE_TOP_BOTTOM);
                break;
            }
            case ID_WIN_ZINDEX: {
                handle = GUI_TEXTVIEW_Create(0, 10, 10, 180, 100, 0, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("All buttons with same z-index. Auto z-index is allowed."));
                
                /* Create button group with the same index */
                handle = GUI_BUTTON_Create(0, 200, 10, 150, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Button1 z-i: 0"));
                handle = GUI_BUTTON_Create(0, 300, 20, 150, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Button2 z-i: 0"));
                handle = GUI_BUTTON_Create(0, 250, 30, 150, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Button3 z-i: 0"));
                
                
                handle = GUI_TEXTVIEW_Create(0, 10, 120, 180, 100, 0, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Each button has dedicated z-index. Auto z-index is not allowed."));
                handle = GUI_BUTTON_Create(0, 200, 120, 150, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Button4 z-i: 1"));
                GUI_WIDGET_SetZIndex(handle, 1);
                handle = GUI_BUTTON_Create(0, 300, 130, 150, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Button5 z-i: 3"));
                GUI_WIDGET_SetZIndex(handle, 3);
                handle = GUI_BUTTON_Create(0, 250, 140, 150, 40, h, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Button6 z-i: 2"));
                GUI_WIDGET_SetZIndex(handle, 2);
                break;
            }
            case ID_WIN_TRANSP: {
                //GUI_WIDGET_SetTransparency(h, 0xF0);
                
                handle = GUI_BUTTON_Create(ID_BUTTON_2, 10, 50, 300, 35, 0, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Opaque button"));
                GUI_WIDGET_SetTransparency(handle, 0xFF);
                handle = GUI_BUTTON_Create(ID_BUTTON_3, 10, 90, 300, 35, 0, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Opaque button"));
                GUI_WIDGET_SetTransparency(handle, 0xC0);
                
                handle = GUI_BUTTON_Create(ID_BUTTON_1, 5, 30, 300, 35, 0, 0, 0);
                GUI_WIDGET_SetText(handle, _GT("Transparent button"));
                GUI_WIDGET_SetTransparency(handle, 0x80);
                
                handle = GUI_SLIDER_Create(ID_SLIDER_1, 10, 150, 400, 50, 0, slider_callback, 0);
                GUI_SLIDER_SetMin(handle, 0);
                GUI_SLIDER_SetMax(handle, 0xFF);
                GUI_SLIDER_SetValue(handle, 0x80);
                break;
            }
            default:
                break;  
        }
    }
    return ret;
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
    if (cmd == GUI_WC_Click) {
        GUI_LED_Toggle(h);              /* Toggle status */
    }
    return ret;
}

uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    GUI_ID_t id;
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    
    id = GUI_WIDGET_GetId(h);
    if (cmd == GUI_WC_Init) {
        if (id == ID_CHECKBOX_LED) {
            GUI_CHECKBOX_SetChecked(h, TM_DISCO_LedIsOn(LED_GREEN));
        }
    } else if (cmd == GUI_WC_ValueChanged) {
        switch (id) {
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
        
    }
    return ret;
}

uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    GUI_ID_t id;
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
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
                case ID_BTN_WIN_SLIDER:
                case ID_BTN_WIN_ZINDEX:
                case ID_BTN_WIN_TRANSP: 
                case ID_BTN_WIN_WINDOW: {
                    btn_user_data_t* data = GUI_WIDGET_GetUserData(h);
                    uint32_t diff = id - ID_BTN_WIN_BTN;
                    if (data) {
                        GUI_HANDLE_p tmp;
                        if ((tmp = GUI_WIDGET_GetById(data->win_id)) != 0) {
                            GUI_WIDGET_Show(tmp);
                            GUI_WIDGET_PutOnFront(tmp);
                        } else {
                            //tmp = GUI_WINDOW_CreateChild(data->win_id, 5, 5, 470, 262, GUI_WINDOW_GetDesktop(), window_callback, NULL);
                            tmp = GUI_WINDOW_Create(data->win_id, 40, 20, 300, 200, GUI_WINDOW_GetDesktop(), window_callback, 0);
                            GUI_WIDGET_SetExpanded(tmp, 1);
                            GUI_WIDGET_SetText(tmp, data->win_text);
                            GUI_WIDGET_SetZIndex(tmp, diff);
                            GUI_WIDGET_PutOnFront(tmp);
                            if (data->win_id == ID_WIN_TRANSP) {
                                //GUI_WIDGET_SetTransparency(tmp, 0x80);
                            }
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
    return ret;
}

uint8_t dialog_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            GUI_HANDLE_p tmp;
            switch (GUI_WIDGET_GetId(h)) {
                case ID_WIN_DIALOG:
                    tmp = GUI_CONTAINER_Create(0, 0, 0, GUI_WIDGET_GetWidth(h), GUI_WIDGET_GetHeight(h), h, 0, 0);
                    handle = GUI_TEXTVIEW_Create(0, 10, 10, GUI_WIDGET_GetWidth(h) - 20, GUI_WIDGET_GetHeight(h) - 52, tmp, 0, 0);
                    if (TM_DISCO_LedIsOn(LED_GREEN)) {
                        GUI_WIDGET_SetText(handle, _GT("LED is ON. Do you want to turn it off?"));
                    } else  {
                        GUI_WIDGET_SetText(handle, _GT("LED is OFF. Do you want to turn it on?"));
                    }
                    handle = GUI_BUTTON_Create(ID_BTN_DIALOG_CONFIRM, 0,   118, 150, 32, tmp, button_callback, 0);
                    GUI_WIDGET_SetText(handle, _GT("OK"));
                    handle = GUI_BUTTON_Create(ID_BTN_DIALOG_CANCEL,  150, 118, 150, 32, tmp, button_callback, 0);
                    GUI_WIDGET_SetText(handle, _GT("CANCEL"));
                    GUI_WIDGET_SetFocus(handle);
                    break;
            }
        }
        case GUI_WC_Draw: {
            break;
        }
        default:
            break;
    }
    return ret;
}

uint8_t slider_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            GUI_SLIDER_SetMin(h, 0);
            GUI_SLIDER_SetMax(h, 300);
            GUI_SLIDER_SetValue(h, 212);
            break;
        }
        case GUI_WC_ValueChanged: {
            int32_t value = GUI_SLIDER_GetValue(h); /* Get widget handle */
            switch (GUI_WIDGET_GetId(h)) {
                case ID_SLIDER_0: {
                    GUI_HANDLE_p tmp;
                    tmp = GUI_WIDGET_GetById(ID_BUTTON_0);
                    if (tmp) {
                        GUI_WIDGET_SetSize(tmp, value, value / 4);
                    }
                    break;
                }
                case ID_SLIDER_1: {
                    GUI_HANDLE_p tmp;
                    tmp = GUI_WIDGET_GetById(ID_BUTTON_1);
                    if (tmp) {
                        GUI_WIDGET_SetTransparency(tmp, value);
                    }
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

uint8_t edittext_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = GUI_WIDGET_ProcessDefaultCallback(h, cmd, param, result);
    if (cmd == GUI_WC_TextChanged) {
        GUI_ID_t id = GUI_WIDGET_GetId(h);          /* Get widget ID */
        switch (id) {
            case ID_EDITTEXT_1: {
                GUI_HANDLE_p tmp = GUI_WIDGET_GetById(ID_TEXTVIEW_2);
                if (tmp) {
                    GUI_WIDGET_SetText(tmp, GUI_WIDGET_GetText(h));
                    GUI_WIDGET_Invalidate(tmp);
                }
                break;
            }
        }
    }
    return ret;
}

void read_touch(void) {
    static GUI_TouchData_t p = {0}, t = {0};
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
}

/* 1ms handler */
void TM_DELAY_1msHandler() {
    //osSystickHandler();                             /* Kernel systick handler processing */
#if defined(STM32F439_EVAL)
    static uint32_t Time = 0;
    if (Time % 20 == 0) {
        read_touch();
    }
    Time++;
#endif
    osSystickHandler();
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
    if (GPIO_PIN_13 == GPIO_Pin) {
        read_touch();
        if (touch_semaphore) {
            //osSemaphoreRelease(touch_semaphore);
        }
    } else if (DISCO_BUTTON_PIN == GPIO_Pin) {
        pressed = 1;
    }
}

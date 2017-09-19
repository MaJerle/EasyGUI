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
#include "gui_lcd.h"

#include "math.h"

#include "ff.h"

#include "cmsis_os.h"
#include "cpu_utils.h"

TM_TOUCH_t TS;

GUI_HANDLE_p win1, win2, handle;
GUI_GRAPH_DATA_p graphdata1, graphdata2, graphdata3, graphdata4;
GUI_HANDLE_p dialog1;

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
uint8_t progbar_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result);

/**
 * \brief           
 */
bulk_init_t frontedWidgets[] = {
    {ID_BTN_WIN_BTN,        _GT("Buttons"),         gui_button_create,      button_callback,    {ID_WIN_BTN, _GT("Buttons")}},
    {ID_BTN_WIN_EDIT,       _GT("Edit text"),       gui_button_create,      button_callback,    {ID_WIN_EDIT, _GT("Edit text")}},
    {ID_BTN_WIN_RADIO,      _GT("Radio box"),       gui_button_create,      button_callback,    {ID_WIN_RADIO, _GT("Radio box")}},
    {ID_BTN_WIN_CHECKBOX,   _GT("Check box"),       gui_button_create,      button_callback,    {ID_WIN_CHECKBOX, _GT("Check box")}},
    {ID_BTN_WIN_PROGBAR,    _GT("Progress bar"),    gui_button_create,      button_callback,    {ID_WIN_PROGBAR, _GT("Progress bar")}},
    {ID_BTN_WIN_GRAPH,      _GT("Graph"),           gui_button_create,      button_callback,    {ID_WIN_GRAPH, _GT("Graph")}},
    {ID_BTN_WIN_LISTBOX,    _GT("List box"),        gui_button_create,      button_callback,    {ID_WIN_LISTBOX, _GT("List box")}},
    {ID_BTN_WIN_LED,        _GT("Led"),             gui_button_create,      button_callback,    {ID_WIN_LED, _GT("Led")}},
    {ID_BTN_WIN_TEXTVIEW,   _GT("Text view"),       gui_button_create,      button_callback,    {ID_WIN_TEXTVIEW, _GT("Text view")}},
    {ID_BTN_WIN_DROPDOWN,   _GT("Dropdown"),        gui_button_create,      button_callback,    {ID_WIN_DROPDOWN, _GT("Dropdown")}},
    {ID_BTN_WIN_DIALOG,     _GT("Dialog"),          gui_button_create,      button_callback,    {ID_WIN_DIALOG, _GT("Dialog")}},
    {ID_BTN_WIN_LISTVIEW,   _GT("Listview"),        gui_button_create,      button_callback,    {ID_WIN_LISTVIEW, _GT("Listview")}},
    {ID_BTN_WIN_IMAGE,      _GT("Image"),           gui_button_create,      button_callback,    {ID_WIN_IMAGE, _GT("Image")}},
    {ID_BTN_WIN_SLIDER,     _GT("Slider"),          gui_button_create,      button_callback,    {ID_WIN_SLIDER, _GT("Slider")}},
    {ID_BTN_WIN_ZINDEX,     _GT("z-index"),         gui_button_create,      button_callback,    {ID_WIN_ZINDEX, _GT("z-index test page")}},
    {ID_BTN_WIN_TRANSP,     _GT("Transparency"),    gui_button_create,      button_callback,    {ID_WIN_TRANSP, _GT("Transparency presentation")}},
    {ID_BTN_WIN_WINDOW,     _GT("Window"),          gui_button_create,      button_callback,    {ID_WIN_WINDOW, _GT("Window")}},
    {ID_PROGBAR_CPUUSAGE,   _GT("CPU Usage"),       gui_progbar_create,     progbar_callback,   {0}},
};

#define PI      3.14159265359f

/* Number of columns on LCD */
#if defined(STM32F756_EVAL) || defined(STM32F439_EVAL)
#define LCD_COLUMNS         4
#else
#define LCD_COLUMNS         3
#endif

static float len = 72, radius = 90;
float x, y;

int pressed = 0;

extern GUI_t GUI;

#define IMG_WIDTH           64
#define IMG_HEIGHT          64

const GUI_Char myStr[30] = "Hello world!\r\n";
GUI_STRING_t ptr;

size_t i;
GUI_HANDLE_p h;

void read_touch(void);

void user_thread(void const * arg);
void touch_thread(void const * arg);
void fat_thread(void const * arg);

osThreadDef(user_thread, user_thread, osPriorityNormal, 0, 512);
osThreadDef(touch_thread, touch_thread, osPriorityNormal, 0, 512);
osThreadDef(fat_thread, fat_thread, osPriorityNormal, 0, 1024);

osThreadId user_thread_id, touch_thread_id, fat_thread_id;

FATFS fs;
FRESULT fres;
FIL fil;

BYTE work[16 * _MAX_SS];                        /* Work area (larger is better for processing time) */
int main(void) {
    //hardfault debug code
    SCB->CCR |= 0x10;
    
    TM_RCC_InitSystem();                        /* Init system */
    HAL_Init();                                 /* Init HAL layer */
    TM_DISCO_LedInit();                         /* Init leds */
    TM_DISCO_ButtonInit();                      /* Init button */
    TM_DELAY_Init();                            /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 115200);   /* Init USART for debug purpose */
    
    /* Print first screen message */
    printf("GUI; Compiled: %s %s\r\n", __DATE__, __TIME__);
    TM_GENERAL_DWTCounterEnable();
    
    /* Main button as interrupt */
    TM_EXTI_Attach(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, TM_EXTI_Trigger_Rising);
    
//    fres = f_mkfs("", FM_ANY, 0, work, sizeof(work));   /* Create FAT volume */
//    f_mount(&fs, "", 0);                        /* Register work area */
    
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
    
    gui_init();                                 /* Initialize GUI as RTOS */
    
    gui_widget_setfontdefault(&GUI_Font_Arial_Narrow_Italic_21_AA); /* Set default font for widgets */
    //gui_widget_setfontdefault(&GUI_Font_Comic_Sans_MS_Regular_22); /* Set default font for widgets */
    //gui_widget_setfontdefault(&GUI_Font_Roboto_Italic_14); /* Set default font for widgets */
    
    /* Create all widgets on screen */
    win1 = gui_window_getdesktop();             /* Get desktop window */
    for (state = 0; state < GUI_COUNT_OF(frontedWidgets); state++) {
        handle = frontedWidgets[state].createFunc(frontedWidgets[state].id,
            2 + (state % LCD_COLUMNS) * ((gui_lcd_getwidth() / LCD_COLUMNS)),
            2 + (state / LCD_COLUMNS) * 40,
            ((gui_lcd_getwidth() / LCD_COLUMNS) - 4),
            36, 
            win1, 
            frontedWidgets[state].cbFunc, 0
        );
        gui_widget_settext(handle, frontedWidgets[state].text);
        gui_widget_setuserdata(handle, &frontedWidgets[state].data);
        gui_widget_set3dstyle(handle, 0);
    }
    
    //gui_keyboard_create();                      /* Create virtual keyboard */
    
    touch_thread_id = osThreadCreate(osThread(touch_thread), NULL); /* Create touch thread */
    fat_thread_id = osThreadCreate(osThread(fat_thread), NULL); /* Create FAT thread */
    
    while (1) {                                 /* Start thread execution */
        osDelay(100);

        /**
         * On pressed button, open dialog in blocking and wait for user press to continue
         */
        if (pressed) {
            pressed = gui_dialog_createblocking(ID_WIN_DIALOG, 50, 10, 300, 150, gui_container_create, dialog_callback, 0);
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
            switch (gui_string_unicode_decode(&s, ch)) {
                case UNICODE_OK:
                    if (ch == 'l') {
                        gui_linkedlist_printlist__();
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
                    gui_input_keyadd(&key);
                    key.Keys[0] = 0;
                    gui_input_keyadd(&key);
                    break;
                case UNICODE_PROGRESS:
                    key.Keys[s.t - s.r - 1] = ch;
                default:
                    break;
            }
        }
    }
}


uint8_t data[512 * 128];
/**
 * \brief           User thread for GUI
 * \param[in]       *arg: Pointer to argument for thread
 */
static void
fat_thread(void const * arg) {
    while (1) {
        osDelay(100);
        gui_progbar_setvalue(gui_widget_getbyid(ID_PROGBAR_CPUUSAGE), osGetCPUUsage());
    }
//    osThreadTerminate(NULL);
//    if ((fres = f_mount(&fs, "SD:", 1)) == FR_OK) {
//        printf("Mounted\r\n");
//        if ((fres = f_open(&fil, "SD:file.txt", FA_WRITE | FA_READ | FA_OPEN_ALWAYS)) == FR_OK) {
//            f_puts("String", &fil);
//            f_close(&fil);
//            printf("Written and closed\r\n");
//        } else {
//            printf("Open failed: %d\r\n", (int)fres);
//        }
//        f_mount(NULL, "SD:", 1);
//    } else {
//        printf("Mount failed: %d\r\n", (int)fres);
//    }
//    while (1) {
//        osDelay(1000);
//    }
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
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    if (cmd == GUI_WC_Init) {           /* Window has been just initialized */
        switch (gui_widget_getid(h)) {  /* Button callbacks */
            case ID_WIN_BTN: {
                handle = gui_button_create(0, 0, 0, 100, 40, h, button_callback, 0);
                gui_widget_settext(handle, _GT("Button 1"));
                gui_widget_setpositionpercent(handle, 0, 0);
                gui_widget_setsizepercent(handle, 50, 50);
                
                handle = gui_button_create(0, 10, 60, 100, 40, h, button_callback, 0);
                gui_widget_settext(handle, _GT("Button 2"));
                gui_widget_setpositionpercent(handle, 50, 0);
                gui_widget_setsizepercent(handle, 50, 50);
                
                handle = gui_button_create(0, 0, 0, 100, 40, h, button_callback, 0);
                gui_widget_settext(handle, _GT("Button 3"));
                gui_widget_setpositionpercent(handle, 0, 50);
                gui_widget_setsizepercent(handle, 50, 50);
                
                handle = gui_button_create(0, 10, 60, 100, 30, h, button_callback, 0);
                gui_widget_settext(handle, _GT("Button 4"));
                gui_widget_setpositionpercent(handle, 50, 50);
                gui_widget_setsizepercent(handle, 50, 50);
                break;
            }
            case ID_WIN_CHECKBOX: {     /* Check box */
                handle = gui_checkbox_create(ID_CHECKBOX_LED, 10, 10, 400, 40, h, checkbox_callback, 0);
                gui_widget_settext(handle, _GT("Check box default"));
                handle = gui_checkbox_create(1, 10, 50, 400, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Check box checked"));
                gui_checkbox_setchecked(handle, 1);
                handle = gui_checkbox_create(ID_CHECKBOX_LED, 10, 90, 400, 40, h, checkbox_callback, 0);
                gui_widget_settext(handle, _GT("Check box disabled"));
                gui_checkbox_setdisabled(handle, 1);
                handle = gui_checkbox_create(1, 10, 130, 400, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Check box checked disabled"));
                gui_checkbox_setchecked(handle, 1);
                gui_checkbox_setdisabled(handle, 1);
                break;
            }
            case ID_WIN_RADIO: {        /* Radio box */
                uint8_t i;
                for (i = 0; i < 4; i++) {
                    handle = gui_radio_create(10, 10, 10 + (i * 30), 300, 25, h, 0, 0);
                    switch (i) {
                        case 0: gui_widget_settext(handle, _GT("Radio box default")); break;
                        case 1: gui_widget_settext(handle, _GT("Radio box selected")); break;
                        case 2: gui_widget_settext(handle, _GT("Radio box disabled")); break;
                        case 3: gui_widget_settext(handle, _GT("Radio box disabled checked")); break;
                        default: break;
                    }
                    gui_radio_setgroup(handle, i < 2 ? 1 : 2);
                    gui_radio_setvalue(handle, i);
                    if (i % 2) {
                        gui_radio_setselected(handle);
                    }
                    gui_radio_setdisabled(handle, i / 2);
                }
                break;
            }
            case ID_WIN_LISTBOX: {      /* List box */
                uint8_t i;
                handle = gui_listbox_create(1, 10, 10, 190, 195, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    gui_listbox_addstring(handle, listboxtexts[i]);
                }
                gui_listbox_setsliderauto(handle, 0);
                gui_listbox_setslidervisibility(handle, 1);
                
                handle = gui_listbox_create(1, 210, 10, 200, 195, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    gui_listbox_addstring(handle, listboxtexts[i]);
                }
                gui_listbox_setsliderauto(handle, 0);
                gui_listbox_setslidervisibility(handle, 0);
                break;
            }
            case ID_WIN_LISTVIEW: {     /* Listview */
                size_t len;
                GUI_LISTVIEW_ROW_p row;
                handle = gui_listview_create(0, 10, 10, 100, 40, h, listview_callback, 0);
                gui_widget_setexpanded(handle, 1);
                gui_widget_setfont(handle, &GUI_Font_Roboto_Italic_14);
                
                gui_listview_addcolumn(handle, _GT("Name"), 120);
                gui_listview_addcolumn(handle, _GT("LastName"), 150);
                gui_listview_addcolumn(handle, _GT("Email"), 220);
                gui_listview_addcolumn(handle, _GT("Phone"), 160);
                
                for (len = 0; len < 10; len++) {
                    row = gui_listview_addrow(handle);
                    gui_listview_setitemstring(handle, row, 0, _GT("Name 1"));
                    gui_listview_setitemstring(handle, row, 1, _GT("Last name 2"));
                    gui_listview_setitemstring(handle, row, 2, _GT("info@example.com"));
                    gui_listview_setitemstring(handle, row, 3, _GT("0123456789"));
                }
                
                break;
            }
            case ID_WIN_GRAPH: {        /* Graph window */
                uint16_t i;
                handle = gui_graph_create(ID_GRAPH_MAIN, 10, 10, 400, 220, h, 0, 0);
                gui_widget_setexpanded(handle, 1);

                gui_graph_setminx(handle, -100);
                gui_graph_setmaxx(handle, 100);
                gui_graph_setminy(handle, -100);
                gui_graph_setmaxy(handle, 100);
                gui_widget_invalidate(handle);
                gui_graph_zoomreset(handle);
                gui_graph_zoom(handle, 0.1, 0.5, 0.5);

                //Create data objects
                if (!graphdata1) {
                    graphdata1 = gui_graph_data_create(GUI_GRAPH_TYPE_XY, len);
                    graphdata1->Color = GUI_COLOR_RED;
                }
                if (!graphdata2) {
                    graphdata2 = gui_graph_data_create(GUI_GRAPH_TYPE_YT, len / 2);
                    graphdata2->Color = GUI_COLOR_GREEN;
                }

                //Fill data objects
                for (i = 0; i <= 360; i += 360 / len) {
                    x = cos((float)i * (PI / 180.0f));
                    y = sin((float)i * (PI / 180.0f));
                    gui_graph_data_addvalue(graphdata1, x * radius, y * radius);
                    gui_graph_data_addvalue(graphdata2, x * radius / 3, y * radius / 4);
                }
                gui_graph_attachdata(handle, graphdata1);
                gui_graph_attachdata(handle, graphdata2);
                
                /* Create checkbox handle for graph */
                handle = gui_checkbox_create(ID_CHECKBOX_GRAPH, 17, 15, 200, 30, h, checkbox_callback, 0);
                gui_widget_settext(handle, _GT("SetExpanded"));
                gui_widget_setzindex(handle, 1);
                gui_checkbox_setcolor(handle, GUI_CHECKBOX_COLOR_TEXT, GUI_COLOR_WHITE);
                gui_checkbox_setchecked(handle, gui_widget_isexpanded(gui_widget_getbyid(ID_GRAPH_MAIN)));
                break;
            }
            case ID_WIN_EDIT: {         /* Edit text */
                handle = gui_edittext_create(1, 10, 10, 400, 40, h, 0, 0);
                gui_widget_alloctextmemory(handle, 255);
                gui_widget_settext(handle, _GT("Edit text very long text OLA CIKAS OLA CIKAS OLA CIKASS"));
                gui_widget_setfont(handle, &GUI_Font_Arial_Narrow_Italic_21_AA);
                
                /* Create text field first to hold view of edit text*/
                handle = gui_textview_create(ID_TEXTVIEW_2, 10, 210, 400, 150, h, 0, 0);
                
                /* edittext_callback is responsible to set display text from edit text to text view */
                handle = gui_edittext_create(ID_EDITTEXT_1, 10, 60, 400, 150, h, edittext_callback, 0);
                gui_widget_alloctextmemory(handle, 255);
                gui_edittext_setmultiline(handle, 1);
                gui_edittext_setvalign(handle, GUI_EDITTEXT_VALIGN_TOP);
                gui_widget_settext(handle, _GT("Edit\ntext\nmultiline\n\n\n\nTextText\nTexttttt\nOLAAA\nOLAAAAAAA\n\nEvenMoreText\nMoreee\n"));
                break;
            }
            case ID_WIN_PROGBAR: {      /* Progress bar */
                handle = gui_progbar_create(2, 10, 10, 400, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Progress bar with custom text"));
                
                handle = gui_progbar_create(2, 10, 60, 400, 40, h, 0, 0);
                gui_progbar_setpercentmode(handle, 1);
                gui_progbar_setmin(handle, 20);
                gui_progbar_setmax(handle, 30);
                gui_progbar_setvalue(handle, 22);
                
                handle = gui_progbar_create(3, 10, 110, 400, 40, h, 0, 0);
                gui_progbar_setpercentmode(handle, 1);
                gui_progbar_setmin(handle, 0);
                gui_progbar_setmax(handle, 100);
                gui_progbar_setvalue(handle, 0);
                gui_progbar_setanimation(handle, 1);
                gui_progbar_setvalue(handle, 80);
                break;
            }
            case ID_WIN_LED: {          /* Leds */                
                handle = gui_led_create(ID_LED_1, 10, 80, 30, 30, 0, led_callback, 0);
                gui_led_settype(handle, GUI_LED_TYPE_CIRCLE);
                gui_led_set(handle, 1);
                
                handle = gui_led_create(ID_LED_2, 50, 80, 30, 30, 0, led_callback, 0);
                gui_led_settype(handle, GUI_LED_TYPE_CIRCLE);
                
                handle = gui_led_create(ID_LED_3, 90, 80, 30, 30, 0, led_callback, 0);
                gui_led_settype(handle, GUI_LED_TYPE_RECT);
                gui_led_set(handle, 1);
                handle = gui_led_create(ID_LED_4, 130, 80, 30, 30, 0, led_callback, 0);
                gui_led_settype(handle, GUI_LED_TYPE_RECT);
                
                handle = gui_textview_create(0, 40, 10, 400, 1000, h, 0, 0);
                gui_widget_setfont(handle, &GUI_Font_Roboto_Italic_14);
                gui_widget_settext(handle, _GT("\"LEDs\" are widgets used to indicate some status or any other situation. Press blue button on discovery board to see LED in happen"));
                gui_textview_setcolor(handle, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_TRANS);
                gui_widget_setzindex(handle, -1);
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
                    handle = gui_textview_create(0, 1, 1, 1, 1, 0, 0, 0);
                    gui_widget_setsizepercent(handle, 30, 30);
                    gui_widget_setpositionpercent(handle, 3 + (i % 3) * 33, 3 + (i / 3) * 33);
                    gui_widget_alloctextmemory(handle, 255);
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
                break;
            }
            case ID_WIN_DROPDOWN: {     /* Dropdown */
                uint8_t i;
                handle = gui_dropdown_create(0, 10, 10, 200, 40, h, 0, 0);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    gui_dropdown_addstring(handle, listboxtexts[i]);
                }
                gui_dropdown_setsliderauto(handle, 0);
                gui_dropdown_setslidervisibility(handle, 1);
                
                handle = gui_dropdown_create(0, 220, 180, 200, 40, h, 0, 0);
                gui_dropdown_setopendirection(handle, GUI_DROPDOWN_OPENDIR_UP);
                for (i = 0; i < COUNT_OF(listboxtexts); i++) {
                    gui_dropdown_addstring(handle, listboxtexts[i]);
                }
                break;
            }
            case ID_WIN_IMAGE: {
//                handle = gui_button_create(0, 10, 10, 300, 50, h, 0, 0);
//                gui_widget_settext(handle, _GT("Button"));
//                handle = gui_image_create(0, 2, 2, bmimage_brand.xSize, bmimage_brand.ySize, h, 0, 0);
//                gui_image_setsource(handle, &bmimage_brand);
//                handle = gui_image_create(0, 200, 40, bmimage_voyo.xSize, bmimage_voyo.ySize, h, 0, 0);
//                gui_image_setsource(handle, &bmimage_voyo);
//                handle = gui_image_create(0, 350, 10, bmimage_voyo565.xSize, bmimage_voyo565.ySize, h, 0, 0);
//                gui_image_setsource(handle, &bmimage_voyo565);
                break;
            }
            case ID_WIN_SLIDER: {
                gui_button_create(ID_BUTTON_0, 5, 100, 100, 30, h, 0, 0);
               
                handle = gui_slider_create(ID_SLIDER_0, 5, 5, 400, 50, h, slider_callback, 0);
                gui_slider_setmode(handle, GUI_SLIDER_MODE_LEFT_RIGHT);
                gui_slider_setmin(handle, 100);
                gui_slider_setmax(handle, 300);
                gui_slider_setvalue(handle, 200);
                
                handle = gui_slider_create(0, 5, 70, 400, 15, h, slider_callback, 0);
                gui_slider_setmode(handle, GUI_SLIDER_MODE_RIGHT_LEFT);
                handle = gui_slider_create(0, 430, 10, 10, 230, h, slider_callback, 0);
                gui_slider_setmode(handle, GUI_SLIDER_MODE_BOTTOM_TOP);
                handle = gui_slider_create(0, 450, 10, 20, 230, h, slider_callback, 0);
                gui_slider_setmode(handle, GUI_SLIDER_MODE_TOP_BOTTOM);
                break;
            }
            case ID_WIN_ZINDEX: {
                handle = gui_textview_create(0, 10, 10, 180, 100, 0, 0, 0);
                gui_widget_settext(handle, _GT("All buttons with same z-index. Auto z-index is allowed."));
                
                /* Create button group with the same index */
                handle = gui_button_create(0, 200, 10, 150, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Button1 z-i: 0"));
                handle = gui_button_create(0, 300, 20, 150, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Button2 z-i: 0"));
                handle = gui_button_create(0, 250, 30, 150, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Button3 z-i: 0"));
                
                
                handle = gui_textview_create(0, 10, 120, 180, 100, 0, 0, 0);
                gui_widget_settext(handle, _GT("Each button has dedicated z-index. Auto z-index is not allowed."));
                handle = gui_button_create(0, 200, 120, 150, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Button4 z-i: 1"));
                gui_widget_setzindex(handle, 1);
                handle = gui_button_create(0, 300, 130, 150, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Button5 z-i: 3"));
                gui_widget_setzindex(handle, 3);
                handle = gui_button_create(0, 250, 140, 150, 40, h, 0, 0);
                gui_widget_settext(handle, _GT("Button6 z-i: 2"));
                gui_widget_setzindex(handle, 2);
                break;
            }
            case ID_WIN_TRANSP: {
                //gui_widget_settransparency(h, 0xF0);
                
                handle = gui_button_create(ID_BUTTON_2, 10, 50, 300, 35, 0, 0, 0);
                gui_widget_settext(handle, _GT("Opaque button"));
                gui_widget_settransparency(handle, 0xFF);
                handle = gui_button_create(ID_BUTTON_3, 10, 90, 300, 35, 0, 0, 0);
                gui_widget_settext(handle, _GT("Opaque button"));
                gui_widget_settransparency(handle, 0xC0);
                
                handle = gui_button_create(ID_BUTTON_1, 5, 30, 300, 35, 0, 0, 0);
                gui_widget_settext(handle, _GT("Transparent button"));
                gui_widget_settransparency(handle, 0x80);
                
                handle = gui_slider_create(ID_SLIDER_1, 10, 150, 400, 50, 0, slider_callback, 0);
                gui_slider_setmin(handle, 0);
                gui_slider_setmax(handle, 0xFF);
                gui_slider_setvalue(handle, 0x80);
                break;
            }
            default:
                break;  
        }
    }
    return ret;
}

uint8_t radio_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    if (cmd == GUI_WC_SelectionChanged) {
        uint32_t group = gui_radio_getgroup(h);
        if (group == RADIO_GROUP_VALIGN || group == RADIO_GROUP_HALIGN) {
            GUI_HANDLE_p textview = gui_widget_getbyid(ID_TEXTVIEW_1);
            if (textview) {
                if (group == RADIO_GROUP_VALIGN) {
                    gui_textview_setvalign(textview, (GUI_TEXTVIEW_VALIGN_t)gui_radio_getvalue(h));
                } else if (group == RADIO_GROUP_HALIGN) {
                    gui_textview_sethalign(textview, (GUI_TEXTVIEW_HALIGN_t)gui_radio_getvalue(h));
                }
            }
        }
    }
    return ret;
}

uint8_t led_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    if (cmd == GUI_WC_Click) {
        gui_led_toggle(h);              /* Toggle status */
    }
    return ret;
}

uint8_t checkbox_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    GUI_ID_t id;
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    
    id = gui_widget_getid(h);
    if (cmd == GUI_WC_Init) {
        if (id == ID_CHECKBOX_LED) {
            gui_checkbox_setchecked(h, TM_DISCO_LedIsOn(LED_GREEN));
        }
    } else if (cmd == GUI_WC_ValueChanged) {
        switch (id) {
            case ID_CHECKBOX_LED: {
                if (gui_checkbox_ischecked(h)) {
                    TM_DISCO_LedOn(LED_ALL);
                } else {
                    TM_DISCO_LedOff(LED_ALL);
                }
                break;
            }
            case ID_CHECKBOX_GRAPH: {
                GUI_HANDLE_p handle = gui_widget_getbyid(ID_GRAPH_MAIN); /* Find graph widget if exists */
                if (handle) {
                    gui_widget_setexpanded(handle, gui_checkbox_ischecked(h));
                }
                break;
            }
        }
    }
    return ret;
}

uint8_t listview_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    if (cmd == GUI_WC_SelectionChanged) {
        
    }
    return ret;
}

uint8_t button_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    GUI_ID_t id;
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            break;
        }
        case GUI_WC_Click: {
            id = gui_widget_getid(h);
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
                    btn_user_data_t* data = gui_widget_getuserdata(h);
                    uint32_t diff = id - ID_BTN_WIN_BTN;
                    if (data) {
                        GUI_HANDLE_p tmp;
                        if ((tmp = gui_widget_getbyid(data->win_id)) != 0) {
                            gui_widget_show(tmp);
                            gui_widget_putonfront(tmp);
                        } else {
                            //tmp = gui_window_createChild(data->win_id, 5, 5, 470, 262, gui_window_getdesktop(), window_callback, NULL);
                            tmp = gui_window_create(data->win_id, 40, 20, 300, 200, gui_window_getdesktop(), window_callback, 0);
                            gui_widget_setexpanded(tmp, 1);
                            gui_widget_settext(tmp, data->win_text);
                            gui_widget_setzindex(tmp, diff);
                            gui_widget_putonfront(tmp);
                            if (data->win_id == ID_WIN_TRANSP) {
                                //gui_widget_settransparency(tmp, 0x80);
                            }
                        }
                    }
                    break;
                }
                case ID_BTN_WIN_DIALOG: {
                    gui_dialog_create(ID_WIN_DIALOG, 60, 20, 300, 150, gui_container_create, dialog_callback, 0);
                    break;
                }
                case ID_BTN_DIALOG_CONFIRM:
                case ID_BTN_DIALOG_CANCEL: {
                    gui_dialog_dismiss(gui_widget_getbyid(ID_WIN_DIALOG), ID_BTN_DIALOG_CONFIRM == id);
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
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            GUI_HANDLE_p tmp;
            switch (gui_widget_getid(h)) {
                case ID_WIN_DIALOG:
                    tmp = gui_container_create(0, 0, 0, gui_widget_getwidth(h), gui_widget_getheight(h), h, 0, 0);
                    handle = gui_textview_create(0, 10, 10, gui_widget_getwidth(h) - 20, gui_widget_getheight(h) - 52, tmp, 0, 0);
                    if (TM_DISCO_LedIsOn(LED_GREEN)) {
                        gui_widget_settext(handle, _GT("LED is ON. Do you want to turn it off?"));
                    } else  {
                        gui_widget_settext(handle, _GT("LED is OFF. Do you want to turn it on?"));
                    }
                    handle = gui_button_create(ID_BTN_DIALOG_CONFIRM, 0,   118, 150, 32, tmp, button_callback, 0);
                    gui_widget_settext(handle, _GT("OK"));
                    handle = gui_button_create(ID_BTN_DIALOG_CANCEL,  150, 118, 150, 32, tmp, button_callback, 0);
                    gui_widget_settext(handle, _GT("CANCEL"));
                    gui_widget_setfocus(handle);
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
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            gui_slider_setmin(h, 0);
            gui_slider_setmax(h, 300);
            gui_slider_setvalue(h, 212);
            break;
        }
        case GUI_WC_ValueChanged: {
            int32_t value = gui_slider_getvalue(h); /* Get widget handle */
            switch (gui_widget_getid(h)) {
                case ID_SLIDER_0: {
                    GUI_HANDLE_p tmp;
                    tmp = gui_widget_getbyid(ID_BUTTON_0);
                    if (tmp) {
                        gui_widget_setsize(tmp, value, value / 4);
                    }
                    break;
                }
                case ID_SLIDER_1: {
                    GUI_HANDLE_p tmp;
                    tmp = gui_widget_getbyid(ID_BUTTON_1);
                    if (tmp) {
                        gui_widget_settransparency(tmp, value);
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

uint8_t progbar_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    switch (cmd) {
        case GUI_WC_Init: {
            switch (gui_widget_getid(h)) {
                case ID_PROGBAR_CPUUSAGE:
                    gui_progbar_setmin(h, 0);
                    gui_progbar_setmax(h, 100);
                    gui_progbar_setvalue(h, 0);
                    gui_progbar_setpercentmode(h, 1);
                    gui_progbar_setanimation(h, 1);
                    break;
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

uint8_t edittext_callback(GUI_HANDLE_p h, GUI_WC_t cmd, void* param, void* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, cmd, param, result);
    if (cmd == GUI_WC_TextChanged) {
        GUI_ID_t id = gui_widget_getid(h);          /* Get widget ID */
        switch (id) {
            case ID_EDITTEXT_1: {
                GUI_HANDLE_p tmp = gui_widget_getbyid(ID_TEXTVIEW_2);
                if (tmp) {
                    gui_widget_settext(tmp, gui_widget_gettext(h));
                    gui_widget_invalidate(tmp);
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
    if (update || (t.Status == GUI_TouchState_RELEASED && p.Status != GUI_TouchState_RELEASED)) {
        gui_input_touchadd(&t);
    }
    memcpy(&p, &t, sizeof(p));
}

/* 1ms handler */
void TM_DELAY_1msHandler() {
    //osSystickHandler();                             /* Kernel systick handler processing */
#if defined(STM32F439_EVAL) || defined(STM32F756_EVAL)
    static uint32_t Time = 0;
    if (Time % 50 == 0) {
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

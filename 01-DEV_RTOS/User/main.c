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

#include "gui.h"
#include "gui_window.h"
#include "gui_button.h"
#include "gui_led.h"
#include "gui_progbar.h"
#include "gui_graph.h"

TM_TOUCH_t TS;

GUI_HANDLE_t btn1, btn2, btn3, btn4, btn5, btn6;
GUI_HANDLE_t led[8][2];
GUI_HANDLE_t win1, win2, win3;
GUI_HANDLE_t prog1, prog2, prog3, prog4;

char str[100];

extern GUI_Const GUI_FONT_t GUI_Font_Comic_Sans_MS_Regular_22;
extern GUI_Const GUI_FONT_t GUI_Font_Calibri_Bold_8;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Bold_18;

uint32_t time;

int main(void) {
    uint8_t i, state;
    GUI_Color_t col;
    TM_RCC_InitSystem();                                    /* Init system */
    HAL_Init();                                             /* Init HAL layer */
    TM_DISCO_LedInit();                                     /* Init leds */
    TM_DISCO_ButtonInit();                                  /* Init button */
    TM_DELAY_Init();                                        /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 115200);     /* Init USART for debug purpose */
    
    /* Print first screen message */
    printf("GUI; Compiled: %s %s, sizeof: %d\r\n", __DATE__, __TIME__, sizeof(char *) * 5);
    
    TM_GENERAL_DWTCounterEnable();
    
    GUI_Init();
    
    //2 buttons on main
    win1 = GUI_WINDOW_CreateChild(1, 0, 0, GUI.LCD.Width, 30);
    GUI_WINDOW_SetColor(win1, GUI_WINDOW_COLOR_BG, GUI_COLOR_DARKGRAY);
    //btn1 = GUI_BUTTON_Create(3, 10, 10, 100, 25);
    
    for (i = 0; i < 8; i++) {
        led[i][0] = GUI_LED_Create(i, win1->Width - (8 - i) * 14, 2, 12, 12);
        GUI_LED_SetType(led[i][0], GUI_LED_TYPE_CIRCLE);
    }
    
    //2 buttons on second window
    GUI.WindowActive = GUI.Root.First;      /* First widget is main window */
    win2 = GUI_WINDOW_CreateChild(1, 0, win1->Height, GUI.LCD.Width, GUI.LCD.Height - win1->Height);
    
    btn1 = GUI_BUTTON_Create(3, 50, 10, 150, 50);
    btn2 = GUI_BUTTON_Create(3, 80, 10, 200, 80);
    btn3 = GUI_BUTTON_Create(3, 210, 10, 150, 50);
    GUI_BUTTON_Create(3, 300, 40, 150, 50);
    
    GUI_BUTTON_Create(3, 10, 40, 50, 50);
    GUI_BUTTON_Create(3, 10, 200, 150, 50);
    GUI_BUTTON_Create(3, 200, 250, 150, 50);
    
    GUI_BUTTON_SetFont(btn2, &GUI_Font_Arial_Bold_18);
    GUI_BUTTON_SetText(btn2, "Some regular");
    
    GUI_GRAPH_Create(1, 10, 10, 460, 240);
    
    prog1 = GUI_PROGBAR_Create(1, 90, 50, 100, 80);
    GUI_PROGBAR_SetFont(prog1, &GUI_Font_Arial_Bold_18);
    GUI_PROGBAR_SetText(prog1, "Test1");

    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = 1;
    TM_TOUCH_Init(NULL, &TS);
  
//    time = TM_DELAY_Time();
    state = 40;
	while (1) {
        GUI_Process();
        if ((TM_DELAY_Time() - time) > 1000) {
            time = TM_DELAY_Time();
            GUI_BUTTON_SetSize(btn3, 100, 100);
        }
	}
}

/* 1ms handler */
void TM_DELAY_1msHandler() {
    //osSystickHandler();                     /* Kernel systick handler processing */
}

/* printf handler */
int fputc(int ch, FILE* fil) {
    TM_USART_Putc(DISCO_USART, ch);
    return ch;
}

void TM_EXTI_Handler(uint16_t GPIO_Pin) {
    static GUI_TouchData_t p;
    if (GPIO_Pin == GPIO_PIN_13) {
        GUI_TouchData_t t;
        TM_TOUCH_Read(&TS);
        
        memset((void *)&t, 0x00, sizeof(t));
        t.X = TS.X[0];
        t.Y = TS.Y[0];
        t.Status = TS.NumPresses ? GUI_TouchState_PRESSED : GUI_TouchState_RELEASED;
        
        /* Check differences */
        if (memcmp(&p, &t, sizeof(p)) || t.Status == GUI_TouchState_RELEASED) {
            GUI_INPUT_AddTouch(&t);
            memcpy(&p, &t, sizeof(p));
        }
    }
}

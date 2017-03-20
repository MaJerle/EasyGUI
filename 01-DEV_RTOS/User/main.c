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
#include "gui_edittext.h"

#include "math.h"

TM_TOUCH_t TS;

GUI_HANDLE_t btn1, btn2, btn3, btn4, btn5, btn6;
GUI_HANDLE_t led[8][2];
GUI_HANDLE_t win1, win2, win3;
GUI_HANDLE_t prog1, prog2, prog3, prog4;
GUI_HANDLE_t graph1;
GUI_HANDLE_t graphdata1, graphdata2, graphdata3, graphdata4, graphdata5;
GUI_HANDLE_t edit1, edit2, edit3;

char str[100];

extern GUI_Const GUI_FONT_t GUI_Font_Comic_Sans_MS_Regular_22;
extern GUI_Const GUI_FONT_t GUI_Font_Calibri_Bold_8;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Bold_18;
extern GUI_Const GUI_FONT_t GUI_Font_FontAwesome_Regular_30;

uint32_t time;

uint8_t utf8_decode(const uint8_t c, uint32_t* result);

int main(void) {
    uint32_t result;
    GUI_KeyboardData_t key;
    uint32_t state;
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
    
    edit1 = GUI_EDITTEXT_Create(1, 10, 10, 460, 50);
    GUI_EDITTEXT_SetFont(edit1, &GUI_Font_FontAwesome_Regular_30);
    GUI_EDITTEXT_AllocTextMemory(edit1, 255);
    GUI_EDITTEXT_SetText(edit1, _T("Text test ABCDEFGHIJKLMNOPRSTUV ABCDEFGHIJKLMNOPRSTUV"));
    
    edit2 = GUI_EDITTEXT_Create(1, 10, 70, 460, 50);
    GUI_EDITTEXT_SetFont(edit2, &GUI_Font_FontAwesome_Regular_30);
    GUI_EDITTEXT_AllocTextMemory(edit2, 255);
    GUI_EDITTEXT_SetText(edit2, _T("Text test ABCDEFGHIJKLMNOPRSTUV ABCDEFGHIJKLMNOPRSTUV"));
    
    edit3 = GUI_EDITTEXT_Create(1, 10, 130, 460, 50);
    GUI_EDITTEXT_SetFont(edit3, &GUI_Font_Arial_Bold_18);
    GUI_EDITTEXT_AllocTextMemory(edit3, 255);
    GUI_EDITTEXT_SetText(edit3, _T("Text test ABCDEFGHIJKLMNOPRSTUV ABCDEFGHIJKLMNOPRSTUV"));

    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = 1;
    TM_TOUCH_Init(NULL, &TS);
  
//    time = TM_DELAY_Time();
    state = 0;
	while (1) {
        GUI_Process();
        if ((TM_DELAY_Time() - time) >= 2) {
            time = TM_DELAY_Time();
            
            //GUI_GRAPH_DATA_AddValue(graphdata1, 50 + 50 * sin(2.0f * 3.0f * 3.14159265359f * (float)state / 100.0f));
            //GUI_PROGBAR_SetValue(prog1, (state % 100) + 1);
            state++;
        }
        
        while (!TM_USART_BufferEmpty(DISCO_USART)) {
            key.Key = TM_USART_Getc(DISCO_USART);
            __GUI_DEBUG("ch: %c (%3d)\r\n", key.Key, key.Key);
            if (utf8_decode(key.Key, &result)) {
                key.Key = result;
                GUI_INPUT_AddKey(&key);
            }
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

/**
 * Handle EXTI interrupt routine
 */
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

uint8_t utf8_decode(const uint8_t c, uint32_t* result) {
    static uint8_t remainingBytes = 0;
    static uint32_t res = 0;
    
    if (!remainingBytes) {                  /* First byte received */
        if (c < 0x80) {                     /* One byte only in UTF-8 representation */
            *result = c;                    /* Just return result */
            res = 0;
            return 1;
        }
        if ((c & 0xE0) == 0xC0) {           /* 1 additional byte in a row = 110x xxxx */   
            res = c & 0x1F;                 /* 5 lower bits from first byte as valid data */      
            remainingBytes = 1;
        } else if ((c & 0xF0) == 0xE0) {    /* 2 additional bytes in a row = 1110 xxxx */
            res = c & 0x0F;                 /* 4 lower bits from first byte as valid data*/
            remainingBytes = 2;
        } else if ((c & 0xF8) == 0xF0) {    /* 3 additional bytes in a row = 1111 0xxx */
            res = c & 0x07;                 /* 3 lower bits from first byte as valid data*/   
            remainingBytes = 3;    
        } else {                            /* Invalid UTF-8 byte */
            return 0;
        }
    } else {                                /* Remaining bytes */
        if ((c & 0xC0) == 0x80) {           /* Valid UTF-8 sequence */
            remainingBytes--;               /* Decrease remaining bytes of data */
            res = (res << 6) | (c & 0x3F);  /* Set new value for data */
        } else {
            res = 0;
            remainingBytes = 0;             /* Invalid character */
            return 0;
        }
    }
    
    if (!remainingBytes) {                  /* We reached the end */
        *result = res;
        res = 0;
        return 1;
    }
    return 0;
}

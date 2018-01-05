/**
 * \file            main.c
 * \brief           Program entry point
 */

/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_touch.h"
#include "tm_stm32_exti.h"

#include "tm_stm32_general.h"
#include "main.h"
#include "tm_stm32_sdram.h"

#include "gui/gui_keyboard.h"
#include "gui/gui_lcd.h"

#include "math.h"

#include "cmsis_os.h"
#include "cpu_utils.h"

void read_touch(void);

void user_thread(void const * arg);
void example_button_pressed(void);

osThreadDef(user_thread, user_thread, osPriorityNormal, 0, 512);
osThreadId user_thread_id;

int main(void) {
    //hardfault debug code
    SCB->CCR |= 0x10;
    
    TM_RCC_InitSystem();                        /* Init system */
    HAL_Init();                                 /* Init HAL layer */
    TM_DISCO_LedInit();                         /* Init leds */
    TM_DISCO_ButtonInit();                      /* Init button */
    TM_DELAY_Init();                            /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 921600); /* Init USART for debug purpose */
    
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

/* 1ms handler */
void TM_DELAY_1msHandler() {
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
    } else if (DISCO_BUTTON_PIN == GPIO_Pin) {
        example_button_pressed();
    }
}

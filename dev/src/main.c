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
#include "demo.h"

/* Init thread */
extern void init_thread(void const * arg);
osThreadDef(init_thread, init_thread, osPriorityNormal, 0, 512);

/* Touch screen handle */
TM_TOUCH_t TS;

/**
 * \brief           Application entry point
 */
int
main(void) {
    SCB->CCR |= 0x10;
    
    TM_RCC_InitSystem();                        /* Init system */
    HAL_Init();                                 /* Init HAL layer */
    TM_DISCO_LedInit();                         /* Init leds */
    TM_DISCO_ButtonInit();                      /* Init button */
    TM_DELAY_Init();                            /* Init delay */
    TM_USART_Init(DISCO_USART, DISCO_USART_PP, 921600); /* Init USART for debug purpose */
    
    /* Print first screen message */
    printf("Compiled: %s %s\r\n", __DATE__, __TIME__);
    TM_GENERAL_DWTCounterEnable();
    
    /* Main button as interrupt */
    TM_EXTI_Attach(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, TM_EXTI_Trigger_Rising);
    
    /* Create init thread */
    osThreadCreate(osThread(init_thread), NULL);
    
    osKernelStart();                            /* Start RTOS kernel */
    
	while (1) {

	}
}

/**
 * \brief           Initialization thread for system
 */
void
init_thread(void const * arg) {
    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = TOUCH_ORIENT_INVERT_Y;
    TM_TOUCH_Init(NULL, &TS);
    
    gui_init();                                 /* Init GUI library */
    demo_init();                                /* Init demo GUI elements */
    
    while (1) {
        if (TM_DISCO_ButtonOnPressed()) {
            printf("Button pressed!\r\n");
        }
        osDelay(50);
    }
}


void
vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    printf("Task overflow!: %s\r\n", pcTaskName);
}

/* 1ms handler */
void
TM_DELAY_1msHandler(void) {
    osSystickHandler();
}

/* printf handler */
int
fputc(int ch, FILE* fil) {
    TM_USART_Putc(DISCO_USART, ch);
    return ch;
}

/**
 * \brief           Read touch from screen and send it to
 */
void
read_touch(void) {
    static gui_touch_data_t p = {0}, t = {0};
    uint8_t i, update = 0, diffx, diffy;
    TM_TOUCH_Read(&TS);                         /* Read touch data */
    
    memset((void *)&t, 0x00, sizeof(t));
    t.status = TS.NumPresses ? GUI_TOUCH_STATE_PRESSED : GUI_TOUCH_STATE_RELEASED;
    t.count = TS.NumPresses > GUI_CFG_TOUCH_MAX_PRESSES ? GUI_CFG_TOUCH_MAX_PRESSES : TS.NumPresses;
    for (i = 0; i < t.count; i++) {
        t.x[i] = TS.X[i];
        t.y[i] = TS.Y[i];
    }
    if (t.count != p.count) {
        update = 1;
    } else {
        for (i = 0; i < t.count; i++) {
            diffx = GUI_ABS(p.x[i] - t.x[i]);
            diffy = GUI_ABS(p.y[i] - t.y[i]);
            if (diffx > 2 || diffy > 2) {
                update = 1;
                break;
            }
        }
    }
    
    /* Check differences */
    if (update || (t.status == GUI_TOUCH_STATE_RELEASED && p.status != GUI_TOUCH_STATE_RELEASED)) {
        gui_input_touchadd(&t);
    }
    memcpy(&p, &t, sizeof(p));
}

/*
 * Handle EXTI interrupt routine
 */
void
TM_EXTI_Handler(uint16_t GPIO_Pin) {
    if (GPIO_PIN_13 == GPIO_Pin) {
        read_touch();
    } else if (DISCO_BUTTON_PIN == GPIO_Pin) {
        
    }
}

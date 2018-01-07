/**
 * \file            app.c
 * \brief           Application file
 */

#include "app.h"
#include "tm_stm32_exti.h"

/* Thread definitions */
osThreadDef(user_thread, user_thread, osPriorityNormal, 0, 512);

GUI_HANDLE_p winbase;

/**
 * \brief           Initialization thread for system
 */
void
init_thread(void const * arg) {
    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = TOUCH_ORIENT_INVERT_Y;
    TM_TOUCH_Init(NULL, &TS);
    
    gui_init();                                 /* Init GUI library */
    esp_init(esp_cb_func);                      /* Init ESP library */
    
    osThreadCreate(osThread(user_thread), NULL);/* Create user thread */
    osThreadTerminate(NULL);                    /* Stop current thread */
}

/**
 * \brief           User thread
 */
void
user_thread(void const * arg) {
    gui_keyboard_create();                      /* Create virtual keyboard */
    create_desktop();                           /* Create desktop on screen */
    
    enable_wifi_access_point();                 /* Enable WiFi access point */
    
    while (1) {
        osDelay(1000);
    }
}

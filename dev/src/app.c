/**
 * \file            app.c
 * \brief           Application file
 */

#include "app.h"
#include "tm_stm32_exti.h"
#include "tm_stm32_disco.h"
#include "cpu_utils.h"
#include "netconn_server.h"

/* Thread definitions */
osThreadDef(user_thread, user_thread, osPriorityNormal, 0, 512);

GUI_HANDLE_p winbase;

static char str_buff[100];

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
    esp_restore(1);                             /* Restore device to default settings */
    
    osThreadCreate(osThread(user_thread), NULL);/* Create user thread */
    
    while (1) {
        if (TM_DISCO_ButtonOnPressed()) {
            mqtt_send_data("BTN PRESSED", 11);
        }
        osDelay(50);
    }
    //osThreadTerminate(NULL);                    /* Stop current thread */
}

/**
 * \brief           User thread
 */
void
user_thread(void const * arg) {
    ADC_HandleTypeDef hadc;
    ADC_ChannelConfTypeDef adc_ch;
    uint16_t adc_raw;
    float volt, volt_25, degrees;
    
    gui_keyboard_create();                      /* Create virtual keyboard */
    create_desktop();                           /* Create desktop on screen */
    
    enable_wifi_access_point();                 /* Enable WiFi access point */
    esp_sta_autojoin(1, 1);                     /* Enable station auto join */
    list_access_points();
    start_server();                             /* Start server */
    
    /*
     * Process ADC
     */
    __HAL_RCC_ADC1_CLK_ENABLE();                /* Enable ADC1 clock */
    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.ScanConvMode = DISABLE;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.NbrOfDiscConversion = 0;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.NbrOfConversion = 0;
    hadc.Init.DMAContinuousRequests = DISABLE;
    hadc.Init.EOCSelection = DISABLE;
    HAL_ADC_Init(&hadc);
    
    adc_ch.Channel = ADC_CHANNEL_TEMPSENSOR;
    adc_ch.Offset = 0;
    adc_ch.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    adc_ch.Rank = 1;
    HAL_ADC_ConfigChannel(&hadc, &adc_ch);
    
    while (1) {
        HAL_ADC_Start(&hadc);
        HAL_ADC_PollForConversion(&hadc, 100);
        adc_raw = HAL_ADC_GetValue(&hadc);
        
        volt_25 = 0.76f;                        /* 0.76V at 25 degrees (by datasheet) */
        volt = (float)adc_raw * 3.3f / 4096;    /* Convert ADC to voltage */
        degrees = 25.0f + (volt - volt_25) / 0.0025f;
        
        sprintf(str_buff, "ADC RAW: %d; ADC VOLT: %.2f; degrees: %.2f\r\n", (int)adc_raw, volt, degrees);
        printf("%s", str_buff);
        
        /* Check if client connected and ready */
        if (mqtt_client_is_connected(mqtt_client)) {
            if (mqtt_client_publish(mqtt_client, "ew_st_temp", str_buff, strlen(str_buff), MQTT_QOS_AT_LEAST_ONCE, 0, NULL) == espOK) {
                console_write("Publish started!\r\n");
                osDelay(2000);
            } else {
                console_write("Cannot publish new temp data\r\n");
                osDelay(5000);
            }
        } else {
            osDelay(10000);
        }
    }
}

/**
 * \brief           Write text to console window
 */
void
console_write(const char* str) {
    GUI_HANDLE_p h;
    static char str_b[100];
    uint32_t time;
    
    time = osKernelSysTick() / 1000;
    
    h = gui_widget_getbyid(GUI_ID_DEBUGBOX_LOG);
    if (h != NULL) {
        sprintf(str_b, "%02d:%02d:%02d: %s", (int)(time / 3600), (int)(time / 60), (int)(time % 60), str);
        gui_debugbox_addstring(h, _GT(str_b));
    }
}

/**
 * \brief           Update IP address on LCD screen
 */
void
lcd_update_up(esp_ip_t* ip) {
    GUI_HANDLE_p h;
    
    h = gui_widget_getbyid(GUI_ID_TEXTVIEW_IP_ADDR);
    if (h != NULL) {
        char str[20];
        sprintf(str, "IP: %d.%d.%d.%d", ip->ip[0], ip->ip[1], ip->ip[2], ip->ip[3]);
        gui_widget_settext(h, _GT(str));
    }
}

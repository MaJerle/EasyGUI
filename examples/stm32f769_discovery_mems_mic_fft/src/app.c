/**
 * \file            app.c
 * \brief           Application file
 */

#include "app.h"
#include "tm_stm32_exti.h"
#include "tm_stm32_disco.h"
#include "cpu_utils.h"
#include "netconn_server.h"

#include "arm_math.h"

DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
DFSDM_Channel_HandleTypeDef hdfsdm1_channel1;
DMA_HandleTypeDef hdma_dfsdm1_flt0;

extern gui_const gui_font_t GUI_Font_Arial_Bold_18;

/* Thread definitions */
osThreadDef(user_thread, user_thread, osPriorityNormal, 0, 512);

static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DFSDM1_Init(void);
static void led_init(void);

#define ARR_SIZE(x)         (sizeof(x) / sizeof((x)[0]))

#define LED1_PORT           GPIOJ
#define LED1_PIN            GPIO_PIN_13
#define LED2_PORT           GPIOJ
#define LED2_PIN            GPIO_PIN_5
#define LED3_PORT           GPIOA
#define LED3_PIN            GPIO_PIN_12

uint8_t DmaRecHalfBuffCplt;
uint8_t DmaRecBuffCplt;

typedef struct {
    uint32_t freq_start;                        /*!< Start frequency in units of Hertz */
    uint32_t freq_end;                          /*!< End frequency in units of Hertz */
    
    GPIO_TypeDef* gpio;
    uint16_t pin;
    
    uint32_t freq_start_index;
    uint32_t freq_end_index;
    
    float sum;                                  /*!< Sum of values in band */
    float average;                              /*!< Average between samples */
    float treshold;                             /*!< Treshold value to turn on LED */
} freq_band_t;

/**
 * \brief           List of frequency bands
 */
freq_band_t
freq_bands[] = {
    { 10, 300 , LED1_PORT, LED1_PIN},
    { 300, 1000, LED2_PORT, LED2_PIN },
    { 1000, 4000, LED3_PORT, LED3_PIN },
};

/**
 * Sampling frequency in units of hertz
 */
#define AUDIO_SAMPLING_FREQUENCY                8000

/**
 * Length of single audio buffer to be filled before processed.
 * Note that DMA will work in "double-buffer" mode and actual length will be twice as value here.
 *
 * The same value is used for FFT later. Make sure value is power of 2 and not below 32 and not above 4096.
 */
#define AUDIO_BUFF_SIZE                         1024

/**
 * Raw AUDIO buffer for DMA, split into 2 sections for DMA complete and half-transfer complete
 * DMA works in circular mode
 */
int32_t     audio_raw_buff[2 * AUDIO_BUFF_SIZE];

/**
 * FFT size is the same as audio buffer size.
 * We have to process all input elements
 */
#define FFT_SIZE                                (AUDIO_BUFF_SIZE)

/**
 * FFT input samples to be processed
 */
float32_t   audio_buff_fft_in[FFT_SIZE];

/**
 * Output buffer contains array of pairs (real-imag) numbers:
 * real0, imag0, real1, imag1, ..., real[FFT_SIZE / 2 - 1], imag[FFT_SIZE / 2 - 1].
 *
 * Real FFT result (response) contains only half of spectrum.
 * Spectrum is normally mirrored in frequency and only half of it is useful.
 * Since we have real and imag pair, we still need full FFT size array of half-frequency-response
 */
float32_t   audio_buff_fft_out[FFT_SIZE];

/**
 * Variable contains absolute values, calculated from audio_buff_fft_out variable:
 *
 * out_real[x] = sqrt(in[2 * x]^2 + in[2 * x + 1]^2);
 *
 * We need half of FFT input as we are combining real and imag part to absolute value
 */
float32_t   audio_buff_fft_out_real[FFT_SIZE / 2];

/**
 * \brief           Real-FFT instance
 */
arm_rfft_fast_instance_f32 
rfft_f32;

/**
 * \brief           IIR filter instance
 */
arm_biquad_casd_df1_inst_f32
iir_filt;

/**
 * Variable holding IIR filter states.
 * Each state requires 4 elements
 */
float32_t iir_filt_states[4];

/**
 * IIR filter coefficients
 */
float32_t
iir_filt_coeffs[] = {
    1, -1, 0, 0.8,   0
};

char tmp_text[100];
uint32_t processed_count;
uint32_t processed_count_divider;

/**
 * \brief           User thread
 */
void
user_thread(void const * arg) {
    gui_handle_p h, h_text;
    gui_graph_data_p d;
    size_t i, j;
    int32_t* psrc = NULL;
    uint32_t update = 0;
    float32_t max_value;
    uint32_t max_index;
    
    h = gui_graph_create(GUI_ID_GRAPH_FFT, 10, 10, 780, 400, NULL, gui_graph_callback, 0);
    d = gui_graph_data_create(0, GUI_GRAPH_TYPE_YT, (FFT_SIZE / 2));
    gui_graph_attachdata(h, d);                 /* Attach data to graph */
    
    gui_graph_setminx(h, 0);
    gui_graph_setminy(h, 0);
    
    gui_graph_setmaxx(h, (FFT_SIZE / 2) - 1);
    gui_graph_setmaxy(h, 1000);
    gui_graph_zoomreset(h);
    
    gui_graph_data_setcolor(d, GUI_COLOR_BLUE);
    
    /* Create texts for band values */
    h_text = gui_textview_create(0, 10, 410, 780, 50, NULL, NULL, 0);
    gui_widget_alloctextmemory(h_text, 100);
    gui_textview_setcolor(h_text, GUI_TEXTVIEW_COLOR_TEXT, GUI_COLOR_GREEN);
    
    /* Prepare biquad cascade filter, for DC voltage remove */
    arm_biquad_cascade_df1_init_f32(&iir_filt, 1, iir_filt_coeffs, iir_filt_states);

    /* Start DFSDM data reception */
    HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, (int32_t *)audio_raw_buff, 2 * AUDIO_BUFF_SIZE);
    
    /* Calculate indexes for frequency bands */
    for (i = 0; i < ARR_SIZE(freq_bands); i++) {
        freq_bands[i].freq_start_index = (uint32_t)((float)freq_bands[i].freq_start / ((float)AUDIO_SAMPLING_FREQUENCY / (float)(AUDIO_BUFF_SIZE)));
        freq_bands[i].freq_end_index = (uint32_t)((float)freq_bands[i].freq_end / ((float)AUDIO_SAMPLING_FREQUENCY / (float)(AUDIO_BUFF_SIZE)));
    }
    
    while (1) {
        /*
         * If half-transfer complete DMA is ready, 
         * process first part of buffer
         */
        if (DmaRecHalfBuffCplt) {               /* DFSDM half-complete check */
            DmaRecHalfBuffCplt = 0; 
            psrc = &audio_raw_buff[0];
        }
        
        /*
         * If transfer complete DMA is ready, 
         * process second part of buffer
         */
        if (DmaRecBuffCplt) {                   /* DFSDM transfer-complete check */
            DmaRecBuffCplt = 0;
            psrc = &audio_raw_buff[AUDIO_BUFF_SIZE];
        }

        /* Anything to process? */
        if (psrc != NULL) {                     /* Check if anything to process */
            /* Do the post processing, shift data down 8-bits */
            for (i = 0; i < ARR_SIZE(audio_buff_fft_in); i++) {
                audio_buff_fft_in[i] = (float32_t)(psrc[i] >> 16);
            }
            
            /* Process with high-pass filter to remove DC voltage */
            /* Set the same buffer as input and output */
            arm_biquad_cascade_df1_f32(&iir_filt, audio_buff_fft_in, audio_buff_fft_in, ARR_SIZE(audio_buff_fft_in));
            
            /* Check if UART is free to send new data */
            if (++update) {
                float32_t scale;
                update = 0;
                
                /* Process with Real-FFT */
                arm_rfft_fast_init_f32(&rfft_f32, FFT_SIZE);
                arm_rfft_fast_f32(&rfft_f32, audio_buff_fft_in, audio_buff_fft_out, 0);
                arm_cmplx_mag_f32(audio_buff_fft_out, audio_buff_fft_out_real, ARR_SIZE(audio_buff_fft_out_real));  /* Calculate absolute value from real-cplx pairs */               
                
                /* Normalize FFT result back to normal */
                for (i = 0; i < ARR_SIZE(audio_buff_fft_out_real); i++) {
                    audio_buff_fft_out_real[i] /= FFT_SIZE;
                }
                
//                /* Calculate max value and scale */
//                arm_max_f32(audio_buff_fft_out_real, ARR_SIZE(audio_buff_fft_out_real), &max_value, &max_index);
//                scale = 32767.0f / max_value;
//                
//                /* Add data to plot */
//                for (i = 0; i < ARR_SIZE(audio_buff_fft_out_real); i++) {
//                    //gui_graph_data_addvalue(d, 0, (int16_t)(audio_buff_fft_out_real[i] * scale));
//                }
//                
//                /* Set maximal graph value */
//                gui_graph_setmaxy(h, max_value * scale);
//                gui_graph_zoomreset(h);         /* Reset zoom to default */
//                gui_widget_invalidate(h);
                
                /* Calculate sum values */
                processed_count++;              /* Increase processed count used for averaging */
                processed_count_divider = processed_count > 5 ? 5 : processed_count;
                for (j = 0; j < ARR_SIZE(freq_bands); j++) {
                    freq_bands[j].sum = 0;
                    for (i = 0; i < ARR_SIZE(audio_buff_fft_out_real); i++) {
                        if (i >= freq_bands[j].freq_start_index && i < freq_bands[j].freq_end_index) {
                            freq_bands[j].sum += audio_buff_fft_out_real[i];
                        }
                    }
                    
                    /* Calculate new average value */
                    freq_bands[j].average = (freq_bands[j].average * ((float)(processed_count_divider) - 1.0f) + freq_bands[j].sum * 1.01f) / ((float)processed_count_divider);
                
                    /* Decide to turn on/off LED */
                    if (freq_bands[j].sum > freq_bands[j].average) {
                        freq_bands[j].gpio->BSRR = freq_bands[j].pin;
                    } else {
                        freq_bands[j].gpio->BSRR = freq_bands[j].pin << 16;
                    }
                }
            }
            psrc = NULL;                        /* Clear pbuf once not used anymore */
        }
        osDelay(1);
    }
}

void
HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
    DmaRecHalfBuffCplt = 1;
}

void
HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
    DmaRecBuffCplt = 1;
}

/* DFSDM1 init function */
static void
MX_DFSDM1_Init(void) {
    hdfsdm1_filter0.Instance = DFSDM1_Filter0;
    hdfsdm1_filter0.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    hdfsdm1_filter0.Init.RegularParam.FastMode = ENABLE;
    hdfsdm1_filter0.Init.RegularParam.DmaMode = ENABLE;
    hdfsdm1_filter0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
    hdfsdm1_filter0.Init.FilterParam.Oversampling = 2000000 / AUDIO_SAMPLING_FREQUENCY;
    hdfsdm1_filter0.Init.FilterParam.IntOversampling = 1;
    HAL_DFSDM_FilterInit(&hdfsdm1_filter0);

    hdfsdm1_channel1.Instance = DFSDM1_Channel1;
    hdfsdm1_channel1.Init.OutputClock.Activation = ENABLE;
    hdfsdm1_channel1.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM;
    hdfsdm1_channel1.Init.OutputClock.Divider = 100;
    hdfsdm1_channel1.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
    hdfsdm1_channel1.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
    hdfsdm1_channel1.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
    hdfsdm1_channel1.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
    hdfsdm1_channel1.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    hdfsdm1_channel1.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
    hdfsdm1_channel1.Init.Awd.Oversampling = 1;
    hdfsdm1_channel1.Init.Offset = 0;
    hdfsdm1_channel1.Init.RightBitShift = 0;
    if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel1) != HAL_OK) {

    }
    HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter0, DFSDM_CHANNEL_1, DFSDM_CONTINUOUS_CONV_ON);
}

/** 
 * Enable DMA controller clock
 */
static void
MX_DMA_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/**
 * \brief           Initialize LEDs on F769-Discovery board
 */
static void
led_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    GPIO_InitStruct.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LED3_PIN;
    HAL_GPIO_Init(LED3_PORT, &GPIO_InitStruct);
}

/*
 * Pinout Configuration
 */
static void
MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

static uint32_t HAL_RCC_DFSDM1_CLK_ENABLED = 0;
static uint32_t DFSDM1_Init = 0;

void
HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef* hdfsdm_filter) {
    GPIO_InitTypeDef GPIO_InitStruct;
    if (DFSDM1_Init == 0) {
        /* USER CODE BEGIN DFSDM1_MspInit 0 */

        /* USER CODE END DFSDM1_MspInit 0 */
        /* Peripheral clock enable */
        HAL_RCC_DFSDM1_CLK_ENABLED++;
        if (HAL_RCC_DFSDM1_CLK_ENABLED == 1) {
            __HAL_RCC_DFSDM1_CLK_ENABLE();
        }

        /**DFSDM1 GPIO Configuration    
        PD3     ------> DFSDM1_CKOUT
        PC3     ------> DFSDM1_DATIN1 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_DFSDM1;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_DFSDM1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* DFSDM1 interrupt Init */
        HAL_NVIC_SetPriority(DFSDM1_FLT0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DFSDM1_FLT0_IRQn);
        /* USER CODE BEGIN DFSDM1_MspInit 1 */

        /* USER CODE END DFSDM1_MspInit 1 */
    }

    /* DFSDM1 DMA Init */
    /* DFSDM1_FLT0 Init */
    if (hdfsdm_filter->Instance == DFSDM1_Filter0) {
        hdma_dfsdm1_flt0.Instance = DMA2_Stream0;
        hdma_dfsdm1_flt0.Init.Channel = DMA_CHANNEL_8;
        hdma_dfsdm1_flt0.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_dfsdm1_flt0.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_dfsdm1_flt0.Init.MemInc = DMA_MINC_ENABLE;
        hdma_dfsdm1_flt0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_dfsdm1_flt0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_dfsdm1_flt0.Init.Mode = DMA_CIRCULAR;
        hdma_dfsdm1_flt0.Init.Priority = DMA_PRIORITY_LOW;
        hdma_dfsdm1_flt0.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_dfsdm1_flt0) != HAL_OK) {

        }

        /* Several peripheral DMA handle pointers point to the same DMA handle.
        Be aware that there is only one stream to perform all the requested DMAs. */
        __HAL_LINKDMA(hdfsdm_filter, hdmaInj, hdma_dfsdm1_flt0);
        __HAL_LINKDMA(hdfsdm_filter, hdmaReg, hdma_dfsdm1_flt0);
    }
}

void
HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef* hdfsdm_channel) {
    GPIO_InitTypeDef GPIO_InitStruct;
    if (DFSDM1_Init == 0) {
        /* Peripheral clock enable */
        HAL_RCC_DFSDM1_CLK_ENABLED++;
        if (HAL_RCC_DFSDM1_CLK_ENABLED == 1) {
            __HAL_RCC_DFSDM1_CLK_ENABLE();
        }
        
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_DFSDM1;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_DFSDM1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
}

void
HAL_DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef* hdfsdm_filter) {
    DFSDM1_Init--;
    if (DFSDM1_Init == 0) {
        /* Peripheral clock disable */
        __HAL_RCC_DFSDM1_CLK_DISABLE();
        
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

        /* DFSDM1 interrupt DeInit */
        HAL_NVIC_DisableIRQ(DFSDM1_FLT0_IRQn);

        /* DFSDM1 DMA DeInit */
        HAL_DMA_DeInit(hdfsdm_filter->hdmaInj);
        HAL_DMA_DeInit(hdfsdm_filter->hdmaReg);
    }
}

void
HAL_DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef* hdfsdm_channel) {
    DFSDM1_Init-- ;
    if (DFSDM1_Init == 0) {
        /* USER CODE BEGIN DFSDM1_MspDeInit 0 */

        /* USER CODE END DFSDM1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_DFSDM1_CLK_DISABLE();

        /**DFSDM1 GPIO Configuration    
        PD3     ------> DFSDM1_CKOUT
        PC3     ------> DFSDM1_DATIN1 
        */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

        /* USER CODE BEGIN DFSDM1_MspDeInit 1 */

        /* USER CODE END DFSDM1_MspDeInit 1 */
    }
}


/**
* @brief This function handles DMA2 stream0 global interrupt.
*/
void
DMA2_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_dfsdm1_flt0);
}

/**
* @brief This function handles DFSDM1 filter0 global interrupt.
*/
void
DFSDM1_FLT0_IRQHandler(void) {
    HAL_DFSDM_IRQHandler(&hdfsdm1_filter0);
}

/**
 * \brief           Initialization thread for system
 */
void
init_thread(void const * arg) {
    TM_EXTI_Attach(GPIOI, GPIO_PIN_13, TM_EXTI_Trigger_Rising);
    TS.Orientation = TOUCH_ORIENT_INVERT_Y;
    TM_TOUCH_Init(NULL, &TS);
    
    /* DFSDM setup */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DFSDM1_Init();
    led_init();
    
    /* GUI setup */
    gui_init();
    gui_widget_setfontdefault(&GUI_Font_Arial_Bold_18);
    
    osThreadCreate(osThread(user_thread), NULL);/* Create user thread */
    osThreadTerminate(NULL);                    /* Stop current thread */
}

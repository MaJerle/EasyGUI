#include "lcd_discovery.h"

#define LCD_HSYNC                   41
#define LCD_HBP                     13
#define LCD_HFP                     32
#define LCD_VSYNC                   10
#define LCD_VBP                     2
#define LCD_VFP                     2

/* Variables */
LTDC_HandleTypeDef LTDCHandle;
extern gui_layer_t Layers[];

void _LCD_Init(void) {
    static RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    LTDC_LayerCfgTypeDef layer_cfg;

    /**********************************/
    /* Set up GPIO pins for LCD drive */
    /**********************************/
    /* Init GPIO pins for LTDC */
    TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_4, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
    TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
    TM_GPIO_InitAlternate(GPIOI, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
    TM_GPIO_InitAlternate(GPIOJ, GPIO_PIN_All & ~(GPIO_PIN_12), TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
    TM_GPIO_InitAlternate(GPIOK, 0x00FF & ~(GPIO_PIN_3), TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);

    /* Init pins for LCD control */
    /* Display enable */
    TM_GPIO_Init(GPIOI, GPIO_PIN_12, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Low);
    /* Backlight control */
    TM_GPIO_Init(GPIOK, GPIO_PIN_3, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Low);

    /* Display ON */
    TM_GPIO_SetPinHigh(GPIOI, GPIO_PIN_12);	
    TM_GPIO_SetPinHigh(GPIOK, GPIO_PIN_3);

    /**********************************/
    /*  Set up LTDC driver            */
    /**********************************/
    /* Enable LTDC and DMA2D clocks */
    __HAL_RCC_DMA2D_CLK_ENABLE();
    __HAL_RCC_LTDC_CLK_ENABLE();

    /* Disable LTDC */
    LTDC->GCR &= ~LTDC_GCR_LTDCEN;

    /* Timing configuration */     
    LTDCHandle.Init.HorizontalSync = LCD_HSYNC - 1;
    LTDCHandle.Init.VerticalSync = LCD_VSYNC - 1;
    LTDCHandle.Init.AccumulatedHBP = LCD_HSYNC + LCD_HBP - 1;
    LTDCHandle.Init.AccumulatedVBP = LCD_VSYNC + LCD_VBP - 1;  
    LTDCHandle.Init.AccumulatedActiveH = LCD_HEIGHT + LCD_VSYNC + LCD_VBP - 1;
    LTDCHandle.Init.AccumulatedActiveW = LCD_WIDTH + LCD_HSYNC + LCD_HBP - 1;
    LTDCHandle.Init.TotalWidth = LCD_WIDTH + LCD_HSYNC + LCD_HBP + LCD_HFP - 1; 
    LTDCHandle.Init.TotalHeigh = LCD_HEIGHT + LCD_VSYNC + LCD_VBP + LCD_VFP - 1;

    /* Configure PLLSAI prescalers for LCD */
    /* Enable Pixel Clock */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/2 = 96 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 96/4 = 24 Mhz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Initialize the LCD pixel width and pixel height */
    LTDCHandle.LayerCfg->ImageWidth  = LCD_WIDTH;
    LTDCHandle.LayerCfg->ImageHeight = LCD_HEIGHT;

    /* Background value */
    LTDCHandle.Init.Backcolor.Blue = 0;
    LTDCHandle.Init.Backcolor.Green = 0;
    LTDCHandle.Init.Backcolor.Red = 0;

    /* Polarity */
    LTDCHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    LTDCHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
    LTDCHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
    LTDCHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    /* Init LTDC */
    LTDCHandle.Instance = LTDC;
    HAL_LTDC_Init(&LTDCHandle);
    
    /**********************************/
    /*  Set up LTDC layers            */
    /**********************************/
    /* Layer Init */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = LCD_WIDTH;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = LCD_HEIGHT; 
#if defined(LCD_COLOR_FORMAT_RGB565)
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
#else
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
#endif
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = LCD_WIDTH;
    layer_cfg.ImageHeight = LCD_HEIGHT;

    /* Config layer 1 */
    layer_cfg.Alpha = 255;
    layer_cfg.FBStartAdress = LCD_FRAME_BUFFER;
    HAL_LTDC_ConfigLayer(&LTDCHandle, &layer_cfg, 0);

    /* Config layer 2 */
    layer_cfg.Alpha = 0;
    layer_cfg.FBStartAdress = LCD_FRAME_BUFFER + LCD_FRAME_BUFFER_SIZE;
    HAL_LTDC_ConfigLayer(&LTDCHandle, &layer_cfg, 1);

    /* Init line event interrupt */
    HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);

    HAL_LTDC_SetAlpha(&LTDCHandle, 255, 0);
    HAL_LTDC_SetAlpha(&LTDCHandle, 0, 1);
    
    HAL_LTDC_ProgramLineEvent(&LTDCHandle, 0);
}

/* IRQ callback for line event */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
    uint8_t i = 0;
    for (i = 0; i < GUI_LAYERS; i++) {
        if (Layers[i].Pending) {                /* Is layer waiting for redraw operation */
            LTDC_LAYER(hltdc, !!i)->CACR &= ~(LTDC_LxCACR_CONSTA);
            LTDC_LAYER(hltdc, !!i)->CACR = (255);
            LTDC_LAYER(hltdc, !i)->CACR &= ~(LTDC_LxCACR_CONSTA);
            LTDC_LAYER(hltdc, !i)->CACR = (0);
            
            __HAL_LTDC_RELOAD_CONFIG(hltdc);
            gui_lcd_confirmactivelayer(i);
            break;
        }
    }
    HAL_LTDC_ProgramLineEvent(hltdc, 0); 
}

/* IRQ function for LTDC */
void LTDC_IRQHandler(void) {
    HAL_LTDC_IRQHandler(&LTDCHandle);
}

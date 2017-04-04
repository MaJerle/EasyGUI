/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_ll.h"

#include "tm_stm32_sdram.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
/* Set pixel settings */
#define LCD_WIDTH               480
#define LCD_HEIGHT              272
#define LCD_PIXEL_SIZE          4

/* LCD configuration */
#define LCD_HSYNC               41
#define LCD_HBP                 13
#define LCD_HFP                 32
#define LCD_VSYNC               10
#define LCD_VBP                 2
#define LCD_VFP                 2

/* Frequency division for LTDC */
#define LCD_FREQUENCY_DIV       4

/* Frame buffer settings */
#define LCD_FRAME_BUFFER        ((uint32_t)SDRAM_START_ADR)
#define LCD_FRAME_BUFFER_SIZE   ((uint32_t)(LCD_WIDTH * LCD_HEIGHT * LCD_PIXEL_SIZE))

/* Number of layers */
#define GUI_LAYERS              2
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static LTDC_HandleTypeDef LTDCHandle;
static DMA2D_HandleTypeDef DMA2DHandle;
static GUI_Layer_t Layers[GUI_LAYERS];

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
void _LCD_InitLCD(void) {
    RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;
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

    /* Set LTDC instance */
    LTDCHandle.Instance = LTDC;
    DMA2DHandle.Instance = DMA2D;

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
    periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
    periph_clk_init_struct.PLLSAI.PLLSAIR = LCD_FREQUENCY_DIV;
    periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);

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
    HAL_LTDC_Init(&LTDCHandle);

    /**********************************/
    /*  Set up LTDC layers            */
    /**********************************/
    /* Layer Init */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = LCD_WIDTH;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = LCD_HEIGHT; 
#if LCD_PIXEL_SIZE == 2
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
    HAL_LTDC_ProgramLineEvent(&LTDCHandle, 0); 
    HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);

    HAL_LTDC_SetAlpha(&LTDCHandle, 255, 0);
    HAL_LTDC_SetAlpha(&LTDCHandle, 0, 1);
}

void LCD_Init(GUI_LCD_t* LCD) {
    TM_SDRAM_Init();                /* Init SDRAM */

    _LCD_InitLCD();                 /* Init LCD */
}

void LCD_SetPixel(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Color_t color) {
    uint32_t addr = LCD_FRAME_BUFFER + (layer * LCD_FRAME_BUFFER_SIZE) + LCD_PIXEL_SIZE * (LCD_WIDTH * y + x);
    *(volatile uint32_t *)(addr) = color;
}

GUI_Color_t LCD_GetPixel(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y) {
    return *(volatile GUI_Color_t *)(LCD_FRAME_BUFFER + (layer * LCD_FRAME_BUFFER_SIZE) + LCD_PIXEL_SIZE * (LCD_WIDTH * y + x));
}

void LCD_Fill(GUI_LCD_t* LCD, uint8_t layer, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t OffLine, GUI_Color_t color) {
    DMA2D->CR = 0x00030000UL;                       /* Register to memory and TCIE */
    DMA2D->OCOLR = color;                           /* Color to be used */
    DMA2D->OMAR = (uint32_t)dst;                    /* Destination address */
    DMA2D->OOR = OffLine;                           /* Destination line offset */
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;     /* Defines the number of pixels to be transfered */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; /* Size configuration of area to be transfered */
    DMA2D->CR |= DMA2D_CR_START;                    /* Start actual transfer */
    
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait previous operation to finish */
}

void LCD_Copy(GUI_LCD_t* LCD, uint8_t layer, void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    DMA2D->CR = 0x00000000;             /* Memory to memory transfer mode */

    /* Set up pointers */
    DMA2D->FGMAR = (uint32_t)src;                       
    DMA2D->OMAR = (uint32_t)dst;                       
    DMA2D->FGOR = offLineSrc;                      
    DMA2D->OOR = offLineDst; 

    /* Set up pixel format */  
    DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;

    /* Set up size */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 

    /* Start DMA2D */
    DMA2D->CR |= DMA2D_CR_START; 

    while (DMA2D->CR & DMA2D_CR_START);
}

void LCD_DrawHLine(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color) {
    uint32_t addr = LCD_FRAME_BUFFER + (layer * LCD_FRAME_BUFFER_SIZE) + (LCD_PIXEL_SIZE * (LCD->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, length, 1, LCD->Width - length, color);
}

void LCD_DrawVLine(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color) {
    uint32_t addr = Layers[layer].StartAddress + (LCD_PIXEL_SIZE * (LCD->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, 1, length, LCD->Width - 1, color);
}

void LCD_FillRect(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Color_t color) {
    uint32_t addr = Layers[layer].StartAddress + (LCD_PIXEL_SIZE * (LCD->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, xSize, ySize, LCD->Width - xSize, color);
}

/* IRQ function for LTDC */
void LTDC_IRQHandler(void) {
    HAL_LTDC_IRQHandler(&LTDCHandle);
}

/* IRQ callback for line event */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
    uint8_t i = 0;
    for (i = 0; i < GUI_LAYERS; i++) {
        if (Layers[i].Pending) {                /* Is layer waiting for redraw operation */
            LTDC_LAYER(hltdc, 0)->CFBAR = Layers[i].StartAddress;
            __HAL_LTDC_RELOAD_CONFIG(hltdc);
            GUI_LCD_ConfirmActiveLayer(i);
        }
    }
    HAL_LTDC_ProgramLineEvent(&LTDCHandle, 0); 
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
/* Called for function setup for low-level driver processing */
uint8_t GUI_LL_Init(GUI_LCD_t* LCD, GUI_LL_t* LL) {
    uint8_t i = 0;
    
    /*******************************/
    /* Set up LCD data             */
    /*******************************/
    LCD->Width = LCD_WIDTH;
    LCD->Height = LCD_HEIGHT;
    
    /*******************************/
    /* Set layers count            */
    /*******************************/
    LCD->LayersCount = GUI_LAYERS;              /* We have 2 layers for our low-level driver */
    LCD->Layers = Layers;
    for (i = 0; i < GUI_LAYERS; i++) {          /* Set each layer */
        Layers[i].Num = i;
        Layers[i].StartAddress = LCD_FRAME_BUFFER + (i * LCD_FRAME_BUFFER_SIZE);
    }
    
    /*******************************/
    /* Set up LCD drawing routines */
    /*******************************/
    LL->Init = &LCD_Init;                       /* Must be set by user */
    LL->GetPixel = &LCD_GetPixel;               /* Must be set by user */
    LL->SetPixel = &LCD_SetPixel;               /* Must be set by user */
    
    LL->Copy = &LCD_Copy;                       /* Set copy memory routine */
    LL->DrawHLine = &LCD_DrawHLine;             /* Set drawing vertical line routine */
    LL->DrawVLine = &LCD_DrawVLine;             /* Set drawing horizontal line routine */
    LL->Fill = &LCD_Fill;                       /* Set fill screen routine */
    LL->FillRect = &LCD_FillRect;               /* Set fill rectangle routine */
    
    return 0;                                   /* Initialization successful */
}

uint8_t GUI_LL_Control(GUI_LCD_t* LCD, GUI_LL_Command_t cmd, void* data) {
    switch (cmd) {
        case GUI_LL_Command_SetActiveLayer: {   /* Set new active layer */
            GUI_Byte layer = *(GUI_Byte *)data; /* Read layer as byte */
            LCD->Layers[layer].Pending = 1;     /* Set layer as pending and redraw on next reload */
            break;
        }
        default:
            break;
    }
    return 0;
}

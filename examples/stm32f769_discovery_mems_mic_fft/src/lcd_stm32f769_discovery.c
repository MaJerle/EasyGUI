#include "lcd_discovery.h"

extern gui_layer_t Layers[];

/* Define physical screen sizes */
#define XSIZE_PHYS 800
#define YSIZE_PHYS 480

/* Define the screen area: can be 2 or 4 */
#define ZONES           2
#define VSYNC           1
#define VBP             1
#define VFP             1
#define VACT            LCD_HEIGHT
#define HSYNC           1
#define HBP             1
#define HFP             1
#define HACT            (LCD_WIDTH / ZONES)

LTDC_HandleTypeDef  LTDCHandle;  
DSI_HandleTypeDef   hdsi;

DSI_VidCfgTypeDef   hdsivideo_handle;
DSI_CmdCfgTypeDef   CmdCfg;
DSI_LPCmdTypeDef    LPCmd;
DSI_PLLInitTypeDef  dsiPllInit;

/* Screen needed definitions */
uint8_t pColLeft[]    = {0x00, 0x00, 0x01, 0x8F}; /*   0 -> 399 */
uint8_t pColRight[]   = {0x01, 0x90, 0x03, 0x1F}; /* 400 -> 799 */
uint8_t pPage[]       = {0x00, 0x00, 0x01, 0xDF}; /*   0 -> 479 */
uint8_t pSyncLeft[]   = {0x02, 0x15};             /* Scan @ 533 */

#define INVALID_AREA            0
#define LEFT_AREA               1
#define RIGHT_AREA              2

static int32_t active_area = INVALID_AREA;
int8_t pending_buffer = -1;

gui_layer_t* currentLayer;

#if 0
/* ZONES of the screen */
static uint8_t
pCols[ZONES][4] = {
#if     (ZONES == 4)
    {0x00, 0x00, 0x00, 0xC7}, /*   0 -> 199 */
    {0x00, 0xC8, 0x01, 0x8F}, /* 200 -> 399 */
    {0x01, 0x90, 0x02, 0x57}, /* 400 -> 599 */
    {0x02, 0x58, 0x03, 0x1F}, /* 600 -> 799 */
#elif   (ZONES == 2)
    {0x00, 0x00, 0x01, 0x8F}, /*   0 -> 399 */
    {0x01, 0x90, 0x03, 0x1F}  /* 400 -> 799 */
#elif   (ZONES == 1 )
    {0x00, 0x00, 0x01, 0x3F}, /*   0 -> 799 */
#endif  
};
#endif

/**
 * @brief  BSP LCD Reset
 *         Hw reset the LCD DSI activating its XRES signal (active low for some time)
 *         and desactivating it later.
 */
static void
BSP_LCD_Reset(void) {
    GPIO_InitTypeDef  gpio_init_structure;

    __HAL_RCC_GPIOJ_CLK_ENABLE();

    /* Configure the GPIO on PJ15 */
    gpio_init_structure.Pin   = GPIO_PIN_15;
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

    /* Activate XRES active low */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_RESET);

    HAL_Delay(20); /* wait 20 ms */

    /* Desactivate XRES */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_SET);

    /* Wait for 10ms after releasing XRES before sending commands */
    HAL_Delay(10);
}

/**
  * @brief  Initialize the BSP LCD Msp.
  * Application can surcharge if needed this function implementation
  */
__weak void
BSP_LCD_MspInit(void) {
    /** @brief Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();

    /** @brief Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();

    /** @brief Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /** @brief Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();

    /** @brief Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();

    /** @brief Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();

    /** @brief NVIC configuration for LTDC interrupt that is now enabled */
    HAL_NVIC_SetPriority(LTDC_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);

    /** @brief NVIC configuration for DMA2D interrupt that is now enabled */
    HAL_NVIC_SetPriority(DMA2D_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);

    /** @brief NVIC configuration for DSI interrupt that is now enabled */
    HAL_NVIC_SetPriority(DSI_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DSI_IRQn);
}

/**
  * @brief  Initialize the LTDC
  * @param  None
  * @retval None
  */
void
LTDC_Init(void) {
    LTDCHandle.Instance = LTDC;
    
    /* DeInit */
    HAL_LTDC_DeInit(&LTDCHandle);

    /* LTDC Config */
    /* Timing and polarity */
    LTDCHandle.Init.HorizontalSync = HSYNC;
    LTDCHandle.Init.VerticalSync = VSYNC;
    LTDCHandle.Init.AccumulatedHBP = HSYNC + HBP;
    LTDCHandle.Init.AccumulatedVBP = VSYNC + VBP;
    LTDCHandle.Init.AccumulatedActiveH = VSYNC + VBP + VACT;
    LTDCHandle.Init.AccumulatedActiveW = HSYNC + HBP + HACT;
    LTDCHandle.Init.TotalHeigh = VSYNC + VBP + VACT + VFP;
    LTDCHandle.Init.TotalWidth = HSYNC + HBP + HACT + HFP;

    /* background value */
    LTDCHandle.Init.Backcolor.Blue = 0;
    LTDCHandle.Init.Backcolor.Green = 0;
    LTDCHandle.Init.Backcolor.Red = 0;

    /* Polarity */
    LTDCHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    LTDCHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    LTDCHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    LTDCHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    HAL_LTDC_Init(&LTDCHandle);
}

/**
  * @brief  Initializes the LCD layers.
  * @param  LayerIndex: Layer foreground or background
  * @param  FB_Address: Layer frame buffer
  * @retval None
  */
void LCD_LayertInit(uint16_t LayerIndex, uint32_t Address) {
    LTDC_LayerCfgTypeDef  Layercfg;

    /* Layer Init */
    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = 400;
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = 480; 
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    Layercfg.FBStartAdress = Address;
    Layercfg.Alpha = 255;
    Layercfg.Alpha0 = 0;
    Layercfg.Backcolor.Blue = 0;
    Layercfg.Backcolor.Green = 0;
    Layercfg.Backcolor.Red = 0;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    Layercfg.ImageWidth = 400;
    Layercfg.ImageHeight = 480;

    HAL_LTDC_ConfigLayer(&LTDCHandle, &Layercfg, LayerIndex); 
}

/**
 * \brief           Initialize LCD
 *
 *                  Called from GUI init callback function
 */
void
_LCD_Init(void) {
    DSI_PHY_TimerTypeDef PhyTimings;  
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    
    /* Toggle Hardware Reset of the DSI LCD using
    * its XRES signal (active low) */
    BSP_LCD_Reset();

    /* Call first MSP Initialize only in case of first initialization
    * This will set IP blocks LTDC, DSI and DMA2D
    * - out of reset
    * - clocked
    * - NVIC IRQ related to IP blocks enabled
    */
    BSP_LCD_MspInit();

    /* LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 417 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 417 MHz / 5 = 83.4 MHz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 83.4 / 2 = 41.7 MHz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
    hdsi.Instance = DSI;

    HAL_DSI_DeInit(&(hdsi));

    dsiPllInit.PLLNDIV  = 100;
    dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV5;
    dsiPllInit.PLLODF  = DSI_PLL_OUT_DIV1;  

    hdsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    hdsi.Init.TXEscapeCkdiv = 0x4;
    HAL_DSI_Init(&(hdsi), &(dsiPllInit));

    /* Configure the DSI for Command mode */
    CmdCfg.VirtualChannelID      = 0;
    CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
    CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
    CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
    CmdCfg.ColorCoding           = DSI_RGB888;
    CmdCfg.CommandSize           = HACT;
    CmdCfg.TearingEffectSource   = DSI_TE_DSILINK;
    CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
    CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
    CmdCfg.AutomaticRefresh      = DSI_AR_ENABLE;
    CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
    HAL_DSI_ConfigAdaptedCommandMode(&hdsi, &CmdCfg);

    LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_ENABLE;
    LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_ENABLE;
    LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_ENABLE;
    LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_ENABLE;
    LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_ENABLE;
    LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_ENABLE;
    LPCmd.LPGenLongWrite        = DSI_LP_GLW_ENABLE;
    LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_ENABLE;
    LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_ENABLE;
    LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_ENABLE;
    LPCmd.LPDcsLongWrite        = DSI_LP_DLW_ENABLE;
    HAL_DSI_ConfigCommand(&hdsi, &LPCmd);

    /* Initialize LTDC */
    LTDC_Init();

    /* Start DSI */
    HAL_DSI_Start(&(hdsi));

    /* Configure DSI PHY HS2LP and LP2HS timings */
    PhyTimings.ClockLaneHS2LPTime = 35;
    PhyTimings.ClockLaneLP2HSTime = 35;
    PhyTimings.DataLaneHS2LPTime = 35;
    PhyTimings.DataLaneLP2HSTime = 35;
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 10;
    HAL_DSI_ConfigPhyTimer(&hdsi, &PhyTimings);

    /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)*/
    OTM8009A_Init(OTM8009A_COLMOD_RGB888, 0x01);

    /* Reconfigure the DSI for HS Command mode */
    LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_DISABLE;
    LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_DISABLE;
    LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_DISABLE;
    LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_DISABLE;
    LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_DISABLE;
    LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_DISABLE;
    LPCmd.LPGenLongWrite        = DSI_LP_GLW_DISABLE;
    LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_DISABLE;
    LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_DISABLE;
    LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_DISABLE;
    LPCmd.LPDcsLongWrite        = DSI_LP_DLW_DISABLE;
    HAL_DSI_ConfigCommand(&hdsi, &LPCmd);

    HAL_DSI_ConfigFlowControl(&hdsi, DSI_FLOW_CONTROL_BTA);

    /* Send Display Off DCS Command to display */
    HAL_DSI_ShortWrite(&(hdsi),
                     0,
                     DSI_DCS_SHORT_PKT_WRITE_P1,
                     OTM8009A_CMD_DISPOFF,
                     0x00);

    /* Refresh the display */
    HAL_DSI_Refresh(&hdsi);
  
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    LCD_LayertInit(0, Layers[0].start_address);
  
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    HAL_LTDC_SetPitch(&LTDCHandle, 800, 0);
    
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    /*******************************/
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColLeft);
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_PASET, pPage);

    /* Send Display On DCS Command to display */
    HAL_DSI_ShortWrite(&(hdsi),
                     0,
                     DSI_DCS_SHORT_PKT_WRITE_P1,
                     OTM8009A_CMD_DISPON,
                     0x00);
    
    currentLayer = &Layers[0];
    pending_buffer = 0;
    active_area = LEFT_AREA;
    
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 2, OTM8009A_CMD_WRTESCN, pSyncLeft);
    
    /* Refresh the display */
    HAL_DSI_Refresh(&hdsi);
}

/**
  * @brief  End of Refresh DSI callback.
  * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
  *               the configuration information for the DSI.
  * @retval None
  */
void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi) {
    if (pending_buffer >= 0) {
        if (active_area == LEFT_AREA) {
            /* Mask the TE */
            HAL_DSI_ShortWrite(hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_TEOFF, 0x00);

            __HAL_DSI_WRAPPER_DISABLE(hdsi);
            LTDC_LAYER(&LTDCHandle, 0)->CFBAR = currentLayer->start_address + 400 * 4;
            __HAL_LTDC_RELOAD_CONFIG(&LTDCHandle);
            __HAL_DSI_WRAPPER_ENABLE(hdsi);

            HAL_DSI_LongWrite(hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColRight);
            HAL_DSI_Refresh(hdsi);    

        } else if (active_area == RIGHT_AREA) {
            /* Find next pending layer */
            uint8_t i = 0;
            for (i = 0; i < GUI_LAYERS; i++) {
                if (Layers[i].pending) {
                    currentLayer = &Layers[i];
                    Layers[i].pending = 0;
                    gui_lcd_confirmactivelayer(i);
                    break;
                }
            }

            __HAL_DSI_WRAPPER_DISABLE(hdsi);
            LTDC_LAYER(&LTDCHandle, 0)->CFBAR = currentLayer != NULL ? currentLayer->start_address : Layers[0].start_address;
            __HAL_LTDC_RELOAD_CONFIG(&LTDCHandle);
            __HAL_DSI_WRAPPER_ENABLE(hdsi);
            HAL_DSI_LongWrite(hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColLeft);
            
            /* Start new refresh */
            HAL_DSI_LongWrite(hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 2, OTM8009A_CMD_WRTESCN, pSyncLeft); 
        }
    }
    active_area = (active_area == LEFT_AREA) ? RIGHT_AREA : LEFT_AREA; 
}

/**
  * @brief  DCS or Generic short/long write command
  * @param  NbrParams: Number of parameters. It indicates the write command mode:
  *                 If inferior to 2, a long write command is performed else short.
  * @param  pParams: Pointer to parameter values table.
  * @retval HAL status
  */
void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams) {
    if (NbrParams <= 1) {
        HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]); 
    } else {
        HAL_DSI_LongWrite(&hdsi,  0, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams); 
    } 
}

/**
  * @brief  This function handles DSI global interrupt request.
  * @param  None
  * @retval None
  */
void DSI_IRQHandler(void) {
    HAL_DSI_IRQHandler(&hdsi);
}

/**
  * @brief  This function handles LTDC interrupt request.
  * @param  None
  * @retval None
  */
void LTDC_IRQHandler(void) {
    HAL_LTDC_IRQHandler(&LTDCHandle);
}

void LTDC_ER_IRQHandler(void) { 
    /* Check the interrupt and clear flag */
    HAL_LTDC_IRQHandler(&LTDCHandle);
}

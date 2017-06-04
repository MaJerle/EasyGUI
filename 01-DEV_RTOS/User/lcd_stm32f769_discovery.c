#include "lcd_discovery.h"

extern LTDC_HandleTypeDef LTDCHandle;
extern GUI_Layer_t Layers[];

DSI_HandleTypeDef DSIHandle;
DSI_VidCfgTypeDef DSIVIDEOHandle;

#define LCD_OTM8009A_ID         ((uint32_t) 0)
#define ZONES                   4
#define HACT_OUT                LCD_WIDTH/ZONES      /* !!!! SCREEN DIVIDED INTO 2 AREAS !!!! */

volatile int32_t LCD_ActiveRegion = 1;
volatile int32_t LCD_Refershing = 0;

#define LCD_DSI_PIXEL_DATA_FMT_RBG565  DSI_RGB565   /*!< DSI packet pixel format chosen is RGB565 : 16 bpp */

/**
  * @brief  Reload immediately all LTDC Layers.
  * @param  __HANDLE__  LTDC handle
  * @retval None.
  */
#define __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(__HANDLE__)  ((__HANDLE__)->Instance->SRCR |= LTDC_SRCR_IMR)

uint8_t pPage[] = {0x00, 0x00, 0x01, 0xDF}; /*   0 -> 479 */
uint8_t pCols[ZONES][4] = {
#if (ZONES == 4 )
    {0x00, 0x00, 0x00, 0xC7}, /*   0 -> 199 */
    {0x00, 0xC8, 0x01, 0x8F}, /* 200 -> 399 */
    {0x01, 0x90, 0x02, 0x57}, /* 400 -> 599 */
    {0x02, 0x58, 0x03, 0x1F}, /* 600 -> 799 */
#elif (ZONES == 2 )
    {0x00, 0x00, 0x01, 0x8F}, /*   0 -> 399 */
    {0x01, 0x90, 0x03, 0x1F}  
#elif (ZONES == 1 )
    {0x00, 0x00, 0x03, 0x1F}, /*   0 -> 799 */
#endif  
};

void LCD_SetUpdateRegion(int idx) {
    HAL_DSI_LongWrite(&DSIHandle, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pCols[idx]);
}

/**
  * @brief  De-Initializes the BSP LCD Msp
  * Application can surcharge if needed this function implementation.
  */
void BSP_LCD_MspDeInit(void) {
    HAL_NVIC_DisableIRQ(LTDC_IRQn);
    HAL_NVIC_DisableIRQ(DMA2D_IRQn);
    HAL_NVIC_DisableIRQ(DSI_IRQn);
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_LTDC_CLK_DISABLE();
    __HAL_RCC_DMA2D_CLK_DISABLE();
    __HAL_RCC_DSI_CLK_DISABLE();
}

void BSP_LCD_MspInit(void) {
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();
    __HAL_RCC_DMA2D_CLK_ENABLE();
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();
    __HAL_RCC_DSI_CLK_ENABLE();
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();
    HAL_NVIC_SetPriority(LTDC_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
    HAL_NVIC_SetPriority(DMA2D_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
    HAL_NVIC_SetPriority(DSI_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(DSI_IRQn);
}

void BSP_LCD_Reset(void) {
    GPIO_InitTypeDef  gpio_init_structure;

    __HAL_RCC_GPIOJ_CLK_ENABLE();
    
    gpio_init_structure.Pin   = GPIO_PIN_15;
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_Delay(20); /* wait 20 ms */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_Delay(10);
}

void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams) {
    if (NbrParams <= 1) {
        HAL_DSI_ShortWrite(&DSIHandle, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]); 
    } else {
        HAL_DSI_LongWrite(&DSIHandle,  LCD_OTM8009A_ID, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams); 
    } 
}
void LCD_ReqTear(void) {
#if ZONES > 1
    static uint8_t ScanLineParams[2];
    #if ZONES == 4
        uint16_t scanline = 283;
    #elif ZONES == 2
        uint16_t scanline = 200;
    #endif
    ScanLineParams[0] = scanline >> 8;
    ScanLineParams[1] = scanline & 0x00FF;

    HAL_DSI_LongWrite(&DSIHandle, 0, DSI_DCS_LONG_PKT_WRITE, 2, 0x44, ScanLineParams);     
    /* set_tear_on */
    HAL_DSI_ShortWrite(&DSIHandle, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_TEEON, 0x00);
#endif    
}

void _LCD_Init(void) {
    static RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    LTDC_LayerCfgTypeDef layer_cfg;
    DSI_PLLInitTypeDef dsiPllInit;
    uint32_t LcdClock  = 27429; /*!< LcdClk = 27429 kHz */
    static DSI_CmdCfgTypeDef         CmdCfg;
    static DSI_LPCmdTypeDef          LPCmd;
    static DSI_PHY_TimerTypeDef      PhyTimings;
    GPIO_InitTypeDef GPIO_Init_Structure;
    
    /**********************************/
    /*     Set up DSI peripheral      */
    /**********************************/
    uint32_t laneByteClk_kHz = 0;
    uint32_t                   VSA; /*!< Vertical start active time in units of lines */
    uint32_t                   VBP; /*!< Vertical Back Porch time in units of lines */
    uint32_t                   VFP; /*!< Vertical Front Porch time in units of lines */
    uint32_t                   VACT; /*!< Vertical Active time in units of lines = imageSize Y in pixels to display */
    uint32_t                   HSA; /*!< Horizontal start active time in units of lcdClk */
    uint32_t                   HBP; /*!< Horizontal Back Porch time in units of lcdClk */
    uint32_t                   HFP; /*!< Horizontal Front Porch time in units of lcdClk */
    uint32_t                   HACT; /*!< Horizontal Active time in units of lcdClk = imageSize X in pixels to display */

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

    /*************************DSI Initialization***********************************/

    /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
    DSIHandle.Instance = DSI;

    HAL_DSI_DeInit(&DSIHandle);

    dsiPllInit.PLLNDIV = 100;
    dsiPllInit.PLLIDF = DSI_PLL_IN_DIV5;
    dsiPllInit.PLLODF = DSI_PLL_OUT_DIV1;
    laneByteClk_kHz = 62500; /* 500 MHz / 8 = 62.5 MHz = 62500 kHz */

    /* Set number of Lanes */
    DSIHandle.Init.NumberOfLanes = DSI_TWO_DATA_LANES;

    /* TXEscapeCkdiv = f(LaneByteClk)/15.62 = 4 */
    DSIHandle.Init.TXEscapeCkdiv = laneByteClk_kHz / 15620; 

    /* Init DSI */
    HAL_DSI_Init(&(DSIHandle), &dsiPllInit);
  
    /* Configure the DSI for Command mode */
    CmdCfg.VirtualChannelID      = 0;
    CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
    CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
    CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
#if LCD_PIXEL_SIZE == 2
    CmdCfg.ColorCoding           = DSI_RGB565;
#else
    CmdCfg.ColorCoding           = DSI_RGB888;
#endif
    CmdCfg.CommandSize           = HACT;
    CmdCfg.TearingEffectSource   = DSI_TE_EXTERNAL;
    CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
    CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
    CmdCfg.AutomaticRefresh      = DSI_AR_DISABLE;
    CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
    HAL_DSI_ConfigAdaptedCommandMode(&DSIHandle, &CmdCfg);

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
    HAL_DSI_ConfigCommand(&DSIHandle, &LPCmd);


    /* The following values are same for portrait and landscape orientations */
    VSA  = OTM8009A_480X800_VSYNC;        /* 12  */
    VBP  = OTM8009A_480X800_VBP;          /* 12  */
    VFP  = OTM8009A_480X800_VFP;          /* 12  */
    HSA  = OTM8009A_480X800_HSYNC;        /* 63  */
    HBP  = OTM8009A_480X800_HBP;          /* 120 */
    HFP  = OTM8009A_480X800_HFP;          /* 120 */   
    HACT = LCD_WIDTH;
    VACT = LCD_HEIGHT;

    DSIVIDEOHandle.VirtualChannelID = LCD_OTM8009A_ID;
    DSIVIDEOHandle.ColorCoding = LCD_DSI_PIXEL_DATA_FMT_RBG565;
    DSIVIDEOHandle.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    DSIVIDEOHandle.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    DSIVIDEOHandle.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;  
    DSIVIDEOHandle.Mode = DSI_VID_MODE_BURST; /* Mode Video burst ie : one LgP per line */
    DSIVIDEOHandle.NullPacketSize = 0xFFF;
    DSIVIDEOHandle.NumberOfChunks = 0;
    DSIVIDEOHandle.PacketSize                = HACT; /* Value depending on display orientation choice portrait/landscape */ 
    DSIVIDEOHandle.HorizontalSyncActive      = (HSA * laneByteClk_kHz) / LcdClock;
    DSIVIDEOHandle.HorizontalBackPorch       = (HBP * laneByteClk_kHz) / LcdClock;
    DSIVIDEOHandle.HorizontalLine            = ((HACT + HSA + HBP + HFP) * laneByteClk_kHz)/LcdClock; /* Value depending on display orientation choice portrait/landscape */
    DSIVIDEOHandle.VerticalSyncActive        = VSA;
    DSIVIDEOHandle.VerticalBackPorch         = VBP;
    DSIVIDEOHandle.VerticalFrontPorch        = VFP;
    DSIVIDEOHandle.VerticalActive            = VACT; /* Value depending on display orientation choice portrait/landscape */

    /* Enable or disable sending LP command while streaming is active in video mode */
    DSIVIDEOHandle.LPCommandEnable = DSI_LP_COMMAND_ENABLE; /* Enable sending commands in mode LP (Low Power) */

    /* Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions */
    /* Only useful when sending LP packets is allowed while streaming is active in video mode */
    DSIVIDEOHandle.LPLargestPacketSize = 64;

    /* Largest packet size possible to transmit in LP mode in HFP region during VACT period */
    /* Only useful when sending LP packets is allowed while streaming is active in video mode */
    DSIVIDEOHandle.LPVACTLargestPacketSize = 64;

    /* Specify for each region of the video frame, if the transmission of command in LP mode is allowed in this region */
    /* while streaming is active in video mode                                                                         */
    DSIVIDEOHandle.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;   /* Allow sending LP commands during HFP period */
    DSIVIDEOHandle.LPHorizontalBackPorchEnable  = DSI_LP_HBP_ENABLE;   /* Allow sending LP commands during HBP period */
    DSIVIDEOHandle.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;  /* Allow sending LP commands during VACT period */
    DSIVIDEOHandle.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;   /* Allow sending LP commands during VFP period */
    DSIVIDEOHandle.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;   /* Allow sending LP commands during VBP period */
    DSIVIDEOHandle.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE; /* Allow sending LP commands during VSync = VSA period */

    /* Configure DSI Video mode timings with settings set above */
    HAL_DSI_ConfigVideoMode(&DSIHandle, &DSIVIDEOHandle);
    
    /* Enable the DSI host and wrapper after the LTDC initialization
       To avoid any synchronization issue, the DSI shall be started after enabling the LTDC */
    HAL_DSI_Start(&DSIHandle);
    
    /* Timing Configuration */    
    LTDCHandle.Init.HorizontalSync = (HSA - 1);
    LTDCHandle.Init.AccumulatedHBP = (HSA + HBP - 1);
    LTDCHandle.Init.AccumulatedActiveW = (LCD_WIDTH + HSA + HBP - 1);
    LTDCHandle.Init.TotalWidth = (LCD_WIDTH + HSA + HBP + HFP - 1);
    LTDCHandle.Init.VerticalSync = (VSA - 1);
    LTDCHandle.Init.AccumulatedVBP = (VSA + VBP - 1);
    LTDCHandle.Init.AccumulatedActiveH = (LCD_HEIGHT + VSA + VBP - 1);
    LTDCHandle.Init.TotalHeigh = (LCD_HEIGHT + VSA + VBP + VFP - 1);

    /* Initialize the LCD pixel width and pixel height */
    LTDCHandle.LayerCfg->ImageWidth = LCD_WIDTH;
    LTDCHandle.LayerCfg->ImageHeight = LCD_HEIGHT;   

    /** LCD clock configuration
    * Note: The following values should not be changed as the PLLSAI is also used 
    *      to clock the USB FS
    * PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz 
    * PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 384 Mhz 
    * PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 384 MHz / 7 = 54.85 MHz 
    * LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 54.85 MHz / 2 = 27.429 MHz 
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 7;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Background value */
    LTDCHandle.Init.Backcolor.Blue = 0;
    LTDCHandle.Init.Backcolor.Green = 0;
    LTDCHandle.Init.Backcolor.Red = 0;
    LTDCHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    LTDCHandle.Instance = LTDC;

    /* Get LTDC Configuration from DSI Configuration */
    HAL_LTDC_StructInitFromVideoConfig(&LTDCHandle, &DSIVIDEOHandle);

    /* Initialize the LTDC */  
    HAL_LTDC_Init(&LTDCHandle);
    
 
    /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
    *  depending on configuration set in 'DSIVIDEOHandle'.
    */

    /* Send Display off DCS Command to display */
    HAL_DSI_ShortWrite(&DSIHandle,
                     0,
                     DSI_DCS_SHORT_PKT_WRITE_P1,
                     OTM8009A_CMD_DISPOFF,
                     0x00);

    /* Configure DSI PHY HS2LP and LP2HS timings */
    PhyTimings.ClockLaneHS2LPTime = 35;
    PhyTimings.ClockLaneLP2HSTime = 35;
    PhyTimings.DataLaneHS2LPTime = 35;
    PhyTimings.DataLaneLP2HSTime = 35;
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 10;
    HAL_DSI_ConfigPhyTimer(&DSIHandle, &PhyTimings);  

    OTM8009A_Init(OTM8009A_FORMAT_RBG565, 0x01);

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
    HAL_DSI_ConfigCommand(&DSIHandle, &LPCmd);

    HAL_DSI_ConfigFlowControl(&DSIHandle, DSI_FLOW_CONTROL_BTA);

    /* Enable GPIOJ clock */
    __HAL_RCC_GPIOJ_CLK_ENABLE();

    /* Configure DSI_TE pin from MB1166 : Tearing effect on separated GPIO from KoD LCD */
    /* that is mapped on GPIOJ2 as alternate DSI function (DSI_TE)                      */
    /* This pin is used only when the LCD and DSI link is configured in command mode    */
    /* Not used in DSI Video mode.                                                      */
    GPIO_Init_Structure.Pin       = GPIO_PIN_2;
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull      = GPIO_NOPULL;
    GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF13_DSI;
    HAL_GPIO_Init(GPIOJ, &GPIO_Init_Structure);
    
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
    HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
    
    HAL_LTDC_ProgramLineEvent(&LTDCHandle, 0);
    return;

//    HAL_DSI_LongWrite(&DSIHandle, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pCols[0]);
//    HAL_DSI_LongWrite(&DSIHandle, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_PASET, pPage);

//    /* Update pitch : the draw is done on the whole physical X Size */
//    HAL_LTDC_SetPitch(&LTDCHandle, LCD_WIDTH, 0);
//#if (GUI_LAYERS > 1)    
//    HAL_LTDC_SetPitch(&LTDCHandle, LCD_WIDTH, 1);
//#endif

//    HAL_Delay(20);

//    LCD_ReqTear();

//    /* Send Display off DCS Command to display */
//    HAL_DSI_ShortWrite(&DSIHandle,
//                        0,
//                        DSI_DCS_SHORT_PKT_WRITE_P1,
//                        OTM8009A_CMD_DISPON,
//                        0x00);
//    __HAL_LTDC_ENABLE(&LTDCHandle);
}

/* IRQ callback for line event */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
    uint8_t i = 0;
    
    for (i = 0; i < GUI_LAYERS; i++) {
        if (Layers[i].Pending) {                /* Is layer waiting for redraw operation */
            Layers[i].Pending = 0;
            
            __HAL_LTDC_LAYER(hltdc, 0)->CFBAR = Layers[i].StartAddress;   /* Set layer address */
            
            __HAL_LTDC_RELOAD_CONFIG(hltdc);
            
            GUI_LCD_ConfirmActiveLayer(i);
            break;
        }
    }
    HAL_LTDC_ProgramLineEvent(hltdc, 0);
}

uint8_t ActiveLayer = 0;

///* Tearing effect callback */
//void HAL_DSI_TearingEffectCallback(DSI_HandleTypeDef* hdsi) {
//    uint32_t index = 0;

//    if (!LCD_Refershing) {
//        for(index = 0; index < GUI_LAYERS; index ++) {
//            if (Layers[index].Pending) {
//                GUI_LCD_ConfirmActiveLayer(index);
//                ActiveLayer = index;
//                Layers[index].Pending = 0;
//            }
//        }    
//        LCD_Refershing = 1;
//        LCD_ActiveRegion = 1;
//        HAL_DSI_Refresh(hdsi); 
//    }
//}

//void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef* hdsi) {
//    uint32_t index = 0;

//    if (LCD_ActiveRegion < ZONES) {
//        LCD_Refershing = 1;
//        /* Disable DSI Wrapper */
//        __HAL_DSI_WRAPPER_DISABLE(hdsi);
//        for(index = 0; index < GUI_LAYERS; index ++) {
//            /* Update LTDC configuaration */
//            LTDC_LAYER(&LTDCHandle, index)->CFBAR = Layers[ActiveLayer].StartAddress + LCD_ActiveRegion * HACT_OUT * 2;
//        }
//        __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&LTDCHandle);
//        __HAL_DSI_WRAPPER_ENABLE(hdsi);
//        LCD_SetUpdateRegion(LCD_ActiveRegion++);
//        /* Refresh the right part of the display */
//        HAL_DSI_Refresh(hdsi);
//    } else {
//        LCD_Refershing = 0;

//        __HAL_DSI_WRAPPER_DISABLE(&DSIHandle);
//        for(index = 0; index < GUI_LAYERS; index ++) {
//            LTDC_LAYER(&LTDCHandle, index)->CFBAR = Layers[ActiveLayer].StartAddress;
//        }
//        __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&LTDCHandle);
//        __HAL_DSI_WRAPPER_ENABLE(&DSIHandle);  
//        LCD_SetUpdateRegion(0); 
//    }
//}

/* IRQ function for LTDC */
void LTDC_IRQHandler(void) {
    HAL_LTDC_IRQHandler(&LTDCHandle);
}

///* IRQ function for DSI */
//void DSI_IRQHandler(void) {
//    HAL_DSI_IRQHandler(&DSIHandle);
//}

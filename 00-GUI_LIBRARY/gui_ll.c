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

#include "lcd_discovery.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
GUI_Layer_t Layers[GUI_LAYERS];
DMA2D_HandleTypeDef DMA2DHandle;
LTDC_HandleTypeDef LTDCHandle;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                              Interrupt API                              **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                             LCD specific API                            **/
/******************************************************************************/
/******************************************************************************/
void LCD_Init(GUI_LCD_t* LCD) {
    TM_SDRAM_Init();                                /* Init SDRAM */

    LTDCHandle.Instance = LTDC;
    DMA2DHandle.Instance = DMA2D;
    
    _LCD_Init();                                    /* Init LCD */
}

static uint32_t GetPixelFormat(uint8_t index) {
#if defined(LCD_COLOR_FORMAT_ARGB8888)
    return LTDC_PIXEL_FORMAT_ARGB8888;              /* ARGB8888 format */
#else
    return LTDC_PIXEL_FORMAT_RGB565;                /* RGB565 format */
#endif /* defined(LCD_COLOR_FORMAT_ARGB8888) */
}

uint8_t LCD_Ready(GUI_LCD_t* LCD) {
    return !(DMA2D->CR & DMA2D_CR_START);           /* Return status */
}

GUI_Color_t LCD_GetPixel(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y) {
#if defined(LCD_COLOR_FORMAT_ARGB8888)
    return *(GUI_Color_t *)(GUI.LCD.Layers[layer].StartAddress + LCD_PIXEL_SIZE * (LCD_WIDTH * y + x));
#else
    GUI_Color_t color;
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait till end */

    DMA2D->CR = DMA2D_CR_MODE_0;                    /* Memory to memory transfer mode with pixel conversion */
    DMA2D->FGMAR = (uint32_t)(GUI.LCD.Layers[layer].StartAddress + LCD_PIXEL_SIZE * (LCD_WIDTH * y + x));
    DMA2D->OMAR = (uint32_t)&color;                 /* Set output address */
    DMA2D->FGOR = 0;                                /* Set foreground offline */    
    DMA2D->OOR = 0;                                 /* Set output offline */
    DMA2D->FGPFCCR = GetPixelFormat(layer);         /* Get source pixel format */
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;     /* Set output pixel format */
    DMA2D->NLR = (uint32_t)(1 << 16) | (uint16_t)1; /* Set X and Y */
    DMA2D->CR |= DMA2D_CR_START;                    /* Start transfer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait till end */
    return 0xFF000000UL | color;
#endif /* defined(LCD_COLOR_FORMAT_ARGB8888) */
}

void LCD_Fill(GUI_LCD_t* LCD, uint8_t layer, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t OffLine, GUI_Color_t color) {
#if LCD_PIXEL_SIZE == 2
    uint8_t r, g, b;
    r = (color >> 19) & 0x1F;
    g = (color >> 10) & 0x3F;
    b = (color >>  3) & 0x1F;
    color = 0x00000000UL | (r << 11) | (g << 5) | b;
#endif
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->OCOLR = color;                           /* Color to be used */
    DMA2D->OMAR = (uint32_t)dst;                    /* Destination address */
    DMA2D->OOR = OffLine;                           /* Destination line offset */
    DMA2D->OPFCCR = GetPixelFormat(layer);          /* Defines the number of pixels to be transfered */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; /* Size configuration of area to be transfered */
    
    DMA2D->CR = 0x00030000UL;                       /* Register to memory */
    DMA2D->CR |= DMA2D_CR_START;                    /* Start actual transfer */
}

void LCD_Copy(GUI_LCD_t* LCD, uint8_t layer, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->CR = 0;                                  /* Memory to memory transfer mode */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;   
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = GetPixelFormat(layer);
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 
    DMA2D->CR |= DMA2D_CR_START;
}

void LCD_CopyBlending(GUI_LCD_t* LCD, uint8_t layer, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->CR = DMA2D_CR_MODE_1;                    /* Memory to memory transfer mode */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;  
    DMA2D->FGPFCCR = PixelFormat;                   /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 
    DMA2D->CR |= DMA2D_CR_START;
}

void LCD_DrawImage16(GUI_LCD_t* LCD, uint8_t layer, const GUI_IMAGE_DESC_t* img, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->CR = DMA2D_CR_MODE_1;                    /* Memory to memory transfer mode */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;      /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    /* Enable invert alpha and swap R and B values with hardware */
#if defined(DMA2D_FGPFCCR_AI) && defined(DMA2D_FGPFCCR_RBS)
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_RBS;
#endif  /* defined(DMA2D_FGPFCCR_AM_1) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 
    DMA2D->CR |= DMA2D_CR_START;
}

void LCD_DrawImage24(GUI_LCD_t* LCD, uint8_t layer, const GUI_IMAGE_DESC_t* img, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->CR = DMA2D_CR_MODE_1;                    /* Memory to memory transfer mode */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB888;      /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    /* Enable invert alpha and swap R and B values with hardware */
#if defined(DMA2D_FGPFCCR_AI) && defined(DMA2D_FGPFCCR_RBS)
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_RBS;
#endif  /* defined(DMA2D_FGPFCCR_AM_1) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
    DMA2D->CR |= DMA2D_CR_START;
}

void LCD_DrawImage32(GUI_LCD_t* LCD, uint8_t layer, const GUI_IMAGE_DESC_t* img, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->CR = DMA2D_CR_MODE_1;                    /* Memory to memory with blending and PFC transfer mode */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;    /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    /* Enable invert alpha and swap R and B values with hardware */
#if defined(DMA2D_FGPFCCR_AI) && defined(DMA2D_FGPFCCR_RBS)
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_AI | DMA2D_FGPFCCR_RBS;
#endif  /* defined(DMA2D_FGPFCCR_AM_1) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
    DMA2D->CR |= DMA2D_CR_START;
}

void LCD_DrawHLine(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color) {
    uint32_t addr = Layers[layer].StartAddress + (LCD->PixelSize * (LCD->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, length, 1, LCD->Width - length, color);
}

void LCD_DrawVLine(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color) {
    uint32_t addr = Layers[layer].StartAddress + (LCD->PixelSize * (LCD->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, 1, length, LCD->Width - 1, color);
}

void LCD_FillRect(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Color_t color) {
    uint32_t addr = Layers[layer].StartAddress + (LCD->PixelSize * (LCD->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, xSize, ySize, LCD->Width - xSize, color);
}

void LCD_SetPixel(GUI_LCD_t* LCD, uint8_t layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Color_t color) {
//    uint32_t addr = LCD_FRAME_BUFFER + (layer * LCD_FRAME_BUFFER_SIZE) + LCD_PIXEL_SIZE * (LCD_WIDTH * y + x);
//    *(volatile uint32_t *)(addr) = color;
    LCD_DrawHLine(LCD, layer, x, y, 1, color);
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
uint8_t GUI_LL_Control(GUI_LCD_t* LCD, GUI_LL_Command_t cmd, void* param, void* result) {
    switch (cmd) {
        case GUI_LL_Command_Init: {
            uint8_t i = 0;
            GUI_LL_t* LL = (GUI_LL_t *)param;
            
            /*******************************/
            /* Set up LCD data             */
            /*******************************/
            LCD->Width = LCD_WIDTH;
            LCD->Height = LCD_HEIGHT;
            LCD->PixelSize = LCD_PIXEL_SIZE;
            
            /*******************************/
            /* Set layers count            */
            /*******************************/
            LCD->LayersCount = GUI_LAYERS;      /* We have 2 layers for our low-level driver */
            LCD->Layers = Layers;
            for (i = 0; i < GUI_LAYERS; i++) {  /* Set each layer */
                Layers[i].Num = i;
                Layers[i].StartAddress = LCD_FRAME_BUFFER + (i * LCD_FRAME_BUFFER_SIZE);
//#if defined(STM32F769_DISCOVERY)
//                if (i == 0) {
//                    Layers[i].StartAddress = 0xC0400000;
//                } else {
//                    Layers[i].StartAddress = 0xC0800000;
//                }
//#endif
            }
            
            /*******************************/
            /* Set up LCD drawing routines */
            /*******************************/
            LL->Init = LCD_Init;                /* Must be set by user */
            LL->GetPixel = LCD_GetPixel;        /* Must be set by user */
            LL->SetPixel = LCD_SetPixel;        /* Must be set by user */
            
            LL->IsReady = LCD_Ready;            /* Set is ready function to indicate low-level layer has finished any transmission */
            LL->Copy = LCD_Copy;                /* Set copy memory routine */
            LL->DrawHLine = LCD_DrawHLine;      /* Set drawing vertical line routine */
            LL->DrawVLine = LCD_DrawVLine;      /* Set drawing horizontal line routine */
            LL->Fill = LCD_Fill;                /* Set fill screen routine */
            LL->FillRect = LCD_FillRect;        /* Set fill rectangle routine */
            LL->CopyBlend = LCD_CopyBlending;   /* Set copy with blending */
            LL->DrawImage16 = LCD_DrawImage16;  /* Set draw function for 24bit image (RGB565) format */
            LL->DrawImage24 = LCD_DrawImage24;  /* Set draw function for 24bit image (RGB888) format */
            LL->DrawImage32 = LCD_DrawImage32;  /* Set draw function for 32bit image (ARGB8888/ABGR8888) format */
            
            if (result) {
                *(uint8_t *)result = 0;         /* Successful initialization */
            }
            return 1;                           /* Command processed */
        }
        case GUI_LL_Command_SetActiveLayer: {   /* Set new active layer */
            GUI_Byte layer = *(GUI_Byte *)param;/* Read layer as byte */
            LCD->Layers[layer].Pending = 1;     /* Set layer as pending and redraw on next reload */

            if (result) {
                *(uint8_t *)result = 0;         /* Successful layer set as active */
            }
            return 1;                           /* Command processed */
        }
        default:
            return 0;
    }
}

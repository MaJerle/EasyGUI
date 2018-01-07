/**	
 * \file            gui_ll_stm32_dma2d.c
 * \brief           Low-Level driver for STM32 and DMA2D peripheral
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "system/gui_ll.h"

#include "lcd_discovery.h"

GUI_Layer_t Layers[GUI_LAYERS];
static DMA2D_HandleTypeDef DMA2DHandle;
uint16_t startAddress;

#define DMA2D_START(type) do {                  \
    DMA2D->CR = (type);                         \
    DMA2D->CR |= 0 | 0;             \
    DMA2D->CR |= DMA2D_CR_START;                /* Start the transmission */\
} while (0)

static
void LCD_Init(GUI_LCD_t* LCD) {
    TM_SDRAM_Init();                                /* Init SDRAM */
    
    DMA2DHandle.Instance = DMA2D;
    
    _LCD_Init();                                    /* Init LCD */
}

static
uint32_t GetPixelFormat(GUI_Layer_t* layer) {
#if defined(LCD_COLOR_FORMAT_ARGB8888)
    return DMA2D_OUTPUT_ARGB8888;                   /* ARGB8888 format */
#else
    return DMA2D_OUTPUT_RGB565;                     /* RGB565 format */
#endif /* defined(LCD_COLOR_FORMAT_ARGB8888) */
}

static
uint8_t LCD_Ready(GUI_LCD_t* LCD) {
    return !(DMA2D->CR & DMA2D_CR_START);           /* Return status */
}

static
GUI_Color_t LCD_GetPixel(GUI_LCD_t* LCD, GUI_Layer_t* layer, GUI_Dim_t x, GUI_Dim_t y) {
#if defined(LCD_COLOR_FORMAT_ARGB8888)
    return *(GUI_Color_t *)(layer->StartAddress + GUI.LCD.PixelSize * (layer->Width * y + x));
#else
    GUI_Color_t color;
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait till end */
    
    DMA2D->FGMAR = (uint32_t)(layer->StartAddress + GUI.LCD.PixelSize * (layer->Width * y + x));
    DMA2D->OMAR = (uint32_t)&color;                 /* Set output address */
    DMA2D->FGOR = 0;                                /* Set foreground offline */    
    DMA2D->OOR = 0;                                 /* Set output offline */
    DMA2D->FGPFCCR = GetPixelFormat(layer);         /* Get source pixel format */
    DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;     /* Set output pixel format */
    DMA2D->NLR = (uint32_t)(1 << 16) | (uint16_t)1; /* Set X and Y */

    DMA2D_START(DMA2D_M2M_PFC);                     /* Start DMA2D transfer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait till end */
    return 0xFF000000UL | color;
#endif /* defined(LCD_COLOR_FORMAT_ARGB8888) */
}

static
void LCD_Fill(GUI_LCD_t* LCD, GUI_Layer_t* layer, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t OffLine, GUI_Color_t color) {
#if LCD_PIXEL_SIZE == 2
    uint8_t r, g, b;
//    r = (color >> 20) & 0x0F;
//    g = (color >> 12) & 0x0F;
//    b = (color >>  4) & 0x0F;
//    color = 0x0000F000UL | (r << 8) | (g << 4) | b;
    r = (color >> 19) & 0x1F;
    g = (color >> 10) & 0x3F;
    b = (color >>  3) & 0x1F;
    color = 0x00000000UL | (r << 11) | (g << 5) | b;
#endif
    if (!xSize || !ySize) {
        return;
    }
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->OCOLR = color;                           /* Color to be used */
    DMA2D->OMAR = (uint32_t)dst;                    /* Destination address */
    DMA2D->OOR = OffLine;                           /* Destination line offset */
    DMA2D->OPFCCR = GetPixelFormat(layer);          /* Defines the number of pixels to be transfered */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; /* Size configuration of area to be transfered */
    
    DMA2D_START(DMA2D_R2M);                         /* Start DMA2D transfer */
}

static
void LCD_Copy(GUI_LCD_t* LCD, GUI_Layer_t* layer, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);
    
    if (!xSize || !ySize) {
        return;
    }
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = PixelFormat;
    DMA2D->BGPFCCR = PixelFormat;
    DMA2D->OPFCCR = PixelFormat;
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
    
    DMA2D_START(DMA2D_M2M);                         /* Start DMA2D transfer */
}

/* Copy layers with blending with alpha combine */
static
void LCD_CopyBlending(GUI_LCD_t* LCD, GUI_Layer_t* layer, const void* src, void* dst, uint8_t alphaSrc, uint8_t alphaDst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = PixelFormat;                   /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_AM_0 | alphaSrc << 24;               /* Set alpha for source */
    DMA2D->BGPFCCR |= alphaDst << 24;               /* Set alpha for destination */
    
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
    
    DMA2D_START(DMA2D_M2M_BLEND);                   /* Start DMA2D transfer */
}

static
void LCD_DrawImage16(GUI_LCD_t* LCD, GUI_Layer_t* layer, const GUI_IMAGE_DESC_t* img, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    if (!xSize || !ySize) {
        return;
    }
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = DMA2D_INPUT_RGB565;            /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    /* Enable invert alpha and swap R and B values with hardware */
#if defined(DMA2D_FGPFCCR_AI) && defined(DMA2D_FGPFCCR_RBS)
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_RBS;
#endif  /* defined(DMA2D_FGPFCCR_AM_1) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 
    
    DMA2D_START(DMA2D_M2M_BLEND);                   /* Start DMA2D transfer */
}

static
void LCD_DrawImage24(GUI_LCD_t* LCD, GUI_Layer_t* layer, const GUI_IMAGE_DESC_t* img, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = DMA2D_INPUT_RGB888;            /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    /* Enable invert alpha and swap R and B values with hardware */
#if defined(DMA2D_FGPFCCR_RBS)
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_RBS;
#endif  /* defined(DMA2D_FGPFCCR_AM_1) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
    
    DMA2D_START(DMA2D_M2M_BLEND);                   /* Start DMA2D transfer */
}

static
void LCD_DrawImage32(GUI_LCD_t* LCD, GUI_Layer_t* layer, const GUI_IMAGE_DESC_t* img, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;
    DMA2D->FGPFCCR = DMA2D_INPUT_ARGB8888;          /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    
    /* Enable invert alpha and swap R and B values with hardware */
#if defined(DMA2D_FGPFCCR_AI) && defined(DMA2D_FGPFCCR_RBS)
    DMA2D->FGPFCCR |= DMA2D_FGPFCCR_AI | DMA2D_FGPFCCR_RBS;
#endif  /* defined(DMA2D_FGPFCCR_AM_1) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;
    
    DMA2D_START(DMA2D_M2M_BLEND);                   /* Start DMA2D transfer */
}

static
void LCD_CopyChar(GUI_LCD_t* LCD, GUI_Layer_t* layer, const void* src, void* dst, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Dim_t offLineSrc, GUI_Dim_t offLineDst, GUI_Color_t color) {
    uint32_t PixelFormat = GetPixelFormat(layer);   /* Get pixel format of specific layer */
    
    if (!xSize || !ySize) {
        return;
    }
    while (DMA2D->CR & DMA2D_CR_START);             /* Wait finished */
    DMA2D->FGMAR = (uint32_t)src;
    DMA2D->BGMAR = (uint32_t)dst;                       
    DMA2D->OMAR = (uint32_t)dst;
    DMA2D->FGOR = offLineSrc;
    DMA2D->BGOR = offLineDst;
    DMA2D->OOR = offLineDst;  
    DMA2D->FGCOLR = color & 0x00FFFFFFUL;           /* Since foreground input color is A4, value in this register will be used for blending purpose */
    DMA2D->FGPFCCR = DMA2D_INPUT_A8;                /* Foreground PFC Control Register */
    DMA2D->BGPFCCR = PixelFormat;                   /* Background PFC Control Register (Defines the BG pixel format) */
    DMA2D->OPFCCR  = PixelFormat;                   /* Output     PFC Control Register (Defines the output pixel format) */
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 
    
    DMA2D_START(DMA2D_M2M_BLEND);                   /* Start DMA2D transfer */
}

static
void LCD_DrawHLine(GUI_LCD_t* LCD, GUI_Layer_t* layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color) {
    uint32_t addr = layer->StartAddress + (LCD->PixelSize * (layer->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, length, 1, layer->Width - length, color);
}

static
void LCD_DrawVLine(GUI_LCD_t* LCD, GUI_Layer_t* layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t length, GUI_Color_t color) {
    uint32_t addr = layer->StartAddress + (LCD->PixelSize * (layer->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, 1, length, layer->Width - 1, color);
}

static
void LCD_FillRect(GUI_LCD_t* LCD, GUI_Layer_t* layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Dim_t xSize, GUI_Dim_t ySize, GUI_Color_t color) {
    uint32_t addr = layer->StartAddress + (LCD->PixelSize * (layer->Width * y + x));
    
    LCD_Fill(LCD, layer, (void *)addr, xSize, ySize, layer->Width - xSize, color);
}

static
void LCD_SetPixel(GUI_LCD_t* LCD, GUI_Layer_t* layer, GUI_Dim_t x, GUI_Dim_t y, GUI_Color_t color) {
    LCD_DrawHLine(LCD, layer, x, y, 1, color);
}

/* Process DMA2D interrupt */
void DMA2D_IRQHandler(void) {
    HAL_DMA2D_IRQHandler(&DMA2DHandle);
}

void TransferErrorCallback(DMA2D_HandleTypeDef* hdma2d) {
     while (1);
}
uint8_t gui_ll_control(GUI_LCD_t* LCD, GUI_LL_Command_t cmd, void* param, void* result) {
    switch (cmd) {
        case GUI_LL_Command_Init: {
            uint8_t i = 0;
            GUI_LL_t* LL = (GUI_LL_t *)param;
            
            /*******************************/
            /* Assign memory to GUI        */
            /*******************************/
            TM_SDRAM_Init();
            do {
#if defined(STM32F7xx)
#if defined(__GNUC__)
                static uint8_t DTCMMemory1[0x10000];
#else
                static uint8_t DTCMMemory1[0x10000] __attribute__((at(0x20001000)));    /* TCM RAM */
#endif
#else
                static uint8_t DTCMMemory1[0x10000] __attribute__((at(0x10000000)));    /* CCM RAM */
#endif
#if defined(__GNUC__)
                static uint8_t SDRAMMemory[SDRAM_HEAP_SIZE] __attribute__((section(".sdram_data"))); /* SDRAM heap memory */
#else
                static uint8_t SDRAMMemory[SDRAM_HEAP_SIZE] __attribute__((at(SDRAM_START_ADR + SDRAM_MEMORY_SIZE - SDRAM_HEAP_SIZE))); /* SDRAM heap memory */
#endif
                static GUI_MEM_Region_t const regions[] = {
                    {DTCMMemory1, sizeof(DTCMMemory1)},
                    {SDRAMMemory, sizeof(SDRAMMemory)},
                };
                gui_mem_assignmemory(regions, GUI_COUNT_OF(regions));
            } while (0);
            
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
            LL->CopyChar = LCD_CopyChar;        /* Set draw function for char copy with alpha information */
            
            if (result) {
                *(uint8_t *)result = 0;         /* Successful initialization */
            }
            
            HAL_NVIC_SetPriority(DMA2D_IRQn, 4, 0); /* Set priority level for DMA2D */
            HAL_NVIC_EnableIRQ(DMA2D_IRQn);     /* Enable IRQ */
            
            return 1;                           /* Command processed */
        }
        case GUI_LL_Command_SetActiveLayer: {   /* Set new active layer */
            GUI_Layer_t* layer = (GUI_Layer_t *)param;/* Read layer as byte */
            layer->Pending = 1;                 /* Set layer as pending and redraw on next reload */

            if (result) {
                *(uint8_t *)result = 0;         /* Successful layer set as active */
            }
            return 1;                           /* Command processed */
        }
        default:
            return 0;
    }
}

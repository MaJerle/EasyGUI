#ifndef __LCD_DISCOVERY
#define __LCD_DISCOVERY

#include "stm32fxxx_hal.h"
#define GUI_INTERNAL
#include "gui_ll.h"
#include "tm_stm32_sdram.h"

#define LCD_PIXEL_SIZE          4

/* Set pixel settings */
#if defined(STM32F7_DISCOVERY)
#define LCD_WIDTH               480
#define LCD_HEIGHT              272

#define LCD_HSYNC               41
#define LCD_HBP                 13
#define LCD_HFP                 32
#define LCD_VSYNC               10
#define LCD_VBP                 2
#define LCD_VFP                 2
#endif /* defined(STM32F7_DISCOVERY) */

#if defined(STM32F769_DISCOVERY)
#include "otm8009a.h"

#define LCD_WIDTH               800
#define LCD_HEIGHT              480

#endif /* defined(STM32F769_DISCOVERY) */

/* Frame buffer settings */
#define LCD_FRAME_BUFFER        ((uint32_t)SDRAM_START_ADR)
#define LCD_FRAME_BUFFER_SIZE   ((uint32_t)(LCD_WIDTH * LCD_HEIGHT * LCD_PIXEL_SIZE))

/* Number of layers */
#define GUI_LAYERS              4

void _LCD_Init(void);

#endif /* __LCD_DISCOVERY */

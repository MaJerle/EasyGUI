/**	
 * \file            gui_ll_sdl_win32.c
 * \brief           Low-Level SDL implementation for WIN32
 */
 
/*
 * Copyright (c) 2018 Tilen Majerle
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#include "system/gui_ll.h"
#include "gui/gui_mem.h"
#include "SDL.h"

#define LCD_WIDTH                           800
#define LCD_HEIGHT                          480
#define LCD_PIXEL_SIZE                      4
#define LCD_LAYERS                          2

uint32_t frame_buffer[LCD_LAYERS][LCD_WIDTH * LCD_HEIGHT];

gui_layer_t layers[LCD_LAYERS];

/**
 * \brief           Init SDL here
 */
uint8_t
lcd_init(gui_lcd_t* lcd) {

}

/**
 * \brief           Low-Level control function
 */
uint8_t
gui_ll_control(gui_lcd_t* LCD, GUI_LL_Command_t cmd, void* param, void* result) {
    switch (cmd) {
        case GUI_LL_Command_Init: {
            uint8_t i = 0;
            gui_ll_t* LL = (gui_ll_t *)param;
            static big_array[0x100000];
            static GUI_MEM_Region_t regions[] = {
                {big_array, sizeof(big_array)}
            };
            
            /*******************************/
            /* Assign memory to GUI        */
            /*******************************/
            gui_mem_assignmemory(regions, GUI_COUNT_OF(regions));
            
            /*******************************/
            /* Set up LCD data             */
            /*******************************/
            LCD->width = LCD_WIDTH;
            LCD->height = LCD_HEIGHT;
            LCD->pixel_size = LCD_PIXEL_SIZE;
            
            /*******************************/
            /* Set layers count            */
            /*******************************/
            LCD->layer_count = 2;               /* We have 2 layers for our low-level driver */
            LCD->layers = layers;
            for (i = 0; i < LCD_LAYERS; i++) {  /* Set each layer */
                layers[i].num = i;
                layers[i].start_address = &frame_buffer[i];
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
            
            if (result != NULL) {
                *(uint8_t *)result = 0;         /* Successful initialization */
            }
            
            return 1;                           /* Command processed */
        }
        case GUI_LL_Command_SetActiveLayer: {   /* Set new active layer */
            uint8_t layer = *(uint8_t *)param;/* Read layer as byte */
            LCD->layers[layer].pending = 1;     /* Set layer as pending and redraw on next reload */

            if (result) {
                *(uint8_t *)result = 0;         /* Successful layer set as active */
            }
            gui_lcd_confirmactivelayer(layer);  /* Confirm use of new layer */
            return 1;                           /* Command processed */
        }
        default:
            return 0;
    }
}

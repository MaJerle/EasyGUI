/**	
 * \file            gui_ll_sdl_win32.c
 * \brief           Low-Level SDL implementation for WIN32
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
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
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#include "system/gui_ll.h"
#include "gui/gui_mem.h"
#include "SDL.h"

#if !__DOXYGEN__

#define LCD_WIDTH                           800
#define LCD_HEIGHT                          480
#define LCD_PIXEL_SIZE                      4
#define LCD_LAYERS                          2

static uint32_t frame_buffer[LCD_LAYERS][LCD_WIDTH * LCD_HEIGHT];

static gui_layer_t layers[LCD_LAYERS];

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;
static SDL_Event evt;

volatile static uint8_t sdl_initialized = 0;
volatile static uint8_t sdl_refresh = 0;
volatile static uint8_t sdl_quit = 0;

static int sdl_thread(void * param);
static int sdl_event_filter(void *userdata, SDL_Event * event);
static int my_mouse_evt(void);

/**
 * \brief           Init SDL here
 */
uint8_t
lcd_init(gui_lcd_t* lcd) {
    SDL_CreateThread(sdl_thread, "SDL Thread", NULL);
    while (!sdl_initialized) {}
    return 1;
}

/**
 * \brief           SDL thread manager
 */
static int
sdl_thread(void* arg) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init problems: %s\r\n", SDL_GetError());
    }
    SDL_SetEventFilter(sdl_event_filter, NULL);

    window = SDL_CreateWindow("PC Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LCD_WIDTH, LCD_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, LCD_WIDTH, LCD_HEIGHT);

    memset(frame_buffer[0], 0x7F, sizeof(frame_buffer[0]));
    memset(frame_buffer[1], 0x7F, sizeof(frame_buffer[1]));
    SDL_UpdateTexture(texture, NULL, frame_buffer[0], LCD_WIDTH * sizeof(uint32_t));

    sdl_refresh = 1;
    sdl_initialized = 1;

    while (1) {
        if (sdl_refresh) {
            sdl_refresh = 0;
            SDL_UpdateTexture(texture, NULL, frame_buffer[0], LCD_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        while (SDL_PollEvent(&evt)) {
            my_mouse_evt();
        }
        SDL_Delay(30);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static int 
sdl_event_filter(void* userdata, SDL_Event* evt) {
    return 1;
}

static int
my_mouse_evt(void) {
    static is_down = 0;
    static gui_touch_data_t ts;

    switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (evt.button.button == SDL_BUTTON_LEFT) {
                ts.count = 1;
                ts.status = GUI_TOUCH_STATE_PRESSED;
                ts.x[0] = (gui_dim_t)evt.button.x;
                ts.y[0] = (gui_dim_t)evt.button.y;
                gui_input_touchadd(&ts);

                is_down = 1;
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            if (evt.button.button == SDL_BUTTON_LEFT) {
                ts.count = 0;
                ts.status = GUI_TOUCH_STATE_RELEASED;
                ts.x[0] = evt.button.x;
                ts.y[0] = evt.button.y;
                gui_input_touchadd(&ts);

                is_down = 0;
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            if (is_down) {
                ts.count = 1;
                ts.status = GUI_TOUCH_STATE_PRESSED;
                ts.x[0] = evt.button.x;
                ts.y[0] = evt.button.y;
                gui_input_touchadd(&ts);
            }
            break;
        }
        case SDL_FINGERDOWN: {
            printf("Finger count: \r\n");
            break;
        }
    }
    return 0;
}

/**
 *
 */
void
lcd_fill(gui_lcd_t* LCD, gui_layer_t* layer, void* dst, gui_dim_t xSize, gui_dim_t ySize, gui_dim_t OffLine, gui_color_t color) {
    gui_dim_t x, y;
    gui_dim_t* addr = dst;

    for (y = 0; y < ySize; y++) {
        for (x = 0; x < xSize; x++) {
            *addr++ = (uint32_t)color;
        }
        addr += OffLine;
    }
}

/**
 *
 */
void
lcd_fillrect(gui_lcd_t* LCD, gui_layer_t* layer, gui_dim_t x, gui_dim_t y, gui_dim_t xSize, gui_dim_t ySize, gui_color_t color) {
    gui_dim_t xp, yp;
    uint32_t* fb = layer->start_address;

    for (yp = 0; yp < ySize; yp++) {
        for (xp = 0; xp < xSize; xp++) {
            fb[(y + yp) * layer->width + (x + xp)] = (uint32_t)color;
        }
    }
}

/**
 * \brief           Copy from source to destination
 */
void
lcd_copy(gui_lcd_t* LCD, gui_layer_t* layer, void* dst, const void* src, gui_dim_t xSize, gui_dim_t ySize, gui_dim_t offLineDst, gui_dim_t offLineSrc) {
    gui_dim_t y;
    const uint32_t* s = src;
    uint32_t* d = dst;

    for (y = 0; y < ySize; y++) {
        memcpy(d, s, sizeof(uint32_t) * xSize);
        s += xSize + offLineSrc;
        d += xSize + offLineDst;
    }
}

void
lcd_drawhline(gui_lcd_t* LCD, gui_layer_t* layer, gui_dim_t x, gui_dim_t y, gui_dim_t length, gui_color_t color) {
    lcd_fillrect(LCD, layer, x, y, length, 1, color);
}

void
lcd_drawvline(gui_lcd_t* LCD, gui_layer_t* layer, gui_dim_t x, gui_dim_t y, gui_dim_t length, gui_color_t color) {
    lcd_fillrect(LCD, layer, x, y, 1, length, color);
}

void
lcd_setpixel(gui_lcd_t* LCD, gui_layer_t* layer, gui_dim_t x, gui_dim_t y, gui_color_t color) {
    uint32_t* fb = layer->start_address;
    fb[y * layer->width + x] = (uint32_t)color;
}

gui_color_t
lcd_getpixel(gui_lcd_t* LCD, gui_layer_t* layer, gui_dim_t x, gui_dim_t y) {
    uint32_t* fb = layer->start_address;
    return fb[y * layer->width + x];
}

uint8_t
lcd_ready(gui_lcd_t* LCD) {
    return 1;
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
            static gui_mem_region_t regions[] = {
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
                layers[i].start_address = &frame_buffer[0];
            }
            
            /*******************************/
            /* Set up LCD drawing routines */
            /*******************************/
            LL->Init = lcd_init;                /* Must be set by user */
            LL->GetPixel = lcd_getpixel;        /* Must be set by user */
            LL->SetPixel = lcd_setpixel;        /* Must be set by user */
            //
            LL->IsReady = lcd_ready;            /* Set is ready function to indicate low-level layer has finished any transmission */
            LL->Copy = lcd_copy;                /* Set copy memory routine */
            LL->DrawHLine = lcd_drawhline;      /* Set drawing vertical line routine */
            LL->DrawVLine = lcd_drawvline;      /* Set drawing horizontal line routine */
            LL->Fill = lcd_fill;                /* Set fill screen routine */
            LL->FillRect = lcd_fillrect;        /* Set fill rectangle routine */
            //LL->CopyBlend = LCD_CopyBlending;   /* Set copy with blending */
            //LL->DrawImage16 = LCD_DrawImage16;  /* Set draw function for 24bit image (RGB565) format */
            //LL->DrawImage24 = LCD_DrawImage24;  /* Set draw function for 24bit image (RGB888) format */
            //LL->DrawImage32 = LCD_DrawImage32;  /* Set draw function for 32bit image (ARGB8888/ABGR8888) format */
            //LL->CopyChar = LCD_CopyChar;        /* Set draw function for char copy with alpha information */
            
            if (result != NULL) {
                *(uint8_t *)result = 0;         /* Successful initialization */
            }
            
            return 1;                           /* Command processed */
        }
        case GUI_LL_Command_SetActiveLayer: {   /* Set new active layer */
            uint8_t layer = *(uint8_t *)param;  /* Read layer as byte */
            LCD->layers[layer].pending = 0;     /* Set layer as pending and redraw on next reload */

            if (result) {
                *(uint8_t *)result = 0;         /* Successful layer set as active */
            }
            sdl_refresh = 1;
            gui_lcd_confirmactivelayer(layer);  /* Confirm use of new layer */
            return 1;                           /* Command processed */
        }
        default:
            return 0;
    }
}

#endif /* !__DOXYGEN__ */

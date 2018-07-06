// sdl_test.cpp : Defines the entry point for the console application.
//
#include "SDL.h"
#include "stdio.h"

#include "gui/gui_widget.h"

int
main(int argc, char *argv[]) {
    uint32_t* pixels;
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Renderer* renderer;
    SDL_Event evt;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL init problems: %s\r\n", SDL_GetError());
    }

    /* Create window and renderer */
    window = SDL_CreateWindow("EasyGUI Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 480, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0xD6, 0x5a, 0x66, 0xFF);
    SDL_RenderClear(renderer);

    /* Create renderer for points */
    SDL_SetRenderDrawColor(renderer, 0x00, 0x10, 0x00, 0xFF);
    SDL_RenderDrawPoint(renderer, 0, 0);
    SDL_RenderDrawPoint(renderer, 1, 0);

    SDL_RenderDrawPoint(renderer, 10, 10);
    SDL_RenderDrawPoint(renderer, 11, 10);
    SDL_RenderDrawPoint(renderer, 10, 11);
    SDL_RenderDrawPoint(renderer, 11, 11);
    SDL_RenderPresent(renderer);

    while (1) {
        if (SDL_PollEvent(&evt)) {
            printf("Event received: 0x%4X\r\n", (unsigned)evt.type);
            switch (evt.type) {
                case SDL_QUIT: goto out;
                case SDL_KEYDOWN: {
                    printf("KeyDown: 0x%08X\r\n", (unsigned)evt.key.keysym.sym);
                    break;
                }
                case SDL_KEYUP: {
                    printf("KeyUp\r\n");
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    printf("MouseDown: %02X\r\n", (unsigned)evt.button.which);
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    printf("MouseUp: %02X\r\n", (unsigned)evt.button.which);
                    break;
                }
                case SDL_FINGERMOTION: {
                    printf("FingerMove: x = %d; y = %d\r\n",
                        (int)(800.0f * evt.tfinger.x),
                        (int)(480.0f * evt.tfinger.y));
                    break;
                }
                default: break;
            }
        }
    }
out:
    
    return 0;
}

// sdl_test.cpp : Defines the entry point for the console application.
//
#include "SDL.h"
#include "stdio.h"

#include "demo.h"

int
main(int argc, char *argv[]) {
    gui_init();
    
    demo_init();

    while (1) {
        Sleep(100);
    }
    return 0;
}

#pragma once

#include <SDL2/SDL.h>
#include <iostream>


class Window {
    private:
        bool keep_window_open = true;
        SDL_Window *window = nullptr;
        SDL_Surface *window_surface = nullptr;
    public:
    Window() {
        StartWindow();
    };
    int StartWindow();
    bool tick();
    
};

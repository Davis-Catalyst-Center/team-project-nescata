#pragma once
#include<SDL2/SDL.h>
class Window {
    private:
        bool keep_window_open = true;
    public:
    Window(bool keep_window_open) : keep_window_open(keep_window_open) {};
    void StartWindow(){};
    
}
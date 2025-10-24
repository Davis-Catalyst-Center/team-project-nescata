#pragma once
#include<SDL2/SDL.h>
class Window {
    private:
        bool keep_window_open = true;
    public:
    Window() {
        StartWindow();
    };
    void StartWindow();
    void tick();
    
}

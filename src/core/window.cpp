#include "window.hpp"

int Window::StartWindow(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }


    window = SDL_CreateWindow(
        "nescata",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        256, 224,
        0
    );

    if(!window)
    {
        std::cout << "Failed to create window\n";
        return -1;
    }

    window_surface = SDL_GetWindowSurface(window);

    if(!window_surface)
    {
        std::cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);
    return 0;
}

bool Window::tick(){
    
    SDL_Event e;
    while(SDL_PollEvent(&e) > 0)
    {
        switch(e.type)
        {
            case SDL_QUIT:
                keep_window_open = false;
                break;
        }

        
    }
    SDL_UpdateWindowSurface(window);
    return keep_window_open;
}


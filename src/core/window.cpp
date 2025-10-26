#include "window.hpp"

int Window::StartWindow() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }

    window = SDL_CreateWindow(
        "nescata",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        256,
        224,
        0
    );

    if (!window) {
        std::cout << "Failed to create window\n";
        return -1;
    }

    window_surface = SDL_GetWindowSurface(window);

    if (!window_surface) {
        std::cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);
    return 0;
}

bool Window::pollEvent(SDL_Event* event) {
	return SDL_PollEvent(event);
}

void Window::updateSurface() {
	SDL_UpdateWindowSurface(window);
}

void Window::closeWindow() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::waitForVsync() {
}


// Drawing functions

void Window::fillRect(int x, int y, int w, int h, uint32 color) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_FillRect(window_surface, &rect, color);
}
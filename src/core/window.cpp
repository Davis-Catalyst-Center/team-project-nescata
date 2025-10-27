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

void Window::fillScreen(uint32 color) {
	SDL_FillRect(window_surface, nullptr, color);
}

void Window::drawPixel(int x, int y, uint32 color) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 1;
	rect.h = 1;
	SDL_FillRect(window_surface, &rect, color);
}

void Window::drawBuffer(uint32* buffer) {
    if (!buffer) {
        return;
    }
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 256;
    rect.h = 224;

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        buffer,
        256,
        224,
        32,
        256 * sizeof(uint32),
		0xFF000000,
		0x00FF0000,
		0x0000FF00,
		0x000000FF
    );

    if (surface) {
        SDL_BlitSurface(surface, nullptr, window_surface, &rect);
        SDL_FreeSurface(surface);
    }
}

uint32* Window::makeBufferFromImage(const char* filepath) {
    SDL_Surface* loadedImage = SDL_LoadBMP(filepath);
    if (!loadedImage) {
        std::cout << "Failed to load image: " << SDL_GetError() << "\n";
        return nullptr;
    }

    SDL_Surface* formattedSurface = SDL_CreateRGBSurfaceWithFormat(0, 256, 224, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!formattedSurface) {
        SDL_FreeSurface(loadedImage);
        return nullptr;
    }

    SDL_BlitScaled(loadedImage, nullptr, formattedSurface, nullptr);
    SDL_FreeSurface(loadedImage);

    uint32* buffer = new uint32[256 * 224];

    SDL_LockSurface(formattedSurface);
    memcpy(buffer, formattedSurface->pixels, 256 * 224 * sizeof(uint32));
    SDL_UnlockSurface(formattedSurface);

    SDL_FreeSurface(formattedSurface);
    return buffer;
}
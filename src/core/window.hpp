#pragma once

#include <SDL2/SDL.h>
#include <iostream>

#include "types.hpp"

class Window {
    private:
        bool keep_window_open = true;
        SDL_Window *window = nullptr;
        SDL_Surface *window_surface = nullptr;
    public:
    Window() {

    };
    int StartWindow();
	bool pollEvent(SDL_Event* event);
    void updateSurface();
    void closeWindow();

	void waitForVsync();

	// Drawing functions

	void fillRect(int x, int y, int w, int h, uint32 color);
	void fillScreen(uint32 color);
	void drawPixel(int x, int y, uint32 color);
	void drawBuffer(uint32* buffer);

	// just to test
	uint32* makeBufferFromImage(const char* filepath);
};

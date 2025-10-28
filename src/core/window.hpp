#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <queue>
#include <vector>

#include "types.hpp"

class Window {
    private:
        bool keep_window_open = true;
        SDL_Window *window = nullptr;
        SDL_Surface *window_surface = nullptr;
        SDL_AudioDeviceID audio_device = 0;
        SDL_AudioSpec audio_spec;
        std::queue<std::vector<uint8>> audio_queue;
    public:

	int width = 256;
	int height = 240;
	
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
	void setLogicalSize(int width, int height);

	// Audio functions
	bool initAudio(int frequency = 44100, uint16_t format = AUDIO_S16SYS, int channels = 1, int samples = 2048);
	void queueAudio(std::vector<uint8>* buffer);
	void pauseAudio(bool pause_on);
	uint32 getQueuedAudioSize() const;
	void clearAudioQueue();
	void closeAudio();
};

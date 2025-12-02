#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <queue>
#include <vector>
#include <ctime>
#include <string>

#include "types.hpp"

class Window {
private:
	bool keep_window_open = true;
	int64 timeAlive = 0;
	
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr; // Used for the emulator framebuffer
	
	SDL_AudioDeviceID audio_device = 0;
	SDL_AudioSpec audio_spec;
	std::queue<std::vector<uint8>> audio_queue;

	// Helper to extract RGBA from uint32 (ARGB8888)
	void setRenderColor(uint32 color);

public:
	Window() {};

	int WIDTH = 256;
	int HEIGHT = 240;

	int PIXEL_SCALE = 3;

	int StartWindow();
	bool pollEvent(SDL_Event* event);
	void updateSurface(double emulationSpeed = 1.0);
	void closeWindow();

	void waitForVsync();

	// Drawing functions
	void fillRect(int x, int y, int w, int h, uint32 color);
	void fillScreen(uint32 color);

	// Note: getPixel is very slow on Renderers and is rarely needed for rendering logic.
	// It is kept for compatibility but should be avoided.
	uint32 getPixel(int x, int y); 
	
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

	// text rendering functions
	void drawText(int x, int y, const std::string& text, uint32 textColor = 0xFFFFFFFF);
};
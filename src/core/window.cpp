#include "window.hpp"
#include "ui/font.hpp"


int Window::StartWindow() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "Failed to initialize the SDL2 library\n";
		return -1;
	}

	window = SDL_CreateWindow(
		"nescata",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		256,
		240,
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

void Window::updateSurface(double emulationSpeed) {
	// current time from ctime in milliseconds
	int64 currentTime = SDL_GetTicks64();
	double targetFrameTime = (16.666 * 2) / emulationSpeed; // approx 60 fps
	int64 timeSinceLastFrame = currentTime - timeAlive;
	if (timeSinceLastFrame < targetFrameTime) {
		SDL_Delay(targetFrameTime - timeSinceLastFrame);
	}
	timeAlive = currentTime;
	SDL_UpdateWindowSurface(window);
}

void Window::closeWindow() {
	closeAudio();
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::waitForVsync() {
}

// helpers

uint32 Window::compositeColors(uint32 color1, uint32 color2) {
	Color c1, c2, result;
	c1.value = color1;
	c2.value = color2;

	// Alpha blending formula: out = src * alpha + dst * (1 - alpha)
	float alpha = c2.a / 255.0f;
	result.r = (c2.r * alpha + c1.r * (1.0f - alpha));
	result.g = (c2.g * alpha + c1.g * (1.0f - alpha));
	result.b = (c2.b * alpha + c1.b * (1.0f - alpha));
	result.a = 0xFF; // fully opaque

	return result.value;
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

uint32 Window::getPixel(int x, int y) {
	uint32* pixels = (uint32*)window_surface->pixels;
	int width = window_surface->w;
	return pixels[y * width + x];
}

void Window::drawPixel(int x, int y, uint32 color) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 1;
	rect.h = 1;
	if (color >> 24 != 0xFF) { // if not fully opaque, composite
		uint32 destColor = getPixel(x, y);
		color = compositeColors(destColor, color);
	}
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
	rect.h = 240;

	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
		buffer,
		256,
		240,
		32,
		256 * sizeof(uint32),
		SDL_PIXELFORMAT_ARGB8888
	);

	if (surface) {
		SDL_BlitSurface(surface, nullptr, window_surface, &rect);
		SDL_FreeSurface(surface);
	}
}

void Window::setLogicalSize(int width, int height) {
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (renderer) {
		SDL_RenderSetLogicalSize(renderer, width, height);
		SDL_DestroyRenderer(renderer);
	}
}

bool Window::initAudio(int frequency, uint16_t format, int channels, int samples) {
	SDL_AudioSpec wanted;
	wanted.freq = frequency;
	wanted.format = format;
	wanted.channels = channels;
	wanted.samples = samples;
	wanted.callback = nullptr; // Using queue mode
	wanted.userdata = nullptr;

	audio_device = SDL_OpenAudioDevice(nullptr, 0, &wanted, &audio_spec, 0);
	if (audio_device == 0) {
		std::cout << "Failed to open audio device: " << SDL_GetError() << std::endl;
		return false;
	}

	// Start playing
	SDL_PauseAudioDevice(audio_device, 0);
	return true;
}

void Window::queueAudio(std::vector<uint8>* buffer) {
	if (audio_device != 0 && buffer != nullptr) {
		SDL_QueueAudio(audio_device, buffer->data(), buffer->size());
	}
}

void Window::pauseAudio(bool pause_on) {
	if (audio_device != 0) {
		SDL_PauseAudioDevice(audio_device, pause_on ? 1 : 0);
	}
}

uint32 Window::getQueuedAudioSize() const {
	if (audio_device != 0) {
		return SDL_GetQueuedAudioSize(audio_device);
	}
	return 0;
}

void Window::clearAudioQueue() {
	if (audio_device != 0) {
		SDL_ClearQueuedAudio(audio_device);
	}
}

void Window::closeAudio() {
	if (audio_device != 0) {
		SDL_CloseAudioDevice(audio_device);
		audio_device = 0;
	}
}

void Window::drawText(int x, int y, const std::string& text, uint32 textColor) {
	// Render using the built-in 8x8 bitmap font. Characters outside 32..127
	// will be rendered as a space.
	if (!window_surface) return;

	int px = x;
	for (char c : text) {
		uint8 code = static_cast<uint8>(c);
		// newline handling
		if (code == '\n') {
			px = x;
			y += 8;
			continue;
		}
		// restrict to printable characters
		if (code < 32 || code > 127) code = 32;
		const uint8* glyph = font6x8[code - 32];

		for (int row = 0; row < 8; ++row) {
			uint8 bits = glyph[row];
			for (int col = 0; col < 6; ++col) {
				if (bits & (1 << (7 - col))) {
					drawPixel(px + col, y + row, textColor);
				} else {
					// slightly transparent background for readability
					uint32 bgColor = 0x7F000000; // ARGB with low alpha
					drawPixel(px + col, y + row, bgColor);
				}
			}
		}

		px += 6; // advance by 6 pixels per character
	}
}
#include "window.hpp"

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
    closeAudio();
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

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        buffer,
        256,
        224,
        32,
        256 * sizeof(uint32),
		SDL_PIXELFORMAT_RGBA8888
    );

    if (surface) {
        SDL_BlitSurface(surface, nullptr, window_surface, &rect);
        SDL_FreeSurface(surface);
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
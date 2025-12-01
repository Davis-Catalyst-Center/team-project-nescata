#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "types.hpp"
#include "palettes.hpp"

#include "apu.hpp"
#include "bus.hpp"
#include "cart.hpp"
#include "composite.hpp"
#include "controller.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "window.hpp"

// ui
#include "ui/message.hpp"


// the core initializes and manages all the components


class Core {
public:
	Bus bus;
	CPU cpu;
	PPU ppu;
	Composite comp;
	APU apu;
	Window window;
	Controller controller1;
	Controller controller2;

	bool enableWindow = true;

	// time management
	double emulationSpeed = 1.0; // 1.0 = normal speed
	bool paused = false;
	bool passFrame = false; // used when paused to advance a single frame

	// messaging system
	std::vector<Message> messages;
	void addMessage(const std::string& text, uint32 textColor);
	void dismissMessage(size_t index); // manual dismissal
	void dismissMessage(); // most recent infinite message
	void updateMessages();
	void renderMessages();

	// states
	void rebindKeys();
	int getScancodeOfSingleKey();

	// keybinds
	int lastKeyScancode = -1;
	bool rebindInProgress = false;

	enum class Keybind {
		A,
		B,
		START,
		SELECT,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	int keys[8] = {
		SDL_SCANCODE_Z,      // A
		SDL_SCANCODE_X,      // B
		SDL_SCANCODE_RETURN, // START
		SDL_SCANCODE_RSHIFT, // SELECT
		SDL_SCANCODE_UP,     // UP
		SDL_SCANCODE_DOWN,   // DOWN
		SDL_SCANCODE_LEFT,   // LEFT
		SDL_SCANCODE_RIGHT,  // RIGHT
	};

	Core();

	void run();
	void reset();
	void fullReset();

	void handleWindowEvents();
	void handleKeyboardEvent(SDL_KeyboardEvent keyEvent);
	uint8 getControllerButtonState() const;
	void processHeldKeys();

	void connectCart(Cart* cart);
	void disconnectCart();
	void setController1(ControllerType type);
	void setController2(ControllerType type);
};
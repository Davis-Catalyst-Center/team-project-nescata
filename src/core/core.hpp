#pragma once

#include <iostream>
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

	// fun variables
	double emulationSpeed = 1.0; // 1.0 = normal speed

	Core();

	void run();
	void reset();
	void fullReset();

	void handleWindowEvents();
	void handleKeyboardEvent(SDL_KeyboardEvent keyEvent);

	void connectCart(Cart* cart);
	void disconnectCart();
	void setController1(ControllerType type);
	void setController2(ControllerType type);
};
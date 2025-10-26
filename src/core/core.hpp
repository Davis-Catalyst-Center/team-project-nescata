#pragma once

#include "types.hpp"

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
	CPU cpu;
	Bus bus;
	PPU ppu;
	Composite comp;
	APU apu;
	Window window;
	Controller controller1;
	Controller controller2;

    Core();

    void run();
	void reset();
	void fullReset();

	void handleWindowEvents();
	void handleKeyboardEvent(SDL_KeyboardEvent keyEvent);

    void loadCart(Cart* cart);
    void unloadCart();
	void setController1(ControllerType type);
	void setController2(ControllerType type);
};
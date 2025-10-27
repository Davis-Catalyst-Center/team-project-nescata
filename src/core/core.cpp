#include "core.hpp"

Core::Core() : cpu(bus), window(){
}

void Core::run() {
    cpu.powerOn();
	int i = 0;
    while (true) {
		cpu.clock();
		// temporary to give window an update chance
		if (i++ % 10000 == 0) {
			window.updateSurface();
			handleWindowEvents();
		}
	}
	
}

void Core::reset() {
	cpu.reset();
}

void Core::fullReset() {
	bus.unloadCart();
	cpu.powerOn();
}

void Core::handleWindowEvents() {
	SDL_Event event;
	while (window.pollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				window.closeWindow();
				exit(0);
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				handleKeyboardEvent(event.key);
				break;
			default:
				break;
		}
	}
}

void Core::handleKeyboardEvent(SDL_KeyboardEvent keyEvent) {
	bool pressed = (keyEvent.type == SDL_KEYDOWN);
	switch (keyEvent.keysym.sym) {
		case SDLK_s:
			controller1.setButtonState(0x01, pressed); // A
			break;
		case SDLK_a:
			controller1.setButtonState(0x02, pressed); // B
			break;
		case SDLK_q:
			controller1.setButtonState(0x04, pressed); // Select
			break;
		case SDLK_w:
			controller1.setButtonState(0x08, pressed); // Start
			break;
		case SDLK_UP:
			controller1.setButtonState(0x10, pressed); // Up
			break;
		case SDLK_DOWN:
			controller1.setButtonState(0x20, pressed); // Down
			break;
		case SDLK_LEFT:
			controller1.setButtonState(0x40, pressed); // Left
			break;
		case SDLK_RIGHT:
			controller1.setButtonState(0x80, pressed); // Right
			break;
		// other controls
		case SDLK_r:
			if (pressed) {
				reset();
			}
			break;
		case SDLK_ESCAPE:
			if (pressed) {
				window.closeWindow();
				exit(0);
			}
			break;
		case SDLK_t: // test
			if (pressed) {
				void* buffer = window.makeBufferFromImage("test.bmp");
				window.drawBuffer(0, 0, (uint32*)buffer);
			}
			break;
		default:
			break;
	}
}

void Core::loadCart(Cart* cart) {
    bus.loadCart(cart);
}

void Core::unloadCart() {
    bus.unloadCart();
}

void Core::setController1(ControllerType type) {
	controller1 = Controller(type);
}

void Core::setController2(ControllerType type) {
	controller2 = Controller(type);
}
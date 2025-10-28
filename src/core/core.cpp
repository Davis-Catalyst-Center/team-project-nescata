#include "core.hpp"

Core::Core() {
	cpu.connectBus(&bus);
	bus.connectAPU(&apu);
	bus.connectPPU(&ppu);
	comp.connectPPU(&ppu);
	ppu.connectComposite(&comp);
	bus.connectController1(&controller1);
	bus.connectController2(&controller2);
}

void Core::run() {
	if (enableWindow) {
		if (window.StartWindow() != 0) {
			std::cerr << "Failed to start window!" << std::endl;
			return;
		}
	}
    cpu.powerOn();
	long int lastCycles = 0;
    while (true) {
		cpu.clock();
		// temporary to give window an update chance
		if (cpu.getCycles() - lastCycles >= 29781) {
			uint32* frameBuffer = comp.renderFrame();
			if (frameBuffer) {
				window.drawBuffer(frameBuffer);
			}
			std::vector<uint8> audioBuffer = apu.getAudioBuffer();
			window.queueAudio(&audioBuffer);
			handleWindowEvents();
			window.updateSurface();
			lastCycles += 29781;
		}
	}
}

void Core::reset() {
	cpu.reset();
}

void Core::fullReset() {
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
		default:
			break;
	}
}

void Core::connectCart(Cart* cart) {
    bus.connectCart(cart);
	comp.connectCart(cart);
}

void Core::disconnectCart() {
    bus.disconnectCart();
	comp.disconnectCart();
}

void Core::setController1(ControllerType type) {
	controller1 = Controller(type);
}

void Core::setController2(ControllerType type) {
	controller2 = Controller(type);
}
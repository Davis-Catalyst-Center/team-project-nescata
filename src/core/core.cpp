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
		if (cpu.clock()) { // returns true if nmi
			
			uint32* frameBuffer = comp.getBuffer();
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
	// do controller state
	const uint8* keyboardState = SDL_GetKeyboardState(NULL);
	uint8 buttonState = (
		(keyboardState[SDL_SCANCODE_S]    << 7) + // a
		(keyboardState[SDL_SCANCODE_A]    << 6) + // b
		(keyboardState[SDL_SCANCODE_Q]    << 5) + // select
		(keyboardState[SDL_SCANCODE_W]    << 4) + // start
		(keyboardState[SDL_SCANCODE_UP]   << 3) + // up
		(keyboardState[SDL_SCANCODE_DOWN] << 2) + // down
		(keyboardState[SDL_SCANCODE_LEFT] << 1) + // left
		(keyboardState[SDL_SCANCODE_RIGHT])       // right
	);
	controller1.setState(buttonState);

	if (keyboardState[SDL_SCANCODE_1]) window.fillRect(20, 20, 20, 20, 0xffffffff);
}

void Core::handleKeyboardEvent(SDL_KeyboardEvent keyEvent) {
	bool pressed = (keyEvent.type == SDL_KEYDOWN);
	switch (keyEvent.keysym.sym) {
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
	ppu.connectCart(cart);
}

void Core::disconnectCart() {
	bus.disconnectCart();
	comp.disconnectCart();
	ppu.disconnectCart();
}

void Core::setController1(ControllerType type) {
	controller1 = Controller(type);
}

void Core::setController2(ControllerType type) {
	controller2 = Controller(type);
}
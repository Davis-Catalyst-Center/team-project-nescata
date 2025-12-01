#include "core.hpp"

Core::Core() {
	cpu.connectBus(&bus);
	bus.connectAPU(&apu);
	bus.connectPPU(&ppu);
	comp.connectPPU(&ppu);
	ppu.connectComposite(&comp);
	ppu.connectCPU(&cpu);
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
	cpu.reset();
	while (true) {
		if (emulationSpeed == 0.0) {
			// paused
			SDL_Delay(100);
			handleWindowEvents();
			continue;
		}
		if (cpu.clock()) { // returns true if nmi
			
			uint32* frameBuffer = comp.getBuffer();
			if (frameBuffer) {
				window.drawBuffer(frameBuffer);
			}
			std::vector<uint8> audioBuffer = apu.getAudioBuffer();
			window.queueAudio(&audioBuffer);
			handleWindowEvents();
			window.updateSurface(emulationSpeed);
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
		(keyboardState[SDL_SCANCODE_S]     << 0) | // A
		(keyboardState[SDL_SCANCODE_A]     << 1) | // B
		(keyboardState[SDL_SCANCODE_Q]     << 2) | // Select
		(keyboardState[SDL_SCANCODE_W]     << 3) | // Start
		(keyboardState[SDL_SCANCODE_UP]    << 4) | // Up
		(keyboardState[SDL_SCANCODE_DOWN]  << 5) | // Down
		(keyboardState[SDL_SCANCODE_LEFT]  << 6) | // Left
		(keyboardState[SDL_SCANCODE_RIGHT] << 7)   // Right
	);
	controller1.setState(buttonState);
}

void Core::handleKeyboardEvent(SDL_KeyboardEvent keyEvent) {
	bool pressed = (keyEvent.type == SDL_KEYDOWN);
	switch (keyEvent.keysym.sym) {
		// other controls
		case SDLK_r:
			if (pressed) {
				fullReset();
				reset();
			}
			break;
		case SDLK_ESCAPE:
			if (pressed) {
				window.closeWindow();
				exit(0);
			}
			break;
		case SDLK_p:
			if (pressed) {
				if (emulationSpeed != 0.0) {
					emulationSpeed = 0.0; // pause
				} else {
					emulationSpeed = 1.0; // unpause
				}
			}
			break;
		case SDLK_EQUALS:
			if (pressed) {
				emulationSpeed += 0.1;
				if (emulationSpeed > 4.0) {
					emulationSpeed = 4.0;
				}
				std::cout << "Emulation speed: " << emulationSpeed << "x" << std::endl;
			}
			break;
		case SDLK_MINUS:
			if (pressed) {
				emulationSpeed -= 0.1;
				if (emulationSpeed < 0.1) {
					emulationSpeed = 0.1;
				}
				std::cout << "Emulation speed: " << emulationSpeed << "x" << std::endl;
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
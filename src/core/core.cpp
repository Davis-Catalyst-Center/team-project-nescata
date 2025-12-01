#include "core.hpp"

Core::Core() {
	cpu.connectBus(&bus);
	bus.connectAPU(&apu);
	bus.connectPPU(&ppu);
	ppu.connectComposite(&comp);
	ppu.connectCPU(&cpu);
	comp.connectPPU(&ppu);
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
		if (paused || emulationSpeed == 0.0) {
			// paused
			if (!passFrame) {
				SDL_Delay(100);
				uint32* frameBuffer = comp.getBuffer();
				if (frameBuffer) {
					window.drawBuffer(frameBuffer);
				}
				handleWindowEvents();
				window.updateSurface(1.0);
			}
		}
		if (cpu.clock()) { // returns true if frame has completed
			uint32* frameBuffer = comp.getBuffer();
			if (frameBuffer) {
				window.drawBuffer(frameBuffer);
			}
			std::vector<uint8> audioBuffer = apu.getAudioBuffer();
			window.queueAudio(&audioBuffer);
			handleWindowEvents();
			// 9999 to skip delay when advancing a single frame
			window.updateSurface(passFrame ? 9999 : emulationSpeed);
			passFrame = false;
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

	updateMessages();
	renderMessages();
	processHeldKeys();

	// update controller state from current keyboard state
	uint8 buttonState = getControllerButtonState();
	controller1.setState(buttonState);
}

void Core::handleKeyboardEvent(SDL_KeyboardEvent keyEvent) {
	bool pressed = (keyEvent.type == SDL_KEYDOWN && keyEvent.repeat == 0);

	switch (keyEvent.keysym.sym) {
		case SDLK_r: // reset
			if (pressed) {
				fullReset();
				reset();
			}
			break;
		case SDLK_ESCAPE: // quit
			if (pressed) {
				window.closeWindow();
				exit(0);
			}
			break;
		case SDLK_p: // pause/unpause
			if (pressed) {
				paused = !paused;
				if (paused) {
					addMessage("Emulation paused.", 0xFFFFFF00);
				} else {
					addMessage("Emulation resumed.", 0xFF00FF00);
				}
			}
			break;
		case SDLK_b: // start rebind flow
			if (pressed) {
				paused = true;
				rebindKeys();
			}
			break;
		case SDLK_f: // advance a single frame when paused
			if (pressed) {
				passFrame = true;
				if (!paused) paused = true;
				addMessage("Advancing a single frame.", 0xFFFFFF00);
			}
			break;
		// speed controls
		case SDLK_EQUALS: // speed up
			if (pressed) {
				emulationSpeed += 0.1;
				if (emulationSpeed > 4.0) {
					emulationSpeed = 4.0;
				}
				addMessage("Emulation speed: " + std::to_string(emulationSpeed) + "x", 0xFFFFFF00);
			}
			break;
		case SDLK_MINUS: // slow down
			if (pressed) {
				emulationSpeed -= 0.1;
				if (emulationSpeed < 0.1) {
					emulationSpeed = 0.1;
				}
				addMessage("Emulation speed: " + std::to_string(emulationSpeed) + "x", 0xFFFFFF00);
			}
			break;
		case SDLK_h: // help
			if (pressed) {
				addMessage("Keybinds:", 0xFFFFFF00);
				addMessage("R - Reset", 0xFFFFFF00);
				addMessage("ESC - Quit", 0xFFFFFF00);
				addMessage("P - Pause/Unpause", 0xFFFFFF00);
				addMessage("B - Rebind Controller Keys", 0xFFFFFF00);
				addMessage("F - Advance Single Frame (when paused)", 0xFFFFFF00);
				addMessage("+ - Increase Emulation Speed", 0xFFFFFF00);
				addMessage("- - Decrease Emulation Speed", 0xFFFFFF00);
			}
			break;
		default:
			if (pressed && rebindInProgress) {
				lastKeyScancode = keyEvent.keysym.scancode;
				rebindInProgress = false;
			}
			break;
	}
}

uint8 Core::getControllerButtonState() const {
	const uint8* keyboardState = SDL_GetKeyboardState(NULL);
	uint8 buttonState = (
		(keyboardState[keys[(int)Keybind::A]]      << 0) | // A
		(keyboardState[keys[(int)Keybind::B]]      << 1) | // B
		(keyboardState[keys[(int)Keybind::SELECT]] << 2) | // Select
		(keyboardState[keys[(int)Keybind::START]]  << 3) | // Start
		(keyboardState[keys[(int)Keybind::UP]]     << 4) | // Up
		(keyboardState[keys[(int)Keybind::DOWN]]   << 5) | // Down
		(keyboardState[keys[(int)Keybind::LEFT]]   << 6) | // Left
		(keyboardState[keys[(int)Keybind::RIGHT]]  << 7)   // Right
	);
	return buttonState;
}

void Core::processHeldKeys() {
	// currently no held key processing needed
}

void Core::addMessage(const std::string& text, uint32 textColor) {
	// add to back of message list
	messages.emplace_back(Message(text, textColor));
}

void Core::dismissMessage(size_t index) {
	// dismiss message at index
	if (index < messages.size()) {
		messages.erase(messages.begin() + index);
	}
}

void Core::dismissMessage() {
	// dismiss most recent infinite message
	for (size_t i = 0; i < messages.size(); ++i) {
		if (messages[i].timeToLive == -1) {
			messages.erase(messages.begin() + i);
			break;
		}
	}
}

void Core::updateMessages() {
	for (int i = 0; i < messages.size();) {
		int currentTime = SDL_GetTicks64();
		if (currentTime - messages[i].timestamp >= messages[i].timeToLive && messages[i].timeToLive != -1) {
			// remove message
			messages.erase(messages.begin() + i);
		} else {
			i++;
		}
	}
}

void Core::renderMessages() {
	int xOffset = 10;
	int yOffset = 10;
	for (const Message& message : messages) {
		// render each message at (xOffset, yOffset)
		window.drawText(xOffset, yOffset, message.text, message.textColor);
		yOffset += 8; // move down for next message
	}
}

void Core::rebindKeys() {
	// Start interactive rebind: ask user to choose which button to rebind
	addMessage("Rebind mode", 0xFFFFFF00);
	addMessage("Press the key for A button", 0xFFFFFF00);
	int newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::A] = newScancode;
	addMessage("A button rebound", 0xFF00FF00);
	addMessage("Press the key for B button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::B] = newScancode;
	addMessage("B button rebound", 0xFF00FF00);
	addMessage("Press the key for SELECT button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::SELECT] = newScancode;
	addMessage("SELECT button rebound", 0xFF00FF00);
	addMessage("Press the key for START button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::START] = newScancode;
	addMessage("START button rebound", 0xFF00FF00);
	addMessage("Press the key for UP button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::UP] = newScancode;
	addMessage("UP button rebound", 0xFF00FF00);
	addMessage("Press the key for DOWN button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::DOWN] = newScancode;
	addMessage("DOWN button rebound", 0xFF00FF00);
	addMessage("Press the key for LEFT button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::LEFT] = newScancode;
	addMessage("LEFT button rebound", 0xFF00FF00);
	addMessage("Press the key for RIGHT button", 0xFFFFFF00);
	newScancode = getScancodeOfSingleKey();
	keys[(int)Keybind::RIGHT] = newScancode;
	addMessage("RIGHT button rebound", 0xFF00FF00);
	addMessage("Rebind complete", 0xFF00FF00);
	paused = false;
}

int Core::getScancodeOfSingleKey() {
	rebindInProgress = true;
	lastKeyScancode = -1;
	// wait until a key is pressed
	while (rebindInProgress) {
		SDL_Delay(100);
		uint32* frameBuffer = comp.getBuffer();
		if (frameBuffer) {
			window.drawBuffer(frameBuffer);
		}
		handleWindowEvents();
		window.updateSurface(1.0);
	}
	return lastKeyScancode;
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
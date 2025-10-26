#pragma once

#include "types.hpp"

#include "controllers/statehandler.hpp"
#include "controllers/standard.hpp"

enum ControllerType {
	DISCONNECTED,
	STANDARD,
};

class Controller {
private:
	ControllerStateHandler* stateHandler;
public:
	Controller(ControllerType type = DISCONNECTED) {
		switch (type) {
			case DISCONNECTED:
				stateHandler = nullptr;
				break;
			case STANDARD:
			default:
				stateHandler = new StandardStateHandler();
				break;
		}
	}
	~Controller() {
		if (stateHandler) {
			delete stateHandler;
		}
	}
	void write(uint8 value) {
		if (stateHandler) {
			stateHandler->write(value);
		}
	}
	uint8 read() {
		if (stateHandler) {
			return stateHandler->read();
		}
		return 0;
	}
	void setButtonState(uint8 buttonMask, bool pressed) {
		StandardStateHandler* standardHandler = dynamic_cast<StandardStateHandler*>(stateHandler);
		if (standardHandler) {
			standardHandler->setButtonState(buttonMask, pressed);
		}
	}
};
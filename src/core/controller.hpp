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
	ControllerType type = DISCONNECTED;
public:
	Controller(ControllerType controllerType = DISCONNECTED) {
		type = controllerType;
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
		switch (type) {
			case DISCONNECTED:
				break;
			case STANDARD:
				stateHandler->setButtonState(buttonMask, pressed);
				break;
			default:
				break;
		}
	}
	void setState(uint8 buttonMask) {
		stateHandler->setState(buttonMask);
	}
};
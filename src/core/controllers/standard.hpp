#pragma once

#include "../types.hpp"

#include "statehandler.hpp"

union StandardControllerState {
	struct {
		uint8 a: 1;
		uint8 b: 1;
		uint8 select: 1;
		uint8 start: 1;
		uint8 up: 1;
		uint8 down: 1;
		uint8 left: 1;
		uint8 right: 1;
	};
	uint8 raw;
};

class StandardStateHandler : public ControllerStateHandler {
private:
	StandardControllerState state;
	bool strobe = false;
	int readIndex = 1;
public:
	StandardStateHandler() {

	}

	void write(uint8 value) {
		strobe = value & 1;
		if (strobe) {
			readIndex = 0;
		}
	}

	uint8 read() {
		uint8 ret = 0;
		if (readIndex < 8) {
			ret = (state.raw >> readIndex) & 1;
		} else {
			ret = 1; // Open bus behavior
		}
		if (!strobe) {
			readIndex++;
		}
		return ret;
	}

	void setButtonState(uint8 buttonMask, bool pressed) {
		if (pressed) {
			state.raw |= buttonMask;
		} else {
			state.raw &= ~buttonMask;
		}
	}

	void setState(uint8 buttonMask) {
		state.raw = buttonMask;
	}
};
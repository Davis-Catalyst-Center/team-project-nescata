#pragma once
#include "../types.hpp"


class ControllerStateHandler {
public:
	virtual void write(uint8 value) = 0;
	virtual uint8 read() = 0;
	virtual void setButtonState(uint8 buttonMask, bool pressed) = 0;
};
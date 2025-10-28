#pragma once

#include <vector>

#include "types.hpp"

#include "window.hpp"

class APU {
public:
	APU();

	void reset();

	void step(int cycles);

	std::vector<uint8> getAudioBuffer();
};
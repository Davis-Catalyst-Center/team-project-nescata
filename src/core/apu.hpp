#pragma once


class APU {

public:
	APU();

	void reset();

	void step(int cycles);

};
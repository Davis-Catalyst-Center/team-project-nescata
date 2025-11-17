#pragma once

#include "types.hpp"
#include "cart.hpp"

class PPU; // forward declaration

class Composite {
private:
	Cart* cart = nullptr;
	PPU* ppu = nullptr;

	uint32 frame_buffer[256 * 240]; // NES resolution

public:
	Composite();

	

	void renderScanline(int scanline);
	uint32* renderFrame();

	void connectPPU(PPU* ppu);
	void disconnectPPU();
	void connectCart(Cart* cart);
	void disconnectCart();
};


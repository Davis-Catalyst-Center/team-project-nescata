#pragma once

#include "types.hpp"
#include "cart.hpp"
#include "palettes.hpp"

class PPU; // forward declaration

class Composite {
private:
	Cart* cart = nullptr;
	PPU* ppu = nullptr;

	uint32 frameBuffer[256 * 240]; // NES resolution

public:
	Composite();

	

	void renderScanline(int scanline);
	void renderBackgroundAtLine(int scanline, uint32* lineBuf);
	void renderSpritesAtLine(int scanline, int spriteIdx, uint32* lineBuf);

	uint32* getBuffer();

	void connectPPU(PPU* ppu);
	void disconnectPPU();
	void connectCart(Cart* cart);
	void disconnectCart();
};



#include "ppu.hpp"
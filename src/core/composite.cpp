#include "composite.hpp"

Composite::Composite() {
	
}

void Composite::renderScanline(int scanline) {
	// fill a line of the frame buffer with a random solid color for testing
	uint32 color = 0xFF000000 | (rand() % 0x00FFFFFF);
	for (int x = 0; x < 256; x++) {
		frame_buffer[scanline * 256 + x] = color;
	}
}

uint32* Composite::renderFrame() {
	return frame_buffer;
}

void Composite::connectPPU(PPU* ppuRef) {
	ppu = ppuRef;
}

void Composite::disconnectPPU() {
	ppu = nullptr;
}

void Composite::connectCart(Cart* cartRef) {
	cart = cartRef;
}

void Composite::disconnectCart() {
	cart = nullptr;
}
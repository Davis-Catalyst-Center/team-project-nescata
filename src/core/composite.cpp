#include "composite.hpp"

Composite::Composite() {
	
}






void Composite::renderScanline(int scanline) {

	ppu->oam.sprites[1];

	// fill a line of the frame buffer with a random solid color for testing
	int pixel = scanline * 256;
	uint32 color = 0xFF0800ff; 
	for (int x = pixel; x < pixel + 256; x++) {
		frame_buffer[pixel + x] = color;
	}
	for (int s = 0; s < 64; s++) {
		frame_buffer[pixel + ppu->oam.sprites[s].x] = 0xff88ffcc;
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
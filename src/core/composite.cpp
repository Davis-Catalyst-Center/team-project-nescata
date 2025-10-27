#include "composite.hpp"

Composite::Composite() {
	
}

uint32* Composite::renderFrame() {
	void* frameBuffer = malloc(256 * 224 * sizeof(uint32));
	// fill random colors for testing
	uint32* buffer = static_cast<uint32*>(frameBuffer);
	for (int i = 0; i < 256 * 224; i++) {
		buffer[i] = (rand() % 0xFFFFFFFF) << 8 | 0xFF;
	}
	return buffer;
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
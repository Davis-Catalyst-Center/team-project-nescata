#include "composite.hpp"

Composite::Composite() {
	
}






void Composite::renderScanline(int scanline) {

	ppu->oam.sprites[1];

	// fill a line of the frame buffer with a random solid color for testing
	int pixel = scanline * 256;
	uint32 color = 0xFF0800ff; 
	
}

uint32 renderbackground(){

	return ; 
}
uint32 renderSprite(int scanline,int spriteIdx){
	int pixel = scanline * 256;
	uint32 retColor[256];
	for (int x = pixel; x < pixel + 256; x++) {
		//retColor[pixel + x] = color;
	}
	for (int s = 0; s < 64; s++) {
		//retColor[pixel + ppu->oam.sprites[s].x] = 0xff88ffcc;
	}


	return ; 
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
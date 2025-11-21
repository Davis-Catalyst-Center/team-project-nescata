#include "composite.hpp"

Composite::Composite() {
	
}






void Composite::renderScanline(int scanline) {
	int pixel = scanline * 256; // start of the line in the frame buffer

	// overscan lines (not visible)
	if (scanline < 0 || scanline >= 240) {
		return;
	}
	
	// actual bg color
	uint32 bgColorIdx = ppu->palette[0] & 0x3F; // get background color index from palette
	uint32 bgColor = defaultARGBpal[bgColorIdx]; // get ARGB color from palette

	for (int x = 0; x < 256; x++) { // fill line with bg color
		frameBuffer[pixel + x] = bgColor;
	}

	// render background tiles
	uint32 bgLine[256] = {0};
	renderBackgroundAtLine(scanline, bgLine);
	// render sprite tiles
	uint32 spriteLine[256] = {0};
	renderSpritesAtLine(scanline, 0, spriteLine);

	// composite bg and sprites onto frame buffer
	for (int x = 0; x < 256; x++) {
		// if sprite pixel is not transparent, draw it over bg
		if ((spriteLine[x] & 0x000000FF) != 0) {
			frameBuffer[pixel + x] = spriteLine[x];
		} else {
			frameBuffer[pixel + x] = bgLine[x];
		}
	}
}

void Composite::renderBackgroundAtLine(int scanline, uint32* lineBuf){

}

void Composite::renderSpritesAtLine(int scanline, int spriteIdx, uint32* lineBuf) {
	int pixel = scanline * 256;

	for (int s = 0; s < 64; s++) {
		lineBuf[pixel + ppu->oam.sprites[s].x] = 0xFFFFD700; // Gold color for sprite
	}

}


uint32* Composite::getBuffer() {
	return frameBuffer;
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
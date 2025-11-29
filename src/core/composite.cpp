#include "composite.hpp"

Composite::Composite() {
	
}






void Composite::renderScanline(int scanline) {
	int pixel = scanline * 256; // not << 8 in case of negative scanlines

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
	// front priority sprites
	uint32 spriteFrontLine[256] = {0};
	renderSpritesAtLine(scanline, 0, spriteFrontLine);
	// back priority sprites
	uint32 spriteBackLine[256] = {0};
	renderSpritesAtLine(scanline, 1, spriteBackLine);

	// composite bg and sprites onto frame buffer
	for (int x = 0; x < 256; x++) {
		// if sprite pixel is not transparent, draw it over bg
		if ((spriteFrontLine[x] & 0xFF000000) != 0) {
			frameBuffer[pixel + x] = spriteFrontLine[x];
		} else if ((bgLine[x] & 0xFF000000) != 0) {
			frameBuffer[pixel + x] = bgLine[x];
		} else if ((spriteBackLine[x] & 0xFF000000) != 0) {
			frameBuffer[pixel + x] = spriteBackLine[x];
		}
	}
}

void Composite::renderBackgroundAtLine(int scanline, uint32* lineBuf) {
}

void Composite::renderSpritesAtLine(int scanline, int priority, uint32* lineBuf) {
	for (int s = 0; s < 64; s++) {
		if (((ppu->oam.sprites[s].attr & 0x20) >> 5) != priority) {
			continue; // skip sprites that don't match the priority
		}

		int spriteY = ppu->oam.sprites[s].y;

		int y = scanline - spriteY;
		if (y < 0 || y >= 8) continue; // tile not on this line
	
		int spriteX = ppu->oam.sprites[s].x;
		int spriteIdx = ppu->oam.sprites[s].tileIdx;
		uint8 attributes = ppu->oam.sprites[s].attr;

		bool flipX = (attributes & 0x40) != 0;
		bool flipY = (attributes & 0x80) != 0;
		uint8 paletteIndex = (attributes & 0x03) + 4; // sprite palettes start at index 4

		uint32 spriteLine[8] = {0};


		// get the full tile data from CHR ROM/RAM
		// each tile is 16 bytes. each byte represents one row of 8 pixels
		// 2 bits per pixel, a bit from each byte, so 2 bytes per row:
		// (01)(01)(01)(01)(01)(01)(01)(01)
		// (23)(23)(23)(23)(23)(23)(23)(23)
		// (45)(45)(45)(45)(45)(45)(45)(45)
		// etc...

		// we only need one row (2 bytes) at a time

		uint8 highByte, lowByte;

		int row = flipY ? (7 - y) : y;
		highByte = cart->readChr(spriteIdx * 16 + row);
		lowByte = cart->readChr(spriteIdx * 16 + row + 8);

		for (int x = 0; x < 8; x++) {
			int bit = flipX ? x : (7 - x);
			uint8 bit0 = (highByte >> bit) & 0x01;
			uint8 bit1 = (lowByte >> bit) & 0x01;
			uint8 colorIdx = (bit1 << 1) | bit0;

			if (colorIdx == 0) {
				lineBuf[spriteX + x] = 0; // transparent pixel
			} else {
				uint8 absPaletteIndex = ppu->palette[paletteIndex * 4 + colorIdx] & 0x3F; // get color index from palette
				lineBuf[spriteX + x] = defaultARGBpal[absPaletteIndex]; // get ARGB color from palette
			}
		}
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
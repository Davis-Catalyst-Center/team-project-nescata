#include "composite.hpp"

Composite::Composite() {
	
}






void Composite::renderScanline(int scanline) {
	int pixel = scanline * 256; // not << 8 in case of negative scanlines
	// start of the line in the frame buffer

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

	return; // temp to only show bg color

	// render background tiles
	uint32 bgLine[256] = {0};
	renderBackgroundAtLine(scanline, bgLine);
	// render sprite tiles
	uint32 spriteLine[256] = {0};
	renderSpritesAtLine(scanline, 0, spriteLine);

	// composite bg and sprites onto frame buffer
	for (int x = 0; x < 256; x++) {
		// if sprite pixel is not transparent, draw it over bg
		if ((spriteLine[x] & 0xFF000000) != 0) {
			frameBuffer[pixel + x] = spriteLine[x];
		} else if ((bgLine[x] & 0xFF000000) != 0) {
			frameBuffer[pixel + x] = bgLine[x];
		}
	}
}

void Composite::renderBackgroundAtLine(int scanline, uint32* lineBuf){

}

void Composite::renderSpritesAtLine(int scanline, int spriteIdx, uint32* lineBuf) {
	int pixel = scanline * 256;

	for (int s = 0; s < 64; s++) {

		int spriteX = ppu->oam.sprites[s].x;
		int spriteY = ppu->oam.sprites[s].y;
		int tileNum = ppu->oam.sprites[s].tileIdx;
		uint8 attributes = ppu->oam.sprites[s].attr;

		bool flipX = (attributes & 0x40) != 0;
		bool flipY = (attributes & 0x80) != 0;
		uint8 paletteIndex = (attributes & 0x03) + 4; // sprite palettes start at index 4

		uint32 spriteLine[8] = {0};
		if (getTileLine(scanline, spriteX, spriteY, paletteIndex, flipX, flipY, spriteLine)) {
			for (int x = 0; x < 8; x++) {
				int drawX = spriteX + x;
				if (drawX >= 0 && drawX < 256) {
					lineBuf[drawX] = spriteLine[x];
				}
			}
		}
	}

}


bool Composite::getTileLine(int scanline, int tileX, int tileY, uint8 palette, bool flipX, bool flipY, uint32* lineBuf) {
	// linebuf is 8 pixels wide
	// just returns if the tile is on this scanline
	// and if so, fills lineBuf with the pixel colors for the tile's line
	int y = scanline + tileY;
	if (y < 0 || y >= 8) return false; // tile not on this line

	// get the full tile


	// for now, fill with a test pattern
	for (int x = 0; x < 8; x++) {
		lineBuf[x] = (x + scanline) % 2 == 0 ? 0xFFFF00FF : 0xFF000000; // Magenta and black pixels
	}
	return true;
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
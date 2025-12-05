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

	// back priority sprites
	uint32 spriteBackLine[256] = {0};
	renderSpritesAtLine(scanline, 1, spriteBackLine);
	// render background tiles
	uint32 bgLine[256] = {0};
	renderBackgroundAtLine(scanline, bgLine);
	// front priority sprites
	uint32 spriteFrontLine[256] = {0};
	renderSpritesAtLine(scanline, 0, spriteFrontLine);

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
	int scrollX = ppu->SCRLget().x | (ppu->ctrl.raw & 1) * 256;
	int scrollY = ppu->SCRLget().y | (ppu->ctrl.raw & 2) * 128;
	// render all 4 nametables to handle scrolling
	// mirroring is handled in PPU nametable read/write functions
	// nametables are laid out in a 2x2 grid at fixed positions
	// we scroll the screen over them, but here we just move them under the screen

	int ntlx = scrollX > 256 ? 512 - scrollX : -scrollX;
	int ntty = scrollY > 240 ? 496 - scrollY : -scrollY;

	renderNametableAtLine(scanline, 0, ntlx,       ntty,       lineBuf);
	renderNametableAtLine(scanline, 1, ntlx + 256, ntty,       lineBuf);
	renderNametableAtLine(scanline, 2, ntlx,       ntty + 240, lineBuf);
	renderNametableAtLine(scanline, 3, ntlx + 256, ntty + 240, lineBuf);
}

void Composite::renderNametableAtLine(int scanline, int nametableIdx, int xPos, int yPos, uint32* lineBuf) {
	int lineInNametable = scanline - yPos;
	if (lineInNametable < -240) lineInNametable += 480;
	if (lineInNametable < 0 || lineInNametable >= 240) {
		return; // line not in this nametable
	}
	if (xPos >= 256 || xPos + 255 < 0) {
		return; // nametable not visible on screen
	}
	int nametableBaseAddr = 0x2000 + nametableIdx * 0x400;
	int attributeBaseAddr = nametableBaseAddr + 0x3C0;

	// Wrap lineInNametable for vertical scrolling
	int wrappedLine = lineInNametable % 240;
	int tileRowOffset = wrappedLine / 8 * 32;
	int tileYInRow = wrappedLine % 8;

	for (int tileCol = 0; tileCol < 32; tileCol++) {
		int tileXPos = xPos + tileCol * 8;
		if (tileXPos >= 256 || tileXPos + 7 < 0) {
			continue; // tile not visible on screen
		}

		uint8 tileIdx = ppu->readNametable(nametableBaseAddr + tileRowOffset + tileCol);

		// get attribute byte
		int attrRow = (wrappedLine / 32);
		int attrCol = (tileCol / 4);
		uint8 attributeByte = ppu->readNametable(attributeBaseAddr + attrRow * 8 + attrCol);

		// determine which quadrant of the attribute byte to use
		int quadrant = ((wrappedLine % 32) / 16) * 2 + ((tileCol % 4) / 2);
		uint8 paletteIndex = (attributeByte >> (quadrant * 2)) & 0x03;

		// get the full tile data from CHR ROM/RAM
		// each tile is 16 bytes. the first 8 bytes are the low bits of each pixel row,
		// and the next 8 bytes are the high bits of each pixel row.
		// 2 bits per pixel, a bit from each byte, so 2 bytes per row:
		// (08)(08)(08)(08)(08)(08)(08)(08)
		// (19)(19)(19)(19)(19)(19)(19)(19)
		// (2A)(2A)(2A)(2A)(2A)(2A)(2A)(2A)
		// (3B)(3B)(3B)(3B)(3B)(3B)(3B)(3B)
		// etc...

		uint8 highByte = cart->readChr(ppu->CTRLbackgroundPatternTableAddress() | tileIdx * 16 + tileYInRow);
		uint8 lowByte  = cart->readChr(ppu->CTRLbackgroundPatternTableAddress() | tileIdx * 16 + tileYInRow + 8);

		for (int x = 0; x < 8; x++) {
			int bit = 7 - x;
			uint8 bit0 = (highByte >> bit) & 0x01;
			uint8 bit1 = (lowByte >> bit) & 0x01;
			uint8 colorIdx = (bit1 << 1) | bit0;

			if (tileXPos + x < 0 || tileXPos + x >= 256) continue; // pixel out of bounds
			if (colorIdx == 0) {
				// transparent pixel, do nothing
			} else {
				uint8 absPaletteIndex = ppu->palette[paletteIndex * 4 + colorIdx] & 0x3F; // get color index from palette
				lineBuf[tileXPos + x] = defaultARGBpal[absPaletteIndex]; // get ARGB color from palette
			}
		}
	}
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

		// get the full tile data from CHR ROM/RAM
		// each tile is 16 bytes. the first 8 bytes are the low bits of each pixel row,
		// and the next 8 bytes are the high bits of each pixel row.
		// 2 bits per pixel, a bit from each byte, so 2 bytes per row:
		// (08)(08)(08)(08)(08)(08)(08)(08)
		// (19)(19)(19)(19)(19)(19)(19)(19)
		// (2A)(2A)(2A)(2A)(2A)(2A)(2A)(2A)
		// (3B)(3B)(3B)(3B)(3B)(3B)(3B)(3B)
		// etc...

		// we only need one row (2 bytes) at a time
		uint8 highByte, lowByte;

		int row = flipY ? (7 - y) : y;
		highByte = cart->readChr(ppu->CTRLspritePatternTableAddress() | spriteIdx * 16 + row);
		lowByte  = cart->readChr(ppu->CTRLspritePatternTableAddress() | spriteIdx * 16 + row + 8);

		for (int x = 0; x < 8; x++) {
			int bit = flipX ? x : (7 - x);
			uint8 bit0 = (highByte >> bit) & 0x01;
			uint8 bit1 = (lowByte >> bit) & 0x01;
			uint8 colorIdx = (bit1 << 1) | bit0;

			if (spriteX + x < 0 || spriteX + x >= 256) continue; // pixel out of bounds

			if (colorIdx == 0) {
				// transparent pixel, do nothing
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
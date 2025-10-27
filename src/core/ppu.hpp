#pragma once

#include "types.hpp"
#include "ppu_registers.hpp"

class PPU {
private:
	// PPU REGISTERS
	PPUCTRL ctrl;
	PPUMASK mask;
	PPUSTAT stat;
	uint8 oamaddr;
	uint8 oamdata;
	PPUSCRL scrl;
	PPUADDR addr;

	uint8 vram[0x4000]; // PPU VRAM
	uint8 oam[256];     // Object Attribute Memory (OAM)
	uint8 buffer;       // Internal read buffer for PPUDATA reads
	
	int cycle;          // Current PPU cycle
	int dot;            // Current PPU dot (pixel) within the scanline
	int scanline;       // Current PPU scanline
	int frame;          // Current frame count

public:
	PPU();

	void reset();

	// PPU Register Read/Writes

	// PPUCTRL
	uint8 CTRLread();
	void CTRLwrite(uint8 value);
	uint16 CTRLnametableAddress();
	uint8 CTRLvramAddressIncrement();
	uint16 CTRLspritePatternTableAddress();
	uint16 CTRLbackgroundPatternTableAddress();
	uint8 CTRLspriteSize();
	bool CTRLisMaster();
	bool CTRLgenerateNMI();

	// PPUMASK
	uint8 MASKread();
	void MASKwrite(uint8 value);
	bool MASKisGrayscale();
	bool MASKshowBackgroundLeft();
	bool MASKshowSpritesLeft();
	bool MASKshowBackground();
	bool MASKshowSprites();
	ColorEmphasis MASKgetEmphasis();

	// PPUSTAT
	uint8 STATread();
	void STATwrite(uint8 value);
	bool STATisInVBlank();
	bool STATsprite0Hit();
	bool STATspriteOverflow();

	// OAMADDR
	uint8 OAMADDRread();
	void OAMADDRwrite(uint8 value);

	// OAMDATA
	uint8 OAMDATAread();
	void OAMDATAwrite(uint8 value);

	void OAMDMAwrite(uint8* values);

	// PPUSCRL
	void SCRLwrite(uint8 value);
	PPUSCRL SCRLget();

	// PPUADDR
	void ADDRwrite(uint8 value);
	PPUADDR ADDRget();

	// PPU Cycles
	void step(int cycles);
};
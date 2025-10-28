#pragma once

#include "types.hpp"

#include "ppu_registers.hpp"

#include "composite.hpp"


class CPU; // forward declaration



class PPU {
private:
	inline static const uint8 MIRROR_TABLE[3][4] = {
		{0, 0, 1, 1}, // HORIZONTAL
		{0, 1, 0, 1}, // VERTICAL
		{0, 1, 2, 3}, // FOUR-SCREEN
	};

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

	// Write toggle for PPUSCROLL / PPUADDR writes (first/second write).
	// Hardware clears this toggle when PPUSTATUS is read.
	bool write_toggle;

	Cart* cart = nullptr;
	Composite* comp = nullptr;
	CPU* cpu = nullptr;

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
	void ADDRincrement(int inc);

	// Read and Write
	uint8 read();
	void write(uint8 value);

	// PPU Cycles
	bool step(int cycles);

	uint8 useBuffer(uint8 value);

	// Additional helper functions for composite rendering
	uint8 OAMread(int index) { return oam[index]; }
	uint8* getVRAM() { return vram; }

	void connectComposite(Composite* comp);
	void disconnectComposite();
	void connectCPU(CPU* cpu);
	void disconnectCPU();
	void connectCart(Cart* cartRef);
	void disconnectCart();
};
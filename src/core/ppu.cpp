#include "ppu.hpp"

PPU::PPU() {
	reset();
}

void PPU::reset() {
	dot = 0;
	scanline = 0;
	frame = 0;
}

void PPU::step(int cycles) { // placeholder implementation
	for (int i = 0; i < cycles; i++) {
		dot++;
		if (dot > 340) {
			dot -= 341;
			if (comp) {
				comp->renderScanline(scanline);
			}
			scanline++;
			if (scanline > 261) {
				scanline = 0;
				frame++;
			}
		}
	}
}


// PPU Register Read/Writes

// PPUCTRL

uint8 PPU::CTRLread() {
	return ctrl.raw;
}

void PPU::CTRLwrite(uint8 value) {
	ctrl.raw = value;
}

uint16 PPU::CTRLnametableAddress() {
	switch (ctrl.N) {
		case 0: return 0x2000;
		case 1: return 0x2400;
		case 2: return 0x2800;
		case 3: return 0x2C00;
		default: return 0x2000; // Should not happen
	}
}

uint8 PPU::CTRLvramAddressIncrement() {
	return (ctrl.I == 0) ? 1 : 32;
}

uint16 PPU::CTRLspritePatternTableAddress() {
	return (ctrl.S == 0) ? 0x0000 : 0x1000;
}

uint16 PPU::CTRLbackgroundPatternTableAddress() {
	return (ctrl.B == 0) ? 0x0000 : 0x1000;
}

uint8 PPU::CTRLspriteSize() {
	return (ctrl.H == 0) ? 8 : 16;
}

bool PPU::CTRLisMaster() {
	return (ctrl.P == 0);
}

bool PPU::CTRLgenerateNMI() {
	return (ctrl.V != 0);
}

// PPUMASK

uint8 PPU::MASKread() {
	return mask.raw;
}

void PPU::MASKwrite(uint8 value) {
	mask.raw = value;
}

bool PPU::MASKisGrayscale() {
	return (mask.g != 0);
}

bool PPU::MASKshowBackgroundLeft() {
	return (mask.m != 0);
}

bool PPU::MASKshowSpritesLeft() {
	return (mask.M != 0);
}

bool PPU::MASKshowBackground() {
	return (mask.b != 0);
}

bool PPU::MASKshowSprites() {
	return (mask.s != 0);
}

ColorEmphasis PPU::MASKgetEmphasis() {
	ColorEmphasis emp;
	emp.R = mask.R;
	emp.G = mask.G;
	emp.B = mask.B;
	return emp;
}

// PPUSTAT

uint8 PPU::STATread() {
	return stat.raw;
}

void PPU::STATwrite(uint8 value) {
	stat.raw = value;
}





// OAMADDR
uint8 PPU::OAMADDRread() {
	return oamaddr;
}

void PPU::OAMADDRwrite(uint8 value) {
	oamaddr = value;
}

// OAMDATA
uint8 PPU::OAMDATAread() {
	return oam[oamaddr];
}

void PPU::OAMDATAwrite(uint8 value) {
	oam[oamaddr] = value;
}

void PPU::OAMDMAwrite(uint8* values) {
	for (int i = 0; i < 256; i++) {
		oam[oamaddr] = values[i]; // In a real implementation, value would come from CPU memory
		oamaddr++;
	}
}




void PPU::connectComposite(Composite* compRef) {
	comp = compRef;
}

void PPU::disconnectComposite() {
	comp = nullptr;
}
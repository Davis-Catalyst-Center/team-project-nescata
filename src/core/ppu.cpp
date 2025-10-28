#include "ppu.hpp"

PPU::PPU() {
	reset();
}

void PPU::reset() {
	// Initialize PPU state and memory to deterministic values
	cycle = 0;
	dot = 0;
	scanline = 0;
	frame = 0;
	buffer = 0;
	oamaddr = 0;
	oamdata = 0;
	ctrl.raw = 0;
	mask.raw = 0;
	stat.raw = 0;
	write_toggle = true; // next write is the first write

	// Clear VRAM and OAM
	for (int i = 0; i < 0x4000; ++i) vram[i] = 0;
	for (int i = 0; i < 256; ++i) oam[i] = 0;
}

bool PPU::step(int cycles) {
	
	return false;
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
	uint8 value = stat.raw;
	// Reading PPUSTATUS clears VBlank (bit V) and resets the write toggle (w)
	stat.V = 0;
	write_toggle = true;
	return value;
}

void PPU::STATwrite(uint8 value) {
    // PPUSTAT writes are ignored
}

bool PPU::STATisInVBlank() {
    return stat.V != 0;
}

bool PPU::STATsprite0Hit() {
    return stat.S != 0;
}

bool PPU::STATspriteOverflow() {
    return stat.O != 0;
}

// PPUSCRL
void PPU::SCRLwrite(uint8 value) {
	// Use member write_toggle instead of a static local so PPUSTATUS reads
	// can reset this toggle as hardware requires.
	if (write_toggle) {
		scrl.x = value;
	} else {
		scrl.y = value;
	}
	write_toggle = !write_toggle;
}

PPUSCRL PPU::SCRLget() {
    return scrl;
}

// PPUADDR
void PPU::ADDRwrite(uint8 value) {
	// Use member write_toggle. First write sets high 6 bits, second write sets low 8 bits.
	if (write_toggle) {
		addr.high = value & 0x3F;  // Only 6 bits are used
	} else {
		addr.low = value;
	}
	write_toggle = !write_toggle;
}

PPUADDR PPU::ADDRget() {
    return addr;
}

void PPU::ADDRincrement(int inc) {
	addr.addr += inc;
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
	// Write 256 bytes into OAM starting at current OAMADDR and wrap around (uint8)
	uint8 addr = oamaddr;
	for (int i = 0; i < 256; i++) {
		oam[addr] = values[i]; // In a real implementation, value would come from CPU memory
		addr++;
	}
	oamaddr = addr; // store updated address (wraps naturally via uint8)
}

uint8 PPU::read() {
	ADDRincrement(CTRLvramAddressIncrement());
	switch (addr.addr) {
		case 0x0000 ... 0x1FFF:
			return useBuffer(cart ? cart->readChr(addr.addr) : 0);
		case 0x2000 ... 0x2FFF:
			;

	}
}

void PPU::write(uint8 value) {

	ADDRincrement(CTRLvramAddressIncrement());
}



uint8 PPU::useBuffer(uint8 value) {
	uint8 tmp = buffer;
	buffer = value;
	return tmp;
}

void PPU::connectComposite(Composite* compRef) {
	comp = compRef;
}

void PPU::disconnectComposite() {
	comp = nullptr;
}

void PPU::connectCPU(CPU* cpuRef) {
    cpu = cpuRef;
}

void PPU::disconnectCPU() {
    cpu = nullptr;
}

void PPU::connectCart(Cart* cartRef) {
	cart = cartRef;
}

void PPU::disconnectCart() {
	cart = nullptr;
}
#include "bus.hpp"


Bus::Bus() {
	// Initialize memory
	std::fill(std::begin(memory), std::end(memory), 0);
	cart = nullptr; // No cartridge loaded
}

uint8 Bus::read(uint16 addr) {
	switch (addr) {
		case 0x0000 ... 0x1FFF: // 2KB RAM
			// mirror the 2KB RAM every 0x800 bytes
			return memory[addr & 0x7ff];
		case 0x2002:
			return ppu->STATread();
		case 0x2004:
			return ppu->OAMDATAread();
		case 0x2007:
			return ppu->read();
		case 0x2016 ... 0x3FFF:
			return 0;
		case 0x4000 ... 0x4015: // APU and I/O registers
			// Placeholder for APU and I/O register read
			return 0;
		case 0x4016:
			if (controller1) return controller1->read();
			else return 0;
		case 0x4017:
			if (controller2) return controller2->read();
			else return 0;
		case 0x4018 ... 0x401F: // APU and I/O functionality that is normally disabled
			// Placeholder for disabled APU and I/O read
			return 0;
		case 0x4020 ... 0xFFFF: // Cartridge space (PRG ROM, PRG RAM, and mapper registers)
			if (cart && !cart->blank) return cart->read(addr); // Delegate to cartridge
			else return 0;
		default:
			return 0;
	}
}

void Bus::write(uint16 addr, uint8 val) {
	switch (addr) {
		case 0x0000 ... 0x1fff: // 2KB RAM
			memory[addr & 0x7ff] = val;
			break;
		case 0x2000 ... 0x3fff: // PPU registers
			// PPU write logic here
			break;
		case 0x4000 ... 0x4015: // APU/IO registers
			// APU/IO write logic here
			break;
		case 0x4016:
			if (controller1) controller1->write(val);
			break;
		case 0x4017:
			if (controller2) controller2->write(val);
			break;
		case 0x4020 ... 0xffff:
			if (cart && !cart->blank) cart->write(addr, val); // Delegate to cartridge
			break;
		default:
			// unmapped, do nothing
			break;
	}
}


bool Bus::clock(int cycles) {
	// cpu sends in cycles passed * 12 to get master clock cycles
	if (apu) {
		apu->step(cycles / 12); // APU runs at CPU clock rate
	}
	// do ppu last to pass nmi
	if (ppu) {
		return ppu->step(cycles / 3); // PPU runs at 3x CPU clock rate
	}
	// if there's no ppu, just return false
	return false;
}


void Bus::connectAPU(APU* apuRef) {
	apu = apuRef;
}

void Bus::disconnectAPU() {
	apu = nullptr;
}

void Bus::connectPPU(PPU* ppuRef) {
	ppu = ppuRef;
}

void Bus::disconnectPPU() {
	ppu = nullptr;
}

void Bus::connectCart(Cart* cartRef) {
	cart = cartRef;
}

void Bus::disconnectCart() {
	cart = nullptr;
}

void Bus::connectController1(Controller* controller1Ref) {
	controller1 = controller1Ref;
}

void Bus::disconnectController1() {
	controller1 = nullptr;
}

void Bus::connectController2(Controller* controller2Ref) {
	controller2 = controller2Ref;
}

void Bus::disconnectController2() {
	controller2 = nullptr;
}

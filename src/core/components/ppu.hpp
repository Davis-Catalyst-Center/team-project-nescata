#pragma once

#include "types.hpp"
#include "ppu_registers.hpp"

class PPU {
private:
	// PPU REGISTERS
	PPUCTRL ctrl;
	PPUMASK mask;
	PPUSTATUS status;
	OAMADDR oamaddr;
	OAMDATA oamdata;
	PPUSCROLL scroll;
	PPUADDR addr;
};
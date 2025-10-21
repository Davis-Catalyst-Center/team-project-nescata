#pragma once

#include "types.hpp"


union PPUCTRL {
	struct {
		uint8 V : 1; // NMI enable
		uint8 P : 1; // PPU master/slave select
		uint8 H : 1; // Sprite height
		uint8 B : 1; // Background tile select
		uint8 S : 1; // Sprite tile select
		uint8 I : 1; // VRAM address increment per CPU read/write of PPU data
		uint8 N : 2; // Base nametable address
	};
	uint8 raw;
};

union PPUMASK {
	struct {
		uint8 B : 1; // Emphasize blue
		uint8 G : 1; // Emphasize green
		uint8 R : 1; // Emphasize red
		uint8 s : 1; // Show sprites
		uint8 b : 1; // Show background
		uint8 M : 1; // Show sprites in leftmost 8 pixels of screen
		uint8 m : 1; // Show background in leftmost 8 pixels of screen
		uint8 g : 1; // Greyscale
	};
	uint8 raw;
};

union PPUSTATUS {
	struct {
		uint8 V : 1; // Vertical blank has started
		uint8 S : 1; // Sprite 0 Hit
		uint8 O : 1; // Sprite overflow
		uint8 unused : 5;  // Unused, always 0
	};
	uint8 raw;
};

struct OAMADDR {
	uint8 addr;
};

struct OAMDATA {
	uint8 data;
};

struct PPUSCROLL {
	uint8 x : 8; // X scroll
	uint8 y : 8; // Y scroll
};

union PPUADDR {
	struct {
		uint8 unused : 2; // Unused to line up to the byte
		uint8 high : 6; // High 6 bits of address
		uint8 low : 8;  // Low 8 bits of address
	};
	struct {
		uint8 unused2 : 2; // Unused to line up to the byte
		uint16 addr : 14; // Full 14-bit address
	};	
};

struct PPUDATA {
	uint8 data;
};

struct OAMDMA {
	uint8 data;
};

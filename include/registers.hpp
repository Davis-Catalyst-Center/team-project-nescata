#pragma once

#include <cstdint>

union PPUCTRL {
	struct {
		uint8_t U : 2; // Base nametable address / unused because that goes to t
		uint8_t I : 1; // VRAM address increment per CPU read/write of PPU data
		uint8_t S : 1; // Sprite tile select
		uint8_t B : 1; // Background tile select
		uint8_t H : 1; // Sprite height
		uint8_t P : 1; // PPU master/slave select
		uint8_t V : 1; // NMI enable
	};
	uint8_t raw;
};

union PPUMASK {
	struct {
		uint8_t g : 1; // Grayscale
		uint8_t m : 1; // Show background in leftmost 8 pixels of screen
		uint8_t M : 1; // Show sprites in leftmost 8 pixels of screen
		uint8_t b : 1; // Show background
		uint8_t s : 1; // Show sprites
		uint8_t R : 1; // Emphasize red
		uint8_t G : 1; // Emphasize green
		uint8_t B : 1; // Emphasize blue
	};
	uint8_t raw;
};

struct ColorEmphasis {
	uint8_t R : 1; // Emphasize red
	uint8_t G : 1; // Emphasize green
	uint8_t B : 1; // Emphasize blue
};

union PPUSTAT {
	struct {
		uint8_t unused : 5;  // Unused, always 0
		uint8_t O : 1; // Sprite overflow
		uint8_t S : 1; // Sprite 0 Hit
		uint8_t V : 1; // Vertical blank has started
	};
	uint8_t raw;
};

union LoopyRegister {
	struct {
		uint8_t addressLow : 8;
		uint8_t addressTop : 7;
		uint8_t unused2 : 1; // padding
	};
	struct {
		uint16_t address : 15;
		uint8_t unused3 : 1; // padding
	};
    struct {
        uint8_t coarseX : 5;
        uint8_t coarseY : 5;
		union {
			struct {
				uint8_t nametableX : 1;
				uint8_t nametableY : 1;
			};
			uint8_t nametable : 2;
		};
        uint8_t fineY : 3;
        uint8_t unused : 1; // padding
    };
    uint16_t raw;
};

union OAM {
	struct {
		uint8_t y : 8;
		uint8_t tileIdx : 8;
		uint8_t attr : 8;
		uint8_t x : 8;
	} sprites[64];
	
	uint8_t raw[256];
};
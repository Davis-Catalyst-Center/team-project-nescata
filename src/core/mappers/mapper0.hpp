#pragma once
#include "../types.hpp"
#include "mapper.hpp"

class Mapper0 : public Mapper {
public:
	Mapper0(std::vector<std::array<uint8, 0x4000>> rom_banks,
			std::vector<std::array<uint8, 0x2000>> chr_banks) {
		prgBankCount = rom_banks.size();
		chrBankCount = chr_banks.size();
		mapperID = 0;
		if (prgBankCount == 1) {
			prgBanks.push_back(rom_banks[0]);
			prgBanks.push_back(rom_banks[0]);
		} else if (prgBankCount == 2) {
			prgBanks.push_back(rom_banks[0]);
			prgBanks.push_back(rom_banks[1]);
		}
		if (chrBankCount == 1) {
			chrBanks.push_back(chr_banks[0]);
		} else {
			chrBanks.push_back(std::array<uint8, 0x2000>{});
		}
	}

	uint8 readRom(uint16 addr) override {
		if (addr >= 0x8000 && addr <= 0xBFFF) {
			return prgBanks[0][addr - 0x8000];
		} else if (addr >= 0xC000 && addr <= 0xFFFF) {
			return prgBanks[1][addr - 0xC000];
		} else {
			return 0; // Should not happen
		}
	}

	void writeRom(uint16 addr, uint8 value) override {
		// Mapper 0 has no bank switching, so writes are ignored.
	}

	uint8 readChr(uint16 addr) override {
		if (addr < 0x2000) {
			return chrBanks[0][addr];
		} else {
			return 0; // Should not happen
		}
	}

	void writeChr(uint16 addr, uint8 value) override {
		// Mapper 0 can have CHR-RAM, but this implementation assumes CHR-ROM.
		// For CHR-ROM, writes are ignored.
	}
};
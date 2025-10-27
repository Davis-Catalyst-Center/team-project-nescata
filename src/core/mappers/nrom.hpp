#pragma once
#include "../types.hpp"
#include "mapper.hpp"

class NROM : public Mapper {
public:
	NROM(
		std::vector<std::array<uint8, 0x4000>>* prgBanksRef,
		std::vector<std::array<uint8, 0x2000>>* chrBanksRef) {
		prgBankCount = prgBanksRef->size();
		chrBankCount = chrBanksRef->size();
		mapperID = 0;
		prgBanks.push_back((*prgBanksRef)[0]);
		if (prgBankCount == 1) {
			prgBanks.push_back((*prgBanksRef)[0]);
		} else if (prgBankCount == 2) {
			prgBanks.push_back((*prgBanksRef)[1]);
		}
		if (chrBankCount == 1) {
			chrBanks.push_back((*chrBanksRef)[0]);
		} else {
			chrBanks.push_back(std::array<uint8, 0x2000>{});
		}
	}

	uint8 read(uint16 addr) override {
		if (addr >= 0x8000 && addr <= 0xBFFF) {
			return prgBanks[0][addr - 0x8000];
		} else if (addr >= 0xC000 && addr <= 0xFFFF) {
			return prgBanks[1][addr - 0xC000];
		} else {
			return 0; // Should not happen
		}
	}

	void write(uint16 addr, uint8 value) override {
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
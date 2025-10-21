// class Mapper0():
// 	def __init__(self, rom_banks, chr_banks):
// 		self.rom_banks = rom_banks
// 		self.chr_banks = chr_banks
// 		self.rom_bank_count = len(rom_banks)
// 		self.chr_bank_count = len(chr_banks)
// 		assert 0 <= self.rom_bank_count <= 2
// 		assert 0 <= self.chr_bank_count <= 1

// 		if self.rom_bank_count == 1:
// 			self.rom = rom_banks[0] + rom_banks[0]
// 		else:
// 			self.rom = rom_banks[0] + rom_banks[1]
		
// 		if self.chr_bank_count == 1:
// 			self.chr = chr_banks[0]
// 		else:
// 			self.chr = bytes([0 for _ in range(0x2000)])

// 	def read_rom(self, addr):
// 		return self.rom[addr - 0x8000]

// 	def write_rom(self, addr, value):
// 		pass

// 	def read_chr(self, addr):
// 		return self.chr[addr]

// 	def write_chr(self, addr, value):
// 		# Mapper 0 can have CHR-RAM, but this implementation assumes CHR-ROM.
// 		# For CHR-ROM, writes are ignored.
// 		pass

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
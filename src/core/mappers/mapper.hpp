#pragma once

#include <vector>
#include <array>

#include "../types.hpp"

class Mapper {
protected:
	int mapperID;
	int prgBankCount;
	int chrBankCount;

	std::vector<std::array<uint8, 0x4000>> prgBanks;
	std::vector<std::array<uint8, 0x2000>> chrBanks;

public:
	virtual uint8 readRom(uint16 addr) = 0;
	virtual void writeRom(uint16 addr, uint8 value) = 0;
	virtual uint8 readChr(uint16 addr) = 0;
	virtual void writeChr(uint16 addr, uint8 value) = 0;
	virtual ~Mapper() = default;

	void loadRomBanks(
		const std::vector<std::array<uint8, 0x4000>>& prgBanks,
		const std::vector<std::array<uint8, 0x2000>>& chrBanks) {
		this->prgBanks = prgBanks;
		this->chrBanks = chrBanks;
		prgBankCount = prgBanks.size();
		chrBankCount = chrBanks.size();
	}
};
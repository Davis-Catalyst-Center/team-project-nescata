#pragma once

#include <vector>
#include <array>

#include <cstdint>

class Mapper {
protected:
	int mapperID;
	int prgBankCount;
	int chrBankCount;

	std::vector<std::array<uint8_t, 0x4000>> prgBanks;
	std::vector<std::array<uint8_t, 0x2000>> chrBanks;

public:
	bool writeProtect;

	virtual uint8_t read(uint16_t addr) = 0;
	virtual void write(uint16_t addr, uint8_t value) = 0;
	virtual uint8_t readChr(uint16_t addr) = 0;
	virtual void writeChr(uint16_t addr, uint8_t value) = 0;
	virtual void reset() = 0;
	virtual ~Mapper() = default;

	void loadRomBanks(
		const std::vector<std::array<uint8_t, 0x4000>>& prgBanks,
		const std::vector<std::array<uint8_t, 0x2000>>& chrBanks) {
		this->prgBanks = prgBanks;
		this->chrBanks = chrBanks;
		prgBankCount = prgBanks.size();
		chrBankCount = chrBanks.size();
	}
};
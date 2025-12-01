#pragma once

#include <vector>
#include <array>
#include <fstream>
#include <string>

#include "mappers/mapper.hpp"
#include "types.hpp"

enum MirroringType {
	HORIZONTAL = 0,
	VERTICAL = 1,
	FOUR_SCREEN = 2
};

class Cart {
public:
	bool blank = true; // is the cart empty?

	enum {
		LOAD_SUCCESS,
		LOAD_EMPTY,
		LOAD_FILE_NOT_FOUND,
		LOAD_INVALID_FORMAT,
		LOAD_UNSUPPORTED_MAPPER
	} loadStatus = LOAD_EMPTY;

	Mapper* mapper = nullptr;

	uint8 header[16];
	int romBankCount = 0;
	int romSize = 0;
	int chrBankCount = 0;
	int chrSize = 0;

	int mapperID = 0;

	bool fourScreen = false;
	bool hasTrainer = false;
	bool batteryBacked = false;
	bool verticalMirroring = false;

	MirroringType mirroring = HORIZONTAL;

	int iNESVersion = 1;

	int trainerSize = 0;
	
	Cart();
	Cart(std::string filename);

	uint8 read(uint16 addr);
	void write(uint16 addr, uint8 val);
	uint8 readChr(uint16 addr);
	void writeChr(uint16 addr, uint8 val);

private:
	void pickMapper(
		int mapperID,
		bool batteryBacked,
		std::vector<std::array<uint8, 0x4000>>* prgBanks,
		std::vector<std::array<uint8, 0x2000>>* chrBanks
	);
};
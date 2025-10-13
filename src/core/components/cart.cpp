#include <fstream>
#include <iostream>
#include "cart.hpp"

#include "mappers/mapper0.hpp"


Cart::Cart() {
	blank = true;
	mapper = nullptr;
}

Cart::Cart(std::string filename) {
	blank = true; // Default to blank until successfully loaded
	mapper = nullptr;

    std::ifstream gameFile(filename, std::ios::binary);
	if (!gameFile.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}

	// Read the 16-byte header
    gameFile.read(reinterpret_cast<char*>(header), 16);

    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        std::cerr << "Not a valid iNES file!" << std::endl;
        return;
    }

	// Get ROM sizes and mapper ID from the header
	int prgBanksCount = header[4];
	int chrBanksCount = header[5];

	int mapperID = (header[6] >> 4) | (header[7] & 0xF0);

	// Skip trainer data if present (512 bytes)
	if (header[6] & 0x04) {
		gameFile.seekg(512, std::ios::cur);
	}

	// Load PRG ROM banks (16KB each)
	std::vector<std::array<uint8, 0x4000>> prgData;
	prgData.resize(prgBanksCount);
	for (int i = 0; i < prgBanksCount; i++) {
		std::array<uint8, 0x4000> bank;
		gameFile.read(reinterpret_cast<char*>(bank.data()), 0x4000);
		prgData[i] = bank;
	}

	// Load CHR ROM banks (8KB each)
	std::vector<std::array<uint8, 0x2000>> chrData;
	chrData.resize(chrBanksCount);
	for (int i = 0; i < chrBanksCount; i++) {
		std::array<uint8, 0x2000> bank;
		gameFile.read(reinterpret_cast<char*>(bank.data()), 0x2000);
		chrData[i] = bank;
	}

	gameFile.close();

	pickMapper(
		mapperID,
		prgData,
		chrData
	);

	blank = false;
}

Cart::~Cart() {
    delete mapper;
	mapper = nullptr;
}

uint8 Cart::read(uint16 addr) {
    if (mapper) {
		if (addr >= 0x8000 && addr <= 0xFFFF) {
			return mapper->readRom(addr);
		}
		// Add other address ranges (like PRG RAM at $6000-$7FFF) if the mapper supports them
	}
	return 0;
}

void Cart::write(uint16 addr, uint8 val) {
    if (mapper) {
		if (addr >= 0x8000 && addr <= 0xFFFF) {
			mapper->writeRom(addr, val);
		}
		// Add other address ranges if the mapper supports them
	}
}

void Cart::pickMapper(int mapperID,
    const std::vector<std::array<uint8, 0x4000>>& prgBanks,
    const std::vector<std::array<uint8, 0x2000>>& chrBanks) {
    switch (mapperID) {
        case 0:
            mapper = new Mapper0(prgBanks, chrBanks);
            break;
        default:
            std::cerr << "Mapper " << static_cast<int>(mapperID) << " not supported!" << std::endl;
            blank = true;
            break;
    }
}
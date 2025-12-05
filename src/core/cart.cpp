#include "cart.hpp"

Cart::Cart() {

}

Cart::Cart(std::string fName) {
	blank = true; // Default to blank until successfully loaded
	mapper = nullptr;

	filename = fName;

	if (filename.empty()) {
		loadStatus = LOAD_EMPTY;
		return; // silently return if no filename provided
	}

	// Load ROM file
	FILE* romFile = fopen(filename.c_str(), "rb");
	if (!romFile) {
		loadStatus = LOAD_FILE_NOT_FOUND;
		return; // silently return if file cannot be opened
	}

	// Read header
	fread(header, sizeof(uint8), 16, romFile);

	// Verify NES file format
	if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
		fclose(romFile);
		loadStatus = LOAD_INVALID_FORMAT;
		return; // silently return if invalid NES file format
	}

	// Parse header information
	romBankCount = header[4];
	romSize = romBankCount * 0x4000; // 16KB units
	chrBankCount = header[5];
	chrSize = chrBankCount * 0x2000; // 8KB units

	uint8 control1 = header[6];
	uint8 control2 = header[7];

	mapperID = (control1 >> 4) | (control2 & 0xF0);

	fourScreen        = (control1 & 0b00001000) != 0;
	hasTrainer        = (control1 & 0b00000100) != 0;
	batteryBacked     = (control1 & 0b00000010) != 0;
	verticalMirroring = (control1 & 0b00000001) != 0;

	if (fourScreen) {
		mirroring = FOUR_SCREEN;
	} else if (verticalMirroring) {
		mirroring = VERTICAL;
	} else {
		mirroring = HORIZONTAL;
	}

	iNESVersion = (control2 >> 2) & 0b00000011;
	trainerSize = hasTrainer ? 512 : 0;

	// Skip trainer if present
	if (hasTrainer) {
		fseek(romFile, 512, SEEK_CUR);
	}

	// Load PRG ROM banks
	std::vector<std::array<uint8, 0x4000>> prgData;
	for (int i = 0; i < romBankCount; i++) {
		std::array<uint8, 0x4000> bank{};
		fread(bank.data(), sizeof(uint8), 0x4000, romFile);
		prgData.push_back(bank);
	}
	// Load CHR ROM banks
	std::vector<std::array<uint8, 0x2000>> chrData;
	if (chrBankCount == 0) {
		// Some carts have CHR RAM instead of ROM; allocate 8KB of CHR RAM
		chrData.push_back(std::array<uint8, 0x2000>{});
	} else {
		for (int i = 0; i < chrBankCount; i++) {
			std::array<uint8, 0x2000> bank{};
			fread(bank.data(), sizeof(uint8), 0x2000, romFile);
			chrData.push_back(bank);
		}
	}

	fclose(romFile);

	pickMapper(
		mapperID,
		batteryBacked,
		&prgData,
		&chrData
	);

	if (mapper) {
		loadStatus = LOAD_SUCCESS;
		blank = false;
	} else {
		loadStatus = LOAD_UNSUPPORTED_MAPPER;
	}
}

uint8 Cart::read(uint16 addr) {
	if (mapper && !blank) {
		return mapper->read(addr);
		// leave handling up to mapper
	}
	return 0;
}

void Cart::write(uint16 addr, uint8 val) {
	if (mapper && !blank) {
		mapper->write(addr, val);
		// leave handling up to mapper
	}
}

uint8 Cart::readChr(uint16 addr) {
	if (mapper && !blank) {
		return mapper->readChr(addr);
		// leave handling up to mapper
	}
	return 0;
}

void Cart::writeChr(uint16 addr, uint8 val) {
	if (mapper && !blank) {
		mapper->writeChr(addr, val);
		// leave handling up to mapper
	}
}

void Cart::pickMapper(
	int mapperID,
	bool batteryBacked,
	std::vector<std::array<uint8, 0x4000>>* prgBanks,
	std::vector<std::array<uint8, 0x2000>>* chrBanks
) {
	switch (mapperID) {
		case 0:
			mapper = new NROM(prgBanks, chrBanks, batteryBacked);
			break;
		default:
			loadStatus = LOAD_UNSUPPORTED_MAPPER;
			blank = true;
			break;
	}
}
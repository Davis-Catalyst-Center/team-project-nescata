#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <string>

#include "mappers/mapper.hpp"
#include "types.hpp"

class Cart {
public:
	bool blank; // is the cart empty?

	Mapper* mapper;

    uint8 header[16];
    Cart();
    Cart(std::string filename);
    ~Cart();

    uint8 read(uint16 addr);
    void write(uint16 addr, uint8 val);

private:
	void pickMapper(int mapperID,
		const std::vector<std::array<uint8, 0x4000>>& prgBanks,
		const std::vector<std::array<uint8, 0x2000>>& chrBanks);
};
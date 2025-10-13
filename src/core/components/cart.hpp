#pragma once

#include <vector>
#include <array>
#include <fstream>
#include <string>
#include "types.hpp"

class Cart {
public:
	bool blank; // is the cart empty?

    // vector of 0x4000 byte rom banks
    std::vector<std::array<uint8, 0x4000>> romBanks;

    uint8 header[16];
	Cart();
    Cart(std::string filename);
    ~Cart();

    uint8 read(uint16 addr);
    void write(uint16 addr, uint8 val);
};
#pragma once

#include <vector>

#include "core/components/types.h"

class Cart {
public:
    // vector of 0x4000 byte rom banks
    std::vector<std::array<uint8, 0x4000>> romBanks;

    Cart(const char* filename);
    ~Cart();
};
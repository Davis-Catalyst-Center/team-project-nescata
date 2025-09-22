#pragma once

#include <vector>

#include "core/types.h"

class Cart {
public:
    // vector of 0x4000 byte rom banks
    std::vector<> romBanks;

    Cart(const char* filename);
    ~Cart();
};
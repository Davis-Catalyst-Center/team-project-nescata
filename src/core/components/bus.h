#pragma once

#include "types.h"

class Bus {
public:
    uint8 memory [0x10000]; // temporary 64KB memory for initial testing

    uint8 read(uint16 addr);
    void write(uint16 addr, uint8 val);
};

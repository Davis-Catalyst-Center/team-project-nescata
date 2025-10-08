#pragma once

#include "types.hpp"
#include "cart.hpp"


class Bus {
public:
    uint8 memory [0x800]; // 2KB internal +memory

    Cart* cart;

    Bus();

    uint8 read(uint16 addr);
    void write(uint16 addr, uint8 val);

    void loadCart(Cart* cart);
    void unloadCart();
};

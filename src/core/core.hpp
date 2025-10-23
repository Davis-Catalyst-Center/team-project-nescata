#pragma once

#include "cpu.hpp"
#include "bus.hpp"


/*
the core initializes and manages all the components
*/


class Core {
public:
    CPU cpu;
    Bus bus;


    Core();

    void run();
    void loadCart(Cart* cart);
    void unloadCart();
};
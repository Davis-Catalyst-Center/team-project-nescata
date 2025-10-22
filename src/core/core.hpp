#pragma once

#include "components/cpu.hpp"
#include "components/bus.hpp"


/*
the core initializes and manages all the components
*/


class Core {
public:
    Bus bus;
    CPU cpu;

    Core();

    void run();
    void loadCart(Cart* cart);
    void unloadCart();
};
#pragma once

#include "components/cpu.h"
#include "components/bus.h"


/*
the core initializes and manages all the components
*/


class Core {
public:
    CPU cpu;
    Bus bus;

    Core();

    void run();
};
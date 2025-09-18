#include "core.h"

Core::Core() {
    cpu = CPU();
    bus = cpu.bus;
}

void Core::run() {
    // Main emulation loop
    while (true) {
        cpu.clock();
    }
}
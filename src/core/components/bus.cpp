#include "bus.h"

uint8 Bus::read(uint16 addr) {
    return memory[addr];
}

void Bus::write(uint16 addr, uint8 val) {
    memory[addr] = val;
}
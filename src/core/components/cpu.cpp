
#include "cpu.h"
#include "bus.h"



// CPU IMPLEMENTATION

CPU::CPU() {
    bus = Bus();
    reset();
}

void CPU::reset() {
    a = 0;
    x = 0;
    y = 0;
    pc = readMem(RESET_VECTOR) | (readMem(RESET_VECTOR + 1) << 8);
    s = 0xFD;    // Stack pointer powerup state
    p.raw = 0x24; // Typical initial status register value
}

void CPU::clock() {
    // Implement CPU clock cycle logic here
}

uint8 CPU::readMem(uint16 addr) {
    return bus.read(addr);
}

void CPU::writeMem(uint16 addr, uint8 val) {
    bus.write(addr, val);
}

uint16 CPU::readMem16(uint16 addr) {
    uint8 low = readMem(addr);
    uint8 high = readMem(addr + 1);
    return (high << 8) | low;
}

void CPU::writeMem16(uint16 addr, uint16 val) {
    writeMem(addr, val & 0xff);
    writeMem(addr + 1, (val >> 8) & 0xff);
}

uint8 CPU::pull() {
    s++;
    return readMem(STACK_BASE + s);
}

void CPU::push(uint8 val) {
    writeMem(STACK_BASE + s, val);
    s--;
}

uint16 CPU::pull16() {
    uint8 low = pull();
    uint8 high = pull();
    return (high << 8) | low;
}

void CPU::push16(uint16 val) {
    push((val >> 8) & 0xff);
    push(val & 0xff);
}
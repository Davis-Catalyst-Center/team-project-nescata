#pragma once

#include "types.h"

class StatusRegister {
public:
    bool n: 1; // Negative
    bool v: 1; // Overflow
    bool u: 1; // Unused
    bool b: 1; // Break
    bool d: 1; // Decimal
    bool i: 1; // Interrupt Disable
    bool z: 1; // Zero
    bool c: 1; // Carry

    uint8 get();
    void set(uint8 val);
};



class CPU {
public:
    // CONSTANTS
    static const uint16 STACK_BASE = 0x0100;
    static const uint16 NMI_VECTOR = 0xFFFA;
    static const uint16 RESET_VECTOR = 0xFFFC;
    static const uint16 IRQ_VECTOR = 0xFFFE;


    // REGISTERS
    int8 a;           // Accumulator
    int8 x;           // X Register
    int8 y;           // Y Register
    uint16 pc;        // Program Counter
    uint8 s;          // Stack Pointer
    StatusRegister p; // Status Register

    Bus bus;

    CPU();
    void reset();
    void clock();

    // MEMORY INTERFACING

    uint8 readMem(uint16 addr);
    void writeMem(uint16 addr, uint8 val);
    uint16 readMem16(uint16 addr);
    void writeMem16(uint16 addr, uint16 val);

    uint8 pull();
    void push(uint8 val);
    uint16 pull16();
    void push16(uint16 val);
};
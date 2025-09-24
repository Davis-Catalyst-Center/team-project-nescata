
#pragma once
#include "types.h"

#include "bus.h"



union StatusRegister {
    struct {
        uint8 n: 1; // Negative
        uint8 v: 1; // Overflow
        uint8 u: 1; // Unused
        uint8 b: 1; // Break
        uint8 d: 1; // Decimal
        uint8 i: 1; // Interrupt Disable
        uint8 z: 1; // Zero
        uint8 c: 1; // Carry
    };
    uint8 raw;
};



class CPU {
public:
    // CONSTANTS

    // static const uint8 CYCLE_MAP[256] = {
        
    // };

    static const uint16 STACK_BASE = 0x0100;
    static const uint16 NMI_VECTOR = 0xFFFA;
    static const uint16 RESET_VECTOR = 0xFFFC;
    static const uint16 IRQ_VECTOR = 0xFFFE;

    // ENUMS
    enum AddressingMode {
        IMPLIED,
        ACCUMULATOR,
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        RELATIVE,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT,
        INDIRECT_X,
        INDIRECT_Y,
    };


    // REGISTERS
    int8 a;           // Accumulator
    int8 x;           // X Register
    int8 y;           // Y Register
    uint16 pc;        // Program Counter
    uint8 s;          // Stack Pointer
    StatusRegister p; // Status Register

    Bus bus;

    // STATE
    long int cycles;


    CPU();
    void reset();
    void run();
    void clock();
    void runInstruction(uint8 opcode);


    // MEMORY INTERFACING

    uint8 readMem(uint16 addr);
    void writeMem(uint16 addr, uint8 val);
    uint16 readMem16(uint16 addr);
    void writeMem16(uint16 addr, uint16 val);
    uint16 readMem16Wrap(uint16 addr);

    uint8 pull();
    void push(uint8 val);
    uint16 pull16();
    void push16(uint16 val);

    uint16 getOperandAddress(AddressingMode mode);


private:

    // CPU INSTRUCTIONS



    void op_NOP(AddressingMode mode);
    void op_ORA(AddressingMode mode);

};

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

    // helpers

    

    // CPU INSTRUCTIONS

    // OFFICIAL OPCODES

    void op_ADC(AddressingMode mode);
    void op_AND(AddressingMode mode);
    void op_ASL(AddressingMode mode);
    void op_BCC(AddressingMode mode);
    void op_BCS(AddressingMode mode);
    void op_BEQ(AddressingMode mode);
    void op_BIT(AddressingMode mode);
    void op_BMI(AddressingMode mode);
    void op_BNE(AddressingMode mode);
    void op_BPL(AddressingMode mode);
    void op_BRK(AddressingMode mode);
    void op_BVC(AddressingMode mode);
    void op_BVS(AddressingMode mode);
    void op_CLC(AddressingMode mode);
    void op_CLD(AddressingMode mode);
    void op_CLI(AddressingMode mode);
    void op_CLV(AddressingMode mode);
    void op_CMP(AddressingMode mode);
    void op_CPX(AddressingMode mode);
    void op_CPY(AddressingMode mode);
    void op_DEC(AddressingMode mode);
    void op_DEX(AddressingMode mode);
    void op_DEY(AddressingMode mode);
    void op_EOR(AddressingMode mode);
    void op_INC(AddressingMode mode);
    void op_INX(AddressingMode mode);
    void op_INY(AddressingMode mode);
    void op_JMP(AddressingMode mode);
    void op_JSR(AddressingMode mode);
    void op_LDA(AddressingMode mode);
    void op_LDX(AddressingMode mode);
    void op_LDY(AddressingMode mode);
    void op_LSR(AddressingMode mode);
    void op_NOP(AddressingMode mode);
    void op_ORA(AddressingMode mode);
    void op_PHA(AddressingMode mode);
    void op_PHP(AddressingMode mode);
    void op_PLA(AddressingMode mode);
    void op_PLP(AddressingMode mode);
    void op_ROL(AddressingMode mode);
    void op_ROR(AddressingMode mode);
    void op_RTI(AddressingMode mode);
    void op_RTS(AddressingMode mode);
    void op_SBC(AddressingMode mode);
    void op_SEC(AddressingMode mode);
    void op_SED(AddressingMode mode);
    void op_SEI(AddressingMode mode);
    void op_STA(AddressingMode mode);
    void op_STX(AddressingMode mode);
    void op_STY(AddressingMode mode);
    void op_TAX(AddressingMode mode);
    void op_TAY(AddressingMode mode);
    void op_TSX(AddressingMode mode);
    void op_TXA(AddressingMode mode);
    void op_TXS(AddressingMode mode);
    void op_TYA(AddressingMode mode);

    // UNOFFICIAL OPCODES

};
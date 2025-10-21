#pragma once

#include <iostream>

#include "types.hpp"
#include "bus.hpp"



union StatusRegister {
	struct {
		uint8 C: 1; // Carry
        uint8 Z: 1; // Zero
        uint8 I: 1; // Interrupt Disable
        uint8 D: 1; // Decimal
        uint8 B: 1; // Break
        uint8 U: 1; // Unused
		uint8 V: 1; // Overflow
        uint8 N: 1; // Negative
    };
    uint8 raw;
};



class CPU {
public:
    // CONSTANTS


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

    enum InterruptVector {
        VECTOR_NMI,
        VECTOR_BRK,
        VECTOR_RESET,
        VECTOR_IRQ,
    };

	// OPCODE LOOKUP TABLES

    inline static const uint8 OPCODE_CYCLES_MAP[256] = {
        7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
    };

    inline static const uint8 OPCODE_EXTRA_CYCLES_MAP[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0
    };

    inline static const AddressingMode OPCODE_ADDRESSING_MAP[256] = {
        IMPLIED, INDIRECT_X, IMPLIED, INDIRECT_X,
        IMPLIED, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, ACCUMULATOR, IMMEDIATE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        IMPLIED, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,

        ABSOLUTE, INDIRECT_X, IMPLIED, INDIRECT_X,
        ZERO_PAGE, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, ACCUMULATOR, IMMEDIATE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        IMPLIED, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,

        IMPLIED, INDIRECT_X, IMPLIED, INDIRECT_X,
        IMPLIED, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, ACCUMULATOR, IMMEDIATE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        IMPLIED, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,

        IMPLIED, INDIRECT_X, IMPLIED, INDIRECT_X,
        IMPLIED, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, ACCUMULATOR, IMMEDIATE,
        INDIRECT, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        IMPLIED, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,

        IMPLIED, INDIRECT_X, IMPLIED, INDIRECT_X,
        ZERO_PAGE, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMPLIED, IMPLIED, IMMEDIATE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_Y, ZERO_PAGE_Y,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ZERO_PAGE,
        ABSOLUTE_Y, ABSOLUTE_X, ZERO_PAGE_Y, ABSOLUTE_Y,

        IMMEDIATE, INDIRECT_X, IMMEDIATE, INDIRECT_X,
        ZERO_PAGE, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, IMPLIED, ZERO_PAGE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_Y, ZERO_PAGE_Y,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y,

        IMMEDIATE, INDIRECT_X, IMPLIED, INDIRECT_X,
        ZERO_PAGE, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, IMPLIED, IMMEDIATE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        IMPLIED, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,

        IMMEDIATE, INDIRECT_X, IMPLIED, INDIRECT_X,
        ZERO_PAGE, ZERO_PAGE, ZERO_PAGE, ZERO_PAGE,
        IMPLIED, IMMEDIATE, IMPLIED, IMMEDIATE,
        ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE,

        RELATIVE, INDIRECT_Y, IMPLIED, INDIRECT_Y,
        IMPLIED, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X,
        IMPLIED, ABSOLUTE_Y, IMPLIED, ABSOLUTE_Y,
        ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X
    };

	// MEMORY CONSTANTS

    static const uint16 STACK_BASE = 0x0100;
    static const uint16 NMI_VECTOR = 0xFFFA;
    static const uint16 RESET_VECTOR = 0xFFFC;
    static const uint16 IRQ_VECTOR = 0xFFFE;



    // REGISTERS
    uint8 a;          // Accumulator
    uint8 x;          // X Register
    uint8 y;          // Y Register
    uint16 pc;        // Program Counter
    uint8 s;          // Stack Pointer
    StatusRegister p; // Status Register

    Bus& bus;

    // STATE
    long int cycles;
    // Instruction logging toggle. When true, each executed instruction will be
    // appended to `cpu.log` in a compact single-line format for debugging.
    bool enableCpuLog = false;

	bool pageCrossed;


    CPU(Bus& busRef);
    void reset();

    // After calling getOperandAddress, this returns whether the addressing
    // calculation crossed a 0xFF->0x00 page boundary (used for extra cycle
    // accounting). This replaces the previous out-parameter pointer.
    bool getLastPageCrossed() const;


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



private:

    // helpers

    uint16 getOperandAddress(AddressingMode mode);
	void addCycles(uint8 opcode);

	// specific helpers

	uint8 _neg(uint8 val);
    void _setZNFlags(uint8 val);
    void _addToAccumulator(uint8 val);
	void _compare(uint8 val1, uint8 val2);
    void _branch(bool condition, AddressingMode mode);
    void _interrupt(InterruptVector vec);
	uint8 _getStatus(bool flagB);
	void _setStatus(uint8 status);




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
	// names following https://www.masswerk.at/6502/6502_instruction_set.html

	void op_ALR(AddressingMode mode);
	void op_ANC(AddressingMode mode);
	void op_ARR(AddressingMode mode);
	




public:

	// RUNNING

    void run();
    void clock();
    void runInstruction(uint8 opcode);

    // Logging helper: write a single-line instruction trace to cpu.log when
    // `enableCpuLog` is true. Format example:
    // C000 4C F5 C5 a:00, x:00, y:00, p:24, sp:FD cyc:7
    void logInstruction(uint16 instrPc, uint8 opcode, const uint8* opcodeBytes, size_t byteCount);

};
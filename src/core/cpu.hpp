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
private:
	// CONSTANTS


	// ENUMS
	enum AddressingMode {
		IMP, // implied
		ACC, // accumulator
		IMM, // immediate
		ZPG, // zeropage
		ZPX, // zeropage,X
		ZPY, // zeropage,Y
		REL, // relative
		ABS, // absolute
		ABX, // absolute,X
		ABY, // absolute,Y
		IND, // indirect
		INX, // (indirect,X)
		INY, // (indirect),Y
	};

	enum InterruptVector {
		VECTOR_NMI,
		VECTOR_BRK,
		VECTOR_RESET,
		VECTOR_IRQ,
	};

	// OPCODE LOOKUP TABLES

	// This table holds the BASE cycle counts for each instruction.
	// Additional cycles for page crosses or taken branches are added conditionally.
	inline static const uint8 OPCODE_CYCLES_MAP[256] = {
	//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
		7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, // 0
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 1
		6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, // 2
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 3
		6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, // 4
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 5
		6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, // 6
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 7
		2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, // 8
		2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, // 9
		2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, // A
		2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, // B
		2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, // C
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // D
		2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, // E
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // F
	};
	
	// NOTE: OPCODE_EXTRA_CYCLES_MAP has been removed.

	inline static const AddressingMode OPCODE_ADDRESSING_MAP[256] = {
	//  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
		IMP, INX, IMP, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, ABS, ABS, ABS, ABS, // 0
		REL, INY, IMP, INY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // 1
		ABS, INX, IMP, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, ABS, ABS, ABS, ABS, // 2
		REL, INY, IMP, INY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // 3
		IMP, INX, IMP, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, ABS, ABS, ABS, ABS, // 4
		REL, INY, IMP, INY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // 5
		IMP, INX, IMP, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, IND, ABS, ABS, ABS, // 6
		REL, INY, IMP, INY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // 7
		IMM, INX, IMM, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, // 8
		REL, INY, IMP, INY, ZPX, ZPX, ZPY, ZPY, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // 9
		IMM, INX, IMM, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, // A
		REL, INY, IMP, INY, ZPX, ZPX, ZPY, ZPY, IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY, // B
		IMM, INX, IMM, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, // C
		REL, INY, IMP, INY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // D
		IMM, INX, IMM, INX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, // E
		REL, INY, IMP, INY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, // F
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

	Bus* bus;

	// STATE
	bool jammed = false;

	long int cycles;
	bool enableCpuLog = false;

	bool pageCrossed;

public:

	CPU();
	void reset();
	void powerOn();

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


	// getters and setters

	bool isJammed();
	void setJammed(bool jammed);
	long int getCycles();
	void enableLogging(bool enable);
	
	void connectBus(Bus* busRef);
	void disconnectBus();

private:


	// helpers

	uint16 getOperandAddress(AddressingMode mode);

	// specific helpers

	uint8 _neg(uint8 val);
	int8 _toSigned(uint8 val);
	uint8 _toUnsigned(int8 val);
	void _setZNFlags(uint8 val);
	void _addToAccumulator(uint8 val);
	void _compare(uint8 val1, uint8 val2);
	void _branch(bool condition);
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
	void op_ANC2(AddressingMode mode);
	void op_ANE(AddressingMode mode);
	void op_ARR(AddressingMode mode);
	void op_DCP(AddressingMode mode);
	void op_ISC(AddressingMode mode);
	void op_LAS(AddressingMode mode);
	void op_LAX(AddressingMode mode);
	void op_LXA(AddressingMode mode);
	void op_RLA(AddressingMode mode);
	void op_RRA(AddressingMode mode);
	void op_SAX(AddressingMode mode);
	void op_SBX(AddressingMode mode);
	void op_SHA(AddressingMode mode);
	void op_SHX(AddressingMode mode);
	void op_SHY(AddressingMode mode);
	void op_SLO(AddressingMode mode);
	void op_SRE(AddressingMode mode);
	void op_TAS(AddressingMode mode);
	void op_USBC(AddressingMode mode);
	void op_JAM(AddressingMode mode);






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

#include "cpu.hpp"



// CPU IMPLEMENTATION

// FUNCTIONS

CPU::CPU(Bus& busRef) : bus(busRef) {
	// Clear any previous CPU instruction log on emulator startup so traces
	// from earlier runs don't contaminate the current run.
	FILE* f = fopen("cpu.log", "w");
	if (f) fclose(f);

	reset();
}



uint8 CPU::readMem(uint16 addr) {
	return bus.read(addr);
}

void CPU::writeMem(uint16 addr, uint8 val) {
	bus.write(addr, val);
}

uint16 CPU::readMem16(uint16 addr) {
	return readMem(addr) | (readMem(addr + 1) << 8);
}

void CPU::writeMem16(uint16 addr, uint16 val) {
	writeMem(addr, val & 0xff);
	writeMem(addr + 1, (val >> 8) & 0xff);
}

uint16 CPU::readMem16Wrap(uint16 addr) {
	if ((addr & 0x00ff) == 0x00ff)  // Fixed operator precedence
		return readMem(addr) | (readMem(addr & 0xff00) << 8);
	return readMem16(addr);
}

uint8 CPU::pull() {
    s++; //Increment stack pointer
    return readMem(STACK_BASE + s);
}

void CPU::push(uint8 val) {
    writeMem(STACK_BASE + s, val); //Write to where S points
    s--; //decrement to point to next free space
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





uint16 CPU::getOperandAddress(AddressingMode mode) {
	int8 offset;
	uint16 addr;
	bool crossed = false;

	switch (mode) {
		case IMPLIED:
			return 0; // No operand
		case ACCUMULATOR:
			return 0; // accumulator
		case IMMEDIATE:
			return pc++;
		case ZERO_PAGE:
			return readMem(pc++);
		case ZERO_PAGE_X:
			return (readMem(pc++) + x) & 0xff;
		case ZERO_PAGE_Y:
			return (readMem(pc++) + y) & 0xff;
		case RELATIVE:
			return readMem(pc);
		case ABSOLUTE:
			addr = readMem16(pc);
			pc += 2;
			return addr;
		case ABSOLUTE_X: {
			uint16 base = readMem16(pc);
			addr = base + x;
			pc += 2;
			return addr;
		}
		case ABSOLUTE_Y: {
			uint16 base = readMem16(pc);
			addr = base + y;
			pc += 2;
			return addr;
		}
		case INDIRECT:
			addr = readMem16Wrap(readMem16(pc));
			pc += 2;
			return addr;
		case INDIRECT_X: {
			uint8 zeroPageAddr = readMem(pc++);
			uint8 effectiveAddr = (zeroPageAddr + x) & 0xFF;
			return readMem16Wrap(effectiveAddr);
		}
		case INDIRECT_Y: {
			uint16 base = readMem(pc++);
			uint16 baseAddr = readMem16Wrap(base);
			addr = baseAddr + y;
			return addr;
		}
		default:
			return 0; // Should not happen
	}
}

void CPU::addCycles(uint8 opcode) {
	cycles += OPCODE_CYCLES_MAP[opcode];
	AddressingMode mode = OPCODE_ADDRESSING_MAP[opcode];
	if (OPCODE_EXTRA_CYCLES_MAP[opcode] == 1) {

	}
}


uint8 CPU::_getStatus(bool flagB) {
	if (flagB)
		return p.raw | 0b00110000;
	else
		return p.raw & 0b11101111 | 0b00100000;
}

void CPU::_setStatus(uint8 status) {
	p.raw = status & 0b11101111 | 0b00100000;
}

// CPU INSTRUCTION HELPERS

uint8 CPU::_neg(uint8 val) {
	return ~val + 1;
}

void CPU::_setZNFlags(uint8 val) {
	p.Z = (val == 0);
	p.N = (val & 0x80) != 0;
}

void CPU::_addToAccumulator(uint8 val) {
	// Perform addition in wider type to capture carry (implicit promotions)
	uint16 sum = a + val + p.C;
	p.C = (sum > 0xFF);
	// Overflow: if sign of a and val are the same and sign of result differs
	p.V = (~(a ^ val) & (a ^ sum) & 0x80) != 0;
	uint8 result = sum & 0xFF;
	a = result;
	_setZNFlags(result);
}

void CPU::_compare(uint8 val1, uint8 val2) {
	uint16 diff = static_cast<uint16>(val1) - static_cast<uint16>(val2);
	p.C = (val1 >= val2);
	uint8 result = diff & 0xFF;
	_setZNFlags(result);
}

void CPU::_branch(bool condition, AddressingMode mode) {
	// Always compute the target (advances PC). getOperandAddress will set
	// pageCrossed if appropriate.

	uint16 addr = getOperandAddress(mode) + pc;

	if (condition) {
		// branch taken: +1 cycle, and +1 more if page crossed
		cycles += 1;
		if ((pc & 0xFF00) != (addr & 0xFF00)) cycles++;
		pc = addr;
	}

	pc++;
	// no branch, no extra cycles
}


void CPU::_interrupt(CPU::InterruptVector vec) {
	uint16 vectorAddr = RESET_VECTOR;
	switch (vec) {
		case CPU::VECTOR_NMI:   vectorAddr = NMI_VECTOR; break;
		case CPU::VECTOR_RESET: vectorAddr = RESET_VECTOR; break;
		case CPU::VECTOR_IRQ:   vectorAddr = IRQ_VECTOR; break;
	}

	push16(pc);

	uint8 flags = p.raw;
	if (vec == CPU::VECTOR_BRK) {
		flags |= (1 << 4);
	} else {
		flags &= ~(1 << 4);
	}
	flags |= (1 << 5);
	push(flags);

	p.I = 1;

	pc = readMem16(vectorAddr);
}






// CPU INSTRUCTIONS

void CPU::op_ADC(AddressingMode mode) {
	// Add with Carry
	uint16 addr = getOperandAddress(mode);
	_addToAccumulator(readMem(addr));
}

void CPU::op_AND(AddressingMode mode) {
	// AND with Accumulator
	uint16 addr = getOperandAddress(mode);
	a &= readMem(addr);
	_setZNFlags(a);
}

void CPU::op_ASL(AddressingMode mode) {
	// Arithmetic Shift Left
	uint16 addr = getOperandAddress(mode);
	if (mode == ACCUMULATOR) {
		p.C = (a & 0x80) != 0;
		a <<= 1;
		_setZNFlags(a);
	} else {
		uint8 val = readMem(addr);
		p.C = (val & 0x80) != 0;
		val <<= 1;
		writeMem(addr, val);
		_setZNFlags(val);
	}
}

void CPU::op_BCC(AddressingMode mode) {
	// Branch if Carry Clear
	_branch(p.C == 0, mode);
}

void CPU::op_BCS(AddressingMode mode) {
	// Branch if Carry Set
	_branch(p.C == 1, mode);
}

void CPU::op_BEQ(AddressingMode mode) {
	// Branch if Equal (Zero set)
	_branch(p.Z == 1, mode);
}

void CPU::op_BIT(AddressingMode mode) {
	// Bit Test
	uint16 addr = getOperandAddress(mode);
	uint8 val = readMem(addr);
	p.Z = (a & val) == 0;
	p.N = (val & 0x80) != 0;
	p.V = (val & 0x40) != 0;
}

void CPU::op_BNE(AddressingMode mode) {
	// Branch if Not Equal (Zero clear)
	_branch(p.Z == 0, mode);
}

void CPU::op_BMI(AddressingMode mode) {
	// Branch if Minus (Negative set)
	_branch(p.N == 1, mode);
}

void CPU::op_BPL(AddressingMode mode) {
	// Branch if Positive (Negative clear)
	_branch(p.N == 0, mode);
}

void CPU::op_BRK(AddressingMode mode) {
	// Force Break
	pc++; // padding byte
	_interrupt(VECTOR_BRK);
}

void CPU::op_BVS(AddressingMode mode) {
	// Branch if Overflow Set
	_branch(p.V == 1, mode);
}

void CPU::op_BVC(AddressingMode mode) {
	// Branch if Overflow Clear
	_branch(p.V == 0, mode);
}

void CPU::op_CLC(AddressingMode mode) {
	// Clear Carry Flag
	p.C = 0;
}

void CPU::op_CLD(AddressingMode mode) {
	// Clear Decimal Flag
	p.D = 0;
}

void CPU::op_CLI(AddressingMode mode) {
	// Clear Interrupt Disable
	p.I = 0;
}

void CPU::op_CLV(AddressingMode mode) {
	// Clear Overflow Flag
	p.V = 0;
}

void CPU::op_CMP(AddressingMode mode) {
	// Compare Accumulator
	uint16 addr = getOperandAddress(mode);
	_compare(a, readMem(addr));
}

void CPU::op_CPX(AddressingMode mode) {
	// Compare X Register
	uint16 addr = getOperandAddress(mode);
	_compare(x, readMem(addr));
}

void CPU::op_CPY(AddressingMode mode) {
	// Compare Y Register
	uint16 addr = getOperandAddress(mode);
	_compare(y, readMem(addr));
}

void CPU::op_DEC(AddressingMode mode) {
	// Decrement Memory
	uint16 addr = getOperandAddress(mode);
	uint8 val = readMem(addr);
	val--;
	writeMem(addr, val);
	_setZNFlags(val);
}

void CPU::op_DEX(AddressingMode mode) {
	// Decrement X Register
	x--;
	_setZNFlags(x);
}

void CPU::op_DEY(AddressingMode mode) {
	// Decrement Y Register
	y--;
	_setZNFlags(y);
}

void CPU::op_EOR(AddressingMode mode) {
	// Exclusive OR with Accumulator
	uint16 addr = getOperandAddress(mode);
	a ^= readMem(addr);
	_setZNFlags(a);
}

void CPU::op_INC(AddressingMode mode) {
	// Increment Memory
	uint16 addr = getOperandAddress(mode);
	uint8 val = readMem(addr);
	val++;
	writeMem(addr, val);
	_setZNFlags(val);
}

void CPU::op_INX(AddressingMode mode) {
	// Increment X Register
	x++;
	_setZNFlags(x);
}

void CPU::op_INY(AddressingMode mode) {
	// Increment Y Register
	y++;
	_setZNFlags(y);
}

void CPU::op_JMP(AddressingMode mode) {
	// Jump
	uint16 addr = getOperandAddress(mode);
	pc = addr;
}

void CPU::op_JSR(AddressingMode mode) {
	// Jump to Subroutine
	uint16 addr = getOperandAddress(mode);
	push16(pc - 1);
	pc = addr;
}

void CPU::op_LDA(AddressingMode mode) {
	// Load Accumulator
	uint16 addr = getOperandAddress(mode);
	a = readMem(addr);
	_setZNFlags(a);
}

void CPU::op_LDX(AddressingMode mode) {
	// Load X Register
	uint16 addr = getOperandAddress(mode);
	x = readMem(addr);
	_setZNFlags(x);
}

void CPU::op_LDY(AddressingMode mode) {
	// Load Y Register
	uint16 addr = getOperandAddress(mode);
	y = readMem(addr);
	_setZNFlags(y);
}

void CPU::op_LSR(AddressingMode mode) {
	// Logical Shift Right
	uint16 addr = getOperandAddress(mode);
	if (mode == ACCUMULATOR) {
		p.C = (a & 0x01) != 0;
		a >>= 1;
		_setZNFlags(a);
	} else {
		uint8 val = readMem(addr);
		p.C = (val & 0x01) != 0;
		val >>= 1;
		writeMem(addr, val);
		_setZNFlags(val);
	}
}

void CPU::op_NOP(AddressingMode mode) {
	// No operation
}

void CPU::op_ORA(AddressingMode mode) {
	// OR with Accumulator
	uint16 addr = getOperandAddress(mode);
	a |= readMem(addr);
	_setZNFlags(a);
}

void CPU::op_PHA(AddressingMode mode) {
	// Push Accumulator
	push(a);
}

void CPU::op_PHP(AddressingMode mode) {
	// Push Processor Status
	push(_getStatus(true));
}

void CPU::op_PLA(AddressingMode mode) {
	// Pull Accumulator
	a = pull();
	_setZNFlags(a);
}

void CPU::op_PLP(AddressingMode mode) {
	// Pull Processor Status
	_setStatus(pull());
	p.U = 1;
}

void CPU::op_ROL(AddressingMode mode) {
	// Rotate Left
	uint16 addr = getOperandAddress(mode);
	if (mode == ACCUMULATOR) {
		uint8 val = a;
		uint8 carry = p.C;
		p.C = (val & 0x80) != 0;
		val = (val << 1) | carry;
		a = val;
		_setZNFlags(a);
	} else {
		uint8 val = readMem(addr);
		uint8 carry = p.C;
		p.C = (val & 0x80) != 0;
		val = (val << 1) | carry;
		writeMem(addr, val);
		_setZNFlags(val);
	}
}

void CPU::op_ROR(AddressingMode mode) {
	// Rotate Right
	uint16 addr = getOperandAddress(mode);
	if (mode == ACCUMULATOR) {
		uint8 val = a;
		uint8 carry = p.C;
		p.C = (val & 0x01) != 0;
		val = (val >> 1) | (carry << 7);
		a = val;
		_setZNFlags(a);
	} else {
		uint8 val = readMem(addr);
		uint8 carry = p.C;
		p.C = (val & 0x01) != 0;
		val = (val >> 1) | (carry << 7);
		writeMem(addr, val);
		_setZNFlags(val);
	}
}

void CPU::op_RTI(AddressingMode mode) {
	// Return from Interrupt
	_setStatus(pull());
	pc = pull16();
}

void CPU::op_RTS(AddressingMode mode) {
	// Return from Subroutine
	pc = pull16() + 1;
}

void CPU::op_SBC(AddressingMode mode) {
	// Subtract with Carry
	uint16 addr = getOperandAddress(mode);
	_addToAccumulator(readMem(addr) ^ 0xFF);
}

void CPU::op_SEC(AddressingMode mode) {
	// Set Carry Flag
	p.C = 1;
}

void CPU::op_SED(AddressingMode mode) {
	// Set Decimal Flag
	p.D = 1;
}

void CPU::op_SEI(AddressingMode mode) {
	// Set Interrupt Disable
	p.I = 1;
}

void CPU::op_STA(AddressingMode mode) {
	// Store Accumulator
	uint16 addr = getOperandAddress(mode);
	writeMem(addr, a);
}

void CPU::op_STX(AddressingMode mode) {
	// Store X Register
	uint16 addr = getOperandAddress(mode);
	writeMem(addr, x);
}

void CPU::op_STY(AddressingMode mode) {
	// Store Y Register
	uint16 addr = getOperandAddress(mode);
	writeMem(addr, y);
}

void CPU::op_TAX(AddressingMode mode) {
	// Transfer Accumulator to X
	x = a;
	_setZNFlags(x);
}

void CPU::op_TAY(AddressingMode mode) {
	// Transfer Accumulator to Y
	y = a;
	_setZNFlags(y);
}

void CPU::op_TSX(AddressingMode mode) {
	// Transfer Stack Pointer to X
	x = s;
	_setZNFlags(x);
}

void CPU::op_TXA(AddressingMode mode) {
	// Transfer X to Accumulator
	a = x;
	_setZNFlags(a);
}

void CPU::op_TXS(AddressingMode mode) {
	// Transfer X to Stack Pointer
	s = x;
}

void CPU::op_TYA(AddressingMode mode) {
	// Transfer Y to Accumulator
	a = y;
	_setZNFlags(a);
}


// UNOFFICIAL OPCODES




// RUNNING

void CPU::reset() {
	a = 0;
	x = 0;
	y = 0;
	pc = 0xC000; // readMem16(RESET_VECTOR); replaced with fixed value for nestest
	s = 0xFD;    // Stack pointer powerup state
	p.raw = 0b00100100; // Typical initial status register value
	// flags: NVUBDIZC

	cycles = 7; // Reset takes time to complete
}

void CPU::run() {
	enableCpuLog = true;
	while (true) {
		clock();
	}
}

void CPU::clock() {
	// Capture program counter at instruction start so log lines show the
	// correct address and bytes for the instruction executed.
	uint16 instrPc = pc;
	uint8 opcode = readMem(pc++);

	
	if (enableCpuLog) {
		// Read up to 3 operand bytes for logging (safe: don't advance pc here)
		uint8 opcodeBytes[3] = {0,0,0};
		// Many opcodes use up to 2 operand bytes; read a small window for logging
		opcodeBytes[0] = readMem(instrPc);
		opcodeBytes[1] = readMem(instrPc + 1);
		opcodeBytes[2] = readMem(instrPc + 2);
		// Determine byte count: use addressing map to get how many bytes
		// the opcode consumes. We'll infer 1, 2 or 3 bytes from the
		// addressing mode table stored in the header.
		auto mode = OPCODE_ADDRESSING_MAP[opcode];
		size_t byteCount = 1;
		switch (mode) {
			case IMMEDIATE:
			case ZERO_PAGE:
			case ZERO_PAGE_X:
			case ZERO_PAGE_Y:
			case INDIRECT_X:
			case INDIRECT_Y:
			case RELATIVE:
				byteCount = 2;
				break;
			case ABSOLUTE:
			case ABSOLUTE_X:
			case ABSOLUTE_Y:
			case INDIRECT:
				byteCount = 3;
				break;
			default:
				byteCount = 1;
		}
		logInstruction(instrPc, opcode, opcodeBytes, byteCount);

		if (cycles == 2603) {
			for (int i = 0; i < 0x200; i++) {
				if (!(i % 16)) {
					printf("\n");
				}
				printf("%02X ", readMem(i));
			}
			printf("\n");
		}
	}

	runInstruction(opcode);
	addCycles(opcode);


	if (cycles >= 29475) {
		enableCpuLog = false;
		for (int i = 0; i < 0x200; i++) {
			if (!(i % 16)) {
				printf("\n");
			}
			printf("%02X ", readMem(i));
		}

		int a[0];
		printf("%d", a[12939024244]); // stop here with segfault
	}
}

void CPU::logInstruction(uint16 instrPc, uint8 opcode, const uint8* opcodeBytes, size_t byteCount) {
	// Open cpu.log for append each time to keep implementation simple and
	// avoid holding a global file handle. This is fine for debugging but
	// could be optimized later.
	FILE* f = fopen("cpu.log", "a");
	if (!f) return;

	// Print PC
	fprintf(f, "%04X ", instrPc);

	// Print up to 3 bytes in hex (space-separated)
	for (size_t i = 0; i < 3; ++i) {
		if (i < byteCount) {
			fprintf(f, "%02X", opcodeBytes[i]);
		} else {
			fprintf(f, "  ");
		}
		if (i < 2) fprintf(f, " ");
	}

	// Print registers in the compact style requested
	// a:00 x:00 y:00 p:00100100 sp:02 cyc:7
	// Note: "%08b" is not standard in C, so format p.raw manually as binary
	char p_bits[9];
	for (int i = 7; i >= 0; --i) {
		p_bits[7 - i] = ((p.raw >> i) & 1) ? '1' : '0';
	}
	p_bits[8] = '\0';
	fprintf(f, " a:%02X x:%02X y:%02X p:%s sp:%02X\n",// cyc:%ld\n",
			a, x, y, p_bits, s, cycles);

	fclose(f);
}

void CPU::runInstruction(uint8 opcode) {
	switch (opcode) {
		// ---------   OPCODES   --------- //     OPCODE | BYTES | CYCLES | ADDRESSING
		case 0x00: op_BRK(IMPLIED);     break; // BRK (0x00) | 1 | 7  | IMPLIED
		case 0x01: op_ORA(INDIRECT_X);  break; // ORA (0x01) | 2 | 6  | (indirect,X)
		case 0x02: op_NOP(IMPLIED);     break;
		case 0x03: op_NOP(IMPLIED);     break;
		case 0x04: op_NOP(IMPLIED);     break;
		case 0x05: op_ORA(ZERO_PAGE);   break; // ORA (0x05) | 2 | 3  | zeropage
		case 0x06: op_ASL(ZERO_PAGE);   break; // ASL (0x06) | 2 | 5  | zeropage
		case 0x07: op_NOP(IMPLIED);     break;
		case 0x08: op_PHP(IMPLIED);     break; // PHP (0x08) | 1 | 3  | implied
		case 0x09: op_ORA(IMMEDIATE);   break; // ORA (0x09) | 2 | 2  | immediate
		case 0x0a: op_ASL(ACCUMULATOR); break; // ASL (0x0A) | 1 | 2  | accumulator
		case 0x0b: op_NOP(IMPLIED);     break;
		case 0x0c: op_NOP(IMPLIED);     break;
		case 0x0d: op_ORA(ABSOLUTE);    break; // ORA (0x0D) | 3 | 4  | absolute
		case 0x0e: op_ASL(ABSOLUTE);    break; // ASL (0x0E) | 3 | 6  | absolute
		case 0x0f: op_NOP(IMPLIED);     break;

		case 0x10: op_BPL(RELATIVE);    break; // BPL (0x10) | 2 | 2**| relative
		case 0x11: op_ORA(INDIRECT_Y);  break; // ORA (0x11) | 2 | 5* | (indirect),Y
		case 0x12: op_NOP(IMPLIED);     break;
		case 0x13: op_NOP(IMPLIED);     break;
		case 0x14: op_NOP(IMPLIED);     break;
		case 0x15: op_ORA(ZERO_PAGE_X); break; // ORA (0x15) | 2 | 4  | zeropage,X
		case 0x16: op_ASL(ZERO_PAGE_X); break; // ASL (0x16) | 2 | 6  | zeropage,X
		case 0x17: op_NOP(IMPLIED);     break;
		case 0x18: op_CLC(IMPLIED);     break; // CLC (0x18) | 1 | 2  | implied
		case 0x19: op_ORA(ABSOLUTE_Y);  break; // ORA (0x19) | 3 | 4* | absolute,Y
		case 0x1a: op_NOP(IMPLIED);     break;
		case 0x1b: op_NOP(IMPLIED);     break;
		case 0x1c: op_NOP(IMPLIED);     break;
		case 0x1d: op_ORA(ABSOLUTE_X);  break; // ORA (0x1D) | 3 | 4* | absolute,X
		case 0x1e: op_ASL(ABSOLUTE_X);  break; // ASL (0x1E) | 3 | 7  | absolute,X
		case 0x1f: op_NOP(IMPLIED);     break;

		case 0x20: op_JSR(ABSOLUTE);    break; // JSR (0x20) | 3 | 6  | absolute
		case 0x21: op_AND(INDIRECT_X);  break; // AND (0x21) | 2 | 6  | (indirect,X)
		case 0x22: op_NOP(IMPLIED);     break;
		case 0x23: op_NOP(IMPLIED);     break;
		case 0x24: op_BIT(ZERO_PAGE);   break; // BIT (0x24) | 2 | 3  | zeropage
		case 0x25: op_AND(ZERO_PAGE);   break; // AND (0x25) | 2 | 3  | zeropage
		case 0x26: op_ROL(ZERO_PAGE);   break; // ROL (0x26) | 2 | 5  | zeropage
		case 0x27: op_NOP(IMPLIED);     break;
		case 0x28: op_PLP(IMPLIED);     break; // PLP (0x28) | 1 | 4  | implied
		case 0x29: op_AND(IMMEDIATE);   break; // AND (0x29) | 2 | 2  | immediate
		case 0x2a: op_ROL(ACCUMULATOR); break; // ROL (0x2A) | 1 | 2  | accumulator
		case 0x2b: op_NOP(IMPLIED);     break;
		case 0x2c: op_BIT(ABSOLUTE);    break; // BIT (0x2C) | 3 | 4  | absolute
		case 0x2d: op_AND(ABSOLUTE);    break; // AND (0x2D) | 3 | 4  | absolute
		case 0x2e: op_ROL(ABSOLUTE);    break; // ROL (0x2E) | 3 | 6  | absolute
		case 0x2f: op_NOP(IMPLIED);     break;

		case 0x30: op_BMI(RELATIVE);    break; // BMI (0x30) | 2 | 2**| relative
		case 0x31: op_AND(INDIRECT_Y);  break; // AND (0x31) | 2 | 5* | (indirect),Y
		case 0x32: op_NOP(IMPLIED);     break;
		case 0x33: op_NOP(IMPLIED);     break;
		case 0x34: op_NOP(IMPLIED);     break;
		case 0x35: op_AND(ZERO_PAGE_X); break; // AND (0x35) | 2 | 4  | zeropage,X
		case 0x36: op_ROL(ZERO_PAGE_X); break; // ROL (0x36) | 2 | 6  | zeropage,X
		case 0x37: op_NOP(IMPLIED);     break;
		case 0x38: op_SEC(IMPLIED);     break; // SEC (0x38) | 1 | 2  | implied
		case 0x39: op_AND(ABSOLUTE_Y);  break; // AND (0x39) | 3 | 4* | absolute,Y
		case 0x3a: op_NOP(IMPLIED);     break;
		case 0x3b: op_NOP(IMPLIED);     break;
		case 0x3c: op_NOP(IMPLIED);     break;
		case 0x3d: op_AND(ABSOLUTE_X);  break; // AND (0x3D) | 3 | 4* | absolute,X
		case 0x3e: op_ROL(ABSOLUTE_X);  break; // ROL (0x3E) | 3 | 7  | absolute,X
		case 0x3f: op_NOP(IMPLIED);     break;

		case 0x40: op_RTI(IMPLIED);     break; // RTI (0x40) | 1 | 6  | implied
		case 0x41: op_EOR(INDIRECT_X);  break; // EOR (0x41) | 2 | 6  | (indirect,X)
		case 0x42: op_NOP(IMPLIED);     break;
		case 0x43: op_NOP(IMPLIED);     break;
		case 0x44: op_NOP(IMPLIED);     break;
		case 0x45: op_EOR(ZERO_PAGE);   break; // EOR (0x45) | 2 | 3  | zeropage
		case 0x46: op_LSR(ZERO_PAGE);   break; // LSR (0x46) | 2 | 5  | zeropage
		case 0x47: op_NOP(IMPLIED);     break;
		case 0x48: op_PHA(IMPLIED);     break; // PHA (0x48) | 1 | 3  | implied
		case 0x49: op_EOR(IMMEDIATE);   break; // EOR (0x49) | 2 | 2  | immediate
		case 0x4a: op_LSR(ACCUMULATOR); break; // LSR (0x4A) | 1 | 2  | accumulator
		case 0x4b: op_NOP(IMPLIED);     break;
		case 0x4c: op_JMP(ABSOLUTE);    break; // JMP (0x4C) | 3 | 3  | absolute
		case 0x4d: op_EOR(ABSOLUTE);    break; // EOR (0x4D) | 3 | 4  | absolute
		case 0x4e: op_LSR(ABSOLUTE);    break; // LSR (0x4E) | 3 | 6  | absolute
		case 0x4f: op_NOP(IMPLIED);     break;

		case 0x50: op_BVC(RELATIVE);    break; // BVC (0x50) | 2 | 2**| relative
		case 0x51: op_EOR(INDIRECT_Y);  break; // EOR (0x51) | 2 | 5* | (indirect),Y
		case 0x52: op_NOP(IMPLIED);     break;
		case 0x53: op_NOP(IMPLIED);     break;
		case 0x54: op_NOP(IMPLIED);     break;
		case 0x55: op_EOR(ZERO_PAGE_X); break; // EOR (0x55) | 2 | 4  | zeropage,X
		case 0x56: op_LSR(ZERO_PAGE_X); break; // LSR (0x56) | 2 | 6  | zeropage,X
		case 0x57: op_NOP(IMPLIED);     break;
		case 0x58: op_CLI(IMPLIED);     break; // CLI (0x58) | 1 | 2  | implied
		case 0x59: op_EOR(ABSOLUTE_Y);  break; // EOR (0x59) | 3 | 4* | absolute,Y
		case 0x5a: op_NOP(IMPLIED);     break;
		case 0x5b: op_NOP(IMPLIED);     break;
		case 0x5c: op_NOP(IMPLIED);     break;
		case 0x5d: op_EOR(ABSOLUTE_X);  break; // EOR (0x5D) | 3 | 4* | absolute,X
		case 0x5e: op_LSR(ABSOLUTE_X);  break; // LSR (0x5E) | 3 | 7  | absolute,X
		case 0x5f: op_NOP(IMPLIED);     break;

		case 0x60: op_RTS(IMPLIED);     break; // RTS (0x60) | 1 | 6  | implied
		case 0x61: op_ADC(INDIRECT_X);  break; // ADC (0x61) | 2 | 6  | (indirect,X)
		case 0x62: op_NOP(IMPLIED);     break;
		case 0x63: op_NOP(IMPLIED);     break;
		case 0x64: op_NOP(IMPLIED);     break;
		case 0x65: op_ADC(ZERO_PAGE);   break; // ADC (0x65) | 2 | 3  | zeropage
		case 0x66: op_ROR(ZERO_PAGE);   break; // ROR (0x66) | 2 | 5  | zeropage
		case 0x67: op_NOP(IMPLIED);     break;
		case 0x68: op_PLA(IMPLIED);     break; // PLA (0x68) | 1 | 4  | implied
		case 0x69: op_ADC(IMMEDIATE);   break; // ADC (0x69) | 2 | 2  | immediate
		case 0x6a: op_ROR(ACCUMULATOR); break; // ROR (0x6A) | 1 | 2  | accumulator
		case 0x6b: op_NOP(IMPLIED);     break;
		case 0x6c: op_JMP(INDIRECT);    break; // JMP (0x6C) | 3 | 5  | indirect
		case 0x6d: op_ADC(ABSOLUTE);    break; // ADC (0x6D) | 3 | 4  | absolute
		case 0x6e: op_ROR(ABSOLUTE);    break; // ROR (0x6E) | 3 | 6  | absolute
		case 0x6f: op_NOP(IMPLIED);     break;

		case 0x70: op_BVS(RELATIVE);    break; // BVS (0x70) | 2 | 2**| relative
		case 0x71: op_ADC(INDIRECT_Y);  break; // ADC (0x71) | 2 | 5* | (indirect),Y
		case 0x72: op_NOP(IMPLIED);     break;
		case 0x73: op_NOP(IMPLIED);     break;
		case 0x74: op_NOP(IMPLIED);     break;
		case 0x75: op_ADC(ZERO_PAGE_X); break; // ADC (0x75) | 2 | 4  | zeropage,X
		case 0x76: op_ROR(ZERO_PAGE_X); break; // ROR (0x76) | 2 | 6  | zeropage,X
		case 0x77: op_NOP(IMPLIED);     break;
		case 0x78: op_SEI(IMPLIED);     break; // SEI (0x78) | 1 | 2  | implied
		case 0x79: op_ADC(ABSOLUTE_Y);  break; // ADC (0x79) | 3 | 4* | absolute,Y
		case 0x7a: op_NOP(IMPLIED);     break;
		case 0x7b: op_NOP(IMPLIED);     break;
		case 0x7c: op_NOP(IMPLIED);     break;
		case 0x7d: op_ADC(ABSOLUTE_X);  break; // ADC (0x7D) | 3 | 4* | absolute,X
		case 0x7e: op_ROR(ABSOLUTE_X);  break; // ROR (0x7E) | 3 | 7  | absolute,X
		case 0x7f: op_NOP(IMPLIED);     break;

		case 0x80: op_NOP(IMPLIED);     break;
		case 0x81: op_STA(INDIRECT_X);  break; // STA (0x81) | 2 | 6  | (indirect,X)
		case 0x82: op_NOP(IMPLIED);     break;
		case 0x83: op_NOP(IMPLIED);     break;
		case 0x84: op_STY(ZERO_PAGE);   break; // STY (0x84) | 2 | 3  | zeropage
		case 0x85: op_STA(ZERO_PAGE);   break; // STA (0x85) | 2 | 3  | zeropage
		case 0x86: op_STX(ZERO_PAGE);   break; // STX (0x86) | 2 | 3  | zeropage
		case 0x87: op_NOP(IMPLIED);     break;
		case 0x88: op_DEY(IMPLIED);     break; // DEY (0x88) | 1 | 2  | implied
		case 0x89: op_NOP(IMPLIED);     break;
		case 0x8a: op_TXA(IMPLIED);     break; // TXA (0x8A) | 1 | 2  | implied
		case 0x8b: op_NOP(IMPLIED);     break;
		case 0x8c: op_STY(ABSOLUTE);    break; // STY (0x8C) | 3 | 4  | absolute
		case 0x8d: op_STA(ABSOLUTE);    break; // STA (0x8D) | 3 | 4  | absolute
		case 0x8e: op_STX(ABSOLUTE);    break; // STX (0x8E) | 3 | 4  | absolute
		case 0x8f: op_NOP(IMPLIED);     break;

		case 0x90: op_BCC(RELATIVE);    break; // BCC (0x90) | 2 | 2**| relative
		case 0x91: op_STA(INDIRECT_Y);  break; // STA (0x91) | 2 | 6  | (indirect),Y
		case 0x92: op_NOP(IMPLIED);     break;
		case 0x93: op_NOP(IMPLIED);     break;
		case 0x94: op_STY(ZERO_PAGE_X); break; // STY (0x94) | 2 | 4  | zeropage,X
		case 0x95: op_STA(ZERO_PAGE_X); break; // STA (0x95) | 2 | 4  | zeropage,X
		case 0x96: op_STX(ZERO_PAGE_Y); break; // STX (0x96) | 2 | 4  | zeropage,Y
		case 0x97: op_NOP(IMPLIED);     break;
		case 0x98: op_TYA(IMPLIED);     break; // TYA (0x98) | 1 | 2  | implied
		case 0x99: op_STA(ABSOLUTE_Y);  break; // STA (0x99) | 3 | 5  | absolute,Y
		case 0x9a: op_TXS(IMPLIED);     break; // TXS (0x9A) | 1 | 2  | implied
		case 0x9b: op_NOP(IMPLIED);     break;
		case 0x9c: op_NOP(IMPLIED);     break;
		case 0x9d: op_STA(ABSOLUTE_X);  break; // STA (0x9D) | 3 | 5  | absolute,X
		case 0x9e: op_NOP(IMPLIED);     break;
		case 0x9f: op_NOP(IMPLIED);     break;

		case 0xa0: op_LDY(IMMEDIATE);   break; // LDY (0xA0) | 2 | 2  | immediate
		case 0xa1: op_LDA(INDIRECT_X);  break; // LDA (0xA1) | 2 | 6  | (indirect,X)
		case 0xa2: op_LDX(IMMEDIATE);   break; // LDX (0xA2) | 2 | 2  | immediate
		case 0xa3: op_NOP(IMPLIED);     break;
		case 0xa4: op_LDY(ZERO_PAGE);   break; // LDY (0xA4) | 2 | 3  | zeropage
		case 0xa5: op_LDA(ZERO_PAGE);   break; // LDA (0xA5) | 2 | 3  | zeropage
		case 0xa6: op_LDX(ZERO_PAGE);   break; // LDX (0xA6) | 2 | 3  | zeropage
		case 0xa7: op_NOP(IMPLIED);     break;
		case 0xa8: op_TAY(IMPLIED);     break; // TAY (0xA8) | 1 | 2  | implied
		case 0xa9: op_LDA(IMMEDIATE);   break; // LDA (0xA9) | 2 | 2  | immediate
		case 0xaa: op_TAX(IMPLIED);     break; // TAX (0xAA) | 1 | 2  | implied
		case 0xab: op_NOP(IMPLIED);     break;
		case 0xac: op_LDY(ABSOLUTE);    break; // LDY (0xAC) | 3 | 4  | absolute
		case 0xad: op_LDA(ABSOLUTE);    break; // LDA (0xAD) | 3 | 4  | absolute
		case 0xae: op_LDX(ABSOLUTE);    break; // LDX (0xAE) | 3 | 4  | absolute
		case 0xaf: op_NOP(IMPLIED);     break;

		case 0xb0: op_BCS(RELATIVE);    break; // BCS (0xB0) | 2 | 2**| relative
		case 0xb1: op_LDA(INDIRECT_Y);  break; // LDA (0xB1) | 2 | 5* | (indirect),Y
		case 0xb2: op_NOP(IMPLIED);     break;
		case 0xb3: op_NOP(IMPLIED);     break;
		case 0xb4: op_LDY(ZERO_PAGE_X); break; // LDY (0xB4) | 2 | 4  | zeropage,X
		case 0xb5: op_LDA(ZERO_PAGE_X); break; // LDA (0xB5) | 2 | 4  | zeropage,X
		case 0xb6: op_LDX(ZERO_PAGE_Y); break; // LDX (0xB6) | 2 | 4  | zeropage,Y
		case 0xb7: op_NOP(IMPLIED);     break;
		case 0xb8: op_CLV(IMPLIED);     break; // CLV (0xB8) | 1 | 2  | implied
		case 0xb9: op_LDA(ABSOLUTE_Y);  break; // LDA (0xB9) | 3 | 4* | absolute,Y
		case 0xba: op_TSX(IMPLIED);     break; // TSX (0xBA) | 1 | 2  | implied
		case 0xbb: op_NOP(IMPLIED);     break;
		case 0xbc: op_LDY(ABSOLUTE_X);  break; // LDY (0xBC) | 3 | 4* | absolute,X
		case 0xbd: op_LDA(ABSOLUTE_X);  break; // LDA (0xBD) | 3 | 4* | absolute,X
		case 0xbe: op_LDX(ABSOLUTE_Y);  break; // LDX (0xBE) | 3 | 4* | absolute,Y
		case 0xbf: op_NOP(IMPLIED);     break;

		case 0xc0: op_CPY(IMMEDIATE);   break; // CPY (0xC0) | 2 | 2  | immediate
		case 0xc1: op_CMP(INDIRECT_X);  break; // CMP (0xC1) | 2 | 6  | (indirect,X)
		case 0xc2: op_NOP(IMPLIED);     break;
		case 0xc3: op_NOP(IMPLIED);     break;
		case 0xc4: op_CPY(ZERO_PAGE);   break; // CPY (0xC4) | 2 | 3  | zeropage
		case 0xc5: op_CMP(ZERO_PAGE);   break; // CMP (0xC5) | 2 | 3  | zeropage
		case 0xc6: op_DEC(ZERO_PAGE);   break; // DEC (0xC6) | 2 | 5  | zeropage
		case 0xc7: op_NOP(IMPLIED);     break;
		case 0xc8: op_INY(IMPLIED);     break; // INY (0xC8) | 1 | 2  | implied
		case 0xc9: op_CMP(IMMEDIATE);   break; // CMP (0xC9) | 2 | 2  | immediate
		case 0xca: op_DEX(IMPLIED);     break; // DEX (0xCA) | 1 | 2  | implied
		case 0xcb: op_NOP(IMPLIED);     break;
		case 0xcc: op_CPY(ABSOLUTE);    break; // CPY (0xCC) | 3 | 4  | absolute
		case 0xcd: op_CMP(ABSOLUTE);    break; // CMP (0xCD) | 3 | 4  | absolute
		case 0xce: op_DEC(ABSOLUTE);    break; // DEC (0xCE) | 3 | 6  | absolute
		case 0xcf: op_NOP(IMPLIED);     break;

		case 0xd0: op_BNE(RELATIVE);    break; // BNE (0xD0) | 2 | 2**| relative
		case 0xd1: op_CMP(INDIRECT_Y);  break; // CMP (0xD1) | 2 | 5* | (indirect),Y
		case 0xd2: op_NOP(IMPLIED);     break;
		case 0xd3: op_NOP(IMPLIED);     break;
		case 0xd4: op_NOP(IMPLIED);     break;
		case 0xd5: op_CMP(ZERO_PAGE_X); break; // CMP (0xD5) | 2 | 4  | zeropage,X
		case 0xd6: op_DEC(ZERO_PAGE_X); break; // DEC (0xD6) | 2 | 6  | zeropage,X
		case 0xd7: op_NOP(IMPLIED);     break;
		case 0xd8: op_CLD(IMPLIED);     break; // CLD (0xD8) | 1 | 2  | implied
		case 0xd9: op_CMP(ABSOLUTE_Y);  break; // CMP (0xD9) | 3 | 4* | absolute,Y
		case 0xda: op_NOP(IMPLIED);     break;
		case 0xdb: op_NOP(IMPLIED);     break;
		case 0xdc: op_NOP(IMPLIED);     break;
		case 0xdd: op_CMP(ABSOLUTE_X);  break; // CMP (0xDD) | 3 | 4* | absolute,X
		case 0xde: op_DEC(ABSOLUTE_X);  break; // DEC (0xDE) | 3 | 7  | absolute,X
		case 0xdf: op_NOP(IMPLIED);     break;

		case 0xe0: op_CPX(IMMEDIATE);   break; // CPX (0xE0) | 2 | 2  | immediate
		case 0xe1: op_SBC(INDIRECT_X);  break; // SBC (0xE1) | 2 | 6  | (indirect,X)
		case 0xe2: op_NOP(IMPLIED);     break;
		case 0xe3: op_NOP(IMPLIED);     break;
		case 0xe4: op_CPX(ZERO_PAGE);   break; // CPX (0xE4) | 2 | 3  | zeropage
		case 0xe5: op_SBC(ZERO_PAGE);   break; // SBC (0xE5) | 2 | 3  | zeropage
		case 0xe6: op_INC(ZERO_PAGE);   break; // INC (0xE6) | 2 | 5  | zeropage
		case 0xe7: op_NOP(IMPLIED);     break;
		case 0xe8: op_INX(IMPLIED);     break; // INX (0xE8) | 1 | 2  | implied
		case 0xe9: op_SBC(IMMEDIATE);   break; // SBC (0xE9) | 2 | 2  | immediate
		case 0xea: op_NOP(IMPLIED);     break; // NOP (0xEA) | 1 | 2  | implied
		case 0xeb: op_NOP(IMPLIED);     break;
		case 0xec: op_CPX(ABSOLUTE);    break; // CPX (0xEC) | 3 | 4  | absolute
		case 0xed: op_SBC(ABSOLUTE);    break; // SBC (0xED) | 3 | 4  | absolute
		case 0xee: op_INC(ABSOLUTE);    break; // INC (0xEE) | 3 | 6  | absolute
		case 0xef: op_NOP(IMPLIED);     break;

		case 0xf0: op_BEQ(RELATIVE);    break; // BEQ (0xF0) | 2 | 2**| relative
		case 0xf1: op_SBC(INDIRECT_Y);  break; // SBC (0xF1) | 2 | 5* | (indirect),Y
		case 0xf2: op_NOP(IMPLIED);     break;
		case 0xf3: op_NOP(IMPLIED);     break;
		case 0xf4: op_NOP(IMPLIED);     break;
		case 0xf5: op_SBC(ZERO_PAGE_X); break; // SBC (0xF5) | 2 | 4  | zeropage,X
		case 0xf6: op_INC(ZERO_PAGE_X); break; // INC (0xF6) | 2 | 6  | zeropage,X
		case 0xf7: op_NOP(IMPLIED);     break;
		case 0xf8: op_SED(IMPLIED);     break; // SED (0xF8) | 1 | 2  | implied
		case 0xf9: op_SBC(ABSOLUTE_Y);  break; // SBC (0xF9) | 3 | 4* | absolute,Y
		case 0xfa: op_NOP(IMPLIED);     break;
		case 0xfb: op_NOP(IMPLIED);     break;
		case 0xfc: op_NOP(IMPLIED);     break;
		case 0xfd: op_SBC(ABSOLUTE_X);  break; // SBC (0xFD) | 3 | 4* | absolute,X
		case 0xfe: op_INC(ABSOLUTE_X);  break; // INC (0xFE) | 3 | 7  | absolute,X
		case 0xff: op_NOP(IMPLIED);     break;

		
		default:
			// should never happen
			// print for debugging for now
			std::cout << "Unknown opcode: " << std::hex << opcode << std::dec << "\n";
			break;
	}
}
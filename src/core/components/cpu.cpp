
#include "cpu.hpp"



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
    p.raw = 0b00100100; // Typical initial status register value
    // flags: NVUBDIZC
}

void CPU::run() {
    while (true) {
        clock();
    }
}

void CPU::clock() {
    uint8 opcode = readMem(pc++);
    runInstruction(opcode);
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
    return (readMem((addr & 0xff00) | ((addr + 1) & 0x00ff)) << 8) | readMem(addr);
}

uint8 CPU::pull() {
    return readMem(STACK_BASE + (++s));
}

void CPU::push(uint8 val) {
    writeMem(STACK_BASE + (s--), val);
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
            offset = static_cast<int8>(readMem(pc++));
            return pc + offset;
        case ABSOLUTE:
            addr = readMem16(pc);
            pc += 2;
            return addr;
        case ABSOLUTE_X:
            addr = readMem16(pc) + x;
            pc += 2;
            return addr;
        case ABSOLUTE_Y:
            addr = readMem16(pc) + y;
            pc += 2;
            return addr;
        case INDIRECT:
            addr = readMem16(readMem16(pc));
            pc += 2;
            return addr;
        case INDIRECT_X:
            return readMem16((readMem(pc++) + x));
        case INDIRECT_Y:
            return readMem16(readMem(pc++)) + y;
        default:
			return 0; // Should not happen
    }
}


void CPU::runInstruction(uint8 opcode) {
    switch (opcode) {
        // ---------   OPCODES   --------- // OPCODE | BYTES | CYCLES | ADDRESSING
        case 0x00: op_NOP(IMPLIED);     break; // NOP (0x00) | 1 | 7 | IMPLIED
        case 0x01: op_NOP(IMPLIED);     break; // ORA (0x01) | 2 | 6 | (INDIRECT,X)
        case 0x02: op_NOP(IMPLIED);     break;
        case 0x03: op_NOP(IMPLIED);     break;
        case 0x04: op_NOP(IMPLIED);     break;
        case 0x05: op_NOP(IMPLIED);     break;
        case 0x06: op_NOP(IMPLIED);     break;
        case 0x07: op_NOP(IMPLIED);     break;
        case 0x08: op_NOP(IMPLIED);     break;
        case 0x09: op_NOP(IMPLIED);     break;
        case 0x0a: op_NOP(IMPLIED);     break;
        case 0x0b: op_NOP(IMPLIED);     break;
        case 0x0c: op_NOP(IMPLIED);     break;
        case 0x0d: op_NOP(IMPLIED);     break;
        case 0x0e: op_NOP(IMPLIED);     break;
        case 0x0f: op_NOP(IMPLIED);     break;

        case 0x10: op_NOP(IMPLIED);     break;
        case 0x11: op_NOP(IMPLIED);     break;
        case 0x12: op_NOP(IMPLIED);     break;
        case 0x13: op_NOP(IMPLIED);     break;
        case 0x14: op_NOP(IMPLIED);     break;
        case 0x15: op_NOP(IMPLIED);     break;
        case 0x16: op_NOP(IMPLIED);     break;
        case 0x17: op_NOP(IMPLIED);     break;
        case 0x18: op_NOP(IMPLIED);     break;
        case 0x19: op_NOP(IMPLIED);     break;
        case 0x1a: op_NOP(IMPLIED);     break;
        case 0x1b: op_NOP(IMPLIED);     break;
        case 0x1c: op_NOP(IMPLIED);     break;
        case 0x1d: op_NOP(IMPLIED);     break;
        case 0x1e: op_NOP(IMPLIED);     break;
        case 0x1f: op_NOP(IMPLIED);     break;

        case 0x20: op_NOP(IMPLIED);     break;
        case 0x21: op_NOP(IMPLIED);     break;
        case 0x22: op_NOP(IMPLIED);     break;
        case 0x23: op_NOP(IMPLIED);     break;
        case 0x24: op_NOP(IMPLIED);     break;
        case 0x25: op_NOP(IMPLIED);     break;
        case 0x26: op_NOP(IMPLIED);     break;
        case 0x27: op_NOP(IMPLIED);     break;
        case 0x28: op_NOP(IMPLIED);     break;
        case 0x29: op_NOP(IMPLIED);     break;
        case 0x2a: op_NOP(IMPLIED);     break;
        case 0x2b: op_NOP(IMPLIED);     break;
        case 0x2c: op_NOP(IMPLIED);     break;
        case 0x2d: op_NOP(IMPLIED);     break;
        case 0x2e: op_NOP(IMPLIED);     break;
        case 0x2f: op_NOP(IMPLIED);     break;

        case 0x30: op_NOP(IMPLIED);     break;
        case 0x31: op_NOP(IMPLIED);     break;
        case 0x32: op_NOP(IMPLIED);     break;
        case 0x33: op_NOP(IMPLIED);     break;
        case 0x34: op_NOP(IMPLIED);     break;
        case 0x35: op_NOP(IMPLIED);     break;
        case 0x36: op_NOP(IMPLIED);     break;
        case 0x37: op_NOP(IMPLIED);     break;
        case 0x38: op_NOP(IMPLIED);     break;
        case 0x39: op_NOP(IMPLIED);     break;
        case 0x3a: op_NOP(IMPLIED);     break;
        case 0x3b: op_NOP(IMPLIED);     break;
        case 0x3c: op_NOP(IMPLIED);     break;
        case 0x3d: op_NOP(IMPLIED);     break;
        case 0x3e: op_NOP(IMPLIED);     break;
        case 0x3f: op_NOP(IMPLIED);     break;

        case 0x40: op_NOP(IMPLIED);     break;
        case 0x41: op_NOP(IMPLIED);     break;
        case 0x42: op_NOP(IMPLIED);     break;
        case 0x43: op_NOP(IMPLIED);     break;
        case 0x44: op_NOP(IMPLIED);     break;
        case 0x45: op_NOP(IMPLIED);     break;
        case 0x46: op_NOP(IMPLIED);     break;
        case 0x47: op_NOP(IMPLIED);     break;
        case 0x48: op_NOP(IMPLIED);     break;
        case 0x49: op_NOP(IMPLIED);     break;
        case 0x4a: op_NOP(IMPLIED);     break;
        case 0x4b: op_NOP(IMPLIED);     break;
        case 0x4c: op_NOP(IMPLIED);     break;
        case 0x4d: op_NOP(IMPLIED);     break;
        case 0x4e: op_NOP(IMPLIED);     break;
        case 0x4f: op_NOP(IMPLIED);     break;

        case 0x50: op_NOP(IMPLIED);     break;
        case 0x51: op_NOP(IMPLIED);     break;
        case 0x52: op_NOP(IMPLIED);     break;
        case 0x53: op_NOP(IMPLIED);     break;
        case 0x54: op_NOP(IMPLIED);     break;
        case 0x55: op_NOP(IMPLIED);     break;
        case 0x56: op_NOP(IMPLIED);     break;
        case 0x57: op_NOP(IMPLIED);     break;
        case 0x58: op_NOP(IMPLIED);     break;
        case 0x59: op_NOP(IMPLIED);     break;
        case 0x5a: op_NOP(IMPLIED);     break;
        case 0x5b: op_NOP(IMPLIED);     break;
        case 0x5c: op_NOP(IMPLIED);     break;
        case 0x5d: op_NOP(IMPLIED);     break;
        case 0x5e: op_NOP(IMPLIED);     break;
        case 0x5f: op_NOP(IMPLIED);     break;

        case 0x60: op_NOP(IMPLIED);     break;
        case 0x61: op_NOP(IMPLIED);     break;
        case 0x62: op_NOP(IMPLIED);     break;
        case 0x63: op_NOP(IMPLIED);     break;
        case 0x64: op_NOP(IMPLIED);     break;
        case 0x65: op_NOP(IMPLIED);     break;
        case 0x66: op_NOP(IMPLIED);     break;
        case 0x67: op_NOP(IMPLIED);     break;
        case 0x68: op_NOP(IMPLIED);     break;
        case 0x69: op_NOP(IMPLIED);     break;
        case 0x6a: op_NOP(IMPLIED);     break;
        case 0x6b: op_NOP(IMPLIED);     break;
        case 0x6c: op_NOP(IMPLIED);     break;
        case 0x6d: op_NOP(IMPLIED);     break;
        case 0x6e: op_NOP(IMPLIED);     break;
        case 0x6f: op_NOP(IMPLIED);     break;

        case 0x70: op_NOP(IMPLIED);     break;
        case 0x71: op_NOP(IMPLIED);     break;
        case 0x72: op_NOP(IMPLIED);     break;
        case 0x73: op_NOP(IMPLIED);     break;
        case 0x74: op_NOP(IMPLIED);     break;
        case 0x75: op_NOP(IMPLIED);     break;
        case 0x76: op_NOP(IMPLIED);     break;
        case 0x77: op_NOP(IMPLIED);     break;
        case 0x78: op_NOP(IMPLIED);     break;
        case 0x79: op_NOP(IMPLIED);     break;
        case 0x7a: op_NOP(IMPLIED);     break;
        case 0x7b: op_NOP(IMPLIED);     break;
        case 0x7c: op_NOP(IMPLIED);     break;
        case 0x7d: op_NOP(IMPLIED);     break;
        case 0x7e: op_NOP(IMPLIED);     break;
        case 0x7f: op_NOP(IMPLIED);     break;

        case 0x80: op_NOP(IMPLIED);     break;
        case 0x81: op_NOP(IMPLIED);     break;
        case 0x82: op_NOP(IMPLIED);     break;
        case 0x83: op_NOP(IMPLIED);     break;
        case 0x84: op_NOP(IMPLIED);     break;
        case 0x85: op_NOP(IMPLIED);     break;
        case 0x86: op_NOP(IMPLIED);     break;
        case 0x87: op_NOP(IMPLIED);     break;
        case 0x88: op_NOP(IMPLIED);     break;
        case 0x89: op_NOP(IMPLIED);     break;
        case 0x8a: op_NOP(IMPLIED);     break;
        case 0x8b: op_NOP(IMPLIED);     break;
        case 0x8c: op_NOP(IMPLIED);     break;
        case 0x8d: op_NOP(IMPLIED);     break;
        case 0x8e: op_NOP(IMPLIED);     break;
        case 0x8f: op_NOP(IMPLIED);     break;

        case 0x90: op_NOP(IMPLIED);     break;
        case 0x91: op_NOP(IMPLIED);     break;
        case 0x92: op_NOP(IMPLIED);     break;
        case 0x93: op_NOP(IMPLIED);     break;
        case 0x94: op_NOP(IMPLIED);     break;
        case 0x95: op_NOP(IMPLIED);     break;
        case 0x96: op_NOP(IMPLIED);     break;
        case 0x97: op_NOP(IMPLIED);     break;
        case 0x98: op_NOP(IMPLIED);     break;
        case 0x99: op_NOP(IMPLIED);     break;
        case 0x9a: op_NOP(IMPLIED);     break;
        case 0x9b: op_NOP(IMPLIED);     break;
        case 0x9c: op_NOP(IMPLIED);     break;
        case 0x9d: op_NOP(IMPLIED);     break;
        case 0x9e: op_NOP(IMPLIED);     break;
        case 0x9f: op_NOP(IMPLIED);     break;

        case 0xa0: op_NOP(IMPLIED);     break;
        case 0xa1: op_NOP(IMPLIED);     break;
        case 0xa2: op_NOP(IMPLIED);     break;
        case 0xa3: op_NOP(IMPLIED);     break;
        case 0xa4: op_NOP(IMPLIED);     break;
        case 0xa5: op_NOP(IMPLIED);     break;
        case 0xa6: op_NOP(IMPLIED);     break;
        case 0xa7: op_NOP(IMPLIED);     break;
        case 0xa8: op_NOP(IMPLIED);     break;
        case 0xa9: op_NOP(IMPLIED);     break;
        case 0xaa: op_NOP(IMPLIED);     break;
        case 0xab: op_NOP(IMPLIED);     break;
        case 0xac: op_NOP(IMPLIED);     break;
        case 0xad: op_NOP(IMPLIED);     break;
        case 0xae: op_NOP(IMPLIED);     break;
        case 0xaf: op_NOP(IMPLIED);     break;

        case 0xb0: op_NOP(IMPLIED);     break;
        case 0xb1: op_NOP(IMPLIED);     break;
        case 0xb2: op_NOP(IMPLIED);     break;
        case 0xb3: op_NOP(IMPLIED);     break;
        case 0xb4: op_NOP(IMPLIED);     break;
        case 0xb5: op_NOP(IMPLIED);     break;
        case 0xb6: op_NOP(IMPLIED);     break;
        case 0xb7: op_NOP(IMPLIED);     break;
        case 0xb8: op_NOP(IMPLIED);     break;
        case 0xb9: op_NOP(IMPLIED);     break;
        case 0xba: op_NOP(IMPLIED);     break;
        case 0xbb: op_NOP(IMPLIED);     break;
        case 0xbc: op_NOP(IMPLIED);     break;
        case 0xbd: op_NOP(IMPLIED);     break;
        case 0xbe: op_NOP(IMPLIED);     break;
        case 0xbf: op_NOP(IMPLIED);     break;

        case 0xc0: op_NOP(IMPLIED);     break;
        case 0xc1: op_NOP(IMPLIED);     break;
        case 0xc2: op_NOP(IMPLIED);     break;
        case 0xc3: op_NOP(IMPLIED);     break;
        case 0xc4: op_NOP(IMPLIED);     break;
        case 0xc5: op_NOP(IMPLIED);     break;
        case 0xc6: op_NOP(IMPLIED);     break;
        case 0xc7: op_NOP(IMPLIED);     break;
        case 0xc8: op_NOP(IMPLIED);     break;
        case 0xc9: op_NOP(IMPLIED);     break;
        case 0xca: op_NOP(IMPLIED);     break;
        case 0xcb: op_NOP(IMPLIED);     break;
        case 0xcc: op_NOP(IMPLIED);     break;
        case 0xcd: op_NOP(IMPLIED);     break;
        case 0xce: op_NOP(IMPLIED);     break;
        case 0xcf: op_NOP(IMPLIED);     break;

        case 0xd0: op_NOP(IMPLIED);     break;
        case 0xd1: op_NOP(IMPLIED);     break;
        case 0xd2: op_NOP(IMPLIED);     break;
        case 0xd3: op_NOP(IMPLIED);     break;
        case 0xd4: op_NOP(IMPLIED);     break;
        case 0xd5: op_NOP(IMPLIED);     break;
        case 0xd6: op_NOP(IMPLIED);     break;
        case 0xd7: op_NOP(IMPLIED);     break;
        case 0xd8: op_NOP(IMPLIED);     break;
        case 0xd9: op_NOP(IMPLIED);     break;
        case 0xda: op_NOP(IMPLIED);     break;
        case 0xdb: op_NOP(IMPLIED);     break;
        case 0xdc: op_NOP(IMPLIED);     break;
        case 0xdd: op_NOP(IMPLIED);     break;
        case 0xde: op_NOP(IMPLIED);     break;
        case 0xdf: op_NOP(IMPLIED);     break;

        case 0xe0: op_NOP(IMPLIED);     break;
        case 0xe1: op_NOP(IMPLIED);     break;
        case 0xe2: op_NOP(IMPLIED);     break;
        case 0xe3: op_NOP(IMPLIED);     break;
        case 0xe4: op_NOP(IMPLIED);     break;
        case 0xe5: op_NOP(IMPLIED);     break;
        case 0xe6: op_NOP(IMPLIED);     break;
        case 0xe7: op_NOP(IMPLIED);     break;
        case 0xe8: op_NOP(IMPLIED);     break;
        case 0xe9: op_NOP(IMPLIED);     break;
        case 0xea: op_NOP(IMPLIED);     break;
        case 0xeb: op_NOP(IMPLIED);     break;
        case 0xec: op_NOP(IMPLIED);     break;
        case 0xed: op_NOP(IMPLIED);     break;
        case 0xee: op_NOP(IMPLIED);     break;
        case 0xef: op_NOP(IMPLIED);     break;

        case 0xf0: op_NOP(IMPLIED);     break;
        case 0xf1: op_NOP(IMPLIED);     break;
        case 0xf2: op_NOP(IMPLIED);     break;
        case 0xf3: op_NOP(IMPLIED);     break;
        case 0xf4: op_NOP(IMPLIED);     break;
        case 0xf5: op_NOP(IMPLIED);     break;
        case 0xf6: op_NOP(IMPLIED);     break;
        case 0xf7: op_NOP(IMPLIED);     break;
        case 0xf8: op_NOP(IMPLIED);     break;
        case 0xf9: op_NOP(IMPLIED);     break;
        case 0xfa: op_NOP(IMPLIED);     break;
        case 0xfb: op_NOP(IMPLIED);     break;
        case 0xfc: op_NOP(IMPLIED);     break;
        case 0xfd: op_NOP(IMPLIED);     break;
        case 0xfe: op_NOP(IMPLIED);     break;
        case 0xff: op_NOP(IMPLIED);     break;

        
        default:
            // Handle unknown opcode
            // should NEVER happen
            // so force error without including a lib
            // because i'm lazy
            int errorer = 1 / 0;
            break;
    }
}

// CPU INSTRUCTION HELPERS

void CPU::_setZNFlags(uint8 val) {
    p.z = (val == 0);
    p.n = (val & 0x80) != 0;
}

void CPU::_addToAccumulator(uint8 val) {
    // Perform addition in wider type to capture carry (implicit promotions)
    uint16 sum = a + val + p.c;
    p.c = (sum > 0xFF);
    // Overflow: if sign of a and val are the same and sign of result differs
    p.v = (~(a ^ val) & (a ^ sum) & 0x80) != 0;
    uint8 result = sum & 0xFF;
    a = result;
    _setZNFlags(result);
}


// CPU INSTRUCTIONS

void CPU::op_ADC(AddressingMode mode) {
	// Add with Carry
	_addToAccumulator(readMem(getOperandAddress(mode)));
}

void CPU::op_NOP(AddressingMode mode) {
    // No operation
}
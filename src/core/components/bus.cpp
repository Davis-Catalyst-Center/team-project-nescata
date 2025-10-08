#include <iostream>

#include "bus.hpp"


// BUS IMPLEMENTATION

Bus::Bus() {
    // Initialize memory
    std::fill(std::begin(memory), std::end(memory), 0);
    cart = nullptr; // No cartridge loaded
}

uint8 Bus::read(uint16 addr) {
    switch (addr) {
        case 0x0000 ... 0x1fff: // 2KB RAM
            // mirror the 2KB RAM every 0x800 bytes
            return memory[addr & 0x7ff];
        case 0x2000 ... 0x3fff: // PPU registers (mirrored every 8 bytes)
            // Placeholder for PPU register read
            return 0;
        case 0x4000 ... 0x4017: // APU and I/O registers
            // Placeholder for APU and I/O register read
            return 0;
        case 0x4018 ... 0x401f: // APU and I/O functionality that is normally disabled
            // Placeholder for disabled APU and I/O read
            return 0;
        case 0x4020 ... 0xffff: // Cartridge space (PRG ROM, PRG RAM, and mapper registers)
            // See if the cart wants to handle this read
            if (cart) return cart->read(addr);
            else return 0;
        default:
            std::cout << "Read from unmapped address: " << std::hex << addr << std::dec << "\n";
            return 0;
    }
}

void Bus::write(uint16 addr, uint8 val) {
    switch (addr) {
        case 0x0000 ... 0x1fff: // 2KB RAM
            memory[addr] = val;
            break;
        case 0x4020 ... 0xffff:
            if (cart) cart->write(addr, val);
            break;
        default:
            break;
    }
}

void Bus::loadCart(Cart* cartPtr) {
    cart = cartPtr;
}

void Bus::unloadCart() {
    cart = nullptr;
}


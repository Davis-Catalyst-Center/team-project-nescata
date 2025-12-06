#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include "mapper.hpp"

// written by ai

class MMC1 : public Mapper {
private:
    uint8_t shiftRegister = 0x00;
    int writeCount = 0;

    uint8_t controlReg = 0x0C; // Default to Mode 3
    uint8_t chrBank0Reg = 0;
    uint8_t chrBank1Reg = 0;
    uint8_t prgBankReg = 0;

    int currentPrgBankLowIndex = 0;
    int currentPrgBankHighIndex = 0;
    
    // CHR mapping: Vector Index + Offset (0 or 0x1000)
    int currentChrBankLowIndex = 0;
    int currentChrBankLowOffset = 0;
    int currentChrBankHighIndex = 0;
    int currentChrBankHighOffset = 0;

    std::array<uint8_t, 0x2000> prgRam;
    std::array<uint8_t, 0x2000> chrRam;
    bool hasChrRam = false;

public:
    MMC1(
        std::vector<std::array<uint8_t, 0x4000>>* prgBanksRef,
        std::vector<std::array<uint8_t, 0x2000>>* chrBanksRef,
        bool batteryBacked
    ) {
        mapperID = 1;
        this->prgBanks = *prgBanksRef;
        this->chrBanks = *chrBanksRef;
        prgBankCount = prgBanks.size();
        chrBankCount = chrBanks.size();
        
        hasChrRam = (chrBankCount == 0);
        if (hasChrRam) chrRam.fill(0);
        prgRam.fill(0);
        
        reset();
    }

    void reset() override {
        shiftRegister = 0x00;
        writeCount = 0;
        controlReg = 0x0C; 
        chrBank0Reg = 0;
        chrBank1Reg = 0;
        prgBankReg = 0;
        updateOffsets();
    }

    uint8_t read(uint16_t addr) override {
        if (addr >= 0x6000 && addr <= 0x7FFF) {
            return prgRam[addr & 0x1FFF];
        }
        if (addr >= 0x8000 && addr <= 0xBFFF) {
            return prgBanks[currentPrgBankLowIndex][addr - 0x8000];
        } 
        else if (addr >= 0xC000 && addr <= 0xFFFF) {
            return prgBanks[currentPrgBankHighIndex][addr - 0xC000];
        }
        return 0;
    }

    void write(uint16_t addr, uint8_t value) override {
        // PRG-RAM (Always writable in standard MMC1)
        if (addr >= 0x6000 && addr <= 0x7FFF) {
            prgRam[addr & 0x1FFF] = value;
            return;
        }

        // Registers
        if (addr >= 0x8000) {
            if (value & 0x80) { // Reset Shift Register
                shiftRegister = 0x00;
                writeCount = 0;
                controlReg |= 0x0C; 
                updateOffsets();
                return;
            }

            shiftRegister |= ((value & 0x01) << writeCount);
            writeCount++;

            if (writeCount == 5) {
                uint8_t target = (addr >> 13) & 0x03;
                switch (target) {
                    case 0: controlReg = shiftRegister; break;
                    case 1: chrBank0Reg = shiftRegister; break;
                    case 2: chrBank1Reg = shiftRegister; break;
                    case 3: prgBankReg = shiftRegister; break;
                }
                shiftRegister = 0x00;
                writeCount = 0;
                updateOffsets();
            }
        }
    }

    uint8_t readChr(uint16_t addr) override {
        if (hasChrRam) return chrRam[addr & 0x1FFF];

        if (addr < 0x1000) {
            return chrBanks[currentChrBankLowIndex][currentChrBankLowOffset + addr];
        } else if (addr < 0x2000) {
            return chrBanks[currentChrBankHighIndex][currentChrBankHighOffset + (addr & 0x0FFF)];
        }
        return 0;
    }

    void writeChr(uint16_t addr, uint8_t value) override {
        if (hasChrRam) chrRam[addr & 0x1FFF] = value;
    }

private:
    void updateOffsets() {
        // PRG Logic
        uint8_t prgMode = (controlReg >> 2) & 0x03;
        uint8_t romBank = prgBankReg & 0x0F;

        if (prgMode <= 1) { // 32KB Mode
            currentPrgBankLowIndex = (romBank & 0xFE) % prgBankCount;
            currentPrgBankHighIndex = (romBank | 0x01) % prgBankCount;
        } else if (prgMode == 2) { // Fix First, Switch High
            currentPrgBankLowIndex = 0;
            currentPrgBankHighIndex = romBank % prgBankCount;
        } else { // Switch Low, Fix Last (Mode 3)
            currentPrgBankLowIndex = romBank % prgBankCount;
            currentPrgBankHighIndex = prgBankCount - 1;
        }

        // CHR Logic
        bool chrMode8k = !((controlReg >> 4) & 0x01);
        if (hasChrRam) return;

        if (chrMode8k) {
            // 8KB Mode (align to even)
            uint8_t selected8k = (chrBank0Reg >> 1) % chrBankCount;
            currentChrBankLowIndex = selected8k;
            currentChrBankLowOffset = 0;
            currentChrBankHighIndex = selected8k;
            currentChrBankHighOffset = 0x1000;
        } else {
            // 4KB Mode
            int bank0 = chrBank0Reg;
            int bank1 = chrBank1Reg;
            currentChrBankLowIndex = (bank0 / 2) % chrBankCount;
            currentChrBankLowOffset = (bank0 % 2) * 0x1000;
            currentChrBankHighIndex = (bank1 / 2) % chrBankCount;
            currentChrBankHighOffset = (bank1 % 2) * 0x1000;
        }
    }
};
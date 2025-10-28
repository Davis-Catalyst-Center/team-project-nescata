#pragma once
#include <cstdint>

class ppu {
private:
    uint8_t vram[0x4000];
    uint8_t oam[0x100];
    uint8_t pram[0x20];

    uint8_t ppuctrl;
    uint8_t ppumask;
    uint8_t ppustatus;
    uint8_t oamaddr;
    uint8_t oamdata;
    uint8_t ppuscroll;
    uint8_t ppuaddr;
    uint8_t ppudata;
    uint16_t vramaddr;
    uint16_t tempaddr; 
    uint8_t finexscroll;          
    bool writetoggle;

    int scanline;
    int cycle; 

public:
    ppu();
    ~ppu();
    
    // Main functions
    void step();                    // Execute one PPU cycle
    void reset();                   // Reset PPU state
    
    // CPU interface
    uint8_t read_register(uint16_t addr);
    void write_register(uint16_t addr, uint8_t data);
    
    // Memory access
    uint8_t read_vram(uint16_t addr);
    void write_vram(uint16_t addr, uint8_t data);
    
    // Rendering functions
    void render_scanline();
    void render_background();
    void render_sprites();
    
    // Utility
    bool is_rendering();
    bool nmi_occurred();
};
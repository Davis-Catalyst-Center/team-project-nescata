#include "bus.hpp"
#include "ppu.hpp"

ppu::ppu(){
    for (int i = 0; i < 0x4000; i++) {
        vram[i] = 0;
    }

    for (int i = 0; i < 0x100; i++) {
        oam[i] = 0;
    }

    for (int i = 0; i < 0x20; i++) {
        pram[i] = 0;
    }

    ppuctrl = 0;
    ppumask = 0;
    ppustatus = 0;
    oamaddr = 0;
    oamdata = 0;
    ppuscroll = 0;
    ppuaddr = 0;
    ppudata = 0;
    vramaddr = 0;
    tempaddr = 0;
    finexscroll = 0;
    writetoggle = false;
    scanline = 0;
    cycle = 0;
}
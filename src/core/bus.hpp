#pragma once

#include "types.hpp"

#include "apu.hpp"
#include "ppu.hpp"
#include "cart.hpp"
#include "controller.hpp"


class Bus {
private:
    uint8 memory [0x800]; // 2KB internal memory

	APU* apu = nullptr;
	PPU* ppu = nullptr;
    Cart* cart = nullptr;
	Controller* controller1 = nullptr;
	Controller* controller2 = nullptr;
public:

    Bus();

    uint8 read(uint16 addr);
    void write(uint16 addr, uint8 val);

	void clock(int cycles);

	void connectAPU(APU* apu);
	void disconnectAPU();
	void connectPPU(PPU* ppu);
	void disconnectPPU();
    void connectCart(Cart* cart);
    void disconnectCart();
	void connectController1(Controller* controller1);
	void disconnectController1();
	void connectController2(Controller* controller2);
	void disconnectController2();
};

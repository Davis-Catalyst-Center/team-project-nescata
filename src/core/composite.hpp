#pragma once

#include "types.hpp"
#include "ppu.hpp"
#include "cart.hpp"

class Composite {
private:
	Cart* cart = nullptr;
	PPU* ppu = nullptr;

public:
	Composite();

	void connectPPU(PPU* ppu);
	void disconnectPPU();
	void connectCart(Cart* cart);
	void disconnectCart();
};
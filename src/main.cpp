#include "core/core.hpp"
#include "core/cart.hpp"



int main(int argc, char* argv[]) {
    Core core;
    Cart cart(argc > 1 ? argv[1] : "roms/nestest.nes");

    core.connectCart(&cart);
	core.setController1(STANDARD);
    core.run();
    return 0;
}
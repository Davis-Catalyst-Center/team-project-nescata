#include "core/core.hpp"
#include "core/cart.hpp"



int main() {
    Core core;
    Cart cart("roms/nestest.nes");

    core.loadCart(&cart);
    core.run();
    return 0;
}
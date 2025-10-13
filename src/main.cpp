#include "core/core.hpp"
#include "core/components/cart.hpp"



int main() {
    Core core;
    Cart cart("path_to_rom.nes");

    core.loadCart(cart);
    core.run();
    return 0;
}
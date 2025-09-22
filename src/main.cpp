#include "core/core.h"
#include "cart.h"



int main() {
    Core core;
    Cart cart("path_to_rom.nes");

    core.loadCart(&cart);
    core.run();
    return 0;
}
#include "core.hpp"

Core::Core() {
    // Initialize CPU and Bus
}

void Core::run() {
    // Main emulation loop
    // TODO: Implement CPU clock or main loop
}

void Core::loadCart(Cart& cart) {
    bus.loadCart(cart);
}

void Core::unloadCart() {
    bus.unloadCart();
}

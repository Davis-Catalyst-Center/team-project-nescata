#include "core.hpp"

Core::Core() : cpu(bus) {
    // cpu is initialized via member initializer list
}

void Core::run() {
    cpu.reset();
    cpu.run();
}

void Core::loadCart(Cart* cart) {
    bus.loadCart(cart);
}

void Core::unloadCart() {
    bus.unloadCart();
}

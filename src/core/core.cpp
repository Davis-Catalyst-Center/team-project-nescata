#include "core.hpp"

Core::Core() : cpu(bus), window(){
}

void Core::run() {
    cpu.powerOn();
    cpu.run();
}

void Core::reset() {
	cpu.reset();
}

void Core::fullReset() {
	bus.unloadCart();
	cpu.powerOn();
}

void Core::loadCart(Cart* cart) {
    bus.loadCart(cart);
}

void Core::unloadCart() {
    bus.unloadCart();
}

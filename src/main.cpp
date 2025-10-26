#include "core/core.hpp"
#include "core/cart.hpp"



int main(int argc, char* argv[]) {
    Core core;
    Cart cart;
    if (argc == 1) {
		cart = Cart();
	} else {
		cart = Cart(argv[1]);
	}

    core.loadCart(&cart);
    core.run();
    return 0;
}
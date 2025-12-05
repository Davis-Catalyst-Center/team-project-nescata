#include "core.hpp"
#include "cart.hpp"


int main(int argc, char* argv[]) {
	Core core;
	Cart cart(argc > 1 ? argv[1] : "");

	core.connectCart(&cart);
	if (!cart.blank)
		cart.mapper->writeProtect = false;
	core.setController1(STANDARD);
	// core.setController2(STANDARD);
	core.run();
	return 0;
}
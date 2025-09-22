#include <fstream>
#include <iostream>
#include "cart.h"

Cart::Cart(const char* filename) {

    int safe = 1;
    std::ifstream gameFile(filename, std::ios::binary);
    while (safe == 1){
        if (gameFile.is_open()){
            std::cout<<"Yes"<<std::endl;
            break;
        }
    }

    char ch;

    for (int i = 0; i < 16; i++) {
        gameFile.get(ch);
        header[i] = ch;
    }

    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        std::cerr << "Not a valid iNES file!" << std::endl;
        return;
    }
    for (int i = 0; i < 16; i++) {
        gameFile.get(ch);
        header[i] = ch;
    }
    // Constructor implementation (e.g., load ROM file)
}

Cart::~Cart() {
    // Destructor implementation (e.g., free resources)
}
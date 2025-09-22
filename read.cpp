#include <fstream>
#include <string>
#include <iostream>
int main(){
    int safe = 1;
    std::ifstream gameFile("roms/nestest.nes");
    while (safe == 1){
        if (gameFile.is_open()){
            std::cout<<"Yes"<<std::endl;
            break;
        }    
        break;
    }
    char ch;
    //while (gameFile.get(ch)) {
    //    std::cout << ch;
    //    }
    std::cout<<"Over"<<std::endl;
    return 0;
}
#include <fstream>
#include <string>
#include <iostream>
int main(){
    int safe = 1;
    while (safe == 1){
        std::ifstream gameFile("nestest.nes");
        if (!gameFile.is_open()){
            std::cout<<"No"<<std::endl;
        }    
    }
    std::cout<<"Over"<<std::endl;
    return 0;
}
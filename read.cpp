#include <fstream>
#include <string>
#include <iostream>
int main(){
    int safe = 1;
    std::ifstream gameFile("nestest.nes");
    while (safe == 1){
        if (gameFile.is_open()){
            std::cout<<"No"<<std::endl;
            break;
        }    
        break;
    }
    char ch;
    while (gameFile.get(ch)) {
        // Process each 'ch' here
        std::cout << ch;
        }
    std::cout<<"Over"<<std::endl;
    return 0;
}
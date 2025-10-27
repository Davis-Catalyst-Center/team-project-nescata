mkdir build
g++ -std=c++17 -g -o build/main.out src/*.cpp src/*/*.cpp -lSDL2 && \
echo "compiled" && gdb ./build/main.out
mkdir -p build
g++ -std=c++17 -g -o build/main.out src/*.cpp src/*/*.cpp -lSDL2 && \
echo "compiled" && \
gdb --args ./build/main.out $@
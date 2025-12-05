mkdir -p build
g++ -std=c++17 -Iinclude -o build/main.out src/*.cpp src/*/*.cpp -lSDL2 && \
echo "compiled" && \
./build/main.out $@
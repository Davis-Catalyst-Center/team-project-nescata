g++ -std=c++17 -lSDL2 -o build/main.out src/*.cpp src/*/*.cpp -lSDL2 && \
echo "compiled" && ./build/main.out $1
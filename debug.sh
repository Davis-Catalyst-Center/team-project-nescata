g++ -std=c++17 -g -o main.out src/*.cpp src/*/*.cpp -lSDL2 && \
echo "compiled" && gdb ./main.out